#include "nanodet_openvino.h"

int person_num=0;
inline float fast_exp(float x) 
{
    union {
        uint32_t i;
        float f;
    } v{};
    v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
    return v.f;
}

inline float sigmoid(float x) 
{
    return 1.0f / (1.0f + fast_exp(-x));
}

template<typename _Tp>
int activation_function_softmax(const _Tp* src, _Tp* dst, int length)
{
    const _Tp alpha = *std::max_element(src, src + length);
    _Tp denominator{ 0 };

    for (int i = 0; i < length; ++i) 
    {
        dst[i] = fast_exp(src[i] - alpha);
        denominator += dst[i];
    }

    for (int i = 0; i < length; ++i) 
    {
        dst[i] /= denominator;
    }

    return 0;
}

NanoDet::NanoDet(const char* model_path)
{
    InferenceEngine::Core ie;
    InferenceEngine::CNNNetwork model = ie.ReadNetwork(model_path);
    // prepare input settings
    InferenceEngine::InputsDataMap inputs_map(model.getInputsInfo());
    input_name_ = inputs_map.begin()->first;
    InferenceEngine::InputInfo::Ptr input_info = inputs_map.begin()->second;
    //input_info->setPrecision(InferenceEngine::Precision::FP32);
    //input_info->setLayout(InferenceEngine::Layout::NCHW);



    //prepare output settings
    InferenceEngine::OutputsDataMap outputs_map(model.getOutputsInfo());
    for (auto &output_info : outputs_map) 
    {
        //std::cout<< "Output:" << output_info.first <<std::endl;
        output_info.second->setPrecision(InferenceEngine::Precision::FP32);
    }

    //get network
    network_ = ie.LoadNetwork(model, "CPU");
    infer_request_ = network_.CreateInferRequest();

}

NanoDet::~NanoDet()
{
}

void NanoDet::preprocess(cv::Mat& image, InferenceEngine::Blob::Ptr& blob)
{
    int img_w = image.cols;
    int img_h = image.rows;
    int channels = 3;

    InferenceEngine::MemoryBlob::Ptr mblob = InferenceEngine::as<InferenceEngine::MemoryBlob>(blob);
    if (!mblob) 
    {
        THROW_IE_EXCEPTION << "We expect blob to be inherited from MemoryBlob in matU8ToBlob, "
            << "but by fact we were not able to cast inputBlob to MemoryBlob";
    }
    // locked memory holder should be alive all time while access to its buffer happens
    auto mblobHolder = mblob->wmap();

    float *blob_data = mblobHolder.as<float *>();


    for (size_t c = 0; c < channels; c++) 
    {
        for (size_t  h = 0; h < img_h; h++) 
        {
            for (size_t w = 0; w < img_w; w++) 
            {
                blob_data[c * img_w * img_h + h * img_w + w] =
                    (float)image.at<cv::Vec3b>(h, w)[c];
            }
        }
    }
}

std::vector<BoxInfo> NanoDet::detect(cv::Mat image, float score_threshold, float nms_threshold)
{
    //auto start = std::chrono::steady_clock::now();

    InferenceEngine::Blob::Ptr input_blob = infer_request_.GetBlob(input_name_);
    preprocess(image, input_blob);

    // do inference
    infer_request_.Infer();

    // get output
    std::vector<std::vector<BoxInfo>> results;
    results.resize(this->num_class_);

    for (const auto& head_info : this->heads_info_)
    {
        const InferenceEngine::Blob::Ptr dis_pred_blob = infer_request_.GetBlob(head_info.dis_layer);
        const InferenceEngine::Blob::Ptr cls_pred_blob = infer_request_.GetBlob(head_info.cls_layer);

        auto mdis_pred = InferenceEngine::as<InferenceEngine::MemoryBlob>(dis_pred_blob);
        auto mdis_pred_holder = mdis_pred->rmap();
        const float *dis_pred = mdis_pred_holder.as<const float *>();

        auto mcls_pred = InferenceEngine::as<InferenceEngine::MemoryBlob>(cls_pred_blob);
        auto mcls_pred_holder = mcls_pred->rmap();
        const float *cls_pred = mcls_pred_holder.as<const float *>();
        // std::cout << "c:" << cls_pred.c << " h:" << cls_pred.h <<" w:" <<cls_pred.w <<std::endl;
        this->decode_infer(cls_pred, dis_pred, head_info.stride, score_threshold, results);
    }

    std::vector<BoxInfo> dets;
    for (int i = 0; i < (int)results.size(); i++)
    {
        this->nms(results[i], nms_threshold);
        
        for (auto& box : results[i])
        {
            dets.push_back(box);
        }
    }
    person_num=dets.size();
    //auto end = std::chrono::steady_clock::now();
    //double time = std::chrono::duration<double, std::milli>(end - start).count();
    //std::cout << "inference time:" << time << "ms" << std::endl;
    return dets;
}

void NanoDet::decode_infer(const float*& cls_pred, const float*& dis_pred, int stride, float threshold, std::vector<std::vector<BoxInfo>>& results)
{
    int feature_h = input_size_ / stride;
    int feature_w = input_size_ / stride;
    //cv::Mat debug_heatmap = cv::Mat::zeros(feature_h, feature_w, CV_8UC3);
    for (int idx = 0; idx < feature_h * feature_w; idx++)
    {
        int row = idx / feature_w;
        int col = idx % feature_w;
        float score = 0;
        int cur_label = 0;
        
        for (int label = 0; label < num_class_; label++)
        {
            if (cls_pred[idx * num_class_ +label] > score)
            {
                score = cls_pred[idx * num_class_ + label];
                cur_label = label;
            }
        }
        if (score > threshold)
        {
            //std::cout << row << "," << col <<" label:" << cur_label << " score:" << score << std::endl;
            const float* bbox_pred = dis_pred + idx * (reg_max_ + 1) * 4;
            results[cur_label].push_back(this->disPred2Bbox(bbox_pred, cur_label, score, col, row, stride));
            //debug_heatmap.at<cv::Vec3b>(row, col)[0] = 255;
            //cv::imshow("debug", debug_heatmap);
        }

    }
}

BoxInfo NanoDet::disPred2Bbox(const float*& dfl_det, int label, float score, int x, int y, int stride)
{
    float ct_x = (x + 0.5) * stride;
    float ct_y = (y + 0.5) * stride;
    std::vector<float> dis_pred;
    dis_pred.resize(4);
    for (int i = 0; i < 4; i++)
    {
        float dis = 0;
        float* dis_after_sm = new float[reg_max_ + 1];
        activation_function_softmax(dfl_det + i * (reg_max_ + 1), dis_after_sm, reg_max_ + 1);
        for (int j = 0; j < reg_max_ + 1; j++)
        {
            dis += j * dis_after_sm[j];
        }
        dis *= stride;
        //std::cout << "dis:" << dis << std::endl;
        dis_pred[i] = dis;
        delete[] dis_after_sm;
    }
    float xmin = (std::max)(ct_x - dis_pred[0], .0f);
    float ymin = (std::max)(ct_y - dis_pred[1], .0f);
    float xmax = (std::min)(ct_x + dis_pred[2], (float)this->input_size_);
    float ymax = (std::min)(ct_y + dis_pred[3], (float)this->input_size_);

    //std::cout << xmin << "," << ymin << "," << xmax << "," << xmax << "," << std::endl;
    return BoxInfo { xmin, ymin, xmax, ymax, score, label };
}

void NanoDet::nms(std::vector<BoxInfo>& input_boxes, float NMS_THRESH)
{
    std::sort(input_boxes.begin(), input_boxes.end(), [](BoxInfo a, BoxInfo b) { return a.score > b.score; });
    std::vector<float> vArea(input_boxes.size());
    for (int i = 0; i < int(input_boxes.size()); ++i) 
    {
        vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1)
            * (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
    }
    for (int i = 0; i < int(input_boxes.size()); ++i) 
    {
        for (int j = i + 1; j < int(input_boxes.size());) 
        {
            float xx1 = (std::max)(input_boxes[i].x1, input_boxes[j].x1);
            float yy1 = (std::max)(input_boxes[i].y1, input_boxes[j].y1);
            float xx2 = (std::min)(input_boxes[i].x2, input_boxes[j].x2);
            float yy2 = (std::min)(input_boxes[i].y2, input_boxes[j].y2);
            float w = (std::max)(float(0), xx2 - xx1 + 1);
            float h = (std::max)(float(0), yy2 - yy1 + 1);
            float inter = w * h;
            float ovr = inter / (vArea[i] + vArea[j] - inter);
            if (ovr >= NMS_THRESH) 
            {
                input_boxes.erase(input_boxes.begin() + j);
                vArea.erase(vArea.begin() + j);
            }
            else 
            {
                j++;
            }
        }
    }
}

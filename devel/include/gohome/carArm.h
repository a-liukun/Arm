// Generated by gencpp from file gohome/carArm.msg
// DO NOT EDIT!


#ifndef GOHOME_MESSAGE_CARARM_H
#define GOHOME_MESSAGE_CARARM_H

#include <ros/service_traits.h>


#include <gohome/carArmRequest.h>
#include <gohome/carArmResponse.h>


namespace gohome
{

struct carArm
{

typedef carArmRequest Request;
typedef carArmResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;

}; // struct carArm
} // namespace gohome


namespace ros
{
namespace service_traits
{


template<>
struct MD5Sum< ::gohome::carArm > {
  static const char* value()
  {
    return "f12396c1cf3ed85338aaa27ae2758520";
  }

  static const char* value(const ::gohome::carArm&) { return value(); }
};

template<>
struct DataType< ::gohome::carArm > {
  static const char* value()
  {
    return "gohome/carArm";
  }

  static const char* value(const ::gohome::carArm&) { return value(); }
};


// service_traits::MD5Sum< ::gohome::carArmRequest> should match
// service_traits::MD5Sum< ::gohome::carArm >
template<>
struct MD5Sum< ::gohome::carArmRequest>
{
  static const char* value()
  {
    return MD5Sum< ::gohome::carArm >::value();
  }
  static const char* value(const ::gohome::carArmRequest&)
  {
    return value();
  }
};

// service_traits::DataType< ::gohome::carArmRequest> should match
// service_traits::DataType< ::gohome::carArm >
template<>
struct DataType< ::gohome::carArmRequest>
{
  static const char* value()
  {
    return DataType< ::gohome::carArm >::value();
  }
  static const char* value(const ::gohome::carArmRequest&)
  {
    return value();
  }
};

// service_traits::MD5Sum< ::gohome::carArmResponse> should match
// service_traits::MD5Sum< ::gohome::carArm >
template<>
struct MD5Sum< ::gohome::carArmResponse>
{
  static const char* value()
  {
    return MD5Sum< ::gohome::carArm >::value();
  }
  static const char* value(const ::gohome::carArmResponse&)
  {
    return value();
  }
};

// service_traits::DataType< ::gohome::carArmResponse> should match
// service_traits::DataType< ::gohome::carArm >
template<>
struct DataType< ::gohome::carArmResponse>
{
  static const char* value()
  {
    return DataType< ::gohome::carArm >::value();
  }
  static const char* value(const ::gohome::carArmResponse&)
  {
    return value();
  }
};

} // namespace service_traits
} // namespace ros

#endif // GOHOME_MESSAGE_CARARM_H

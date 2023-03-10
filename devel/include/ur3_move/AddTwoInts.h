// Generated by gencpp from file ur3_move/AddTwoInts.msg
// DO NOT EDIT!


#ifndef UR3_MOVE_MESSAGE_ADDTWOINTS_H
#define UR3_MOVE_MESSAGE_ADDTWOINTS_H

#include <ros/service_traits.h>


#include <ur3_move/AddTwoIntsRequest.h>
#include <ur3_move/AddTwoIntsResponse.h>


namespace ur3_move
{

struct AddTwoInts
{

typedef AddTwoIntsRequest Request;
typedef AddTwoIntsResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;

}; // struct AddTwoInts
} // namespace ur3_move


namespace ros
{
namespace service_traits
{


template<>
struct MD5Sum< ::ur3_move::AddTwoInts > {
  static const char* value()
  {
    return "6a2e34150c00229791cc89ff309fff21";
  }

  static const char* value(const ::ur3_move::AddTwoInts&) { return value(); }
};

template<>
struct DataType< ::ur3_move::AddTwoInts > {
  static const char* value()
  {
    return "ur3_move/AddTwoInts";
  }

  static const char* value(const ::ur3_move::AddTwoInts&) { return value(); }
};


// service_traits::MD5Sum< ::ur3_move::AddTwoIntsRequest> should match
// service_traits::MD5Sum< ::ur3_move::AddTwoInts >
template<>
struct MD5Sum< ::ur3_move::AddTwoIntsRequest>
{
  static const char* value()
  {
    return MD5Sum< ::ur3_move::AddTwoInts >::value();
  }
  static const char* value(const ::ur3_move::AddTwoIntsRequest&)
  {
    return value();
  }
};

// service_traits::DataType< ::ur3_move::AddTwoIntsRequest> should match
// service_traits::DataType< ::ur3_move::AddTwoInts >
template<>
struct DataType< ::ur3_move::AddTwoIntsRequest>
{
  static const char* value()
  {
    return DataType< ::ur3_move::AddTwoInts >::value();
  }
  static const char* value(const ::ur3_move::AddTwoIntsRequest&)
  {
    return value();
  }
};

// service_traits::MD5Sum< ::ur3_move::AddTwoIntsResponse> should match
// service_traits::MD5Sum< ::ur3_move::AddTwoInts >
template<>
struct MD5Sum< ::ur3_move::AddTwoIntsResponse>
{
  static const char* value()
  {
    return MD5Sum< ::ur3_move::AddTwoInts >::value();
  }
  static const char* value(const ::ur3_move::AddTwoIntsResponse&)
  {
    return value();
  }
};

// service_traits::DataType< ::ur3_move::AddTwoIntsResponse> should match
// service_traits::DataType< ::ur3_move::AddTwoInts >
template<>
struct DataType< ::ur3_move::AddTwoIntsResponse>
{
  static const char* value()
  {
    return DataType< ::ur3_move::AddTwoInts >::value();
  }
  static const char* value(const ::ur3_move::AddTwoIntsResponse&)
  {
    return value();
  }
};

} // namespace service_traits
} // namespace ros

#endif // UR3_MOVE_MESSAGE_ADDTWOINTS_H

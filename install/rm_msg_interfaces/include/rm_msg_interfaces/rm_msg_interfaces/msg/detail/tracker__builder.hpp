// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from rm_msg_interfaces:msg/Tracker.idl
// generated code does not contain a copyright notice

#ifndef RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__BUILDER_HPP_
#define RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "rm_msg_interfaces/msg/detail/tracker__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace rm_msg_interfaces
{

namespace msg
{

namespace builder
{

class Init_Tracker_radius
{
public:
  explicit Init_Tracker_radius(::rm_msg_interfaces::msg::Tracker & msg)
  : msg_(msg)
  {}
  ::rm_msg_interfaces::msg::Tracker radius(::rm_msg_interfaces::msg::Tracker::_radius_type arg)
  {
    msg_.radius = std::move(arg);
    return std::move(msg_);
  }

private:
  ::rm_msg_interfaces::msg::Tracker msg_;
};

class Init_Tracker_v_yaw
{
public:
  explicit Init_Tracker_v_yaw(::rm_msg_interfaces::msg::Tracker & msg)
  : msg_(msg)
  {}
  Init_Tracker_radius v_yaw(::rm_msg_interfaces::msg::Tracker::_v_yaw_type arg)
  {
    msg_.v_yaw = std::move(arg);
    return Init_Tracker_radius(msg_);
  }

private:
  ::rm_msg_interfaces::msg::Tracker msg_;
};

class Init_Tracker_yaw
{
public:
  explicit Init_Tracker_yaw(::rm_msg_interfaces::msg::Tracker & msg)
  : msg_(msg)
  {}
  Init_Tracker_v_yaw yaw(::rm_msg_interfaces::msg::Tracker::_yaw_type arg)
  {
    msg_.yaw = std::move(arg);
    return Init_Tracker_v_yaw(msg_);
  }

private:
  ::rm_msg_interfaces::msg::Tracker msg_;
};

class Init_Tracker_velocity
{
public:
  explicit Init_Tracker_velocity(::rm_msg_interfaces::msg::Tracker & msg)
  : msg_(msg)
  {}
  Init_Tracker_yaw velocity(::rm_msg_interfaces::msg::Tracker::_velocity_type arg)
  {
    msg_.velocity = std::move(arg);
    return Init_Tracker_yaw(msg_);
  }

private:
  ::rm_msg_interfaces::msg::Tracker msg_;
};

class Init_Tracker_position
{
public:
  explicit Init_Tracker_position(::rm_msg_interfaces::msg::Tracker & msg)
  : msg_(msg)
  {}
  Init_Tracker_velocity position(::rm_msg_interfaces::msg::Tracker::_position_type arg)
  {
    msg_.position = std::move(arg);
    return Init_Tracker_velocity(msg_);
  }

private:
  ::rm_msg_interfaces::msg::Tracker msg_;
};

class Init_Tracker_id
{
public:
  explicit Init_Tracker_id(::rm_msg_interfaces::msg::Tracker & msg)
  : msg_(msg)
  {}
  Init_Tracker_position id(::rm_msg_interfaces::msg::Tracker::_id_type arg)
  {
    msg_.id = std::move(arg);
    return Init_Tracker_position(msg_);
  }

private:
  ::rm_msg_interfaces::msg::Tracker msg_;
};

class Init_Tracker_trackering
{
public:
  explicit Init_Tracker_trackering(::rm_msg_interfaces::msg::Tracker & msg)
  : msg_(msg)
  {}
  Init_Tracker_id trackering(::rm_msg_interfaces::msg::Tracker::_trackering_type arg)
  {
    msg_.trackering = std::move(arg);
    return Init_Tracker_id(msg_);
  }

private:
  ::rm_msg_interfaces::msg::Tracker msg_;
};

class Init_Tracker_header
{
public:
  Init_Tracker_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Tracker_trackering header(::rm_msg_interfaces::msg::Tracker::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_Tracker_trackering(msg_);
  }

private:
  ::rm_msg_interfaces::msg::Tracker msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::rm_msg_interfaces::msg::Tracker>()
{
  return rm_msg_interfaces::msg::builder::Init_Tracker_header();
}

}  // namespace rm_msg_interfaces

#endif  // RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__BUILDER_HPP_

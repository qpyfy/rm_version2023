// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from rm_msg_interfaces:msg/Tracker.idl
// generated code does not contain a copyright notice

#ifndef RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__TRAITS_HPP_
#define RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "rm_msg_interfaces/msg/detail/tracker__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'position'
#include "geometry_msgs/msg/detail/point__traits.hpp"
// Member 'velocity'
#include "geometry_msgs/msg/detail/vector3__traits.hpp"

namespace rm_msg_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const Tracker & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: trackering
  {
    out << "trackering: ";
    rosidl_generator_traits::value_to_yaml(msg.trackering, out);
    out << ", ";
  }

  // member: id
  {
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << ", ";
  }

  // member: position
  {
    out << "position: ";
    to_flow_style_yaml(msg.position, out);
    out << ", ";
  }

  // member: velocity
  {
    out << "velocity: ";
    to_flow_style_yaml(msg.velocity, out);
    out << ", ";
  }

  // member: yaw
  {
    out << "yaw: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw, out);
    out << ", ";
  }

  // member: v_yaw
  {
    out << "v_yaw: ";
    rosidl_generator_traits::value_to_yaml(msg.v_yaw, out);
    out << ", ";
  }

  // member: radius
  {
    out << "radius: ";
    rosidl_generator_traits::value_to_yaml(msg.radius, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Tracker & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: header
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "header:\n";
    to_block_style_yaml(msg.header, out, indentation + 2);
  }

  // member: trackering
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "trackering: ";
    rosidl_generator_traits::value_to_yaml(msg.trackering, out);
    out << "\n";
  }

  // member: id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "id: ";
    rosidl_generator_traits::value_to_yaml(msg.id, out);
    out << "\n";
  }

  // member: position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "position:\n";
    to_block_style_yaml(msg.position, out, indentation + 2);
  }

  // member: velocity
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "velocity:\n";
    to_block_style_yaml(msg.velocity, out, indentation + 2);
  }

  // member: yaw
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "yaw: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw, out);
    out << "\n";
  }

  // member: v_yaw
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "v_yaw: ";
    rosidl_generator_traits::value_to_yaml(msg.v_yaw, out);
    out << "\n";
  }

  // member: radius
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "radius: ";
    rosidl_generator_traits::value_to_yaml(msg.radius, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Tracker & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace rm_msg_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use rm_msg_interfaces::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const rm_msg_interfaces::msg::Tracker & msg,
  std::ostream & out, size_t indentation = 0)
{
  rm_msg_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use rm_msg_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const rm_msg_interfaces::msg::Tracker & msg)
{
  return rm_msg_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<rm_msg_interfaces::msg::Tracker>()
{
  return "rm_msg_interfaces::msg::Tracker";
}

template<>
inline const char * name<rm_msg_interfaces::msg::Tracker>()
{
  return "rm_msg_interfaces/msg/Tracker";
}

template<>
struct has_fixed_size<rm_msg_interfaces::msg::Tracker>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<rm_msg_interfaces::msg::Tracker>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<rm_msg_interfaces::msg::Tracker>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__TRAITS_HPP_

// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from rm_msg_interfaces:msg/Tracker.idl
// generated code does not contain a copyright notice

#ifndef RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__STRUCT_HPP_
#define RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.hpp"
// Member 'position'
#include "geometry_msgs/msg/detail/point__struct.hpp"
// Member 'velocity'
#include "geometry_msgs/msg/detail/vector3__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__rm_msg_interfaces__msg__Tracker __attribute__((deprecated))
#else
# define DEPRECATED__rm_msg_interfaces__msg__Tracker __declspec(deprecated)
#endif

namespace rm_msg_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Tracker_
{
  using Type = Tracker_<ContainerAllocator>;

  explicit Tracker_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init),
    position(_init),
    velocity(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->trackering = false;
      this->id = "";
      this->yaw = 0.0;
      this->v_yaw = 0.0;
      this->radius = 0.0;
    }
  }

  explicit Tracker_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init),
    id(_alloc),
    position(_alloc, _init),
    velocity(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->trackering = false;
      this->id = "";
      this->yaw = 0.0;
      this->v_yaw = 0.0;
      this->radius = 0.0;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _trackering_type =
    bool;
  _trackering_type trackering;
  using _id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _id_type id;
  using _position_type =
    geometry_msgs::msg::Point_<ContainerAllocator>;
  _position_type position;
  using _velocity_type =
    geometry_msgs::msg::Vector3_<ContainerAllocator>;
  _velocity_type velocity;
  using _yaw_type =
    double;
  _yaw_type yaw;
  using _v_yaw_type =
    double;
  _v_yaw_type v_yaw;
  using _radius_type =
    double;
  _radius_type radius;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__trackering(
    const bool & _arg)
  {
    this->trackering = _arg;
    return *this;
  }
  Type & set__id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->id = _arg;
    return *this;
  }
  Type & set__position(
    const geometry_msgs::msg::Point_<ContainerAllocator> & _arg)
  {
    this->position = _arg;
    return *this;
  }
  Type & set__velocity(
    const geometry_msgs::msg::Vector3_<ContainerAllocator> & _arg)
  {
    this->velocity = _arg;
    return *this;
  }
  Type & set__yaw(
    const double & _arg)
  {
    this->yaw = _arg;
    return *this;
  }
  Type & set__v_yaw(
    const double & _arg)
  {
    this->v_yaw = _arg;
    return *this;
  }
  Type & set__radius(
    const double & _arg)
  {
    this->radius = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    rm_msg_interfaces::msg::Tracker_<ContainerAllocator> *;
  using ConstRawPtr =
    const rm_msg_interfaces::msg::Tracker_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<rm_msg_interfaces::msg::Tracker_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<rm_msg_interfaces::msg::Tracker_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      rm_msg_interfaces::msg::Tracker_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<rm_msg_interfaces::msg::Tracker_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      rm_msg_interfaces::msg::Tracker_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<rm_msg_interfaces::msg::Tracker_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<rm_msg_interfaces::msg::Tracker_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<rm_msg_interfaces::msg::Tracker_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__rm_msg_interfaces__msg__Tracker
    std::shared_ptr<rm_msg_interfaces::msg::Tracker_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__rm_msg_interfaces__msg__Tracker
    std::shared_ptr<rm_msg_interfaces::msg::Tracker_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Tracker_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->trackering != other.trackering) {
      return false;
    }
    if (this->id != other.id) {
      return false;
    }
    if (this->position != other.position) {
      return false;
    }
    if (this->velocity != other.velocity) {
      return false;
    }
    if (this->yaw != other.yaw) {
      return false;
    }
    if (this->v_yaw != other.v_yaw) {
      return false;
    }
    if (this->radius != other.radius) {
      return false;
    }
    return true;
  }
  bool operator!=(const Tracker_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Tracker_

// alias to use template instance with default allocator
using Tracker =
  rm_msg_interfaces::msg::Tracker_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace rm_msg_interfaces

#endif  // RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__STRUCT_HPP_

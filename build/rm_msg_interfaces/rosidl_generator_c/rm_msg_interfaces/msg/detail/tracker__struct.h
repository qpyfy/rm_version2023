// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rm_msg_interfaces:msg/Tracker.idl
// generated code does not contain a copyright notice

#ifndef RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__STRUCT_H_
#define RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'id'
#include "rosidl_runtime_c/string.h"
// Member 'position'
#include "geometry_msgs/msg/detail/point__struct.h"
// Member 'velocity'
#include "geometry_msgs/msg/detail/vector3__struct.h"

/// Struct defined in msg/Tracker in the package rm_msg_interfaces.
typedef struct rm_msg_interfaces__msg__Tracker
{
  std_msgs__msg__Header header;
  bool trackering;
  rosidl_runtime_c__String id;
  geometry_msgs__msg__Point position;
  geometry_msgs__msg__Vector3 velocity;
  double yaw;
  double v_yaw;
  double radius;
} rm_msg_interfaces__msg__Tracker;

// Struct for a sequence of rm_msg_interfaces__msg__Tracker.
typedef struct rm_msg_interfaces__msg__Tracker__Sequence
{
  rm_msg_interfaces__msg__Tracker * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rm_msg_interfaces__msg__Tracker__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // RM_MSG_INTERFACES__MSG__DETAIL__TRACKER__STRUCT_H_

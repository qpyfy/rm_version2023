// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from rm_msg_interfaces:msg/Tracker.idl
// generated code does not contain a copyright notice
#include "rm_msg_interfaces/msg/detail/tracker__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `id`
#include "rosidl_runtime_c/string_functions.h"
// Member `position`
#include "geometry_msgs/msg/detail/point__functions.h"
// Member `velocity`
#include "geometry_msgs/msg/detail/vector3__functions.h"

bool
rm_msg_interfaces__msg__Tracker__init(rm_msg_interfaces__msg__Tracker * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    rm_msg_interfaces__msg__Tracker__fini(msg);
    return false;
  }
  // trackering
  // id
  if (!rosidl_runtime_c__String__init(&msg->id)) {
    rm_msg_interfaces__msg__Tracker__fini(msg);
    return false;
  }
  // position
  if (!geometry_msgs__msg__Point__init(&msg->position)) {
    rm_msg_interfaces__msg__Tracker__fini(msg);
    return false;
  }
  // velocity
  if (!geometry_msgs__msg__Vector3__init(&msg->velocity)) {
    rm_msg_interfaces__msg__Tracker__fini(msg);
    return false;
  }
  // yaw
  // v_yaw
  // radius
  return true;
}

void
rm_msg_interfaces__msg__Tracker__fini(rm_msg_interfaces__msg__Tracker * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // trackering
  // id
  rosidl_runtime_c__String__fini(&msg->id);
  // position
  geometry_msgs__msg__Point__fini(&msg->position);
  // velocity
  geometry_msgs__msg__Vector3__fini(&msg->velocity);
  // yaw
  // v_yaw
  // radius
}

bool
rm_msg_interfaces__msg__Tracker__are_equal(const rm_msg_interfaces__msg__Tracker * lhs, const rm_msg_interfaces__msg__Tracker * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__are_equal(
      &(lhs->header), &(rhs->header)))
  {
    return false;
  }
  // trackering
  if (lhs->trackering != rhs->trackering) {
    return false;
  }
  // id
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->id), &(rhs->id)))
  {
    return false;
  }
  // position
  if (!geometry_msgs__msg__Point__are_equal(
      &(lhs->position), &(rhs->position)))
  {
    return false;
  }
  // velocity
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->velocity), &(rhs->velocity)))
  {
    return false;
  }
  // yaw
  if (lhs->yaw != rhs->yaw) {
    return false;
  }
  // v_yaw
  if (lhs->v_yaw != rhs->v_yaw) {
    return false;
  }
  // radius
  if (lhs->radius != rhs->radius) {
    return false;
  }
  return true;
}

bool
rm_msg_interfaces__msg__Tracker__copy(
  const rm_msg_interfaces__msg__Tracker * input,
  rm_msg_interfaces__msg__Tracker * output)
{
  if (!input || !output) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__copy(
      &(input->header), &(output->header)))
  {
    return false;
  }
  // trackering
  output->trackering = input->trackering;
  // id
  if (!rosidl_runtime_c__String__copy(
      &(input->id), &(output->id)))
  {
    return false;
  }
  // position
  if (!geometry_msgs__msg__Point__copy(
      &(input->position), &(output->position)))
  {
    return false;
  }
  // velocity
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->velocity), &(output->velocity)))
  {
    return false;
  }
  // yaw
  output->yaw = input->yaw;
  // v_yaw
  output->v_yaw = input->v_yaw;
  // radius
  output->radius = input->radius;
  return true;
}

rm_msg_interfaces__msg__Tracker *
rm_msg_interfaces__msg__Tracker__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rm_msg_interfaces__msg__Tracker * msg = (rm_msg_interfaces__msg__Tracker *)allocator.allocate(sizeof(rm_msg_interfaces__msg__Tracker), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(rm_msg_interfaces__msg__Tracker));
  bool success = rm_msg_interfaces__msg__Tracker__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
rm_msg_interfaces__msg__Tracker__destroy(rm_msg_interfaces__msg__Tracker * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    rm_msg_interfaces__msg__Tracker__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
rm_msg_interfaces__msg__Tracker__Sequence__init(rm_msg_interfaces__msg__Tracker__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rm_msg_interfaces__msg__Tracker * data = NULL;

  if (size) {
    data = (rm_msg_interfaces__msg__Tracker *)allocator.zero_allocate(size, sizeof(rm_msg_interfaces__msg__Tracker), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = rm_msg_interfaces__msg__Tracker__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        rm_msg_interfaces__msg__Tracker__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
rm_msg_interfaces__msg__Tracker__Sequence__fini(rm_msg_interfaces__msg__Tracker__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      rm_msg_interfaces__msg__Tracker__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

rm_msg_interfaces__msg__Tracker__Sequence *
rm_msg_interfaces__msg__Tracker__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rm_msg_interfaces__msg__Tracker__Sequence * array = (rm_msg_interfaces__msg__Tracker__Sequence *)allocator.allocate(sizeof(rm_msg_interfaces__msg__Tracker__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = rm_msg_interfaces__msg__Tracker__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
rm_msg_interfaces__msg__Tracker__Sequence__destroy(rm_msg_interfaces__msg__Tracker__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    rm_msg_interfaces__msg__Tracker__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
rm_msg_interfaces__msg__Tracker__Sequence__are_equal(const rm_msg_interfaces__msg__Tracker__Sequence * lhs, const rm_msg_interfaces__msg__Tracker__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!rm_msg_interfaces__msg__Tracker__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
rm_msg_interfaces__msg__Tracker__Sequence__copy(
  const rm_msg_interfaces__msg__Tracker__Sequence * input,
  rm_msg_interfaces__msg__Tracker__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(rm_msg_interfaces__msg__Tracker);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rm_msg_interfaces__msg__Tracker * data =
      (rm_msg_interfaces__msg__Tracker *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!rm_msg_interfaces__msg__Tracker__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          rm_msg_interfaces__msg__Tracker__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!rm_msg_interfaces__msg__Tracker__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}

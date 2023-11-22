cmake_minimum_required(VERSION 3.8)
project(ros2_video)

## Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## By adding -Wall and -Werror, the compiler does not ignore warnings anymore,
## enforcing cleaner code.
#使用 -Wall 和 -Werror 选项，分别表示开启所有警告和将警告视为错误
add_definitions(-Wall -Werror)

## Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake_auto REQUIRED)
find_package(OpenCV REQUIRED)
ament_auto_find_build_dependencies()
# ament_target_dependencies(${PROJECT_NAME}
#   "rclcpp"
#   "sensor_msgs"
#   "image_transport"
#   "camera_info_manager"
#   "opencv2"
# )

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/video_node.cpp
)



target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

#注册一个ros2的组件
rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN ros2_video_node::VideoNode
  EXECUTABLE ${PROJECT_NAME}_node
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  list(APPEND AMENT_LINT_AUTO_EXCLUDE
    ament_cmake_copyright
    ament_cmake_cpplint
    ament_cmake_uncrustify
  )
  ament_lint_auto_find_test_dependencies()
  
  find_package(ament_cmake_gtest)
  ament_add_gtest(video_node src/video_node.cpp)
  target_link_libraries(video_node ${PROJECT_NAME})

endif()

ament_auto_package(
  INSTALL_TO_SHARE
  launch
  config
)

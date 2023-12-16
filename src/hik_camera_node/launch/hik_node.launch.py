import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
     node_params = os.path.join(
          get_package_share_directory('hik_camera_node'), 'config', 'camera_params.yaml')
     node = Node(
          package='hik_camera',
          executable='hik_camera_node',
          parameters=[node_params],
          output='screen',
          emulate_tty=True)


     return LaunchDescription([node])
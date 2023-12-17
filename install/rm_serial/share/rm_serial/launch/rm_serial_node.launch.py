import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
import yaml
from launch.substitutions import Command
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
     serial_node = Node(
          package='rm_serial',
          executable='rm_serial_node',
          output='screen',
          emulate_tty=True)

     return LaunchDescription([serial_node])
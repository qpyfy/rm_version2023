import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
import yaml
from launch.substitutions import Command
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
     node_params = os.path.join(
     get_package_share_directory('ros2_video'), 'config', 'node_params.yaml')
     try:
          node = Node(
          package='ros2_video',
          executable='ros2_video_node',
          parameters=[node_params],
          output='screen',
          emulate_tty=True)
          
          use_sim_time = LaunchConfiguration('use_sim_time', default='true')

          launch_params = yaml.safe_load(open(os.path.join(
               get_package_share_directory('start_directory'), 'config', 'launch_param.yaml')))
          
          robot_description = Command(['xacro ', os.path.join(
          get_package_share_directory('rm_gimbal_description'), 'urdf', 'rm_gimbal.urdf.xacro'),
          ' xyz:=', launch_params['odom2camera']['xyz'], ' rpy:=', launch_params['odom2camera']['rpy']])
   
          robot_state_publisher = Node(
          package='robot_state_publisher',
          executable='robot_state_publisher',
          parameters=[{'robot_description': robot_description,
                 'publish_frequency': 1000.0,
                 'use_sim_time': use_sim_time} ]
          )

          return LaunchDescription([node,robot_state_publisher])
     except:
          return 1    
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import Command
from launch_ros.actions import Node


def generate_launch_description():
    tracker_config = os.path.join(
        get_package_share_directory('rm_tracker'), 'config', 'tracker.yaml')
    rm_tracker_node = Node(
        package='rm_tracker',
        executable='rm_tracker_node',
        output='screen',
        emulate_tty=True,
        parameters=[tracker_config],
    )
    detector_config = os.path.join(
        get_package_share_directory('rm_detector'), 'config', 'detector.yaml')
    rm_detector_node = Node(
        package='rm_detector',
        executable='rm_detector_node',
        output='screen',
        emulate_tty=True,
        parameters=[detector_config],
    )
    video_config = os.path.join(
    get_package_share_directory('ros2_video'), 'config', 'node_params.yaml')
    video_node = Node(
        package='ros2_video',
        executable='ros2_video_node',
        parameters=[video_config],
        output='screen',
        emulate_tty=True,
    )
    launch_params = os.path.join(get_package_share_directory('start_directory'),'config','launch_param.yaml')

    robot_description = Command(['xacro ', os.path.join(
    get_package_share_directory('rm_gimbal_description'), 'urdf', 'rm_gimbal.urdf.xacro'),
    ' xyz:=', launch_params['odom2camera']['xyz'], ' rpy:=', launch_params['odom2camera']['rpy']])

    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': robot_description,
                 'publish_frequency': 1000.0}]
)

    return LaunchDescription([rm_tracker_node,rm_detector_node,video_node,robot_state_publisher])
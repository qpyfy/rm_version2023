import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    config = os.path.join(
        get_package_share_directory('rm_detector'), 'config', 'detector.yaml')
    rm_detector_node = Node(
        package='rm_detector',
        executable='rm_detector_node',
        output='screen',
        emulate_tty=True,
        parameters=[config],
    )

    return LaunchDescription([rm_detector_node])
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
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



    return LaunchDescription([rm_tracker_node])
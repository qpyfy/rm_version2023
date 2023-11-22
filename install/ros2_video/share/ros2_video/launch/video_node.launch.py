import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
node_params = os.path.join(
    get_package_share_directory('ros2_video'), 'config', 'node_params.yaml')

def generate_launch_description():
 try:
    node = Node(
        package='ros2_video',
        executable='ros2_video_node',
        parameters=[node_params],
        output='screen',
        emulate_tty=True,

        )
    return LaunchDescription([node])
 except:
       return 1    
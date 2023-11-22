from setuptools import find_packages
from setuptools import setup

setup(
    name='rm_msg_interfaces',
    version='0.0.0',
    packages=find_packages(
        include=('rm_msg_interfaces', 'rm_msg_interfaces.*')),
)

/*
*	@Author: wentao min,1397257241@qq.com
*	@Date:	 2022.04.12
*/
#ifndef  __USART_H__
#define  __USART_H__
 
//串口相关的头文件
#include     <iostream>
#include     <stdint.h>
#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include     <string.h>
#include     <queue>
#include     <deque>
#include     "rm_msg_interfaces/msg/armor.hpp"
#include     "rm_msg_interfaces/msg/tracker.hpp"
#include     <time.h>
#include     <message_filters/subscriber.h>
#include     <tf2_ros/buffer.h>
#include     <tf2_ros/create_timer_ros.h>
#include     <tf2_ros/message_filter.h>
#include     <tf2_ros/transform_listener.h>

#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <visualization_msgs/msg/marker_array.hpp>


#define FALSE -1
#define TRUE 0
#define SOF_STANDARD 0xa5
#define READ_LENGTH 16


#define dev "/dev/ttyUSB0"
#define Baudrate 115200

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

// /*
//  * @Brief: 控制战车帧结构体{13byte}
//  */
// typedef struct ControlFrame
// {
//     uint8_t  SOF;
//     uint8_t  cmd_id;
//     uint8_t  crc8;
//     float    pitch;
//     float    yaw;
//     uint16_t  crc16;
//     ControlFrame()
//     {
//         SOF = 0xa5;
//         cmd_id = 1;
//     }
// }_controlFrame;

// /*
//  * @Brief: {3byte}
//  */
// typedef struct Frame_Header_t
// {
//     uint8_t  SOF;
//     uint8_t  cmd_id;
//     uint8_t  crc8;
// }_Frame_Header_t;

// /**
//  * @brief {11byte}
//  * 
//  */
// typedef struct
// {
//     uint8_t  fric_speed;
//     uint8_t  robot_color;
//     uint8_t  is_start_aim;

//     float    pitch_angle;
//     float    yaw_angle;
// }AutoAim_Data_Tx_t;

// /*
//  * @Brief: 战车回传数据帧结构体{16byte}
//  */
// typedef struct FeedBackFrame
// {
//     _Frame_Header_t Frame_Header;

//     AutoAim_Data_Tx_t AutoAim_Data;

//     uint16_t crc16;
// }_feedBackFrame;
class Serial_Node:public rclcpp::Node
{
Serial_Node(const rclcpp::NodeOptions & options);
void serial_send(rm_msg_interfaces::msg::Tracker msg);
rclcpp::Subscription<rm_msg_interfaces::msg::Tracker>::SharedPtr tracker_sub_;
int fd;

int UART0_Open(char*port);
void UART0_Close() ;
int UART0_Set(int speed,int flow_ctrl,int databits,int stopbits,int parity);
int UART0_Init(int speed,int flow_ctrl,int databits,int stopbits,int parity) ;
int UART0_Recv(char *rcv_buf,int data_len);
int UART0_Send(char *send_buf,int data_len);

//串口线程(主)函数
void Serial_thread();
void serial_receive(std::queue<unsigned char>& fifo);
void unpack_feedbackframe(std::queue<unsigned char>& fifo);
//char & get_queue_front_deep_copy(std::queue<char>& fifo);

unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);

};
#endif /*__USART_H__*/

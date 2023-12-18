#include "../include/rm_serial_node.hpp"
#include <mutex>
#include <thread>
#include "rm_serial_node.hpp"

/**
 * @brief extern variables
 * 
 */


/**
 * @brief Serial receive and unpack_feedback mutex
 * 
 */
std::mutex fifo_source_mutex;

/**
 * @brief Serial_thread
 * 
 */
Serial_Node::Serial_Node(const rclcpp::NodeOptions & options): Node("serial_node")
{
    RCLCPP_INFO(this->get_logger(), "Initializing serial node");
    tracker_sub_ = this->create_subscription<rm_msg_interfaces::msg::Tracker>("tracker", 10, std::bind(&Serial_Node::serial_send, this, std::placeholders::_1));
    joint_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);
    fd = -1;           
    int err;
    std::queue<unsigned char> fifo;

    fd = open(dev,O_RDWR | O_NOCTTY | O_NDELAY); 

    if(-1 == fd)
    {
        perror("Can't Open Serial Port");
        return;
    }
    do
    {
        err = UART0_Init(Baudrate,0,8,1,'N');
        usleep(100);
    }while(-1 == err );


    std::thread receive_thread(&Serial_Node::serial_receive,this, std::ref(fifo));
    
    std::thread unpack_feedback_thread(&Serial_Node::unpack_feedbackframe,this, std::ref(fifo));

    receive_thread.join();
    unpack_feedback_thread.join();

    UART0_Close();
}


void Serial_Node::serial_send(rm_msg_interfaces::msg::Tracker msg)
{
    // { 13 byte }
    unsigned char send_buff[13] = {};
    unsigned char cmd_id = 0x01;
        if (rclcpp::ok())
        {
            // SOF      { 1 byte }
            send_buff[0] = SOF_STANDARD;
            // cmd_id   { 1 byte }
            send_buff[1] = cmd_id;
            // crc8     { 1 byte }
            *(unsigned char*)(send_buff + 2) = Get_CRC8_Check_Sum((unsigned char*)(send_buff), 2 , 0xff);

            // x    { 4 byte }
            *(float*)(send_buff + 3) = msg.position.x;

             // y    { 4 byte }
            *(float*)(send_buff + 7) = msg.position.y;

             // z    { 4 byte }
            *(float*)(send_buff + 11) = msg.position.z;
            
            // yaw      { 4 byte }
            *(float*)(send_buff + 15) = msg.yaw;

            // RADIUS      { 4 byte }
            *(float*)(send_buff + 19) = msg.radius;

            // crc16    { 2 byte }
            *(unsigned char*)(send_buff + 23) = Get_CRC16_Check_Sum((unsigned char*)(send_buff), 11 , 0xffff);

            UART0_Send((char*)(&send_buff),25);

        std::cout << "send_successfully: ";
        for (int i = 0; i < 13 ; i++)
        {
            //printf("0x%x ", *((unsigned char*)(&send_buff) + i));
            std::cout.setf(std::ios_base::showbase);
            std::cout << std::hex << *((unsigned char*)(send_buff) + i);
        }
        std::cout << std::endl <<"pitch and yaw =" << *(float*)(send_buff + 3) << " and " << *(float*)(send_buff + 7) << std::endl;

    }
    
}

void Serial_Node::serial_receive(std::queue<unsigned char>& fifo)
{
    std::vector<uint8_t> read;
    int len = 0;
    
    while (rclcpp::ok())
    {
        len = UART0_Recv(&read,sizeof(read));
        {
            ReceivePacket packet = fromVector(read);    
            std::unique_lock<std::mutex> push_fifo_lock(fifo_source_mutex);
            for (int i = 0; i < len ; i++)
            {
                fifo.push(read[i]);
            }
            sensor_msgs::msg::JointState joint_state;
            joint_state.header.stamp = this->now();
            joint_state.name.push_back("pitch_joint");
            joint_state.name.push_back("yaw_joint");
            joint_state.position.push_back(packet.pitch);
            joint_state.position.push_back(packet.yaw);      
            joint_pub_->publish(joint_state);
        }
        len = 0;
        // std::cout << "receive_successfully: ";
        // for (int i = 0; i < 16 ; i++)
        // {
        //     printf("0x%x ", *((unsigned char*)(&receive_buff) + i));
        //     //std::cout << std::hex << *((unsigned char*)(&send_buff) + i);
        // }

        usleep(200);
    }   


}
// 解包校验
void Serial_Node::unpack_feedbackframe(std::queue<unsigned char>& fifo)
{
    unsigned char read_buff[16] = {};
    std::deque<char> feedback;
    unsigned char e;

    while (rclcpp::ok())
    {
        while( (!fifo.empty()) && feedback.size() < READ_LENGTH )
        {
            //消息队列资源锁自动管理
            std::unique_lock<std::mutex> unpack_fifo_lock(fifo_source_mutex);

            //以fifo元素依次填充至feed队列
            do
            {
                feedback.push_back(fifo.front());
                fifo.pop();
            }while( !fifo.empty() && feedback.size() < READ_LENGTH);

            
            //delete front data until SOF
            e = feedback.front();
            while ((e != SOF_STANDARD) && (!feedback.empty()) )
            {
                feedback.pop_front();
                e = feedback.front();
            }
        }
    
        if (feedback.size() == READ_LENGTH)
        {
            std::copy(read_buff,read_buff+READ_LENGTH,feedback.begin());

            if(Verify_CRC8_Check_Sum(read_buff,2) == read_buff[2] && Verify_CRC16_Check_Sum(read_buff,14))
            {
                // std::cout << "Read_buff : " << std::endl;
                // std::cout << std::hex << *(float *)(read_buff + 6) << *(float *)(read_buff + 10) << std::endl;

                

                std::cout << "receive_successfully: ";
                for (int i = 0; i < READ_LENGTH; i++)
                {
                    printf("0x%x ", *((unsigned char*)(&read_buff) + i));
                    //std::cout << std::hex << *((unsigned char*)(&send_buff) + i);
                }
            }
            feedback.pop_front();
        }
        else
        {
            usleep(100);
        }
    }

}

/* char & get_queue_front_deep_copy(std::queue<char>& fifo)
{
    char data = fifo.front();

    return data;
}
 */
//functional function
/*******************************************************************
*名称：             UART0_Open
*功能：             打开串口并返回串口设备文件描述
*入口参数：         fd      文件描述符
                    port    串口号(ttyTHS0,ttyTHS1,ttyTHS2)
*出口参数：正确返回为1，错误返回为0
*******************************************************************/
int Serial_Node::UART0_Open(char*port)
{
    fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);
    if (fd<0)
    {
        perror("Can't Open Serial Port");
        return(FALSE);
    }
    //恢复串口为阻塞状态
    if(fcntl(fd, F_SETFL, 0) < 0)
    {
        printf("fcntl failed!\n");
        return(FALSE);
    }
    else
    {
        printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
    }
    //测试是否为终端设备
    if(0 == isatty(STDIN_FILENO))
    {
        printf("standard input is not a terminal device\n");
        return(FALSE);
    }
    else
    {
        printf("isatty success!\n");
    }
    printf("fd->open=%d\n",fd);
    return fd;
}
/*******************************************************************
*名称：             UART0_Close
*功能：             关闭串口并返回串口设备文件描述
*入口参数：         fd          文件描述符
                    port        串口号(ttyS0,ttyS1,ttyS2)
*出口参数：void
*******************************************************************/
void Serial_Node::UART0_Close()
{
    close(fd);
}
/*******************************************************************
*名称：             UART0_Set
*功能：             设置串口数据位，停止位和效验位
*入口参数：         fd          串口文件描述符
*                   speed       串口速度
*                   flow_ctrl   数据流控制
*                   databits    数据位   取值为 7 或者8
*                   stopbits    停止位   取值为 1 或者2
*                   parity      效验类型 取值为N,E,O,,S
*出口参数：正确返回为1，错误返回为0
*******************************************************************/
int Serial_Node::UART0_Set(int speed,int flow_ctrl,int databits,int stopbits,int parity)
{

    int   i;
    int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};
    int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};

    struct termios options;

    /*  tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，
        该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.  */
    if( tcgetattr(fd,&options)  !=  0)
    {
        perror("SetupSerial 1");
        return(FALSE);
    }

    //设置串口输入波特率和输出波特率
    for ( i= 0;  i < int(sizeof(speed_arr) / sizeof(int));  i++)
    {
        if  (speed == name_arr[i])
        {
            cfsetispeed(&options, speed_arr[i]);
            cfsetospeed(&options, speed_arr[i]);
        }
    }

    //修改控制模式，保证程序不会占用串口
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;

    //设置数据流控制
    switch(flow_ctrl)
    {

        case 0 ://不使用流控制
              options.c_cflag &= ~CRTSCTS;
              break;

        case 1 ://使用硬件流控制
              options.c_cflag |= CRTSCTS;
              break;
        case 2 ://使用软件流控制
              options.c_cflag |= IXON | IXOFF | IXANY;
              break;
    }
    //设置数据位
    //屏蔽其他标志位
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
        case 5    :
                     options.c_cflag |= CS5;
                     break;
        case 6    :
                     options.c_cflag |= CS6;
                     break;
        case 7    :
                 options.c_cflag |= CS7;
                 break;
        case 8:
                 options.c_cflag |= CS8;
                 break;
        default:
                 fprintf(stderr,"Unsupported data size\n");
                 return (FALSE);
    }
    //设置校验位
    switch (parity)
    {
        case 'n':
        case 'N': //无奇偶校验位。
                 options.c_cflag &= ~PARENB;
                 options.c_iflag &= ~INPCK;
                 break;
        case 'o':
        case 'O'://设置为奇校验
                 options.c_cflag |= (PARODD | PARENB);
                 options.c_iflag |= INPCK;
                 break;
        case 'e':
        case 'E'://设置为偶校验
                 options.c_cflag |= PARENB;
                 options.c_cflag &= ~PARODD;
                 options.c_iflag |= INPCK;
                 break;
        case 's':
        case 'S': //设置为空格
                 options.c_cflag &= ~PARENB;
                 options.c_cflag &= ~CSTOPB;
                 break;
        default:
                 fprintf(stderr,"Unsupported parity\n");
                 return (FALSE);
    }
    // 设置停止位
    switch (stopbits)
    {
        case 1:
                 options.c_cflag &= ~CSTOPB; break;
        case 2:
                 options.c_cflag |= CSTOPB; break;
        default:
                       fprintf(stderr,"Unsupported stop bits\n");
                       return (FALSE);
    }

    //修改输出模式，原始数据输出
    options.c_oflag &= ~OPOST;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag &= ~(ISIG | ICANON);

    //设置等待时间和最小接收字符
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */

    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读
    tcflush(fd,TCIFLUSH);

    //激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("com set error!\n");
        return (FALSE);
    }
    return (TRUE);
}
/*******************************************************************
*名称：                UART0_Init()
*功能：                串口初始化
*入口参数：            fd         文件描述符
*                      speed      串口速度
*                      flow_ctrl  数据流控制
*                      databits   数据位   取值为 7 或者8
*                      stopbits   停止位   取值为 1 或者2
*                      parity     效验类型 取值为N,E,O,,S
*
*出口参数：正确返回为1，错误返回为0
*******************************************************************/
int Serial_Node::UART0_Init(int speed,int flow_ctrl,int databits,int stopbits,int parity)
{
    //设置串口数据帧格式
    if (UART0_Set(115200,0,8,1,'N') == FALSE)
    {
        return FALSE;
    }
    else
    {
        return  TRUE;
    }
}
/*******************************************************************
* 名称：            UART0_Recv
* 功能：            接收串口数据
* 入口参数：        fd         文件描述符
*                   rcv_buf    接收串口中数据存入rcv_buf缓冲区中
*                   data_len   一帧数据的长度
* 出口参数：        正确返回为1，错误返回为0
*******************************************************************/
int Serial_Node::UART0_Recv(std::vector<uint8_t>* rcv_buf,int data_len)
{
    int len,fs_sel;
    fd_set fs_read;

    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);

    time.tv_sec = 1;
    time.tv_usec = 0;

    //串口的多路通信
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
    printf("fs_sel = %d\n",fs_sel);
    if(fs_sel)
    {
        len = read(fd,rcv_buf,data_len);
        return len;
    }
    else
    {
        return FALSE;
    }
}
/********************************************************************
* 名称：            UART0_Send
* 功能：            发送数据
* 入口参数：        fd           文件描述符
*                   send_buf     存放串口发送数据
*                   data_len     一帧数据的个数
* 出口参数：        正确返回为1，错误返回为0
*******************************************************************/
int Serial_Node::UART0_Send(char *send_buf,int data_len)
{
    int len = 0;

    len = write(fd,send_buf,data_len);
    if (len == data_len )
    {
        printf("send data is %s\n",send_buf);
        return len;
    }
    else
    {

        tcflush(fd,TCOFLUSH);
        return FALSE;
    }

}

//crc8 generator polynomial:G(x)=x8+x5+x4+1
const unsigned char CRC8_INIT = 0xff;
const unsigned char CRC8_TAB[256] =
{
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};
unsigned char Serial_Node::Get_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8)
{
	unsigned char ucIndex;

	while (dwLength--)
	{
		ucIndex = ucCRC8 ^ (*pchMessage++);
		ucCRC8 = CRC8_TAB[ucIndex];
	}

	return (ucCRC8);
}
/*
** Descriptions: CRC8 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
unsigned int Serial_Node::Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
	unsigned char ucExpected = 0;

	if ((pchMessage == 0) || (dwLength <= 2)) return 0;

	ucExpected = Get_CRC8_Check_Sum(pchMessage, dwLength - 1, CRC8_INIT);
	return (ucExpected == pchMessage[dwLength - 1]);
}
/*
** Descriptions: append CRC8 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
void Serial_Node::Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
	unsigned char ucCRC = 0;

	if ((pchMessage == 0) || (dwLength <= 2)) return;

	ucCRC = Get_CRC8_Check_Sum((unsigned char *)pchMessage, dwLength - 1, CRC8_INIT);
	pchMessage[dwLength - 1] = ucCRC;
}
uint16_t CRC_INIT = 0xffff;
const uint16_t wCRC_Table[256] =
{
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
/*
** Descriptions: CRC16 checksum function
** Input: Data to check,Stream length, initialized checksum
** Output: CRC checksum
*/
uint16_t Serial_Node::Get_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC)
{
	uint8_t chData;

	if (pchMessage == NULL)
	{
		return 0xFFFF;
	}

	while (dwLength--)
	{
		chData = *pchMessage++;
		(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];
	}

	return wCRC;
}
/*
** Descriptions: CRC16 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
uint32_t Serial_Node::Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
	uint16_t wExpected = 0;

	if ((pchMessage == NULL) || (dwLength <= 2))
	{
		return 0;
	}

	wExpected = Get_CRC16_Check_Sum(pchMessage, dwLength - 2, CRC_INIT);
	return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]);
}
/*
** Descriptions: append CRC16 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
void Serial_Node::Append_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
	uint16_t wCRC = 0;

	if ((pchMessage == NULL) || (dwLength <= 2))
	{
		return;
	}

	wCRC = Get_CRC16_Check_Sum((uint8_t *)pchMessage, dwLength - 2, CRC_INIT);
	pchMessage[dwLength - 2] = (uint8_t)(wCRC & 0x00ff);
	pchMessage[dwLength - 1] = (uint8_t)((wCRC >> 8) & 0x00ff);

}
inline Serial_Node::ReceivePacket Serial_Node::fromVector(const std::vector<uint8_t> & data)
{
  ReceivePacket packet;
  std::copy(data.begin(), data.end(), reinterpret_cast<uint8_t *>(&packet));
  return packet;
}

inline std::vector<uint8_t> Serial_Node::toVector(const SendPacket & data)
{
  std::vector<uint8_t> packet(sizeof(SendPacket));
  std::copy(
    reinterpret_cast<const uint8_t *>(&data),
    reinterpret_cast<const uint8_t *>(&data) + sizeof(SendPacket), packet.begin());
  return packet;
}

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(Serial_Node)

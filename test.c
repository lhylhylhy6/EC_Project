
#define DBG_TAG "nmea"
#define DBG_LVL DBG_LOG
#include "gps.h"



/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};

static rt_device_t serial;
static struct rt_messagequeue rx_mq;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if (result == -RT_EFULL)
    {
        /* 消息队列满 */
    //    rt_kprintf("message queue full！\n");
    }
    return result;
}

//#define __GPS_DEBUG
static double char_to_double(char* str)
{
    double ret = 0;
    int i =0;
    for(;str[i] != '.';i++)
    {
        ret *=10;
        ret += str[i] - '0';
    }
    i++;

    int len = 1;
    double decimals = 0;
    for(;str[i] != '\0';i++)
    {
        len = len *10;
        decimals *=10;
        decimals += str[i] - '0';
    }
    decimals = decimals / len;
    ret += decimals;
    return ret;
}

static void parse_jw(char *str ,gpsINFO* info)
{
    int i = 0;
    char ss[RT_SERIAL_RB_BUFSZ + 1];
    int ss_index = 0;
    while(str[i++] != ',');
    //解析纬度
    for(;str[i] != ',';i++)
    {
        ss[ss_index++] = str[i];
    }
    i++;
    ss[ss_index] = '\0';
    info->lat = char_to_double(ss);
    while(str[i++] != ',');
    //解析纬度

    rt_memset(ss, 0, sizeof (ss));
    ss_index = 0;
    for(;str[i] != ',';i++)
    {
        ss[ss_index++] = str[i];
    }
    i++;
    ss[ss_index] = '\0';
    info->lon = char_to_double(ss);
}


static void nmea_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
    static char rx_buffer[RT_SERIAL_RB_BUFSZ + 1];

    char ss[128];   //打印字符串buffer

//    double deg_lat; //转换成[degree].[degree]格式的纬度
//    double deg_lon; //转换成[degree].[degree]格式的经度

    gpsINFO info = {0,0};          //GPS解码后得到的信息



     char gpsstr_buffer[RT_SERIAL_RB_BUFSZ + 1];
    int gpsstr_buffer_index =0;
    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);


            //rt_kprintf("%s",rx_buffer);
            rx_buffer[rx_length] = '\0';
            //rt_kprintf("%s",rx_buffer);
            int flag = 0;
            for(int i=0;i!= rx_length;i++)
            {
                if(rx_buffer[i] == '\n')
                    {flag =1;break;}
                gpsstr_buffer[gpsstr_buffer_index++] = rx_buffer[i];
            }

            if(!flag ) continue;//正确数据帧格式


            gpsstr_buffer[gpsstr_buffer_index] = '\0';
            if(rt_strstr(gpsstr_buffer, "$GNGLL") != NULL)
            {
                parse_jw(gpsstr_buffer,&info);
            } else{
                gpsstr_buffer_index = 0;
                rt_memset(&gpsstr_buffer, 0, sizeof(gpsstr_buffer));
                continue;
            }
            gpsstr_buffer_index = 0;
            rt_memset(&gpsstr_buffer, 0, sizeof(gpsstr_buffer));

            snprintf(ss, 128, "wd:%f,jd:%f", info.lat/100, info.lon/100);
            mqtt_up_GPS(ss) ;//mqtt上报onenet经纬度信息
   //        rt_kprintf("%s \n",ss);
//            snprintf(ss, 128, "high:%f m", info.elv);
//            rt_kprintf("%s \n",ss);
//            snprintf(ss, 128, "v:%f km/h", info.speed);
//            rt_kprintf("%s \n",ss);
//            snprintf(ss, 128, "hangxiang:%f du", info.direction);
//            rt_kprintf("%s \n",ss);
 //           snprintf(ss, 128, "used GPS:%d,show GPS:%d", info.satinfo.inuse, info.satinfo.inview);
  //          rt_kprintf("%s \n",ss);
//            snprintf(ss, 128, "PDOP:%f,HDOP:%f,VDOP:%f", info.PDOP, info.HDOP, info.VDOP);
//            rt_kprintf("%s \n",ss);
        }
    }
}
//参数为nmea库想要解析的串口名称
int nmea_thread_init(char* argv)
{
    rt_err_t ret = RT_EOK;
    static char msg_pool[256];
  //  static char up_msg_pool[256];
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    /* 查找串口设备 */
    serial = rt_device_find(argv);
    /* 初始化消息队列 */
    rt_mq_init(&rx_mq, "rx_mq", msg_pool, sizeof(struct rx_msg), sizeof(msg_pool), RT_IPC_FLAG_FIFO);
    //修改波特率为9600
    config.baud_rate = NMEALIB_UART_BAUDRATE;
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

    rt_device_open(serial,  RT_DEVICE_FLAG_INT_RX); /* 以 DMA 接收及轮询发送方式打开串口设备 */
    rt_device_set_rx_indicate(serial, uart_input); /* 设置接收回调函数 */

    rt_thread_t thread = rt_thread_create("nmea", nmea_thread_entry, RT_NULL, 4096, 25, 10); /* 创建 serial 线程 */

    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}

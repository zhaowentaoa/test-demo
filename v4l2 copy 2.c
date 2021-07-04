#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <sys/time.h>



#define TRUE            (1)
#define FALSE           (0)

#define FILE_VIDEO      "/dev/video0"
#define IMAGE           "./img/demo"

#define IMAGEWIDTH      640
#define IMAGEHEIGHT     480

#define FRAME_NUM       4

int fd;
struct v4l2_buffer buf;

struct buffer
{
    void * start;
    unsigned int length;
    long long int timestamp;
} *buffers;


int v4l2_init()
{
    struct v4l2_capability cap;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_format fmt;
    struct v4l2_streamparm stream_para;

    //打开摄像头设备
    if ((fd = open(FILE_VIDEO, O_RDWR)) == -1) 
    {
        printf("Error opening V4L interface\n");
        return FALSE;
    }

    //查询设备属性
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) 
    {
        printf("Error opening device %s: unable to query device.\n",FILE_VIDEO);
        return FALSE;
    }
    else
    {
        printf("driver:\t\t%s\n",cap.driver);
        printf("card:\t\t%s\n",cap.card);
        printf("bus_info:\t%s\n",cap.bus_info);
        printf("version:\t%d\n",cap.version);
        printf("capabilities:\t%x\n",cap.capabilities);
        
        if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) 
        {
            printf("Device %s: supports capture.\n",FILE_VIDEO);
        }

        if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) 
        {
            printf("Device %s: supports streaming.\n",FILE_VIDEO);
        }
    }


    //显示所有支持帧格式
    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("Support format:\n");
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
    {
        printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
        fmtdesc.index++;
    }

    //检查是否支持某帧格式
    struct v4l2_format fmt_test;
    fmt_test.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt_test.fmt.pix.pixelformat=V4L2_PIX_FMT_RGB32;
    if(ioctl(fd,VIDIOC_TRY_FMT,&fmt_test)==-1)
    {
        printf("not support format RGB32!\n");      
    }
    else
    {
        printf("support format RGB32\n");
    }


    //查看及设置当前格式
    printf("set fmt...\n");
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32; //jpg格式
    //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//yuv格式

    fmt.fmt.pix.height = IMAGEHEIGHT;
    fmt.fmt.pix.width = IMAGEWIDTH;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    printf("fmt.type:\t\t%d\n",fmt.type);
    printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
    printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
    printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
    printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);
    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
    {
        printf("Unable to set format\n");
        return FALSE;
    }

    printf("get fmt...\n"); 
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
    {
        printf("Unable to get format\n");
        return FALSE;
    }
    {
        printf("fmt.type:\t\t%d\n",fmt.type);
        printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
        printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
        printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
        printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);
    }

    //设置及查看帧速率，这里只能是30帧，就是1秒采集30张图
    memset(&stream_para, 0, sizeof(struct v4l2_streamparm));
    stream_para.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    stream_para.parm.capture.timeperframe.denominator = 30;
    stream_para.parm.capture.timeperframe.numerator = 1;

    if(ioctl(fd, VIDIOC_S_PARM, &stream_para) == -1)
    {
        printf("Unable to set frame rate\n");
        return FALSE;
    }
    if(ioctl(fd, VIDIOC_G_PARM, &stream_para) == -1)
    {
        printf("Unable to get frame rate\n");
        return FALSE;       
    }
    {
        printf("numerator:%d\ndenominator:%d\n",stream_para.parm.capture.timeperframe.numerator,stream_para.parm.capture.timeperframe.denominator);
    }
    return TRUE;
    printf("\n");
}










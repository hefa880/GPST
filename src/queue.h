#ifndef _QUEUE_H_
#define	_QUEUE_H_



#define	Q_NO_DATA				0		//队列中无数据
#define	Q_HAVE_DATA				1		//队列中有数据

#define	Q_OPT_SUCCEED			0		//队列操作成功
#define	Q_OPT_FAILED			1		//队列操作失败

//以下定义队列控制结构体，在队列应用中，定义一个STRUCT_QUEUE_CTRL Q，再定义一个队列BUF，同时对要对BUF及Q初始化
#pragma pack(1)
typedef struct
{
#if 0
    volatile u16   InCnt;	//入队计数器
    volatile u16   OutCnt;//出队计数器
    volatile u16   size;	//队列大小
    volatile u8	state;	//队列状态		1:队列中有数据   0:队列中无数据
#else

    u16   InCnt;	//入队计数器
    u16   OutCnt;//出队计数器
    u16   size;	//队列大小
    u8	state;	//队列状态		1:队列中有数据   0:队列中无数据
#endif
} STRUCT_QUEUE_CTRL;		//队列控制结构体
#pragma pack()

u8 QueuePost(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 data);
u8 QueuePost_OutInt(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 data);
u8 QueueAccept(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 *pErr);
u8 QueueAccept_OutInt(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 *pErr);
void QueueFlush(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf);
void QueueFlush_OutInt(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf);

void QueueInit(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u16 Q_len);
u16  QueueReadLen(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf);

#endif

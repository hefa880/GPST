#ifndef _QUEUE_H_
#define	_QUEUE_H_



#define	Q_NO_DATA				0		//������������
#define	Q_HAVE_DATA				1		//������������

#define	Q_OPT_SUCCEED			0		//���в����ɹ�
#define	Q_OPT_FAILED			1		//���в���ʧ��

//���¶�����п��ƽṹ�壬�ڶ���Ӧ���У�����һ��STRUCT_QUEUE_CTRL Q���ٶ���һ������BUF��ͬʱ��Ҫ��BUF��Q��ʼ��
#pragma pack(1)
typedef struct
{
#if 0
    volatile u16   InCnt;	//��Ӽ�����
    volatile u16   OutCnt;//���Ӽ�����
    volatile u16   size;	//���д�С
    volatile u8	state;	//����״̬		1:������������   0:������������
#else

    u16   InCnt;	//��Ӽ�����
    u16   OutCnt;//���Ӽ�����
    u16   size;	//���д�С
    u8	state;	//����״̬		1:������������   0:������������
#endif
} STRUCT_QUEUE_CTRL;		//���п��ƽṹ��
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

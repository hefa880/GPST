#include "includes.h"

/*********************************************************************************************************
** 函数名称: QueueCtrlInit
** 功能描述: Queue初始化
** 输　入:QueueCtrl:队列控制结构体变量,Q_len 队列缓存长度,pQueueBuf队列缓存指针
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
void QueueInit(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u16 Q_len)
{
  u16 i;
  QueueCtrl->InCnt=0;
  QueueCtrl->OutCnt=0;
  QueueCtrl->size=Q_len;
  QueueCtrl->state=Q_NO_DATA;
  for(i=0; i<QueueCtrl->size; i++)
  {
    pQueueBuf[i]=0;//初始化数据为0
  }
}

/*********************************************************************************************************
** 函数名称: QueuePost
** 功能描述: 入队操作
** 输　入:QueueCtrl:队列控制结构体变量,pQueueBuf队列缓存指针,data 入队数据
** 输　出:Q_OPT_SUCCEED:入队成功  Q_OPT_FAILED入队失败
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
u8 QueuePost(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 data)
{
  u8 ret;//,cpu_sr;
  // OS_ENTER_CRITICAL();
  //if((QueueCtrl->size) > (QueueCtrl->InCnt))		//没超缓冲区大小
  if ((Q_NO_DATA==QueueCtrl->state) || (QueueCtrl->InCnt!=QueueCtrl->OutCnt))
  {
    pQueueBuf[QueueCtrl->InCnt++] = data;	//入队
    (QueueCtrl->InCnt) %= QueueCtrl->size;		//入队计数器超最大值后，从0开始
    QueueCtrl->state=Q_HAVE_DATA;			//队列非空标志
    ret=Q_OPT_SUCCEED;						//操作成功
  }
  else
  {
    ret=Q_OPT_FAILED;						//操作失败,说明队列满都还没取走
  }
  //  OS_EXIT_CRITICAL();
  return ret;
  
}

u8 QueuePost_OutInt(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 data)
{
    u8 ret;

    INT_Disable();
    ret = QueuePost( QueueCtrl, pQueueBuf, data );
    INT_Enable();

    return ret;
}

/*********************************************************************************************************
** 函数名称: QueueAccept
** 功能描述: 入队操作
** 输　入:QueueCtrl:队列控制结构体变量,pQueueBuf队列缓存指针,pErr 错误状态指针
** 输　出:*pErr:  Q_OPT_FAILED:队列读取失败     Q_OPT_SUCCEED:队列读取成功  ret:成功操作后，返回数据
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
u8 QueueAccept(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 *pErr)
{
  u8 ret=0;//,cpu_sr;
  //  OS_ENTER_CRITICAL();
  if(Q_NO_DATA== QueueCtrl->state)				//队列无数据
  {
    *pErr=Q_OPT_FAILED;						//队列读取失败
  }
  else
  {
    ret = pQueueBuf[QueueCtrl->OutCnt++];	//读数据
    (QueueCtrl->OutCnt) %= QueueCtrl->size;	//循环
    if((QueueCtrl->OutCnt) == (QueueCtrl->InCnt)) //当取出数==放入数时，说明数据取出完了
    {
      QueueCtrl->state = Q_NO_DATA;		//队列状态改为无数据状态
    }
    *pErr=Q_OPT_SUCCEED;					//队列读取操作成功
  }
  
  //  OS_EXIT_CRITICAL();
  return ret;
}

u8 QueueAccept_OutInt(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 *pErr)
{
    u8 ret;

    INT_Disable();
    ret = QueueAccept( QueueCtrl, pQueueBuf, pErr );
    INT_Enable();

    return ret;
}

/*********************************************************************************************************
** 函数名称: QueueFlush
** 功能描述: 队列清空
** 输　入:QueueCtrl:队列控制结构体变量,pQueueBuf队列缓存指针
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
void QueueFlush(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf)
{
  u16 i;
  //u8 cpu_sr;
  //OS_ENTER_CRITICAL();
  QueueCtrl->InCnt=0;
  QueueCtrl->OutCnt=0;
  QueueCtrl->state=Q_NO_DATA;
  for(i=0; i<QueueCtrl->size; i++)
  {
    pQueueBuf[i]=0;
  }
  //OS_EXIT_CRITICAL();
  
}

void QueueFlush_OutInt(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf)
{
    INT_Disable();
    QueueFlush( QueueCtrl, pQueueBuf );
    INT_Enable();
}

/*********************************************************************************************************
** 函数名称: QueueFlush
** 功能描述: 读队列中数据的个数
** 输　入:QueueCtrl:队列控制结构体变量,pQueueBuf队列缓存指针
** 输　出:
** 全局变量:
** 调用模块:
** 作　者:
** 日　期:
********************************************************************************************************/
u16  QueueReadLen(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf)
{
  u16 i;
  //  ENTER_CRITICAL();
  i=QueueCtrl->InCnt;
  return i;
  // EXIT_CRITICAL();
  
}


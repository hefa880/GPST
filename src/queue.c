#include "includes.h"

/*********************************************************************************************************
** ��������: QueueCtrlInit
** ��������: Queue��ʼ��
** �䡡��:QueueCtrl:���п��ƽṹ�����,Q_len ���л��泤��,pQueueBuf���л���ָ��
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
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
    pQueueBuf[i]=0;//��ʼ������Ϊ0
  }
}

/*********************************************************************************************************
** ��������: QueuePost
** ��������: ��Ӳ���
** �䡡��:QueueCtrl:���п��ƽṹ�����,pQueueBuf���л���ָ��,data �������
** �䡡��:Q_OPT_SUCCEED:��ӳɹ�  Q_OPT_FAILED���ʧ��
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
********************************************************************************************************/
u8 QueuePost(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 data)
{
  u8 ret;//,cpu_sr;
  // OS_ENTER_CRITICAL();
  //if((QueueCtrl->size) > (QueueCtrl->InCnt))		//û����������С
  if ((Q_NO_DATA==QueueCtrl->state) || (QueueCtrl->InCnt!=QueueCtrl->OutCnt))
  {
    pQueueBuf[QueueCtrl->InCnt++] = data;	//���
    (QueueCtrl->InCnt) %= QueueCtrl->size;		//��Ӽ����������ֵ�󣬴�0��ʼ
    QueueCtrl->state=Q_HAVE_DATA;			//���зǿձ�־
    ret=Q_OPT_SUCCEED;						//�����ɹ�
  }
  else
  {
    ret=Q_OPT_FAILED;						//����ʧ��,˵������������ûȡ��
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
** ��������: QueueAccept
** ��������: ��Ӳ���
** �䡡��:QueueCtrl:���п��ƽṹ�����,pQueueBuf���л���ָ��,pErr ����״ָ̬��
** �䡡��:*pErr:  Q_OPT_FAILED:���ж�ȡʧ��     Q_OPT_SUCCEED:���ж�ȡ�ɹ�  ret:�ɹ������󣬷�������
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
********************************************************************************************************/
u8 QueueAccept(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf,u8 *pErr)
{
  u8 ret=0;//,cpu_sr;
  //  OS_ENTER_CRITICAL();
  if(Q_NO_DATA== QueueCtrl->state)				//����������
  {
    *pErr=Q_OPT_FAILED;						//���ж�ȡʧ��
  }
  else
  {
    ret = pQueueBuf[QueueCtrl->OutCnt++];	//������
    (QueueCtrl->OutCnt) %= QueueCtrl->size;	//ѭ��
    if((QueueCtrl->OutCnt) == (QueueCtrl->InCnt)) //��ȡ����==������ʱ��˵������ȡ������
    {
      QueueCtrl->state = Q_NO_DATA;		//����״̬��Ϊ������״̬
    }
    *pErr=Q_OPT_SUCCEED;					//���ж�ȡ�����ɹ�
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
** ��������: QueueFlush
** ��������: �������
** �䡡��:QueueCtrl:���п��ƽṹ�����,pQueueBuf���л���ָ��
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
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
** ��������: QueueFlush
** ��������: �����������ݵĸ���
** �䡡��:QueueCtrl:���п��ƽṹ�����,pQueueBuf���л���ָ��
** �䡡��:
** ȫ�ֱ���:
** ����ģ��:
** ������:
** �ա���:
********************************************************************************************************/
u16  QueueReadLen(STRUCT_QUEUE_CTRL *QueueCtrl,u8 *pQueueBuf)
{
  u16 i;
  //  ENTER_CRITICAL();
  i=QueueCtrl->InCnt;
  return i;
  // EXIT_CRITICAL();
  
}


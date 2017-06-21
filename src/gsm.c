#include"includes.h"

STRUCT_QUEUE_CTRL GsmRxQueueCtrl;
volatile STRUCT_ATCOMMAND  STU_AtCommand;
/*
*********************************************************************************************************
*	�� �� ��:u8  SendAtCommand(u8 *AtCommend ,u16 len,u8* atflag,u8 times,u8 waittime)
*	����˵��:500msɨ��һ��
*                  һ��atָ��ͽ���ʱ��ſ��Է�����һ��
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
*********************************************************************************************************
*/

u8  SendAtCommand(u8 *AtCommend ,u16 len,u8* atflag,u8 times,u8 waittime,void(*GsmSendDate)(u8*sendstr,u16 len))
{

    if(STU_AtCommand.waittime) STU_AtCommand.waittime--;
    switch(STU_AtCommand.AtStu)
    {
    case 0:/*start send*/
        STU_AtCommand.times=times;
        STU_AtCommand.AtState=AT_STATE_BUSY;

        STU_AtCommand.findstrlen=sizeof(atflag)-1;
        Mymemcpy((u8*)STU_AtCommand.findstr,atflag,sizeof(atflag)-1);
        STU_AtCommand.findstrstu=0;
        STU_AtCommand.AtStu++;
        return AT_WAIT;
    case 1:
        if(STU_AtCommand.times)
        {
            STU_AtCommand.waittime=10*(u16)waittime;
            GsmSendDate(AtCommend,len);
            STU_AtCommand.times--;
            STU_AtCommand.AtStu++;
            return AT_WAIT;
        }
        else /*end send*/
        {
            STU_AtCommand.AtStu=0;
            STU_AtCommand.AtState=AT_STATE_IDLE;
            return AT_ERROR;
        }
    case 2:
        if(STU_AtCommand.AtState==AT_STATE_IDLE)/*�յ�Ӧ�𷵻�ok*/
        {
            STU_AtCommand.AtStu=0;
            return AT_OK;
        }
        else  if( STU_AtCommand.waittime==0)/*��ʱ�ط�*/
        {
            STU_AtCommand.AtStu=1;
        }
        return AT_WAIT;
    default:
        STU_AtCommand.AtStu=0;
        return AT_ERROR;


    }



}

/*
*********************************************************************************************************
*	�� �� ��:void InitGsmQueue(void)
*	����˵��: GSM�������ݶ��г�ʼ��
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void InitGsmQueue(void)
{
    STU_AtCommand.AtStu=0;
    QueueInit(&GsmRxQueueCtrl,(u8 *)STU_AtCommand.rev,MaxRevLen);
    //���г�ʼ��
}


/*
*********************************************************************************************************
*	�� �� ��:void RevGsmQueue(u8 indate)
*	����˵��: GSM�������ݽ���
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RevGsmQueue(u8 indate)
{
    u8 err;
    err=QueuePost(&GsmRxQueueCtrl,(u8 *)STU_AtCommand.rev,indate);

    if(err!=Q_OPT_SUCCEED)
    {
        QueueFlush(&GsmRxQueueCtrl,(u8 *)STU_AtCommand.rev);
    }
}

/*
*********************************************************************************************************
*	�� �� ��:void LookForGsmAtString(u8 date)
*	����˵��: GSM�����в���at�ַ���
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LookForGsmAtString(u8 date)
{
    static u8 strmaxlen;
    //�����STU_AtCommand.AtState=AT_STATE_BUSY �ж����ҵ���Ҫ�ҵ��ַ���STU_AtCommand.AtState=AT_STATE_IDLE

    if(STU_AtCommand.AtState!=AT_STATE_BUSY)
    {
        return;
    }

    switch( STU_AtCommand.findstrstu)//
    {
    case 0:
        if(date==STU_AtCommand.findstr[0])
        {
            STU_AtCommand.findstrstu++;
            strmaxlen=GetArryLen((u8*)STU_AtCommand.findstr,FIND_STR_MAX_LEN);
            STU_AtCommand.findstrlen=strmaxlen;
            STU_AtCommand.findstrlen--;
            if(strmaxlen==1)
            {
                STU_AtCommand.findstrstu=0;
                STU_AtCommand.AtState=AT_STATE_IDLE;
                break;
            }
        }
        break;
    case 1:
        if(date!=STU_AtCommand.findstr[strmaxlen-STU_AtCommand.findstrlen])
        {
            STU_AtCommand.findstrstu=0;
            break;
        }
        STU_AtCommand.findstrlen--;
        if(0==STU_AtCommand.findstrlen)
        {
            STU_AtCommand.findstrstu=0;
            STU_AtCommand.AtState=AT_STATE_IDLE;
            break;
        }
        break;
    default:
        STU_AtCommand.findstrstu=0;
        break;


    }




}
/*
*********************************************************************************************************
*	�� �� ��u8  ProcessGsmQueue(void(*revnetfunction)(u8 indate))
*	����˵��: GSM�������ݽ���
*
*       ��    �� ��liupeng
*	��    �Σ�
*       ��    ����version 1.0
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//#pragma optimize=none
u8  ProcessGsmQueue(void(*revnetfunction)(u8 indate))
{
    u8 data,err;
    do
    {
        data=QueueAccept_OutInt(&GsmRxQueueCtrl,(u8 *)STU_AtCommand.rev,&err);
        if(err==Q_OPT_SUCCEED)
        {
            if(debug==DEBUGGSM)
                PrintUsart(data);
            LookForGsmAtString(data);
            GsmGetStatues(data,revnetfunction);
            if( GsmRecDataStu.flag==GetAFram)
            {
                ProcessNetData();
            }
        }
    }
    while(err==Q_OPT_SUCCEED);

    return err;
}




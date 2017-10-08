#include"includes.h"
#include "ue866_gpio.h"
#include "ue866_operate.h"


typedef enum
{
    UE866_RESULT_START,
    UE866_RESULT_OK = 1,
    UE866_RESULT_WAIT,
    UE866_RESULT_ERR,
    UE866_RESULT_TIME_OUT,
    UE866_RESULT_IDLE,
    UE866_RESULT_MAX
} UE866_RESULT;

typedef enum
{
    UE866_ATCMD_ID_START = 0,   ///  unuse
    UE866_ATCMD_ID_AT = 1,
    UE866_ATCMD_ID_ATE,
    UE866_ATCMD_ID_COPS,
    UE866_ATCMD_ID_CGMR,
    UE866_ATCMD_ID_CGMM,
    UE866_ATCMD_ID_CACHEDNS,
    UE866_ATCMD_ID_CGSN,
    UE866_ATCMD_ID_CFLO,
    UE866_ATCMD_ID_K,
    UE866_ATCMD_ID_QSS,
    UE866_ATCMD_ID_CLIP,
    UE866_ATCMD_ID_CMGF,
    UE866_ATCMD_ID_MONI,
    UE866_ATCMD_ID_MSCLASS,
    UE866_ATCMD_ID_SCFG,
    UE866_ATCMD_ID_SCFGEXT,
    UE866_ATCMD_ID_CGDCONT,  //  查询与配置当前APN


    UE866_ATCMD_ID_CFG_END,  //////////  ///  unuse


    UE866_ATCMD_ID_CSQ,
    UE866_ATCMD_ID_CREG,
    UE866_ATCMD_ID_CGREG,
    UE866_ATCMD_ID_CGATT,
    UE866_ATCMD_ID_SGACT,  // ActiveGprs.  return  IP #SGACT:10.23.109.115   returns ERROR if there is not any socket associated to it (see AT#SCFG).

    UE866_ATCMD_ID_SERVINFO, // 获取当前基站服务信息


    //   UE866_ATCMD_ID_CNMI,
    //   UE866_ATCMD_ID_CMGL,
    //   UE866_ATCMD_ID_CMGD,

    UE866_ATCMD_ID_SD,///  send buf
    UE866_ATCMD_ID_SI,
    UE866_ATCMD_ID_SS,
    UE866_ATCMD_ID_AGPSSND,

    UE866_ATCMD_ID_SSENDEXT,
    UE866_ATCMD_ID_SRECV,
    //UE866_ATCMD_ID_ATA,
    // UE866_ATCMD_ID_ATD,
    //    UE866_ATCMD_ID_CLCC,

    UE866_ATCMD_ID_CFG_POWER,       ///   ///  unuse
    UE866_ATCMD_ID_CFUN_FIVE,
    UE866_ATCMD_ID_ATH,
    UE866_ATCMD_ID_SH,
    UE866_ATCMD_ID_SYSHAL,
    //   UE866_ATCMD_ID_SHDN,
    //  UE866_ATCMD_ID_CFUN_FIVE,

    UE866_ATCMD_ID_ATZ0,
    UE866_ATCMD_ID_MAX,

    UE866_ATCMD_ID_NORMAL_END,
    UE866_ATCMD_ID_LOOP_END,
    UE866_ATCMD_ID_ERR_RETRY_END,
    UE866_ATCMD_ID_MAX_2

} UE866_ATCMD_ID_T;

typedef enum
{
    UE866_ACTION_START,
    UE866_ACTION_GET = 1,
    UE866_ACTION_SET,
    UE866_ACTION_SEND,
    UE866_ACTION_REV,
    UE866_ACTION_HOLD,
    UE866_ACTION_MAX
} UE866_ACTION;

typedef enum
{
    UE866_MODE_START,
    UE866_MODE_CFG = 1,
    UE866_MODE_NORMAL,
    UE866_MODE_LOOP,
    UE866_MODE_ERR_RETRY,
    UE866_MODE_SLEEP,
    UE866_MODE_MAX
} UE866_MODE;



typedef   UE866_RESULT (*ue866_func)(void *agrv );

typedef  __packed struct
{
    UE866_ATCMD_ID_T  cmd_id;
    u8 *at_cmd;
    //  u8 try_times;
    //   u8 time_out_sec;
    ue866_func pat_func;
} ue866_operate_at_cmd_t;

typedef  __packed struct
{
    UE866_MODE                                     ue866_mode;
    u8                                                          index;
    UE866_ATCMD_ID_T                          cmd_id;
    //    UE866_ATCMD_ID_T                          last_cmd_id;
    u8                                                          try_times;
    u8                                                           sleep_times;
    u32                                                         last_operate_time_sec;
    u8                                                            reserve;
    UE866_ACTION                                     reserve_action;  // 1,set, 2,get  for customer define
    UE866_RESULT                                      status;
    char                                                         *pbuf;
    u16                                                            buf_len;
} ue866_operate_status_t;


/**********************************************************************************************************/
#define SETZERO(x)   memset((char*)&x, 0,sizeof(x))
#define INCREASE(tbl , index)    ( index>= sizeof(tbl)? ( sizeof(tbl) %index) : (index+1))
#define AGPSSTRING_ASK_MAX   3

extern u8 gGsmPowerDown;

static void  ue866_operate_buf_send(u8 *str, u16 len );
static UE866_RESULT  ue866_operate_at_cmd_send( ue866_operate_status_t *pstu );

static UE866_RESULT ue866_at_test(void *agrv );
static UE866_RESULT ue866_at_imei(void *agrv );
static UE866_RESULT ue866_at_qss(void *agrv );
static UE866_RESULT ue866_at_moni(void *agrv );
static UE866_RESULT ue866_at_scfg(void *agrv );
static UE866_RESULT ue866_at_sh(void *agrv );
static UE866_RESULT ue866_at_csq(void *agrv );
static UE866_RESULT ue866_at_creg(void *agrv );
static UE866_RESULT ue866_at_cgreg(void *agrv );
static UE866_RESULT ue866_at_sd(void *agrv );
static UE866_RESULT ue866_at_servinfo(void *agrv );
static UE866_RESULT ue866_at_cgdcont(void *agrv );
static UE866_RESULT ue866_at_si(void *agrv );
static UE866_RESULT ue866_at_ss(void *agrv );
static UE866_RESULT ue866_at_cmgl(void *agrv );

static UE866_RESULT  ue866_at_sgact(void *agrv );
static UE866_RESULT ue866_at_cfun(void *agrv );

static UE866_RESULT  ue866_at_agpssnd(void *agrv );
static UE866_RESULT ue866_at_ssendext(void *agrv );
static UE866_RESULT ue866_at_srecv(void *agrv );

static UE866_RESULT ue866_at_atd(void *agrv );
static UE866_RESULT  ue866_at_clcc(void *agrv );


static UE866_RESULT ue866_operate_get_gsm_buf_data ( void );

static UE866_RESULT ue866_operate_mode_cfg(void);
static UE866_RESULT ue866_operate_mode_normal(void);
static UE866_RESULT ue866_operate_mode_loop(void);
static UE866_RESULT ue866_operate_mode_err_retry(void);


static void ue866_operate_dispatch( void );
static void ue866_operate_reset_network_status(void);




/**************************************************************************************************************/
static const STU_APN ue866_StuApn[] =
{
    /*CSL 1010 / One2Free – Name: hkcsl；APN: mobile; CC:454; MNC:00*/
    {
        .oper = "45400",
        .strapn = "mobile",
        .NetId = EMPTY_MODE,

    },
    /*SmarTone – Name: smartone；APN: smartone; CC:454; MNC:06*/
    {
        .oper = "45406",
        .strapn = "smartone",
        .NetId = EMPTY_MODE,

    },
    /*3HK – Name: 3HK；APN: mobile.three.com.hk; CC:454; MNC:03*/
    {
        .oper = "45403",
        .strapn = "mobile.three.com.hk",
        .NetId = EMPTY_MODE,

    },
    /*PCCW – Name: pccw；APN: pccw; CC:454; MNC:19*/
    {
        .oper = "45419",
        .strapn = "pccw",
        .NetId = EMPTY_MODE,

    },
    /*China Mobile – Name: cmhk；APN – cmhk; CC:454; MNC:12*/
    {
        .oper = "45412",
        .strapn = "cmhk",
        .NetId = EMPTY_MODE,

    },
    /*中国联通NAPN – uninet; CC:460; MNC:01*/
    {
        .oper = "46001",
        //.strapn="uninet",
        // .strapn="3gnet",
        .strapn = "unim2m.gzm2mapn",
        .NetId = EMPTY_MODE,

    },
    /*中国移动APN – cmnet; CC:460; MNC:00*/
    {
        .oper = "46000",
        .strapn = "cmnet",
        .NetId = EMPTY_MODE,

    }
    ,
    /*中国电线APN – cmnet; CC:460; MNC:03*/
    {
        .oper = "46003",
        .strapn = "ctnet",
        .NetId = EMPTY_MODE,
    },
};


ue866_operate_status_t g_ue866_status = {0};
static u8  g_at_cmd_public_buf[64] = {0};

const UE866_ATCMD_ID_T at_cmd_tbl_cfg_mode[] =
{
    UE866_ATCMD_ID_ATZ0,
    UE866_ATCMD_ID_AT,
    UE866_ATCMD_ID_ATE,
    UE866_ATCMD_ID_COPS,
    UE866_ATCMD_ID_CGMR,
    UE866_ATCMD_ID_CGMM,
    UE866_ATCMD_ID_CACHEDNS,
    UE866_ATCMD_ID_CGSN,
    UE866_ATCMD_ID_CFLO,
    UE866_ATCMD_ID_K,
    UE866_ATCMD_ID_QSS,    //  check sim card
    UE866_ATCMD_ID_CLIP,
    UE866_ATCMD_ID_CMGF,
    UE866_ATCMD_ID_MONI,
    UE866_ATCMD_ID_MSCLASS,
    UE866_ATCMD_ID_SCFG,
    UE866_ATCMD_ID_SCFGEXT,
    UE866_ATCMD_ID_CGDCONT,  //  查询与配置当前APN
    UE866_ATCMD_ID_CGATT,        /* GPRS 1:Attach(依靠GPRS) Or 0:Detach(GPRS分离) */

    UE866_ATCMD_ID_CSQ,               //  如果信号低，每尝试5次之后，休息2*n  min,n[1,5]后再尝试，超过5之后重新复位上电
    UE866_ATCMD_ID_CREG,
    UE866_ATCMD_ID_CGREG,
    UE866_ATCMD_ID_SGACT,
    UE866_ATCMD_ID_SERVINFO,
    UE866_ATCMD_ID_SD,
    UE866_ATCMD_ID_SS,

    UE866_ATCMD_ID_CFG_END  //////////  转到  at_cmd_tbl_normal_mode状态
};


const UE866_ATCMD_ID_T at_cmd_tbl_normal_mode[] =
{
    // 读取本地数据,如果有数据
    //    UE866_ATCMD_ID_CFUN_FIVE,  //  使用3G进入正常模式
    UE866_ATCMD_ID_SI,                       // 读取网络数据缓存状态,如果有数据，则取出来
    UE866_ATCMD_ID_SRECV,            // 读取网络数据缓存
    UE866_ATCMD_ID_SS,                  // 检查网络状态
    UE866_ATCMD_ID_SSENDEXT,  //  发送本地数据
    UE866_ATCMD_ID_SD,
    UE866_ATCMD_ID_NORMAL_END
};

const UE866_ATCMD_ID_T at_cmd_tbl_loop_mode[] =
{
    UE866_ATCMD_ID_SI,
    // 读取本地数据,如果有数据，则跳到 at_cmd_tbl_normal_mode,否则一直执行这表
    UE866_ATCMD_ID_CSQ,
    //  UE866_ATCMD_ID_CREG,
    UE866_ATCMD_ID_CGREG,
    UE866_ATCMD_ID_SERVINFO,
    UE866_ATCMD_ID_MONI,

    UE866_ATCMD_ID_AGPSSND,
    //   UE866_ATCMD_ID_SH,

    UE866_ATCMD_ID_CFUN_FIVE,

    UE866_ATCMD_ID_LOOP_END

};

const UE866_ATCMD_ID_T at_cmd_tbl_err_and_retry_mode[] =
{
    /// 只有在 normal_mode，loop_mode下才可以调用,其他状态不调用
    //如果这状态通不过，每尝试10次之后，休息2*n  min,n[1,5]后再尝试，超过5之后重新复位上电
    UE866_ATCMD_ID_ATH,                   //  终止所有连接
    UE866_ATCMD_ID_CSQ,
    UE866_ATCMD_ID_CGREG,
    UE866_ATCMD_ID_SERVINFO,
    UE866_ATCMD_ID_SD,                   // 重新尝试开始新的连接
    UE866_ATCMD_ID_SS,                   // 如果正常，则转交给   at_cmd_tbl_normal_mode

    UE866_ATCMD_ID_ATZ0,           /// 出错后则全部复位
    UE866_ATCMD_ID_SYSHAL,     // 关闭模块, 10s后再打开
    UE866_ATCMD_ID_ERR_RETRY_END
};


/***********************************************************************************/


static UE866_RESULT ue866_at_test(void *agrv )
{
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times >= 5  )
    {
        // It must get something wrong, rebboot model
        // pstu->try_times++;
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
        return ret;
    }
    else if(/* pstu->try_times < 6  &&*/ get_system_time() - pstu->last_operate_time_sec >= 2 )
    {

        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "OK");

    if( NULL != p && NULL != memchr(p, '\r', 128)  )
    {
        //  parse respond
        ret = UE866_RESULT_OK;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
    }

    return ret;
}


static UE866_RESULT ue866_at_imei(void *agrv )
{
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL, *pTem = NULL ;
    u8  i, j = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times >= 5  )
    {
        // It must get something wrong, rebboot model
        // pstu->try_times++;
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
        return ret;
    }
    else if(/* pstu->try_times < 6  &&*/ get_system_time() - pstu->last_operate_time_sec >= 2 )
    {

        ue866_operate_at_cmd_send( pstu );
    }


    p = memchr((char *)&STU_AtCommand.rev, '#', 64); //MaxRevLen
    p = strstr(p, "#CGSN: ");

    if(NULL !=  p && NULL != memchr(p, '\r', 128) )
    {
        p += 7;
        pTem = strstr(p, "\r\n");

        if( NULL != pTem)
        {
            // #CGSN: 353465070085319\r\nOK
            // read IMEI
            //TODO
            j = pTem - p;

            if ( j > 18 )
            {
                j = 18;
            }

            SETZERO (GsmSta.IMEI);

            for ( i = 0; i < ( j / 2 ); i++ )
            {
                GsmSta.IMEI[8 - i] = ( ( acstohex ( * ( pTem - 2 - 2 * i ) ) << 4 ) & 0xf0 ) | ( acstohex ( * ( pTem - 1 - 2 * i ) ) & 0x0f );
            }

            if ( j % 2 )
            {
                GsmSta.IMEI[8 - i] = acstohex ( * ( pTem - 1 - 2 * i ) ) & 0x0f;
            }

            memcpy ( &GsmSto.ID[1], GsmSta.IMEI, 9 );
            WriteGsmStoreDate();

            //  parse respond
            ret = UE866_RESULT_OK;
            pstu->try_times  = 0;
            pstu->last_operate_time_sec = 0;
            pstu->status = UE866_RESULT_IDLE;
        }
    }

    return ret;
}


static UE866_RESULT ue866_at_qss(void *agrv )
{
    /*
    #QSS: <mode>,<status>
    <mode> - type of notification 0 - disabled (factory default): it is possible only to query the current SIM status
    through Read command AT#QSS? 1 - enabled: the ME informs at every SIM status change through the following basic
    unsolicited indication:

    <status> - current SIM status 0 - SIM NOT INSERTED 1 - SIM INSERTED

    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;


    if( pstu->try_times > 5  )
    {
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else if(/* pstu->try_times < 5  &&*/ get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        // It must get something wrong, rebboot model

        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, '#', 128); //MaxRevLen
    p = strstr(p, "QSS");

    if( NULL != p  && NULL != memchr(p, '\r', 128) )
    {
        // read SIM  Read command reports whether the unsolicited indication
        //TODO
        p = memchr(p, ',', 128);
        //  parse respond
        p++;

        if(atoi(p) == 1)
        {
            GsmSta.QSS = OK;
            ret = UE866_RESULT_OK;
            pstu->try_times  = 0;
            pstu->last_operate_time_sec = 0;
            pstu->status = UE866_RESULT_IDLE;
        }
        else
        {
            GsmSta.QSS = NOT_OK;
            myprintf ("SIM NOT INSERTED\r\n");
        }
    }

    return ret;
}

static UE866_RESULT ue866_at_moni(void *agrv )
{
    /*
    Set command sets one cell out of seven, in the neighbour list of the serving cell including it, from which extract
    GSM /UMTS-related information
    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;


    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else if( /*pstu->try_times < 5  &&*/ get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, 'O', 128); //MaxRevLen
    p = strstr(p, "OK");

    if( NULL != p  && NULL != memchr(p, '\r', 128)  )
    {
        // read SIM  Read command reports whether the unsolicited indication
        //TODO

        //  parse respond
        ret = UE866_RESULT_OK;
        //     pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
        // InitGsmQueue();
    }

    return ret;
}


static UE866_RESULT ue866_at_scfg(void *agrv )
{
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times >= 7  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else   if( /*pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        // "AT#SCFG=1,1,900,0,600,10\r\n"
        pstu->reserve = 1;// ++;

        SETZERO (g_at_cmd_public_buf);
        sprintf(g_at_cmd_public_buf, "AT#SCFG=%d,1,900,0,600,10\r\n", pstu->reserve); //  特殊处理
        ue866_operate_at_cmd_send( pstu );

        if(  pstu->reserve >= 2 )
        {
            pstu->try_times = 6;
        }

    }

    p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "OK");


    if( NULL != p  && NULL != memchr(p, '\r', MaxRevLen - 5) )
    {
        //
        //TODO
        //  parse respond
        ret = UE866_RESULT_OK;
        //   pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
        // InitGsmQueue();
    }

    return ret;
}


static UE866_RESULT ue866_at_sh(void *agrv )
{
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;


    if( pstu->try_times >= 6  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else if(/* pstu->try_times < 6  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        /*网络连接专用  Socket Shutdown  */
        SETZERO (g_at_cmd_public_buf);
        sprintf(g_at_cmd_public_buf, "AT#SH=%d\r\n", pstu->try_times + 1); //  特殊处理
        ue866_operate_at_cmd_send( pstu );
    }


    p = memchr((char *)&STU_AtCommand.rev, 'O', 128); //MaxRevLen
    p = strstr(p, "OK");

    if( NULL != p && NULL != memchr(p, '\r', 128) )
    {
        //
        //TODO

        ret = UE866_RESULT_OK;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
        ue866_operate_reset_network_status ( );
        // GsmSta.IpOneConnect = NOT_OK;
        g_ue866_status.cmd_id = UE866_ATCMD_ID_CSQ;

    }

    return ret;
}



static UE866_RESULT ue866_at_cfun(void *agrv )
{
    // "AT+CFUN=5\r", ue866_at_cfun
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times >= 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else    if(/* pstu->try_times < 5 && */ get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        /*网络连接专用  Socket Shutdown  */
        SETZERO (g_at_cmd_public_buf);
        sprintf(g_at_cmd_public_buf, "AT+CFUN=%d\r\n", GsmSta.gsm_p ==  MASK_POWER_STATUS_NOMAL ? 1 : 5 ); //  特殊处理
        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1 ); //MaxRevLen
    p = strstr(p, "OK");

    if( NULL != p && NULL != memchr(p, '\r', MaxRevLen - 2) )
    {
        //TODO
        ret = UE866_RESULT_OK;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;

    }

    return ret;
}

static UE866_RESULT ue866_at_csq(void *agrv )
{
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL, *pFind = NULL, *pTem = NULL;
    u16 i = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times >= 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_TIME_OUT;
        ret = UE866_RESULT_TIME_OUT;
        return ret;
    }
    else  if(/* pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }

    pFind = (char *)&STU_AtCommand.rev;
    i = 0;

    while( i++ < MaxRevLen )
    {
        p = memchr(pFind, '+', MaxRevLen - i); //MaxRevLen

        if(NULL !=  p)
        {
            p = strstr(p, "CSQ:");

            if(NULL != p)
            {
                pTem = strstr(p, "\r\n");

                if( NULL != pTem )
                {
                    p += 4;
                    GsmSta.CSQ = atoi(p);
                    // p++;
                    pFind++;
                }
            }
            else
            {
                pFind++;
            }
        }
        else
        {
            break;
        }

    }

    //  parse respond
    //  if(NULL != p )
    {
        if( GsmSta.CSQ  < 5  )
        {
            ret = UE866_RESULT_OK;
            pstu->status = UE866_RESULT_OK;

        }
        else
        {
            ret = UE866_RESULT_OK;
            pstu->status = UE866_RESULT_IDLE;
            pstu->try_times  = 0;
        }
    }

    return ret;
}


static UE866_RESULT ue866_at_creg(void *agrv )
{
    /*
    *+CREG: <mode>,<stat>[,<Lac>,<Ci>[,<AcT>]]
    *<stat>
    0 - not registered, ME is not currently searching a new operator to register to
    1 - registered, home network
    2 - not registered, but ME is currently searching a new operator to register to
    3 - registration denied
    4 -unknown
    5 - registered, roaming
    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    u8 stat = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else if(/* pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, '+', 128); //MaxRevLen
    p = strstr(p, "CREG:");

    if( NULL != p && NULL != memchr(p, '\r', 128) )
    {
        //
        //TODO

        p = memchr(p, ',', 128); //MaxRevLen
        p += 1;
        stat = atoi(p);

        if( 1 == stat || 5 == stat  )
        {
            GsmSta.CREG = OK;
            pstu->last_operate_time_sec = 0;

        }
        else
        {
            GsmSta.CREG = NOT_OK;
            pstu->cmd_id = UE866_ATCMD_ID_CSQ;

        }

        pstu->status = UE866_RESULT_IDLE;
        ret = UE866_RESULT_OK;

        pstu->try_times  = 0;

    }

    return ret;
}

static UE866_RESULT ue866_at_cgreg(void *agrv )
{
    /*
    *+CGREG: <n>,<stat>[,<lac>,<ci>[,<AcT>,<rac>]]
    *<stat>
    0 - not registered, ME is not currently searching a new operator to register to
    1 - registered, home network
    2 - not registered, but ME is currently searching a new operator to register to
    3 - registration denied
    4 -unknown
    5 - registered, roaming

    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    u8 stat = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else if(/* pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, '+', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "CGREG:");

    if( NULL != p && NULL != memchr(p, '\r', MaxRevLen - 1)  )
    {
        //
        //TODO
        p = memchr(p, ',', 128); //MaxRevLen
        p += 1;
        stat = atoi(p);

        if( 1 == stat || 5 == stat  )
        {
            GsmSta.CGREG = OK;
            pstu->last_operate_time_sec = 0;

        }
        else
        {
            GsmSta.CGREG = NOT_OK;
            pstu->cmd_id = UE866_ATCMD_ID_CSQ;
        }

        pstu->try_times  = 0;
        pstu->status = UE866_RESULT_IDLE;
        ret = UE866_RESULT_OK;

    }

    return ret;
}


static UE866_RESULT ue866_at_sd(void *agrv )
{
    /*
    *
    AT#SD=<connId>, <txProt>,<rPort>, <IPaddr> [,<closureType> [,<lPort> [,<connMode>]]
    <txProt> - transmission protocol    0 - TCP   1 - UDP
    <connMode> - Connection mode   0 - online mode connection (default)   1 - command mode connection

    Open socket 1 in online mode

    AT#SD=1,0,80,”www.google.com”,0,0,0
    CONNECT

    Open socket 1 in command mode

    AT#SD=1,0,80,”www.google.com”,0,0,1
    OK
    */

    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    int len = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;


    if( OK == GsmSta.IpOneConnect )
    {
        pstu->status = UE866_RESULT_IDLE;
        ret = UE866_RESULT_IDLE;
    }
    else  if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_TIME_OUT;
        ret = UE866_RESULT_TIME_OUT;
    }
    else  if(/* pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        //GSM_DEFAULT_URL
        if( 0x00  != GsmSto.strip[78] )
        {
            WriteGsmStoreDateToDefault();
        }

        SETZERO (g_at_cmd_public_buf);
        len = sprintf(g_at_cmd_public_buf, "AT#SD=1,0,%s,\"%s\",0,0,1\r\n",  GsmSto.port, GsmSto.strip); //
        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "OK");


    if( NULL != p && NULL != memchr(p, '\r', MaxRevLen - 1) )
    {
        //
        //TODO
        //  parse respond
        //      GsmSta.IpOneConnect = OK;   // 等待SS的结果
        ret = UE866_RESULT_OK;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
    }
    else
    {
        p = memchr((char *)&STU_AtCommand.rev, 'E', MaxRevLen - 1); //MaxRevLen
        p = strstr(p, "ERROR");

        if( NULL != p )
        {
            //  SETZERO ( g_ue866_status);
            //g_ue866_status.cmd_id = UE866_ATCMD_ID_SH;
            ret = UE866_RESULT_ERR;
            pstu->status = UE866_RESULT_ERR;
        }
    }

    return ret;
}



static UE866_RESULT ue866_at_servinfo(void *agrv )
{
    /*
    *
    (GSM network)
         #SERVINFO: <B-ARFCN>,<dBM>,<NetNameAsc>,<NetCode>, <BSIC>,<LAC>,<TA>,<GPRS>[,[<PB-ARFCN>],[<NOM>], <RAC>,[<PAT>]]

     (UMTS network)
        #SERVINFO: <UARFCN>, <dBM>, <NetNameAsc>,<NetCode>,  <PSC>,<LAC>,<DRX>,<SD>,<RSCP>, <NOM>,<RAC>
    */

    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_TIME_OUT;
        ret = UE866_RESULT_TIME_OUT;
    }
    else  if( /*pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, '#', 512); //MaxRevLen
    p = strstr(p, "#SERVINFO:");


    if( NULL != p && NULL != memchr(p, '\r', 128) )
    {
        //
        //TODO
        //  parse respond
        p += 10;

        if( (GsmSta.operator_flag & 0x01) == 0x01 )
        {
            p = strstr(p, "46001");   // 45403
        }
        else  if( (GsmSta.operator_flag & 0x04) == 0x04 )
        {
            p = strstr(p, "45403");   // 45403
        }


        if( NULL == p  )
        {
            pstu->status = UE866_RESULT_ERR;
            ret = UE866_RESULT_ERR;
        }
        else
        {
            ret = UE866_RESULT_OK;
            pstu->try_times  = 0;
            pstu->last_operate_time_sec = 0;
            pstu->status = UE866_RESULT_IDLE;
        }

    }

    return ret;
}


static UE866_RESULT ue866_at_cnmi(void *agrv )
{
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else if(/* pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }


    p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "OK");

    if( NULL != p && NULL != memchr(p, '\r', 128) )
    {
        //
        //TODO

        //  parse respond
        ret = UE866_RESULT_OK;
        //    pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
        //  InitGsmQueue();
    }

    return ret;
}


static UE866_RESULT ue866_at_cgdcont(void *agrv )
{
    /*
    * Config APNS
    */
    // { UE866_ATCMD_ID_CGDCONT, "AT+CGDCONT?\r", }, /*查询apn  AT+CGDCONT=1,”IP”,”APN”,”10.10.10.10”,0,0*/
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else if( /*pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        SETZERO (g_at_cmd_public_buf);

        // AT+CGDCONT=1,”IP”,”APN”,”10.10.10.10”,0,0
        if( 0 == pstu->try_times )
        {
            memcpy(g_at_cmd_public_buf, "AT+CGDCONT?\r\n", sizeof("AT+CGDCONT?\r\n")); //  特殊处理
        }
        else
        {

            sprintf(g_at_cmd_public_buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", GsmSto.strapn); //
        }

        ue866_operate_at_cmd_send( pstu );
    }


    p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "OK");


    if( NULL != p && NULL != memchr(p, '\r', 128) )
    {
        //
        //TODO

        //  parse respond
        ret = UE866_RESULT_OK;
        //   pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
        //InitGsmQueue();
    }

    return ret;
}

static UE866_RESULT ue866_at_si(void *agrv )
{
    /* #SI: <connId>,<sent>,<received>,<buff_in>,<ack_waiting>  */

    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL, *pTem = NULL;
    u8 i = 0;
    u16 len = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times >= 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else if(/* pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        //   AT#SI=1\r\n
        //  AT#SI=?\r\n show all
        ue866_operate_at_cmd_send( pstu );
    }


    p = memchr((char *)&STU_AtCommand.rev, '#', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "#SI");


    if( NULL != p  && NULL != memchr(p, '\r', 128) )
    {
        //
        //TODO
        //  parse respond
        //     #SI: <connId>,<sent>,<received>,<buff_in>,<ack_waiting>
        // for( i = 0; i < 4; i++)

        while(  (pTem = strtok(p, ",") ) != NULL)
        {
            i++;
            p = NULL;

            if(  NULL != pTem)
            {
                if( 4 == i )
                {
                    len = atoi(pTem); // buff_in
                    //  GsmSta.asklen = GsmSta.RevLen = len > 1000 ?  (len - 1000) : len;
                    GsmSta.RevLen = len > 1000 ?  (len - 1000) : len;
                    GsmSta.DateCome = NOT_OK;
                    break;
                }
                else if(  5 == i )
                {

                }
            }
        }

        ret = UE866_RESULT_OK;
        // pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
    }

    return ret;
}


static UE866_RESULT ue866_at_ss(void *agrv )
{
    /*
    Socket Status
    #SS: <connId1>,<state1>,<locIP1>,<locPort1>,<remIP1>,<remPort1>
    <state> - actual state of the socket:
    0 - Socket Closed.
    1 - Socket with an active data transfer connection.
    2 - Socket suspended.
    3 - Socket suspended with pending data.
    4 - Socket listening.
    5 - Socket with an incoming connection. Waiting for the user accept or shutdown command.
    6 - Socket resolving DNS.
    7 - Socket connecting.
    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    u8  conn_id = 0, conn_state = 0, i = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times >= 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else  if( /*pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }


    p = memchr((char *)&STU_AtCommand.rev, '#', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "#SS:");

    if( NULL != p && NULL != memchr(p, '\r', 128) )
    {
        //
        //TODO
        for( i = 0; i < 6; i++ )
        {
            p = strstr(p, "#SS:");

            if( NULL != p )
            {
                //  parse respond
                p += 4; //  connId1
                conn_id = atoi(p);

                p = memchr(p, ',', 128);
                p += 1; // state1
                conn_state =  atoi(p);

                if( 1 == conn_id )
                {
                    pstu->reserve = conn_state;
                    //  if( conn_state )
                    //  GsmSta.SocketState = pstu->reserve ;
                    break;
                }
            }
            else
            {
                break;
            }
        }

        switch( pstu->reserve )
        {
            case 0:
                GsmSta.IpOneConnect = NOT_OK;
                //    SETZERO (g_ue866_status);
                g_ue866_status.cmd_id = UE866_ATCMD_ID_SD;
                //    pstu->status = UE866_RESULT_ERR;
                //    ret = UE866_RESULT_ERR;
                GsmSta.asklen = GsmSta.RevLen = 0;
                GsmSta.DateCome = NOT_OK;
                break;

            case 1:
            case 2:
            case 3:
            case 5:
                GsmSta.IpOneConnect = OK;
                GsmSta.SocketState  = OK;
                ret = UE866_RESULT_OK;
                pstu->try_times  = 0;
                pstu->last_operate_time_sec = 0;
                pstu->status = UE866_RESULT_IDLE;
                break;

            case 4:
            case 6:
            case 7:
                pstu->try_times  = 0;
                pstu->last_operate_time_sec += 2;
                pstu->status = UE866_RESULT_WAIT;
                break;
        }
    }

    return ret;
}


static UE866_RESULT ue866_at_cmgl(void *agrv )
{
    /*
    +CMGL: <index>,<stat>,<alpha>,<length><CR><LF><pdu>[...]]
    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else  if( /*pstu->try_times < 5  &&*/ get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }

    p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "OK");

    if( NULL != p && NULL != memchr(p, '\r', 128) )
    {
        //
        //TODO

        //  parse respond
        ret = UE866_RESULT_OK;
        //   pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
        //  InitGsmQueue();
    }

    return ret;
}

static UE866_RESULT  ue866_at_cmgd(void *agrv )
{
    /*
    +CMGD
    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;

    }
    else  if( /*pstu->try_times < 5  &&*/ get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }

    // if( NULL != strstr((char *)STU_AtCommand.rev,"+CMGD") )
    if( NULL != strstr((char *)STU_AtCommand.rev, "OK") )
    {
        //
        //TODO

        //  parse respond
        ret = UE866_RESULT_OK;
        //        pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;

    }

    return ret;

}


static UE866_RESULT  ue866_at_sgact(void *agrv )
{

    /*
    ActiveGprs
    *"AT#SGACT=1,%d\r\n"
    *#SGACT:10.23.109.115
    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL, *pTem = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else  if( /*pstu->try_times < 5  &&*/ get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        SETZERO (g_at_cmd_public_buf);
        //    if( NOT_OK == GsmSta.IpOneConnect )
        {
            // Active GPRS

            if( 0 == pstu->reserve )//||   ( 0 == pstu->reserve  ) )
            {
                
                // sprintf(g_at_cmd_public_buf,  "AT#SGACT=1,%d,,\r\n", pstu->reserve); //
                sprintf(g_at_cmd_public_buf,  "AT#SGACT?\r\n"); //
            }
            else
            {
                // sprintf(g_at_cmd_public_buf,  "AT#SGACT?\r\n"); //
                //  pstu->reserve = 1;

                sprintf(g_at_cmd_public_buf,  "AT#SGACT=1,%d,,\r\n", pstu->reserve > 0 ? 1 : 0 ); //
            }

        }


        ue866_operate_at_cmd_send( pstu );

    }


    p = memchr((char *)&STU_AtCommand.rev, '#', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "SGACT:");
    pTem = strstr(p, "\r\n");

    if( NULL != p &&  NULL != pTem)
    {
        //
        //TODO
        p += 7;

        if(atoi(p) > 3  && pstu->reserve > 0)
        {
            ret = UE866_RESULT_OK;
            pstu->try_times  = 0;
            pstu->last_operate_time_sec = 0;
            pstu->status = UE866_RESULT_IDLE;
        }
        else if(atoi(p) == 1  &&  0 == pstu->reserve )
        {
           // pstu->last_operate_time_sec -= 1;
            pstu->reserve = 1;
            InitGsmQueue();
        }
        //  parse respond
    }
    else
    {
        p = memchr((char *)&STU_AtCommand.rev, 'E', MaxRevLen - 1);
        p = strstr(p, "ERROR");

        //  if( NULL != p )
        //   {
        // SETZERO ( g_ue866_status);
        // g_ue866_status.cmd_id = UE866_ATCMD_ID_SERVINFO;
        //  }
    }


    return ret;
}

static UE866_RESULT  ue866_at_agpssnd(void *agrv )
{
    /*
    AT#AGPSSND=0,0,0,3
    OK
    #AGPSRING: 200,22.523435,114.03293,0,2900,"",0

    #AGPSMPRRING: 1
    */
    //#define AGPSSTRING_TEST  "#AGPSRING: 200,22.523435,114.03293,0,2900,"",0\r\n #AGPSMPRRING: 1"
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL, *pTem = NULL, *pFind = NULL;
    u16 i = 0;
    double dtmp;

    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > AGPSSTRING_ASK_MAX  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_TIME_OUT;
        ret = UE866_RESULT_TIME_OUT;
        // ret = UE866_RESULT_OK;
        //   pstu->status = UE866_RESULT_IDLE;
        // pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;

    }

    else  if( /* pstu->try_times < 5 && */get_system_time() - pstu->last_operate_time_sec >= 10 )
    {
        InitGsmQueue();
        ue866_operate_at_cmd_send( pstu );
    }

#ifdef  AGPSSTRING_TEST
    pFind =  AGPSSTRING_TEST;

#else
    pFind = (char *)&STU_AtCommand.rev;

#endif

    i = 0;

    while( i++ < MaxRevLen )
    {
        p = memchr(pFind, '#', MaxRevLen - i); //MaxRevLen

        if(NULL !=  p)
        {
            p = strstr(p, "AGPSRING:");

            if(NULL != p)
            {
                pTem = strstr(p, "\r\n");

                if(NULL != pTem )
                {
                    break;
                }
            }
            else
            {
                pFind++;
            }
        }
        else
        {
            break;
        }

    }


    if( NULL != p && NULL != pTem )
    {
        //
        //TODO
        //  parse respond
        if( NULL != strstr(p, "get")  && NULL != strstr(p, "200") /*&&  pstu->try_times > AGPSSTRING_ASK_MAX*/ )
        {

            ret = UE866_RESULT_OK;
            // pstu->try_times  = 0;
            pstu->last_operate_time_sec = 0;
            pstu->status = UE866_RESULT_IDLE;
        }
        else if( NULL == strstr(p, "get")  && NULL != strstr(p, "200") )
        {
            ret = UE866_RESULT_OK;
            pstu->try_times  = 0;
            pstu->last_operate_time_sec = 0;
            pstu->status = UE866_RESULT_IDLE;

            i = 0;

            while((pTem = strtok(p, ",")) != NULL)
            {
                //   "#AGPSRING: 200,22.523435,114.03293,0,2900,"",0\r\n #AGPSMPRRING: 1"
                if( 1 == i )
                {
                    dtmp = atof(pTem);

                    if(  ( s32 ) ( dtmp * 1000000 ) != 0 )
                    {
                        GsmSta.Latitude = ( s32 ) ( dtmp * 1000000 );
                    }
                }

                if( 2 == i )
                {
                    dtmp = atof(pTem);

                    if(  ( s32 ) ( dtmp * 1000000 ) != 0 )
                    {
                        GsmSta.longitude = ( s32 ) ( dtmp * 1000000 );
                    }
                    else
                    {
                        GsmSta.Latitude = 6666666;
                        GsmSta.longitude = 0;
                    }
                }

                if( 4 == i )
                {
                    dtmp = atoi(pTem);
                    GsmSta.Uncerten = ( u16 ) ( dtmp * 10 );
                    GsmSta.varitygsmlon = CalacXORVarity ( ( u8 * ) &GsmSta.Latitude, 10 );

                    GsmSta.askm2malerag = 0;
                    GsmSta.askm2m = 0;
                }

                i++;
                p = NULL;
            }


        }
        else
        {
            //  InitGsmQueue();
            // pstu->last_operate_time_sec = get_system_time() ;
        }
    }
    else
    {
        p = memchr((char *)&STU_AtCommand.rev, 'a', MaxRevLen - 1); //MaxRevLen
        p = strstr(p, "at#");

        if( NULL != p )
        {
            pstu->cmd_id = UE866_ATCMD_ID_ATE;
            ue866_operate_at_cmd_send( pstu );
            pstu->cmd_id = UE866_ATCMD_ID_AGPSSND;
            //pstu->last_operate_time_sec = get_system_time();
            InitGsmQueue();
        }

    }

    return ret;

}



static UE866_RESULT ue866_at_ssendext(void *agrv )
{
    /*
    *AT#SSENDEXT= <connId>, <bytestosend>
    at#ssendext=1,256
    > .............................. ; // Terminal echo of bytes sent is displayed here
    OK

    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    u16 i = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( 0 == GsmSta.SendingLen )
    {
        pstu->status = UE866_RESULT_IDLE;
        ret = UE866_RESULT_OK;
    }
    else if( pstu->try_times >= 5  )
    {
        // It must get something wrong, rebboot model
        if(  UE866_ACTION_REV == pstu->reserve_action )
        {
            pstu->reserve_action = UE866_ACTION_START;
            //  GsmSta.DateCome = OK;
            GsmSta.SendingLen  = 0;
            GsmSta.Sending = DATAS_IDLES;
            ret = UE866_RESULT_OK;
            pstu->try_times  = 0;
            pstu->last_operate_time_sec = 0;
            pstu->status = UE866_RESULT_IDLE;
            pstu->last_operate_time_sec = 0;
            pstu->try_times  = 0;

        }
        else
        {
            pstu->status = UE866_RESULT_TIME_OUT;
            ret = UE866_RESULT_TIME_OUT;
            GsmSta.Sending = DATAS_IDLES;
            GsmSta.IpOneConnect = NOT_OK;
        }

        return ret;

    }
    else if( /*pstu->try_times < 5  && */ DATAS_IDLES == GsmSta.Sending ||
                                          get_system_time() - pstu->last_operate_time_sec >= 5 )
    {
        SETZERO (g_at_cmd_public_buf);
        InitGsmQueue();
        sprintf(g_at_cmd_public_buf, "AT#SSENDEXT=1,%d\r\n", GsmSta.SendingLen ); //  特殊处理

        GsmSta.Sending = DATAS_SENDING;
        pstu->reserve_action = UE866_ACTION_SEND;
        ue866_operate_at_cmd_send( pstu );

    }


    if( UE866_ACTION_SEND == pstu->reserve_action )
    {
        p = memchr((char *)&STU_AtCommand.rev, '>', MaxRevLen - 1); //MaxRevLen

    }
    else  if( UE866_ACTION_GET == pstu->reserve_action )
    {
        p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
        p = strstr(p, "OK\r\n");
    }

    /*
       else if(  UE866_ACTION_REV == pstu->reserve_action )
       {
           // SRING: 1,8760
           p = memchr((char *)&STU_AtCommand.rev, 'S', MaxRevLen - 1); //MaxRevLen

           while( i++ <  MaxRevLen - 1 )
           {
               if(NULL != p)
               {
                   p += 1;

                   if(*(p) != 'R' )
                   {
                       p = memchr(p, 'S', MaxRevLen - i); //MaxRevLen

                       if( NULL == p )
                       {
                           break;
                       }

                   }
                   else if(*(p) == 'R' )
                   {

                       break;
                   }
               }
           }

           p = strstr(p, "RING:");

           if(NULL == p )
           {
               p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
               p = strstr(p, "OK\r\n");
           }

       }
    */

    if( NULL != p)
    {
        //
        //TODO

        switch( pstu->reserve_action )
        {
            case  UE866_ACTION_SEND:
                InitGsmQueue();
                GsmSta.Sending = DATAS_SENDING;

                ue866_operate_buf_send( GsmSta.SendingBuf, GsmSta.SendingLen );
                pstu->reserve_action = UE866_ACTION_GET; //  获取返回状态
                pstu->last_operate_time_sec = get_system_time() ;

                break;

            case UE866_ACTION_GET: // 收到模块回应OK即可
                //   pstu->reserve_action = UE866_ACTION_REV; //  等待远端返回应答
                //pstu->last_operate_time_sec = get_system_time() ;


                pstu->reserve_action = UE866_ACTION_START;
                GsmSta.SendingLen  = 0;
                GsmSta.Sending = DATAS_IDLES;
                ret = UE866_RESULT_OK;
                pstu->try_times  = 0;
                pstu->last_operate_time_sec = 0;
                pstu->status = UE866_RESULT_IDLE;
                pstu->last_operate_time_sec = 0;
                pstu->try_times  = 0;
                break;
#if 0

            case UE866_ACTION_REV:
                p = strstr(p, ",");

                if( NULL != p )
                {
                    p += 1;
                    GsmSta.RevLen = atoi(p);
                    GsmSta.asklen = GsmSta.RevLen ;
                }

                pstu->reserve_action = UE866_ACTION_START;
                //   GsmSta.DateCome = OK;
                GsmSta.SendingLen  = 0;
                //   GsmSta.Sending = DATAS_IDLES;
                ret = UE866_RESULT_OK;
                pstu->try_times  = 0;
                pstu->last_operate_time_sec = 0;
                pstu->status = UE866_RESULT_IDLE;
                pstu->last_operate_time_sec = 0;
                pstu->try_times  = 0;
                break;
#endif
        }

        //  pstu->try_times  = 0;
        // pstu->last_operate_time_sec = 0;
    }
    else
    {
        p = memchr((char *)&STU_AtCommand.rev, 'E', MaxRevLen - 1); //MaxRevLen
        p = strstr(p, "ERROR");

        if( NULL != p )
        {
            //  SETZERO ( g_ue866_status);
            // g_ue866_status.cmd_id = UE866_ATCMD_ID_SH;
            pstu->status = UE866_RESULT_ERR;
            ret = UE866_RESULT_ERR;
        }
    }

    return ret;
}


static UE866_RESULT ue866_at_srecv(void *agrv )
{
    /*
    AT#SRECV //获取网络数据？
    AT#SRECV=1,23
    #SRECV:1,23
    message from second host

    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;


    if( pstu->try_times >= 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_TIME_OUT;
        ret = UE866_RESULT_TIME_OUT;
        GsmSta.asklen = GsmSta.RevLen = 0;
        GsmSta.Sending = DATAS_IDLES;

        return ret;

    }
    else if(0 ==  GsmSta.RevLen  || /**/ NOT_OK == GsmSta.DateCome  )
    {

        pstu->status = UE866_RESULT_IDLE;
        ret = UE866_RESULT_IDLE;
        GsmSta.DateCome = NOT_OK;
        //  GsmSta.asklen = GsmSta.RevLen = 0;
        //  GsmSta.Sending = DATAS_IDLES;

        return ret;
    }
    else if( /*pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 && 0 == GsmSta.asklen )
    {
        SETZERO (g_at_cmd_public_buf);

        sprintf(g_at_cmd_public_buf, "AT#SRECV=1,%d\r\n", GsmSta.RevLen); //

        ue866_operate_at_cmd_send( pstu );
    }

    // #SRECV:1
    p = memchr((char *)&STU_AtCommand.rev, 'O', MaxRevLen - 1); //MaxRevLen
    p = strstr(p, "OK\r\n");


    if( NULL != p )
    {
        //
        //TODO
        //  parse respond
        ret = UE866_RESULT_OK;
        GsmSta.Sending = DATAS_IDLES;
        //   pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
        GsmSta.asklen = GsmSta.RevLen = 0;
        GsmSta.DateCome = NOT_OK;

    }
    else
    {
        /*
            p = memchr((char *)&STU_AtCommand.rev, 'E', MaxRevLen - 1); //MaxRevLen
            p = strstr(p, "ERROR");

            if( NULL != p )
            {
                //  SETZERO ( g_ue866_status);
                // g_ue866_status.cmd_id = UE866_ATCMD_ID_SH;
                pstu->status = UE866_RESULT_ERR;
                ret = UE866_RESULT_ERR;
                GsmSta.asklen = GsmSta.RevLen = 0;
                GsmSta.Sending = DATAS_IDLES;
            }
        */
    }


    return ret;


}


static UE866_RESULT ue866_at_atd(void *agrv )
{
    // ATD<number>[;] <number> - phone number to be dialed
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;

    }
    else if( /*pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        SETZERO (g_at_cmd_public_buf);
        sprintf(g_at_cmd_public_buf, "ATD%s;\r\n", "18616708121"); //  特殊处理
        ue866_operate_at_cmd_send( pstu );
    }

    if( NULL != strstr((char *)STU_AtCommand.rev, "OK") )
    {
        //
        //TODO

        //  parse respond
        ret = UE866_RESULT_OK;
        //   pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
    }

    return ret;
}

static UE866_RESULT  ue866_at_clcc(void *agrv )
{
    /*
    List Current Calls
    [+CLCC:<id1>,<dir>,<stat>,<mode>,<mpty>,<number>,<type> ,<alpha>[<CR><LF>+CLCC:<id2>,<dir>,<stat>,<mode>, <mpty>,<
    number>,<type>,<alpha>[…]]]

    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;

    if( pstu->try_times > 5  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;

    }
    else if(/* pstu->try_times < 5  && */get_system_time() - pstu->last_operate_time_sec >= 2 )
    {
        ue866_operate_at_cmd_send( pstu );
    }

    // if( NULL != strstr((char *)STU_AtCommand.rev,"#AGPSRING") )
    if( NULL != strstr((char *)STU_AtCommand.rev, "OK") )
    {
        //
        //TODO

        //  parse respond
        ret = UE866_RESULT_OK;
        //   pstu->cmd_id+=1;
        pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        pstu->status = UE866_RESULT_IDLE;
    }

    return ret;

}




static UE866_RESULT ue866_at_cops(void *agrv )
{
    /*
    搜索当前存在的网络
    AT+COPS=?\r
    +COPS: [list of supported (<stat> ,<oper (in <format>=0)>,, <oper (in <format>=2)>,< AcT>)s][,,(list of supported <mode>s),

    AT+COPS=0\r // 自动选择网络
    AT+COPS=1,2,46001,2\r //  强制使用3G,
    AT+COPS=1,2,45403,2\r  // 香港 3 3G网络代码
    */
    UE866_RESULT  ret = UE866_RESULT_WAIT;
    char *p = NULL, *pErr = NULL, *pTem = NULL;
    u8 i = 0;
    int systimes = 0;
    ue866_operate_status_t *pstu =  (ue866_operate_status_t *)agrv;
    // char tmp_buf[16]={0};
    systimes = get_system_time() - pstu->last_operate_time_sec;

    if( pstu->try_times > 20  )
    {
        // It must get something wrong, rebboot model
        pstu->status = UE866_RESULT_ERR;
        ret = UE866_RESULT_ERR;
    }
    else  if( /*pstu->try_times < 20  &&*/ systimes  >= 10 )
    {
        InitGsmQueue();
        SETZERO (g_at_cmd_public_buf);

        if( /* pstu->try_times < 4 && pstu->reserve < 2 || */ 0 == GsmSta.operator_flag )
        {
            pstu->reserve = 1;
            sprintf(g_at_cmd_public_buf, "AT+COPS=%s\r\n", "?"); //  特殊处理
        }
        else
        {
            // 1,2,45403,2
            pstu->reserve = 2;

            if( GsmSta.operator_flag & 0x01 == 1 )
            {
                for(i = 0; i < sizeof(ue866_StuApn) / sizeof(STU_APN); i++ )
                {
                    if(0 == memcmp(ue866_StuApn->oper, "46001", 5) )
                    {
                        break;
                    }
                }

                if( i < sizeof(ue866_StuApn) / sizeof(STU_APN) )
                {
                    memcpy( GsmSto.strapn, ue866_StuApn[i].strapn, strlen(ue866_StuApn[i].strapn));    //   ue866_StuApn
                }

                sprintf(g_at_cmd_public_buf, "AT+COPS=%s\r\n", "0"); //  特殊处理
            }
            else
            {
                for(i = 0; i < sizeof(ue866_StuApn) / sizeof(STU_APN); i++ )
                {
                    if(0 == memcmp(ue866_StuApn->oper, "45403", 5) )
                    {
                        break;
                    }
                }

                if( i < sizeof(ue866_StuApn) / sizeof(STU_APN) )
                {
                    memcpy( GsmSto.strapn, ue866_StuApn[i].strapn, strlen(ue866_StuApn[i].strapn));    //   ue866_StuApn
                }

                sprintf(g_at_cmd_public_buf, "AT+COPS=%s\r\n", "1,2,45403,2"); //  特殊处理
            }

        }

        ue866_operate_at_cmd_send( pstu );
        pstu->last_operate_time_sec += 1;
    }

    pErr = memchr((char *)&STU_AtCommand.rev, 'E', 128); //MaxRevLen
    p = memchr((char *)&STU_AtCommand.rev, 'O', 128); //MaxRevLen

    if( NULL != pErr )
    {
        pTem = strstr(pErr, "ERROR");

        if( pTem)
        {
            if( pstu->try_times > 3 )
            {
                pstu->status = UE866_RESULT_ERR;
                ret = UE866_RESULT_ERR;
            }

            pstu->last_operate_time_sec = get_system_time();
            InitGsmQueue();
            p = NULL;
        }


    }

    if( p )
    {
        p = strstr(p, "OK");

        if( NULL != p )
        {
            if(  1 == pstu->reserve)
            {
                p = memchr((char *)&STU_AtCommand.rev, '+', MaxRevLen - 1); //MaxRevLen
                p = strstr(p, "+COPS");
            }
            else if(  2 == pstu->reserve)
            {
                pstu->reserve = 3;
            }
            else
            {
                pstu->reserve = 3;
            }

        }
    }


    if( NULL != p )
    {
        //
        //TODO
        pTem = strstr(p, "46001");

        if( pTem)
        {
            GsmSta.operator_flag |= 1;
            pstu->reserve = 2;
        }
        else
        {
            pTem = strstr(p, "46000");

            if( pTem)
            {
                GsmSta.operator_flag |= 2;
                pstu->reserve = 2;
            }

            pTem = strstr(p, "45403");

            if( pTem)
            {
                GsmSta.operator_flag |= 4;
                pstu->reserve = 2;
            }

            pTem = strstr(p, "45406");

            if( pTem)
            {
                GsmSta.operator_flag |= 8;
                pstu->reserve = 2;
            }

            pTem = strstr(p, "45413");

            if( pTem)
            {
                GsmSta.operator_flag |= 16;
                pstu->reserve = 2;
            }
        }

        //  parse respond
        ret = UE866_RESULT_OK;

        if( 3 == pstu->reserve )
        {
            pstu->try_times  = 0;
            pstu->last_operate_time_sec = 0;
            pstu->status = UE866_RESULT_IDLE;
        }

        //   pstu->cmd_id+=1;
        //  pstu->try_times  = 0;
        pstu->last_operate_time_sec = 0;
        //  pstu->status = UE866_RESULT_IDLE;
    }

    return ret;
}


static ue866_operate_at_cmd_t at_cmd_table[] =
{
    /* test cmd */
    {UE866_ATCMD_ID_AT, "AT\r\n", ue866_at_test}, /*test */

    /* setting cmd*/
    {UE866_ATCMD_ID_ATE, "ATE0\r\n", ue866_at_test}, /* close echo */

    { UE866_ATCMD_ID_COPS, g_at_cmd_public_buf, ue866_at_cops }, /* 注册网络*/


    {UE866_ATCMD_ID_CGMR, "AT+CGMR\r\n", ue866_at_test}, /* Get version */
    {UE866_ATCMD_ID_CGMM, "AT+CGMM\r\n", ue866_at_test}, /*模块名*/
    {UE866_ATCMD_ID_CACHEDNS, "AT#CACHEDNS=1\r\n", ue866_at_test}, /*缓存域名ip4*/
    {UE866_ATCMD_ID_CGSN, "AT#CGSN\r\n", ue866_at_imei}, /*IMEI5*/
    {UE866_ATCMD_ID_CFLO, "AT#CFLO=0\r\n", ue866_at_test }, // /* Command Flow Control;  0 – disable flow control in command mode  */
    {UE866_ATCMD_ID_K, "AT&K0\r", ue866_at_test}, // Set command controls the RS232 flow control behaviour  0 - no flow control  hardware bi-directional flow control (both RTS/CTS active) (factory default)
    {UE866_ATCMD_ID_QSS, "AT#QSS?\r\n",  ue866_at_qss}, /*Read command reports whether the unsolicited indication*/
    {UE866_ATCMD_ID_CLIP, "AT+CLIP=1\r\n", ue866_at_test}, /*  Calling Line Identification Presentatio  1 - enables CLI indication */
    {UE866_ATCMD_ID_CMGF, "AT+CMGF=1\r\n", ue866_at_test}, /*  Message Format     0 - PDU mode;1 - text mode  */
    {UE866_ATCMD_ID_MONI, "AT#MONI=0\r\n", ue866_at_moni }, /* Cell Monitor    #MONI: <cc> <nc> PSC:<psc> RSCP:<rscp>     LAC:,<lac>  Id:<id>     */
    {UE866_ATCMD_ID_MSCLASS, "AT#MSCLASS=8,1\r", ue866_at_test}, /*?? 暂时不明白这个是用于做什么,文档说明是用于追踪基站 12  default    */

    {UE866_ATCMD_ID_SCFG, g_at_cmd_public_buf, ue866_at_scfg}, /*设置Soket 缓冲区，超时*/
    {UE866_ATCMD_ID_SCFGEXT, "AT#SCFGEXT=1,1,0,0,0,0\r\n", ue866_at_test }, /*网络连接专用 Socket Configuration Extended  */
    { UE866_ATCMD_ID_CGDCONT, g_at_cmd_public_buf, ue866_at_cgdcont }, /*查询apn  AT+CGDCONT=1,”IP”,”APN”,”10.10.10.10”,0,0*/


    {UE866_ATCMD_ID_CSQ, "AT+CSQ\r\n", ue866_at_csq}, /*  */
    {UE866_ATCMD_ID_CREG, "AT+CREG?\r\n", ue866_at_creg}, /*  Network Registration Report  +CREG: <stat>[,<Lac>,<Ci>[,<AcT>]]    AcT:0=GSM，2= UTRAN */
    {UE866_ATCMD_ID_CGREG, "AT+CGREG?\r\n", ue866_at_cgreg}, /*  GPRS Network Registration Status,意味着网络可以用 */
    {UE866_ATCMD_ID_CGATT, "AT+CGATT=1\r\n", ue866_at_test }, /* GPRS 1:Attach(依靠GPRS) Or 0:Detach(GPRS分离) */

    { UE866_ATCMD_ID_SD, g_at_cmd_public_buf, ue866_at_sd }, /* 连接远端网络，如服务器 */

    {UE866_ATCMD_ID_SERVINFO, "AT#SERVINFO\r\n", ue866_at_servinfo }, /*网络连接专用 Serving Cell Information */

    //  {UE866_ATCMD_ID_CNMI, "AT+CNMI=3,1\r\n", ue866_at_cnmi}, /*网络连接专用  New Message Indications To Terminal Equipment  */
    // { UE866_ATCMD_ID_CMGL, "AT+CMGL=\"ALL\"\r\n", ue866_at_cmgl}, /*  List Messages  */
    //    { UE866_ATCMD_ID_CMGD, "AT+CMGD=1,4\r\n", ue866_at_cmgd}, /* Delete Message  */


    { UE866_ATCMD_ID_SGACT, g_at_cmd_public_buf, ue866_at_sgact}, /*Get AGPS*/



    { UE866_ATCMD_ID_SI, "AT#SI=1\r\n", ue866_at_si}, /* #SI: <connId>,<sent>,<received>,<buff_in>,<ack_waiting>  */
    { UE866_ATCMD_ID_SS, "AT#SS=1\r\n", ue866_at_ss }, /* Socket Status  */

    { UE866_ATCMD_ID_AGPSSND, "AT#AGPSSND=0,0,0,3\r\n", ue866_at_agpssnd}, /*Get AGPS*/

    { UE866_ATCMD_ID_SSENDEXT, g_at_cmd_public_buf, ue866_at_ssendext}, /* 准备发送数据 */
    { UE866_ATCMD_ID_SRECV, g_at_cmd_public_buf, ue866_at_srecv}, /* 准备REV数据 */

    //  {38,"AT+WS46=22\r",11,"OK",1,30},//  PCCA STD-101 Select Wireless Network
    //   {UE866_ATCMD_ID_ATA, "ATA\r", ue866_at_test}, //  /* used to answer to an incoming call*/ 9

    //    { UE866_ATCMD_ID_ATD, g_at_cmd_public_buf,   ue866_at_atd }, /* Call a phone   "ATD;\r" */
    //    { UE866_ATCMD_ID_CLCC, "AT+CLCC\r",  ue866_at_clcc}, /* Get the Phone Activity Status */

    {UE866_ATCMD_ID_ATH, "ATH\r", ue866_at_test}, // /*  close the current conversation (voice or data). */

    {UE866_ATCMD_ID_SH,  g_at_cmd_public_buf, ue866_at_sh}, /*网络连接专用  Socket Shutdown  */
    { UE866_ATCMD_ID_SYSHAL, "AT#SYSHALT=0,1\r", ue866_at_test}, /*System turn-off   AT#SYSHALT[=  <GPIO_restore>,  <DTR_wakeup_en>, <Reboot_en>] */  //  AT#FASTSYSHALT
    //    { UE866_ATCMD_ID_SHDN, "AT#SHDN\r", ue866_at_test}, /* Software Shutdown  Note: to turn it on again Hardware pin ON/OFF must be tied low.*/
    {UE866_ATCMD_ID_CFUN_FIVE, g_at_cmd_public_buf, ue866_at_cfun}, /* sleep mode */

    {UE866_ATCMD_ID_ATZ0, "ATZ0\r", ue866_at_test},  /* Rest config file */

    //network cmd

};

/*
*********************************************************************************************************
*   函 数 名void GsmUartSendStr(u8 *str,u16 len)
*   功能说明: GSM发送字符串
*
*       作    者 ：liupeng
*   形    参：
*       版    本：version 1.0
*   返 回 值: 无
*********************************************************************************************************
*/
void ue866_operate_uart_send_str ( u8 *str, u16 len )
{
#ifndef GSM_USE_UART
    UartSendStr ( LEUART0, str, len );
#else
    u16 i;


    for ( i = 0; i < len; i++ )
    {
        USART_Tx ( USART2, str[i] );
    }

    if ( debug == DEBUGGSM )
    {
        DebugUartSend ( str, len );
    }

#endif
}

static void  ue866_operate_buf_send(u8 *str, u16 len )
{
    // InitGsmQueue();
    // DTR must be enable
    ue866_gpio_dtr(true);
    // RTS must be enable, to tell model, we are ready to send msg
    // if( 0 ==  ue866_gpio_cts() )  // sleep
    {
        ue866_gpio_rts(true);
    }

    GPIO_PinModeSet ( GSM_TX_PORT, GSM_TX_PIN, gpioModePushPull, 0 );

    ue866_operate_uart_send_str( str,  len );
}


static UE866_RESULT  ue866_operate_at_cmd_send( ue866_operate_status_t *pstu )
{
    UE866_RESULT ret = UE866_RESULT_ERR;
    ue866_operate_at_cmd_t *p;// = (ue866_operate_at_cmd_t *)&at_cmd_table[pstu->cmd_id];
    u8 i = 0;

    for( ; i <  sizeof(at_cmd_table) / sizeof(ue866_operate_at_cmd_t); i++ )
    {
        if(  g_ue866_status.cmd_id  == at_cmd_table[i].cmd_id  )
        {
            p = (ue866_operate_at_cmd_t *)&at_cmd_table[i];
            break;
        }
    }


    if(NULL != pstu )
    {
        ue866_operate_buf_send( p->at_cmd, strlen( (char *)p->at_cmd  ) );
        ret = UE866_RESULT_OK;
        pstu->try_times++;
        // get time
        pstu->last_operate_time_sec  = get_system_time();
        pstu->status = UE866_RESULT_WAIT;

        //   GPIO_PinModeSet ( GSM_TX_PORT, GSM_TX_PIN, gpioModeInput, 0 );
    }

    return ret;
}



int ue866_operate_init(void)
{
    int ret = 0;

    g_ue866_status.ue866_mode  = UE866_MODE_CFG;
    return ret;
}

UE866_RESULT ue866_operate_power_manager(void)
{
    UE866_RESULT  ret = UE866_RESULT_IDLE;
    UE866_MODE    ue866_mode = g_ue866_status.ue866_mode;

    // 判断是否在充电
    if( BATTERY_CHARGE == GsmSta.charging || MASK_POWER_STATUS_OFF  ==  GsmSta.gsm_p ||
        GsmSta.voltage < LOW_VOLTAGE && GsmSta.voltage > LOW_VOLTAGE_PROTECT  ||
        SYSTEM_OFF == StuKey.SystemState || g_ue866_status.sleep_times > 6 )
    {
        // 关闭mode
        if( UE866_ATCMD_ID_SYSHAL != g_ue866_status.cmd_id )
        {
            SETZERO(g_ue866_status);
            g_ue866_status.cmd_id = UE866_ATCMD_ID_SYSHAL;
            g_ue866_status.ue866_mode = ue866_mode;
            ret = UE866_RESULT_OK;
        }
        else
        {
            // power off mode now
            ///  must send power of cmd
            if( get_system_time() -  g_ue866_status.last_operate_time_sec > 10)
            {
                ue866_gpio_power(false);
                GsmSta.gsm_p = MASK_POWER_STATUS_OFF;// 0x01;

                if( SYSTEM_OFF != StuKey.SystemState)
                {
                    if( (get_system_time() -  g_ue866_status.last_operate_time_sec > 90) || GsmSta.askm2m > 0 )
                    {
                        g_ue866_status.last_operate_time_sec = get_system_time();
                        SETZERO(g_ue866_status);
                        GsmSta.gsm_p = MASK_POWER_STATUS_NOMAL;// 0x01;
                    }

                }

            }

            ret = UE866_RESULT_WAIT;
        }

        return ret;
    }

    //判断是否进入睡眠状态/及时检测唤醒标志
    if(  MASK_POWER_STATUS_SLEEP  ==  GsmSta.gsm_p &&
         0 == GsmSta.SendingLen && 0 == GsmSta.RevLen &&
         DATAS_IDLES == GsmSta.Sending )
    {

        if( UE866_ATCMD_ID_CFUN_FIVE != g_ue866_status.cmd_id )
        {
            SETZERO(g_ue866_status);
            g_ue866_status.cmd_id = UE866_ATCMD_ID_CFUN_FIVE;
            g_ue866_status.reserve_action = UE866_ACTION_SET;
            g_ue866_status.ue866_mode = ue866_mode;
            ret = UE866_RESULT_OK;
        }
        else
        {
            if( UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id &&   UE866_ACTION_HOLD ==   g_ue866_status.reserve_action)
            {
                //  已经进入了休眠状态
                ret = UE866_RESULT_OK;
                //  g_ue866_status.last_operate_time_sec  = get_system_time();
            }
            else
            {
                /// 正在发指令让模块进入休眠状态
                ret = UE866_RESULT_OK;
            }
        }

        return ret;
    }
    else if( MASK_POWER_STATUS_WAKEUP  ==  GsmSta.gsm_p  )
    {
        /*
            //检测是否要唤醒，否则继续休眠
            if( UE866_ATCMD_ID_CFUN_FIVE != g_ue866_status.cmd_id  )
            {
                // 异常，需要复位
                if(  ue866_gpio_reset() > 0 )
                {
                    ret = UE866_RESULT_WAIT;
                }
                else
                {
                    ret = UE866_RESULT_OK; // 复位成功 ; 相当于重新开机
                    SETZERO(g_ue866_status);
                    ue866_operate_reset_network_status();
                    //    g_ue866_status.cmd_id = UE866_ATCMD_ID_CFUN_FIVE;
                    ret = UE866_RESULT_WAIT;
                     ue866_operate_init ();
                    GsmSta.gsm_p = MASK_POWER_STATUS_NOMAL;
                }

            }
            else*/
        {
            // 唤醒
            //  SETZERO(g_ue866_status);
            g_ue866_status.ue866_mode = ue866_mode;
            g_ue866_status.cmd_id = UE866_ATCMD_ID_CFUN_FIVE;
            g_ue866_status.reserve_action = UE866_ACTION_START;
            ret = UE866_RESULT_OK;
            GsmSta.gsm_p = MASK_POWER_STATUS_NOMAL;

            ret = UE866_RESULT_OK;
        }

        return ret;
    }

    //判断开机是否成功  开机3秒后尝试 AT
    if( UE866_ATCMD_ID_START  == g_ue866_status.cmd_id &&
        UE866_MODE_START == g_ue866_status.ue866_mode  )
    {
        ret = UE866_RESULT_WAIT;

        if( 0 == g_ue866_status.last_operate_time_sec )
        {
            g_ue866_status.last_operate_time_sec = get_system_time();
            // g_ue866_status.ue866_mode  = UE866_MODE_CFG;
            ret = UE866_RESULT_WAIT;
            GsmSta.gsm_p = MASK_POWER_STATUS_NOMAL;// 0x01;
            ue866_gpio_power(true);
        }
        else  if( get_system_time() -  g_ue866_status.last_operate_time_sec > 3 )
        {
            g_ue866_status.ue866_mode  = UE866_MODE_CFG;
            g_ue866_status.cmd_id = at_cmd_tbl_cfg_mode[0];// UE866_ATCMD_ID_AT;
            g_ue866_status.status = UE866_RESULT_WAIT;
            ret = UE866_RESULT_OK;
        }

        return ret;
    }

    return ret;
}



static UE866_RESULT ue866_operate_mode_cfg(void)
{
    u8 index = 0;

    if(  g_ue866_status.cmd_id > UE866_ATCMD_ID_START )
    {

        if(   UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id  &&
              g_ue866_status.sleep_times > 0  && UE866_ACTION_HOLD == g_ue866_status.reserve_action  )
        {
            //出错 每尝试5次之后，休息2*n  min,n[1,5]后再尝试，超过5之后重新复位上电
            if(  get_system_time()  - g_ue866_status.last_operate_time_sec < 120 * g_ue866_status.sleep_times )
            {
                return  g_ue866_status.status;
            }
        }

        switch(g_ue866_status.status)
        {
            case UE866_RESULT_OK:
                break;

            case UE866_RESULT_WAIT:
                break;

            case UE866_RESULT_ERR:
            case UE866_RESULT_TIME_OUT:

                // 异常处理，复位或是重启
                //    UE866_ATCMD_ID_ATZ0, UE866_ATCMD_ID_AT,UE866_ATCMD_ID_ATE,UE866_ATCMD_ID_COPS,
                if(  g_ue866_status.index <= 3 )
                {
                    if( g_ue866_status.sleep_times > 5 )
                    {
                        StuKey.SystemState = SYSTEM_OFF;
                        g_ue866_status.cmd_id = UE866_ATCMD_ID_SYSHAL;
                    }

                    g_ue866_status.last_operate_time_sec = 0;
                }
                else
                {
                    g_ue866_status.cmd_id  =  UE866_ATCMD_ID_CFUN_FIVE;
                    ue866_operate_set_sleep (true);

                    if( g_ue866_status.sleep_times > 5 )
                    {
                        ///reboot;
                        NVIC_SystemReset();

                        while(1);
                    }

                    g_ue866_status.last_operate_time_sec = get_system_time();


                }

                g_ue866_status.sleep_times++;
                g_ue866_status.try_times = 0;
                g_ue866_status.ue866_mode =  UE866_MODE_CFG;
                g_ue866_status.status = UE866_RESULT_START;
                break;

            case UE866_RESULT_IDLE:

                if( UE866_ACTION_SET == g_ue866_status.reserve_action  &&  UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id )
                {
                    g_ue866_status.status = UE866_RESULT_START;
                    g_ue866_status.reserve_action = UE866_ACTION_HOLD;

                }
                else
                {
                    index = g_ue866_status.index + 1;

                    SETZERO (g_ue866_status);
                    g_ue866_status.ue866_mode =  UE866_MODE_CFG;

                    // g_ue866_status.last_operate_time_sec = get_system_time();
                    g_ue866_status.index  = index;
                    g_ue866_status.cmd_id = at_cmd_tbl_cfg_mode[g_ue866_status.index];


                }



                if( UE866_ATCMD_ID_CFG_END == g_ue866_status.cmd_id )
                {
                    SETZERO (g_ue866_status);
                    g_ue866_status.ue866_mode = UE866_MODE_NORMAL;  // 转到下一个模式
                }

                break;
        }

    }

    return g_ue866_status.status;
}

static UE866_RESULT ue866_operate_mode_normal(void)
{
    u8 index = 0;

    if(  g_ue866_status.cmd_id > UE866_ATCMD_ID_START )
    {

        if(   UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id  &&
              g_ue866_status.sleep_times > 0  && UE866_ACTION_HOLD == g_ue866_status.reserve_action  )
        {
            //出错 每尝试5次之后，休息2*n  min,n[1,5]后再尝试，超过5之后重新复位上电
            if(  get_system_time()  - g_ue866_status.last_operate_time_sec < 120 * g_ue866_status.sleep_times )
            {
                return  g_ue866_status.status;
            }
        }

        switch(g_ue866_status.status)
        {
            case UE866_RESULT_OK:
                break;

            case UE866_RESULT_WAIT:
                break;

            case UE866_RESULT_ERR:

                // 异常处理，复位或是重启
                //  while(1)
            {
                //       g_ue866_status.ue866_mode = UE866_MODE_ERR_RETRY;
            }

            SETZERO (g_ue866_status);
            ue866_operate_reset_network_status();

            g_ue866_status.ue866_mode = UE866_MODE_ERR_RETRY;  // 转到下一个模式
                //////////////////////////////////////////转到err retry ????  /////////////////////////////////////////////////////
            break;

            case UE866_RESULT_TIME_OUT:
                //    if( g_ue866_status.try_times > 5 )
            {
                SETZERO(g_ue866_status);
                g_ue866_status.ue866_mode =  UE866_MODE_NORMAL;
                ue866_operate_set_sleep(true);
                /*
                   g_ue866_status.cmd_id  =  UE866_ATCMD_ID_CFUN_FIVE;
                   g_ue866_status.last_operate_time_sec = get_system_time();
                   g_ue866_status.status = UE866_RESULT_START;
                   g_ue866_status.sleep_times++;
                   g_ue866_status.try_times = 0;

                   if( g_ue866_status.sleep_times > 5 )
                   {
                       NVIC_SystemReset();
                   }*/
            }
            break;

            case UE866_RESULT_IDLE:

                if( UE866_ACTION_SET == g_ue866_status.reserve_action  &&  UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id )
                {
                    g_ue866_status.status = UE866_RESULT_START;
                    g_ue866_status.reserve_action = UE866_ACTION_HOLD;

                }
                else
                {
                    index =  INCREASE(at_cmd_tbl_normal_mode, g_ue866_status.index );

                    SETZERO(g_ue866_status);
                    g_ue866_status.ue866_mode =  UE866_MODE_NORMAL;
                    // g_ue866_status.last_operate_time_sec = get_system_time();
                    g_ue866_status.index  = index;
                    g_ue866_status.cmd_id = at_cmd_tbl_normal_mode[index];
                }

                if(  OK == GsmSta.DateCome ||  GsmSta.RevLen > 0)
                {
                    g_ue866_status.index = 1;
                    g_ue866_status.cmd_id = UE866_ATCMD_ID_SRECV;
                    GsmSta.DateCome = OK;
                }


                if(  UE866_ATCMD_ID_NORMAL_END == g_ue866_status.cmd_id )
                {
                    if( 0 == GsmSta.SendingLen &&  0 == GsmSta.RevLen )
                    {
                        SETZERO (g_ue866_status);
                        g_ue866_status.ue866_mode = UE866_MODE_LOOP;  // 转到下一个模式
                    }
                    else
                    {
                        g_ue866_status.index  = 0;
                        g_ue866_status.cmd_id  = at_cmd_tbl_normal_mode[g_ue866_status.index ];
                    }
                }

                break;
        }

    }
    else
    {
        g_ue866_status.cmd_id = at_cmd_tbl_normal_mode[0];
    }

    return g_ue866_status.status;
}

static UE866_RESULT ue866_operate_mode_loop(void)
{
    u8 index = 0;

    if(  g_ue866_status.cmd_id > UE866_ATCMD_ID_START )
    {

        if(   UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id  &&
              g_ue866_status.sleep_times > 0  && UE866_ACTION_HOLD == g_ue866_status.reserve_action  )
        {
            //出错 每尝试5次之后，休息2*n  min,n[1,5]后再尝试，超过5之后重新复位上电
            if(  get_system_time()  - g_ue866_status.last_operate_time_sec < 120 * g_ue866_status.sleep_times )
            {
                return  g_ue866_status.status;
            }
        }

        switch(g_ue866_status.status)
        {
            case UE866_RESULT_OK:
                break;

            case UE866_RESULT_ERR:
                // 异常处理，复位或是重启
                //////////////////////////////////////////转到err retry ????  /////////////////////////////////////////////////////
                SETZERO (g_ue866_status);
                ue866_operate_reset_network_status();
                g_ue866_status.ue866_mode = UE866_MODE_ERR_RETRY;//  UE866_MODE_ERR_RETRY;  // 转到下一个模式

                break;

            case UE866_RESULT_WAIT:
                break;

            case UE866_RESULT_TIME_OUT:
                g_ue866_status.cmd_id  =  UE866_ATCMD_ID_CFUN_FIVE;
                g_ue866_status.last_operate_time_sec = get_system_time();
                g_ue866_status.status = UE866_RESULT_START;
                g_ue866_status.sleep_times++;
                g_ue866_status.try_times = 0;
                ue866_operate_set_sleep (true);
                GsmSta.askm2m = 0;

                if( g_ue866_status.sleep_times > 5 )
                {
                    ///reboot;
                    NVIC_SystemReset();
                }

                break;

            case UE866_RESULT_IDLE:

                if( UE866_ACTION_SET == g_ue866_status.reserve_action  &&  UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id )
                {
                    g_ue866_status.status = UE866_RESULT_START;
                    g_ue866_status.reserve_action = UE866_ACTION_HOLD;
                    g_ue866_status.last_operate_time_sec  = get_system_time();
                }
                else
                {
                    if(  UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id  && 0 == g_ue866_status.index  )
                    {
                        index = g_ue866_status.index;
                    }
                    else
                    {
                        index = g_ue866_status.index + 1;
                    }

                    /*
                                        if( UE866_ATCMD_ID_AGPSSND == g_ue866_status.cmd_id  )
                                        {
                                            // AGPSSTRING_ASK_MAX
                                            GsmSta.askm2m = 0; // 强制停止请求 进入休眠
                                        }
                    */
                    SETZERO (g_ue866_status);
                    g_ue866_status.ue866_mode = UE866_MODE_LOOP;
                    g_ue866_status.index  = index;
                    g_ue866_status.cmd_id = at_cmd_tbl_loop_mode[index];

                    if(  GsmSta.RevLen  > 0 ||    GsmSta.SendingLen > 0   )
                    {
                        g_ue866_status.ue866_mode = UE866_MODE_NORMAL;  // 转到下一个模式
                        g_ue866_status.index = 0;

                        if( MASK_POWER_STATUS_SLEEP  ==  GsmSta.gsm_p  )
                        {
                            ue866_operate_set_sleep (false);
                        }
                        else
                        {
                            g_ue866_status.cmd_id  = UE866_ATCMD_ID_START;
                        }

                    }
                    else if(  UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id &&
                              0 == GsmSta.askm2m && 0 == GsmSta.RevLen )
                    {
                        ue866_operate_set_sleep (true);
                    }

                    if( UE866_ATCMD_ID_LOOP_END == g_ue866_status.cmd_id  )
                    {
                        g_ue866_status.last_operate_time_sec = get_system_time();
                    }

                }
        }

    }
    else
    {
        g_ue866_status.cmd_id = at_cmd_tbl_loop_mode[0];
    }

    return g_ue866_status.status;
}

static UE866_RESULT ue866_operate_mode_err_retry(void)
{
    u8 index = 0;

    if(  g_ue866_status.cmd_id > UE866_ATCMD_ID_START )
    {

        switch(g_ue866_status.status)
        {
            case UE866_RESULT_OK:
                break;

            case UE866_RESULT_ERR:
            case UE866_RESULT_TIME_OUT:

                // 异常处理，复位或是重启
                //    NVIC_SystemReset();
                // g_ue866_status.index  = 0;
                if( UE866_ATCMD_ID_SD  == g_ue866_status.cmd_id )
                {
                    SETZERO (g_ue866_status);

                    if( GsmSta.operator_flag  > 0 )
                    {
                        g_ue866_status.index = 2;
                        ue866_operate_reset_network_status ( );
                        g_ue866_status.cmd_id = at_cmd_tbl_cfg_mode[g_ue866_status.index  ]; // UE866_ATCMD_ID_ATE
                    }

                    g_ue866_status.ue866_mode = UE866_MODE_CFG;
                }
                else
                {
                    g_ue866_status.cmd_id = at_cmd_tbl_err_and_retry_mode[g_ue866_status.index++  ];
                    g_ue866_status.last_operate_time_sec = get_system_time();
                    g_ue866_status.status = UE866_RESULT_START;
                    g_ue866_status.sleep_times++;
                    g_ue866_status.try_times = 0;

                    if( UE866_ATCMD_ID_ERR_RETRY_END == g_ue866_status.cmd_id )
                    {
                        ///reboot;
                        NVIC_SystemReset();
                    }
                }

                //      ue866_operate_set_sleep (true);
                GsmSta.askm2m = 0;



                break;

            case UE866_RESULT_WAIT:
                break;

            case UE866_RESULT_IDLE:
                index = g_ue866_status.index + 1;
                SETZERO (g_ue866_status);
                g_ue866_status.ue866_mode = UE866_MODE_ERR_RETRY;
                // g_ue866_status.last_operate_time_sec = get_system_time();
                g_ue866_status.index  = index;
                g_ue866_status.cmd_id = at_cmd_tbl_err_and_retry_mode[g_ue866_status.index  ];

                if( UE866_ATCMD_ID_SS  == g_ue866_status.cmd_id )
                {
                    SETZERO (g_ue866_status);
                    g_ue866_status.ue866_mode = UE866_MODE_NORMAL;
                }

                if( UE866_ATCMD_ID_ERR_RETRY_END  == g_ue866_status.cmd_id )
                {
                    //reboot
                    ue866_gpio_reset();
                }

                break;
        }

    }
    else
    {
        g_ue866_status.cmd_id = at_cmd_tbl_err_and_retry_mode[0];
    }

    return g_ue866_status.status;
}



static void ue866_operate_reset_network_status(void)
{
    GsmSta.IpOneConnect = NOT_OK ;
    GsmSta.SocketState   = 0;
    GsmSta.CSQ = 0;
    GsmSta.CREG = 0;
    GsmSta.CGREG = 0;
    GsmSta.RevLen = 0;
    GsmSta.asklen = 0;
    //  GsmSta.SendingLen
    GsmSta.Sending = DATAS_IDLES;

}


static UE866_RESULT ue866_operate_get_gsm_buf_data ( void )
{

    UE866_RESULT ret = UE866_RESULT_OK;
    STRUCT_BLIND_FRAM StuFram;


    if( DATAS_SENDING ==  GsmSta.Sending)
    {
        return  UE866_RESULT_WAIT;
    }

    if ( GsmSta.SendingLen > SENDING_BUF_LEN )
    {
        /*读缓冲数据*/
        GsmSta.SendingLen = 0;


    }

    if( OK == FlashBufRead ( &StuFram ) )
    {
        ReadNetDate ( OK, &StuFram );
    }


    if( GsmSta.SendingLen  > 0)
    {
        ret = UE866_RESULT_OK;
        //SETZERO (g_ue866_status);
        //g_ue866_status.try_times = 0;
        //  g_ue866_status.cmd_id = UE866_ATCMD_ID_SSENDEXT;
    }
    else
    {
        ret = UE866_RESULT_ERR;
    }

    return ret;
}



void    ue866_operate_set_sleep(bool bret)
{
    if( true == bret )
    {
        g_ue866_status.cmd_id = UE866_ATCMD_ID_CFUN_FIVE;
        GsmSta.gsm_p = MASK_POWER_STATUS_SLEEP;

    }
    else
    {
        g_ue866_status.cmd_id = UE866_ATCMD_ID_CFUN_FIVE;
        GsmSta.gsm_p = MASK_POWER_STATUS_WAKEUP;
    }
}

int  ue866_operate_get_sleep(void)
{

    return  GsmSta.gsm_p;//MASK_POWER_STATUS_WAKEUP;
}



static void ue866_operate_uart_reset (void)
{
    InitGsmQueue();
    ue866_gpio_dtr(false);
    ue866_gpio_rts(false);
    GPIO_PinModeSet ( GSM_TX_PORT, GSM_TX_PIN, gpioModeInput, 0 );
}

static void ue866_operate_dispatch( void )
{
    u8 i = 0;
    UE866_RESULT stu = UE866_RESULT_ERR;

    if( g_ue866_status.cmd_id >  UE866_ATCMD_ID_START  && g_ue866_status.cmd_id < UE866_ATCMD_ID_MAX )
    {

        for( i = 0 ; i <  sizeof(at_cmd_table) / sizeof(ue866_operate_at_cmd_t); i++ )
        {
            if(  g_ue866_status.cmd_id  == at_cmd_table[i].cmd_id  )
            {
                stu = at_cmd_table[i].pat_func(&g_ue866_status);
                //  stu = g_ue866_status.status;
                break;
            }
        }

    }
    else
    {
        if(g_ue866_status.cmd_id >= UE866_ATCMD_ID_MAX_2  )
        {
            SETZERO (g_ue866_status);// 异常状态处理
        }

    }

    if(  UE866_RESULT_WAIT != stu )
    {
        ue866_operate_uart_reset();
    }


}


void ue866_operate_manage_at(void)
{
    UE866_RESULT stu = UE866_RESULT_ERR;
    int len = 0;

    stu =  ue866_operate_power_manager();

    if( UE866_RESULT_WAIT ==  stu )
    {
        return;  // next step
    }

    // else if( UE866_RESULT_OK !=  stu )
    {
        //      return;
    }

    ue866_operate_get_gsm_buf_data();

    switch( g_ue866_status.ue866_mode )
    {
        case UE866_MODE_CFG:
            ue866_operate_mode_cfg();
            break;

        case UE866_MODE_NORMAL :
            ue866_operate_mode_normal ( );
            break;

        case UE866_MODE_LOOP :
            ue866_operate_mode_loop ( );

            if( UE866_ATCMD_ID_LOOP_END == g_ue866_status.cmd_id  )
            {
                if( get_system_time() - g_ue866_status.last_operate_time_sec  > 60 || 1 ==  GsmSta.askm2m )
                {
                    SETZERO (g_ue866_status);
                    g_ue866_status.ue866_mode = UE866_MODE_LOOP;
                    g_ue866_status.cmd_id = at_cmd_tbl_loop_mode[0];
                    g_ue866_status.last_operate_time_sec  = get_system_time();
                }
            }

            break;

        case  UE866_MODE_ERR_RETRY:
            ue866_operate_mode_err_retry ( );
            break;

            // default:
            //     ue866_operate_init();
            ///    break;
    }



    if( UE866_ACTION_HOLD == g_ue866_status.reserve_action  &&  UE866_ATCMD_ID_CFUN_FIVE == g_ue866_status.cmd_id )
    {
        if(1 ==  GsmSta.askm2m || GsmSta.SendingLen > 0 || GsmSta.RevLen > 0)
        {
            ue866_operate_set_sleep (false);
        }
        else if( (get_system_time() -  g_ue866_status.last_operate_time_sec)>3 &&  1 == GsmSta.SocketState )
        {
            g_ue866_status.cmd_id = UE866_ATCMD_ID_ATH;//at_cmd_tbl_loop_mode[0];
            g_ue866_status.ue866_mode = UE866_MODE_NORMAL;
            ue866_operate_reset_network_status();
            ue866_operate_dispatch();
        }

        ue866_gpio_dtr(false);
        ue866_gpio_rts(false);
        GPIO_PinModeSet ( GSM_TX_PORT, GSM_TX_PIN, gpioModeInput, 0 );

        return;
    }


    // 无数据或是操作，进入休眠状态
    ue866_operate_dispatch();
}



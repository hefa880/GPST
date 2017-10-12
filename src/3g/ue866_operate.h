#ifndef __UE866_OPERATE_H__
#define __UE866_OPERATE_H__


int ue866_operate_init(void);
void ue866_operate_set_sleep(bool bret);
int  ue866_operate_get_sleep(void);
void ue866_operate_status_print(void);
void ue866_operate_manage_at(void);


#endif
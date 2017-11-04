#ifndef __UE866_GPIO_H__
#define __UE866_GPIO_H__


void ue866_gpio_int(void ); //  
void ue866_gpio_uart_init(void );

int ue866_gpio_power(bool bEnable );  //  �������ǹر������Դ

int ue866_gpio_cts(void);
int ue866_gpio_rts(bool bEnable);
int ue866_gpio_dtr(bool bEnable);
int ue866_gpio_reset(void);  // ��λģ��
bool ue866_gpio_power_state(void);  //��ȡģ���ϵ�״̬


#endif

#ifndef _RTC_H
#define _RTC_H

extern volatile u8 Rtc;
#define SOURCE_FREQUENCY              1382400//32768
#define WAKEUP_INTERVAL_MS            100

#define RTC_COUNT_BETWEEN_WAKEUP 125540//138240  //((SOURCE_FREQUENCY * WAKEUP_INTERVAL_MS*x) / 1000)


void setupRtc(void);
void Delayms(volatile u32 ms);
#endif





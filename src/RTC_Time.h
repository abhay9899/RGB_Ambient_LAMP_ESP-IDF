#ifndef ETH_RTC_TIME_H_
#define ETH_RTC_TIME_H_

#include "stdio.h"
#include "string.h"
#include "time.h"
#include "stdint.h"
#include <sys/time.h>

#define SECONDS_OFFSET 0

//variables for alarm
uint8_t set_alarm_hour;
uint8_t set_alarm_min;
uint8_t set_alarm_sec;

uint8_t get_alarm_hour;
uint8_t get_alarm_min;
uint8_t get_alarm_sec;

void esp_rtc_init(void);
void read_time(void);
uint64_t check_date(int first_date[], int second_date[]);
void read_rtc_date_time(int *datetime);
void rtc_set_date(int *date);
uint64_t check_time(int first_time[], int last_time[]);
int64_t get_time_usec(void);
uint8_t get_alarm(void);
void set_alarm_time(uint8_t hour, uint8_t min, uint8_t sec);

#endif
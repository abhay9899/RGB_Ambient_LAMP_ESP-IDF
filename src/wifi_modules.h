#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "freertos/event_groups.h"
#include "esp_system.h"
#include "driver/twai.h"
#include "nvs_flash.h"
#include "driver/rmt.h"
#include "led_strip.h"

#include "freertos/timers.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_http_server.h"
#include "tcpip_adapter.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "RTC_Time.h"
#include "colors.h"

//for wled
#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define EXAMPLE_CHASE_SPEED_MS (10)
#define LED_NUMBER 56
#define LED_GPIO_NUM 33

#define PUSH_BUTTON_MODE  13

#define IDEAL 1
#define FLAME 2
#define RANDOM_GRADIENT 3
#define CLEAR 4

//for wifi
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

// #define ESP_WIFI_SSID      "abhay_wifi"                     //CONFIG_ESP_WIFI_SSID
// #define ESP_WIFI_PASS      "9811093836!"            //CONFIG_ESP_WIFI_PASSWORD
// #define ESP_WIFI_SSID      "Yatri"                      //CONFIG_ESP_WIFI_SSID
// #define ESP_WIFI_PASS      "yatri123456789"            //CONFIG_ESP_WIFI_PASSWORD

#define ESP_WIFI_SSID      "binodshrestha123"                     //CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS      "CLFBECD965"            //CONFIG_ESP_WIFI_PASSWORD

#define ESP_MAXIMUM_RETRY  5

uint8_t date_time_flag;
int day, month, year, second, minute, hour;
uint8_t wifiSuccessFlag; //for wifi conn success
uint8_t startUpFlag;

int calHour, calMin, calSec;

void wifi_init_sta(void);
void client_post_function(char _url[50], char _data[50], char _key[50]);
void client_get_datetime();

#endif
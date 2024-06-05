#include "wifi_modules.h"

static const char *TAG = "WiFi Config";

static int s_retry_num = 0;

////=====================================Event Handler for WiFi=========================================////
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    // printf("Inside WiFi Event Handler...\n\n");
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        if(s_retry_num < ESP_MAXIMUM_RETRY) 
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
            wifiSuccessFlag = 0;
        } 
        else 
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            s_retry_num = 0;
        }
        ESP_LOGI(TAG,"connect to the AP fail");
        startUpFlag = 1;
        date_time_flag = 0;
    } 
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        // wifiSuccessFlag = 1;
        ESP_LOGI(TAG,"WiFi Connection Successful!\n\n");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        client_get_datetime();
    }
}

////=====================================WiFi Initialization Function====================================////
void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        WIFI_EVENT_STA_DISCONNECTED,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .failure_retry_cnt = 5,

            .pmf_cfg = {
                .capable = true,
                .required = false,
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if(bits & WIFI_CONNECTED_BIT) 
    {
        ESP_LOGI(TAG, "connected to ap SSID: %s password: %s", ESP_WIFI_SSID, ESP_WIFI_PASS);
    } 
    else if(bits & WIFI_FAIL_BIT) 
    {
        ESP_LOGI(TAG, "Failed to connect to SSID: %s, password: %s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
        wifiSuccessFlag = 0;
    } 
    else 
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    // vEventGroupDelete(s_wifi_event_group);
    // esp_netif_deinit();
}

////=================================Handler and Function for HTTP Post================================////
esp_err_t client_post_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);

        if(strstr((char *)evt->data, "success"))
        {
            printf("SUCCESSFULLY POSTED!\n");      
        }
        else if(strstr((char *)evt->data, "Insufficient"))
        {
            printf("INSUFFICIENT USER BALANCE!\n"); 
        }
        else
        {
            printf("POSTING FAILED!\n");
        }
        break;

    default:
        break;
    }
    return ESP_OK;
}

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
        case HTTP_EVENT_ON_DATA:

        	printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
            printf("\n\n");
            // parsing for Date-Time value
            if (strstr((char *)evt->data, "datetime") && date_time_flag == 0)
            {
            	date_time_flag = 1;
                char *data1[15] = {0};
                int i1 = 0;
                char delim1[] = "\"";
                char *ptr1 = strtok(evt->data, delim1);
                while (ptr1 != NULL)
                {
                    data1[i1] = ptr1;
                    ptr1 = strtok(NULL, delim1);
                    i1++;
                }
                // printf("Data: %s\n", (uint8_t *)data1[11]);

                char *data2[5] = {0};
                int i2 = 0;
                char delim2[] = "T";
                char *ptr2 = strtok((char *)data1[11], delim2);
                while (ptr2 != NULL)
                {
                    data2[i2] = ptr2;
                    ptr2 = strtok(NULL, delim2);
                    i2++;
                }
                // printf("Data: %s\n", (uint8_t *)data2[0]);
                // uart_write_bytes(uart_port0, (uint8_t *)data2[2], strlen((char *)data2[2]));
                // uart_write_bytes(uart_port0, (uint8_t *)"\n", strlen("\n"));

                char *_data3[5] = {0};
                int _i3 = 0;
                char _delim3[] = "-";
                char *_ptr3 = strtok((char *)data2[0], _delim3);
                while (_ptr3 != NULL)
                {
                    _data3[_i3] = _ptr3;
                    _ptr3 = strtok(NULL, _delim3);
                    _i3++;
                }
                // printf("Date: %s, %s, %s\n", (uint8_t *)_data3[0], (uint8_t *)_data3[1], (uint8_t *)_data3[2]);
                year = atoi(_data3[0]);  // year
                month = atoi(_data3[1]); // month
                day = atoi(_data3[2]);   // day

                char *_data4[5] = {0};
                int _i4 = 0;
                char _delim4[] = ".";
                char *_ptr4 = strtok((char *)data2[1], _delim4);
                while (_ptr4 != NULL)
                {
                    _data4[_i4] = _ptr4;
                    _ptr4 = strtok(NULL, _delim4);
                    _i4++;
                }

                char *_data5[5] = {0};
                int _i5 = 0;
                char _delim5[] = ":";
                char *_ptr5 = strtok((char *)_data4[0], _delim5);
                while (_ptr5 != NULL)
                {
                    _data5[_i5] = _ptr5;
                    _ptr5 = strtok(NULL, _delim5);
                    _i5++;
                }
                // printf("Time: %s, %s, %s\n", (uint8_t *)_data5[0], (uint8_t *)_data5[1], (uint8_t *)_data5[2]);
                hour = atoi(_data5[0]);   // hour
                minute = atoi(_data5[1]); // minute
                second = atoi(_data5[2]); // second

                printf("Date is: %d, %d, %d  and  Time is: %d, %d, %d\n", year, month, day, hour, minute, second);
                wifiSuccessFlag = 1;

                //setting up RTC and rgb value only if internet connection is successful and time is retrived, else it will skip
                if(startUpFlag == 1)
		        {
			        //if connection successful, retrive time and start rtc
			        //retriving date and time
				    int curr_date[6] = {day, month, year, second, minute, hour};
				    rtc_set_date(curr_date);

				    //set alarm with time for resetting the system once a day
				    set_alarm_time(6, 0, 0); //6 am morning
				    set_color(calHour, calMin, calSec);
				    LEDState = IDEAL; //for time color setting
				    startUpFlag = 0;
				}
            }
            break;

        default:
            break;
    }
    return ESP_OK;
}


void client_post_function(char _url[50], char _data[50], char _key[50])
{
    esp_http_client_config_t config_post = {
        .url = _url,
        .method = HTTP_METHOD_POST,
        .auth_type = HTTP_AUTH_TYPE_BASIC,      /* need to edit sdkconfig to allow this! */
        .event_handler = client_post_handler};
 
    esp_http_client_handle_t client_post = esp_http_client_init(&config_post);

    char  *post_data = _data;
    esp_http_client_set_post_field(client_post, post_data, strlen(post_data));
    esp_http_client_set_header(client_post, "Content-Type", "application/json");
    esp_http_client_set_header(client_post, "key", _key);

    printf("Preparing Data to Post...\n\n");
    esp_http_client_perform(client_post);
    printf("\n");
    printf("Posting Completed...\n\n");
    esp_http_client_cleanup(client_post);
}

void client_get_datetime()
{
    printf("Inside get time utc clock\n\n");
    esp_http_client_config_t config_get = {
        .url = "http://worldtimeapi.org/api/timezone/Asia/Kathmandu",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler
    };

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_err_t ret = esp_http_client_perform(client);
    if (ret != ESP_OK) // if downloading failed condition
    {
        printf("Error getting Date-Time! Check Internet Connection.\n\n");
        wifiSuccessFlag = 0;
    }
    esp_http_client_cleanup(client);
}
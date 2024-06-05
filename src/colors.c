#include "wifi_modules.h"
#include "colors.h"
#include "RTC_Time.h"

static const char *TAG = "example";

uint8_t r_value = 0;
uint8_t g_value = 250;
uint8_t b_value = 200;
uint8_t incFlag = 0;

float f;
float bright = 1;
float f1;

uint8_t valRed = 0;
uint8_t valGreen = 0;
uint8_t valBlue = 255;


//color transitions throughout 24 hours
//for this to work, always calculate the rgb value at startup after time retrival from wifi for RGB value of the specific time to start with

////================================= function for generating color according to Real Time ================================////
void set_color(int hour, int min, int sec)
{
    int diffSec = 0;
    int bitChange = 0;
    int finalTotalSec = sec + (min + (hour * 60))* 60;

    // printf("Initial Time retrived: %d:%d:%d\n", hour, min, sec);
    if(hour >= 22 || hour < 6) //blue being dominant
    {
        if(hour >= 22 || hour < 2)
        {
            if(hour >= 22)
            {
                diffSec = finalTotalSec - (22 * 60 * 60);
            }
            
            else
            {
                diffSec = finalTotalSec + 7200; //plus 2 hours from 10 to 12
            }
            bitChange = diffSec / 56;
            red = 255;
            green = 0;
            printf("bit change: %d  hour: %d\n", bitChange, hour);
            if((bitChange == 255 || bitChange <= 1) && (hour > 22))
            {
                blue = 255;
            }
            else
            {
                blue = bitChange;
            }
            printf("inside between 22 and 2\n");
        }
        
        else 
        {
            diffSec = finalTotalSec - (2 * 60 * 60);
            bitChange = diffSec / 56;
            printf("bit change: %d  hour: %d\n", bitChange, hour);
            if(bitChange > 255 && hour > 2)
            {
                red = 0;
            }
            else
            {
                red = 255 - bitChange;  
            }
            green = 0;
            blue = 255;
            printf("inside between 2 and 6\n");
        } 
    }

    if(hour >= 6 && hour < 14) //green dominant
    {
        if(hour >= 6 && hour < 10)  
        {
            diffSec = finalTotalSec - (6 * 60 * 60);
            bitChange = diffSec / 56;
            red = 0;
            printf("bit change: %d  hour: %d\n", bitChange, hour);
            if((bitChange == 255 || bitChange <= 1) && hour > 6)
            {
                green = 255;
            }
            else
            {
                green = bitChange;
            }
            blue = 255;
            printf("inside between 6 and 10\n");
        }
        else
        {
            diffSec = finalTotalSec - (10 * 60 * 60);
            bitChange = diffSec / 56;
            red = 0;
            green = 255;
            printf("bit change: %d  hour: %d\n", bitChange, hour);
            if(bitChange > 255 && hour > 10)
            {

                blue = 0;
            }
            else
            {
                blue = 255 - bitChange;  
            }
            // blue = 255 - bitChange;
            printf("inside between 10 and 14\n");
        }
    }

    if(hour >= 14 && hour < 22) //red dominant 
    {
        if(hour >= 14 && hour < 18) 
        {
            diffSec = finalTotalSec - (14 * 60 * 60);
            bitChange = diffSec / 56;
            printf("bit change: %d  hour: %d\n", bitChange, hour);
            if((bitChange == 255 || bitChange <= 1) && hour > 14)
            {
                red = 255;
            }
            else
            {
                red = bitChange;
            }
            green = 255;
            blue = 0;
            printf("inside between 14 and 18\n");
        }
        else
        {
            diffSec = finalTotalSec - (18 * 60 * 60);
            bitChange = diffSec / 56;
            red = 255;
            printf("bit change: %d  hour: %d\n", bitChange, hour);
            if(bitChange > 255 && hour > 18)
            {

                green = 0;
            }
            else
            {
                green = 255 - bitChange;  
            }
            
            blue = 0;
            printf("inside between 18 and 22\n");
        }
    }
    printf("Red: %d  Green: %d  Blue: %d\n\n", red, green, blue);
    vTaskDelay(1);
}

////================================= random number generator for a single limit ================================////
uint8_t random_lim(uint8_t lim)
{
    uint8_t r = rand();
    r = (r * lim) >> 8;
    return r;
}

////================================= random number generator between two number limit ================================////
uint8_t random_range(uint8_t min, uint8_t lim)
{
    uint8_t delta = lim - min;
    uint8_t r = random_lim(delta) + min;
    return r;
}

////================================= changing color task ================================////
void change_color_task(void *pvParameters)
{
    while(1)
    {
        if(wifiSuccessFlag == 1)
        {
            get_alarm();
            set_color(calHour, calMin, calSec);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

////================================= Task for LED Status States ================================////
void status_led_task(void *pvParameters)
{
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(LED_GPIO_NUM, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(LED_NUMBER, (led_strip_dev_t)config.channel);
    led_strip_t *strip = led_strip_new_rmt_ws2812(&strip_config);
    if(!strip) 
    {
        ESP_LOGE(TAG, "install WS2812 driver failed");
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100)); 
    vTaskDelay(pdMS_TO_TICKS(50));
   
    while(1)
    {
        switch(LEDState)
        {
          case IDEAL:
                if(wifiSuccessFlag)
                {
                    for (int i = 0; i < LED_NUMBER; i++) 
                    {
                        ESP_ERROR_CHECK(strip->set_pixel(strip, i, bright * red, bright * green, bright * blue)); 
                    }
                    ESP_ERROR_CHECK(strip->refresh(strip, 50));
                    vTaskDelay(pdMS_TO_TICKS(50));
                }
                else
                {
                    for (int j = 1; j < 35; j++)
                    {
                        f = 0.025 * j;
                        for (int i = 0; i < LED_NUMBER; i++)
                        {
                            ESP_ERROR_CHECK(strip->set_pixel(strip, i, f * 255, f * 0, f * 0)); // turquoise color  strip
                        }
                        ESP_ERROR_CHECK(strip->refresh(strip, 50));
                        vTaskDelay(pdMS_TO_TICKS(50));
                    }

                    for (int j = 35; j >= 1; j--)
                    {
                        f = 0.025 * j;
                        for (int i = 0; i < LED_NUMBER; i++)
                        {
                            ESP_ERROR_CHECK(strip->set_pixel(strip, i, f * 255, f * 0, f * 0)); // strip
                        }
                        ESP_ERROR_CHECK(strip->refresh(strip, 50));
                        vTaskDelay(pdMS_TO_TICKS(50));
                    }
                    strip->clear(strip, 20);

            break;
                }
                break;

          case CLEAR:
                for (int i = 0; i < LED_NUMBER; i++) 
                {
                    ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0, 0, 0));
                }
                ESP_ERROR_CHECK(strip->refresh(strip, 10)); 
                strip->clear(strip, 20);
                break;

          case RANDOM_GRADIENT:
                if(incFlag == 0)
                {
                    r_value += 5;
                    g_value -= 5;
                    b_value += 1;
                    if ((r_value >= 250 || g_value <= 0) && b_value >= 250)
                    {
                        incFlag = 1;
                    }
                }
                if(incFlag == 1)
                {
                    r_value -= 5;
                    g_value += 5;
                    b_value -= 1;
                    if ((r_value <= 0 || g_value >= 250) && b_value <= 200)
                    {
                        incFlag = 0;
                    }
                }
                for (int i = 0; i < LED_NUMBER; i++) 
                {
                    ESP_ERROR_CHECK(strip->set_pixel(strip, i, r_value, g_value, b_value));
                }
                ESP_ERROR_CHECK(strip->refresh(strip, 50));
                vTaskDelay(pdMS_TO_TICKS(50));
                break;

            case FLAME: 
                for (int i = 0; i < LED_NUMBER; i++) 
                {
                    ESP_ERROR_CHECK(strip->set_pixel(strip, i, bright * random_range(230, 255), bright * random_range(40, 100), bright * random_range(0, 5)));
                }
                ESP_ERROR_CHECK(strip->refresh(strip, 50)); 
                vTaskDelay(pdMS_TO_TICKS(50));
                break;

            default:
                break;
        }
        vTaskDelay(1);
    }
    vTaskDelay(1); 
}
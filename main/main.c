/*
Created by Abhay Dutta, 14th Feb, 2024 

This project uses WS2812B RGB Addressable Strip with ESP32 module.
The main idea of this project is that it Glows color variants throughout 24 hour of the day, 
in which each 8 hour is assigned to a single color (R, G, B) to be the diominant color.
Here, Red is dominant from 14 -> 22 Hour
      Blue is dominant from 22 -> 6 Hour
      Green is dominant from 6 -> 14 Hour 
      The calculation of these RGB values according to the time is given in the function "set_color(int hour, int min, int sec)" in colors.c file

Time is retrived using WiFi and is fitted to internal RTC.
Untill the wifi connection is established, the light glows in breathing red pattern and meanwhile we can still chnage the modes even if the wifi connection is not established.
The light keeps breathing in red untill the wifi connection is established and time is retrived, and only then the light displays the specific color that its supposed to.
If the wifi connection is down in the middle, the system tries to reconnect to the wifi in the backgrond while the glow mode again turns to breathing red pattern and agian regains
the color combination of that time if the connection is establised properly.

This project also uses a push button as an input for changing the Glow Modes as needed.

This project has 4 Glow modes
1: IDEAL (this is the mode which displays the RBG according to the time of the day and red breathing pattern if the connection is down)  
2: FLAME (Simulation of a candle or flame insdie the lamp (P.S. this is my best flame representation in the limited time I had lol! I think its decent enough)) 
3: RANDOM_GRADIENT (just a random color gradient transition of color combinations I like lol!)
4: CLEAR (Turns off the light, works as a on/off switch without turning off the main power supply)

The system uses 12v as main power supply and requies a buck converter to voltage down to 5 volts for ESP32, where as the LED strip requires 12v from the main source in parallel to buck.
*/


#include "wifi_modules.h"
#include "colors.h"

////================================= Inout button task for changing glow modes ================================////
void input_button_task(void *pvParameters)
{
    uint8_t count = 1;
    while(1)
    {
        if (gpio_get_level(PUSH_BUTTON_MODE) == 1)
        {  
            vTaskDelay(5 / portTICK_PERIOD_MS);
            while(gpio_get_level(PUSH_BUTTON_MODE) != 0);    
            printf("MODE BUTTON PRESSED!\n");
            count = count + 1;
            if(count > 4)
            {
                LEDState = 1; 
                count = 1;     
            } 
            else
            {
                LEDState = count;
            }
            vTaskDelay(1);      
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}

////================================= Initializing tasks in App main ================================////
void app_main(void)
{
    date_time_flag = 0;
    startUpFlag = 1;
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //input button config 
    gpio_set_direction(PUSH_BUTTON_MODE, GPIO_MODE_INPUT);
    LEDState = IDEAL;

    //initializing wifi config
    wifi_init_sta();

    //initialize tasks
    xTaskCreate(status_led_task, "status_led_task", 4096, NULL, 2, NULL);
    xTaskCreate(change_color_task, "change_color_task", 4096, NULL, 1, NULL);
    xTaskCreate(input_button_task, "input_button_task", 2048, NULL, 2, NULL);
}


#ifndef COLORS_H_
#define COLORS_H_

uint8_t red, green, blue;
uint8_t LEDState;

void set_color(int hour, int min, int sec);
void change_color_task(void *pvParameters);
void status_led_task(void *pvParameters);

#endif
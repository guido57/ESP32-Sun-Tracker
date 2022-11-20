#include <Arduino.h>

enum _LED_status {
  _LED_status_idle,       
  _LED_status_sending,
  _LED_status_disconnected
};
typedef  _LED_status LED_status_t;

void LED_set_GPIO_pin(int GPIO_pin);

// use this to set the flashing mode
void LED_set_status(LED_status_t _LED_status);

// call this in your loop()
void LED_loop();

#include <LED.h>

//============================================
// status LED
//============================================
int gpio14_status = 0; // TMS in the silkscreen of ESP32 mini
int last_gpio14_status = 0;
int gpio_LED_pin = LED_BUILTIN ;

void LED_set_GPIO_pin(int GPIO_pin){
    pinMode(GPIO_pin,OUTPUT);
    gpio_LED_pin = GPIO_pin;
}

LED_status_t LED_status;

void LED_set_status(LED_status_t _LED_status){

    LED_status = _LED_status;
}


uint8_t LED_loop_count = 0;
unsigned long last_LED_loop_msecs = 0L;
void LED_loop(){
  if(millis() < last_LED_loop_msecs + 200) // the period is 200 msecs
    return;
  switch (LED_status)
  {
    case _LED_status_idle:
    // 1800 msec off - 200 msec on
    gpio14_status = ((LED_loop_count % 10) < 1) ? HIGH : LOW;
    break;
    case _LED_status_sending:
    // always sending. When sending, the loop is busy so this function won't be called
    gpio14_status = HIGH;
    break;
    case _LED_status_disconnected:
    // 200 msec on - 200 msec off
    gpio14_status = ((LED_loop_count % 2) == 0) ? HIGH : LOW;
    break;
  }
  digitalWrite(gpio_LED_pin, gpio14_status);
  last_gpio14_status = gpio14_status;
  LED_loop_count++;

  last_LED_loop_msecs = millis();
}

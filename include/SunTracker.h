#include <Arduino.h>

class SunTracker{

  protected:
    int gpio_rele1 = 14;
    int gpio_rele2 = 33;
    int gpio_pdr1 = 34;
    int gpio_pdr2 = 35;
    int gpio_runend1 = 32;
    int gpio_runend2 = 27;

    // parameters
    String status = "";
    bool lastrele = false; 

    long run_period = 3000; // 1 seconds
    long last_run = -1L;
    long last_rele_millis = -1;
    bool running = false;
    int max_error = 10; // 0 ... 100
    int sensitivity = 150;

  public:
    /*
      int gpio_rele1 = GPIO pin number for rele1 (the same for rele2 pdr1 pdr2 runend1 runend2)
      long run_period (milliseconds): how often the tracking is performed (default: 3000 milliseconds)
      int max_error (0-100): max % difference between abs(pdr1 - pdr2) and (pdr1+pdr2)/2 considered as a valid orientation (default: 30)
      int sensitivity(0-4095): min value considered enough light to activate tracking (default: 150)
    */
    SunTracker(int gpio_rele1, int gpio_rele2, int gpio_pdr1, int gpio_pdr2, int gpio_runend1, int gpio_runend2);
    SunTracker(int gpio_rele1, int gpio_rele2, int gpio_pdr1, int gpio_pdr2, int gpio_runend1, int gpio_runend2,
              long run_period, int max_error, int sensitivity);

    // get the light level (0 - 4095) of PDR1 o PDR2  
    uint16_t getPDR(int num);

    void setSensitivity(int sensitivity);
    void setMaxError(int _maxerr);
    void setRunPeriod(int _maxperiod_msecs);

    void loop();

   
};

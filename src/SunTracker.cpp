#include "SunTracker.h"

SunTracker::SunTracker(int _gpio_rele1, int _gpio_rele2, int _gpio_pdr1, int _gpio_pdr2, int _gpio_runend1, int _gpio_runend2 ){

  // store the GPIO pin numbers
  gpio_rele1 = _gpio_rele1;
  gpio_rele2 = _gpio_rele2;
  gpio_pdr1 = _gpio_pdr1;
  gpio_pdr2 = _gpio_pdr2;
  gpio_runend1 = _gpio_runend1;
  gpio_runend2 = _gpio_runend2;

  // set the mode for output pins
  pinMode(gpio_rele1, OUTPUT);
  pinMode(gpio_rele2, OUTPUT);
  
  // set the input mode with pullup for run end switches
  pinMode(gpio_runend1, INPUT_PULLUP);
  pinMode(gpio_runend2, INPUT_PULLUP );
    
}

uint16_t SunTracker::getPDR(int num){
      if(num == 1)
        return analogRead(gpio_pdr1) ;
      else if(num == 2)
        return analogRead(gpio_pdr2) ;
      else return 0;
}

void SunTracker::setSensitivity(int _sensitivity){
  if(_sensitivity >=0 && _sensitivity <=4095)
    sensitivity = _sensitivity;
}

void SunTracker::setMaxError(int _maxerr){
  if(_maxerr >=0 && _maxerr <=100)
    max_error = _maxerr;
}

void SunTracker::setRunPeriod(int _run_period_msecs){
  if(_run_period_msecs >0 )
     run_period = _run_period_msecs;

}


void SunTracker::loop(){

  if(millis()> last_rele_millis + 1){
      
      last_rele_millis = millis();
  
      // read pdr1 pdr2 runend1 runend2 
      uint16_t pdr1 = getPDR(1) ;
      uint16_t pdr2 = getPDR(2);
      uint16_t runend1 = digitalRead(gpio_runend1);
      uint16_t runend2 = digitalRead(gpio_runend2);
      uint16_t pdr_avg = (pdr1 + pdr2)/2;
      uint16_t pdr_err = (pdr_avg*max_error)/100;

      // TURN ON/OFF RELE1 AND RELE2
      if (running == false && millis() > last_run + run_period){
        running = true;
      }
      
      if(running){
        
        if( abs(pdr2-pdr1) < pdr_err || (pdr1 < sensitivity && pdr2 <sensitivity) ){
          Serial.printf("Stop turning. pdr1=%d pdr2=%d pdr_err=%d           \r",pdr1,pdr2,pdr_err); 
          running = false;
          digitalWrite(gpio_rele1, LOW); 
          digitalWrite(gpio_rele2, LOW); 
          status = "stop";
          last_run = millis();    
        }
        //   if PDR1 > PDR2 + max_error
        else if(pdr1 > pdr2 && runend1 == 1){
          Serial.printf("Clockwise                                          \r"); 
          digitalWrite(gpio_rele1, HIGH); 
          digitalWrite(gpio_rele2, LOW); 
          status = "cw";
        }else if (pdr1 < pdr2 && runend2 == 1){
          Serial.printf("Counter clockwise                                  \r"); 
          digitalWrite(gpio_rele1, LOW); 
          digitalWrite(gpio_rele2, HIGH); 
          status = "ccw";
      
        }
        
        if(runend1 == 0){
          Serial.printf("Got runend1 -> stop!                             \r");
          status ="re1";    
        }
        if(runend2 == 0){
          Serial.printf("Got runend2 -> stop!                              \r"); 
          status ="re2";    
        }

      }
      //Serial.printf("pdr1=%d pdr2=%d runend1=%d runend2=%d   \r",pdr1,pdr2,runend1, runend2); 
 
  }

}

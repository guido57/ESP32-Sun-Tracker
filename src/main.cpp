#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <AutoConnect.h>
#include "led.h"
#include "timezones.h"
#include "SunTracker.h"

// in InitSPIFS.cpp
void InitSPIFFS();

static const char settings_page[] PROGMEM = R"(
{
  "title": "Sun Tracker Settings",
  "uri": "/settings",
  "menu": true,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "value": "Sets the time zone to set the current local time.<br/>",
      "style": "font-family:Arial;font-weight:bold;text-align:center;margin-bottom:10px;color:DarkSlateBlue"
    },
    {
      "name": "timezone",
      "type": "ACSelect",
      "label": "Select TZ name",
      "option": [],
      "selected": 10
    },
    {
      "name": "newline",
      "type": "ACElement",
      "value": "<br>"
    },
    {
      "name": "caption2",
      "type": "ACText",
      "value": "Set the period between two consecutive runs, in milliseconds.<br/>",
      "style": "font-family:Arial;font-weight:bold;text-align:center;margin-bottom:10px;color:DarkSlateBlue"
    },
    {
      "name": "run_period",
      "type": "ACInput",
      "label": "Milliseconds to sleep between runs",
      "value": "3000",
      "apply": "number"
    },
    {
      "name": "newline",
      "type": "ACElement",
      "value": "<br>"
    },
    {
      "name": "max_error",
      "type": "ACInput",
      "label": "maximum % tracking error 0-100 to be considered a valid tracking",
      "value": "10",
      "apply": "number"
    },
    {
      "name": "newline",
      "type": "ACElement",
      "value": "<br>"
    },
    {
      "name": "sensitivity",
      "type": "ACInput",
      "label": "minimum light level to activate tracking 0 - 4095",
      "value": "150",
      "apply": "number"
    },
    {
      "name": "newline",
      "type": "ACElement",
      "value": "<br>"
    },
    {
      "name": "caption4",
      "type": "ACText",
      "value": "Set the daily start and stop time. To disable, set 00:00 on both.<br/>",
      "style": "font-family:Arial;font-weight:bold;text-align:center;margin-bottom:10px;color:DarkSlateBlue"
    },
    {
      "name": "start_time",
      "type": "ACInput",
      "label": "start time - hh:mm",
      "value": "7:00"
    },
    {
      "name": "newline",
      "type": "ACElement",
      "value": "<br>"
    },
    {
      "name": "stop_time",
      "type": "ACInput",
      "label": "stop time - hh:mm",
      "value": "18:00"
    },
    {
      "name": "newline",
      "type": "ACElement",
      "value": "<br>"
    },
    {
      "name": "settings_submit",
      "type": "ACSubmit",
      "value": "OK",
      "uri": "/settings_submit"
    }
  ]
}
)";

SunTracker st(14, 33, 34, 35, 32, 27);

WebServer Server;
AutoConnect       AcPortal(Server);
AutoConnectConfig AcConfig;       // Enable autoReconnect supported on v0.9.4
AutoConnectAux    ACAuxSettings;

const char* settingsDataFile = "/settings_data.json";

void rootPage() {
  String  content =
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<script type=\"text/javascript\">"
    "setTimeout(\"location.reload()\", 1000);"
    "</script>"
    "</head>"
    "<body>"
    "<h2 align=\"center\" style=\"color:blue;margin:20px;\">Hello, world</h2>"
    "<h3 align=\"center\" style=\"color:gray;margin:10px;\">{{DateTime}}</h3>"
    "<p style=\"text-align:center;\">Reload the page to update the time.</p>"
    "<p></p><p style=\"padding-top:15px;text-align:center\">" AUTOCONNECT_LINK(COG_24) "</p>"
    "</body>"
    "</html>";
  static const char *wd[7] = { "Sun","Mon","Tue","Wed","Thr","Fri","Sat" };
  struct tm *tm;
  time_t  t;
  char    dateTime[40];

  t = time(NULL);
  tm = localtime(&t);
  sprintf(dateTime, "%04d/%02d/%02d(%s) %02d:%02d:%02d.",
    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
    wd[tm->tm_wday],
    tm->tm_hour, tm->tm_min, tm->tm_sec);
  content.replace("{{DateTime}}", String(dateTime));
  Server.send(200, "text/html", content);
}

void SaveSettingsToFile(){
    // Open a parameter file on the SPIFFS.
    SPIFFS.begin();
    File settings_file = SPIFFS.open(settingsDataFile, "w");

    // Save "element" array of the listed parameters
    ACAuxSettings.saveElement(settings_file, { "timezone", "run_period", "max_error", "sensitivity", "start_time", "stop_time" });
    // Close the parameter file.
    settings_file.close();
    SPIFFS.end();
}


void print_settings_file() {
  SPIFFS.begin();
  File settings_data_file = SPIFFS.open(settingsDataFile, "r");
  while(settings_data_file.available()){
    Serial.write(settings_data_file.read());
  }
  settings_data_file.close(); 
  SPIFFS.end();
}

int getSelectedfromName(String timezone, int numoptions){

  uint8_t n = -1;
  for (n = 0; n < numoptions; n++) {
    String  tzName = String(TZ[n].zone);
    if (timezone.equalsIgnoreCase(tzName)) {
      break;
    }
  }
  if(n < numoptions)
    return n;
  else
    return -1;
}

void settingsSubmitPage() {
  // Get the "timezone" value from the page 
  // Values are accessed by the element name
  String  tz = Server.arg("timezone");
  Serial.printf("settingsSubmitPage: the selected timezone is %s\r\n", tz.c_str());
  // Scan all the timezone name to find the selected one (if any)  

  int selected_option_ndx = getSelectedfromName(tz, sizeof(TZ) / sizeof(Timezone_t));  
  if(selected_option_ndx >=0){
    String timezone = TZ[selected_option_ndx].zone;
    configTime(TZ[selected_option_ndx].tzoff * 3600, 0, TZ[selected_option_ndx].ntpServer);
    Serial.println("Time zone: " + timezone);
    Serial.println("ntp server: " + String(TZ[selected_option_ndx].ntpServer));
    //Store the new selected value into aux
    ACAuxSettings["timezone"].as<AutoConnectSelect>().selected = selected_option_ndx+1; 
  }    

  // get the run_period
  String  run_period_string = Server.arg("run_period");
  //Store the new selected value into aux
  ACAuxSettings["run_period"].as<AutoConnectInput>().value = run_period_string; 
  Serial.printf("settingSubmitPage: the run_period is %d\r\n", run_period_string.c_str());
  
  // get the max_error
  String  max_error_string = Server.arg("max_error");
  //Store the new selected value into aux
  ACAuxSettings["max_error"].as<AutoConnectInput>().value = max_error_string; 
  Serial.printf("settingSubmitPage: the max_error is %s\r\n", max_error_string.c_str());
  
  // get the sensitivity
  String  sensitivity_string = Server.arg("sensitivity");
  //Store the new selected value into aux
  ACAuxSettings["sensitivity"].as<AutoConnectInput>().value = sensitivity_string; 
  Serial.printf("settingSubmitPage: the sensitivity is %s\r\n", sensitivity_string.c_str());
  
  // get the start_time
  String  start_time_string = Server.arg("start_time");
  //Store the new selected value into aux
  ACAuxSettings["start_time"].as<AutoConnectInput>().value = start_time_string; 
  Serial.printf("settingSubmitPage: the start_time is %s\r\n", start_time_string.c_str());
  
  // get the stop_time
  String  stop_time_string = Server.arg("stop_time");
  //Store the new selected value into aux
  ACAuxSettings["stop_time"].as<AutoConnectInput>().value = stop_time_string; 
  Serial.printf("settingSubmitPage: the stop_time is %s\r\n", stop_time_string.c_str());
  
  // save the parameters eventually changed by the Settings page
  SaveSettingsToFile();
  Serial.println("after SaveSettingsFile, settings file is:");
  print_settings_file();

  // The /settings_submit page is for building timezone,
  // it then redirects to the root page without the content response.
  Server.sendHeader("Location", String("http://") + Server.client().localIP().toString() + String("/"));
  Server.send(302, "text/plain", "");
  Server.client().flush();
  Server.client().stop();
}


String onHello(AutoConnectAux& aux, PageArgument& args) {
   SPIFFS.begin();
   File settings_data_file = SPIFFS.open(settingsDataFile, "r");
   if(!settings_data_file){
    Serial.printf("the file '%s doesn't exist -> create it\r\n", settingsDataFile);
    SaveSettingsToFile();
    settings_data_file = SPIFFS.open(settingsDataFile, "r");
  }    
  ACAuxSettings.loadElement(settings_data_file);

  settings_data_file.close(); 
  Serial.printf("Loaded settings file: %s\r\n", settingsDataFile);
  SPIFFS.end();

  Serial.println("after onHello: print settings file");
  print_settings_file();


  return String();

}
 
void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  InitSPIFFS();

  Serial.println("setup: print_settings_file");
  print_settings_file();

  // Enable saved past credential by autoReconnect option,
  // even once it is disconnected.
  AcConfig.autoReconnect = false;
  AcConfig.portalTimeout = 1L;
  AcConfig.retainPortal = true;
  
  AcPortal.config(AcConfig);

  // Load aux Settings page and the handler to dynamically load its parameters
  ACAuxSettings.load(settings_page);
  ACAuxSettings.on(onHello);
 
  // Retrieve the select element that holds the time zone code and
  // register the zone mnemonics in advance.
  AutoConnectSelect&  tz = ACAuxSettings["timezone"].as<AutoConnectSelect>();
  for (uint8_t n = 0; n < sizeof(TZ) / sizeof(Timezone_t); n++) {
    tz.add(String(TZ[n].zone));
  }

  //load settings parameters from file 
  SPIFFS.begin();
  File settings_data_file = SPIFFS.open(settingsDataFile, "r");
  //if(true){
  if(!settings_data_file){
    Serial.printf("the file '%s doesn't exist -> create it\r\n", settingsDataFile);
    SaveSettingsToFile();
    settings_data_file = SPIFFS.open(settingsDataFile, "r");
  }    
  ACAuxSettings.loadElement(settings_data_file);
  settings_data_file.close(); 
  Serial.printf("Loaded settings file: %s\r\n", settingsDataFile);
  SPIFFS.end();

  // config the NTP server
  int selected_option_ndx =  ACAuxSettings["timezone"].as<AutoConnectSelect>().selected - 1; 
  String timezone = TZ[selected_option_ndx].zone;
  configTime(TZ[selected_option_ndx].tzoff * 3600, 0, TZ[selected_option_ndx].ntpServer);
  Serial.println("Time zone: " + timezone);
  Serial.println("ntp server: " + String(TZ[selected_option_ndx].ntpServer));
 
  AcPortal.join({ ACAuxSettings });        // Register aux. page

  // Set the web page handlers
  Server.on("/", rootPage); 
  Server.on("/settings_submit", settingsSubmitPage);   // Page handler to set the updated parameters 

  //  set LED on GPIO pin 2
  LED_set_GPIO_pin(2);
  //  start flashing LED rapidly
  LED_set_status(_LED_status_disconnected);

  // Establish a connection with an autoReconnect option.
  if (AcPortal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }else
    Serial.println("Not connected!");
 
}

String status = "";

void loop() {

  // change LED blinking according to WiFi connection status
  if(WiFi.isConnected()){
    // the LED is fixed light 
    LED_set_status(_LED_status_sending);
    if(status != "connected"){
      status = "connected";
      Serial.printf("status=%s\r\n", status.c_str());
      Serial.println("WiFi connected: " + WiFi.localIP().toString());
    }   
  }else{ // end WiFi.isConnected()
    // WiFi is not connected -> set the LED to blink rapidly
    LED_set_status(_LED_status_disconnected);
  
    if(status != "disconnected"){
      status = "disconnected";
      Serial.printf("Not connected. status=%s\r\n", status.c_str());
    }   
  }
  
  // get settings
  int run_period = ACAuxSettings["run_period"].as<AutoConnectInput>().value.toInt();
  int max_error = ACAuxSettings["max_error"].as<AutoConnectInput>().value.toInt();
  int sensitivity = ACAuxSettings["sensitivity"].as<AutoConnectInput>().value.toInt();
  String start_time_string = ACAuxSettings["start_time"].as<AutoConnectInput>().value;
  String stop_time_string = ACAuxSettings["stop_time"].as<AutoConnectInput>().value;
  
  // calculate if tracking at this time
  int start_time_minutes = 60 * start_time_string.substring(0,start_time_string.indexOf(":")).toInt();
  start_time_minutes += start_time_string.substring(start_time_string.indexOf(":")+1).toInt();
  int stop_time_minutes = 60 * stop_time_string.substring(0,stop_time_string.indexOf(":")).toInt();
  stop_time_minutes += stop_time_string.substring(stop_time_string.indexOf(":")+1).toInt();
  time_t t = time(NULL); 
  struct tm *tm = localtime(&t);
  int now_minutes = tm->tm_hour * 60 + tm->tm_min;
  
  Serial.printf("time_minutes start=%d now=%d stop=%d run_period=%d max_error=%d sensitivity=%d                             \r",start_time_minutes, now_minutes, stop_time_minutes, run_period, max_error, sensitivity);
  
  if( start_time_minutes != stop_time_minutes && ( now_minutes > start_time_minutes || now_minutes < stop_time_minutes ) ){
    // tracking_loop

    st.setMaxError(max_error);  
    st.setRunPeriod(run_period);
    st.setSensitivity(sensitivity);
    st.loop();
  } 
  
  AcPortal.handleClient();
  LED_loop();
}
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include "Lato_Heavy_26.h"
#include "Chewy_Regular_45.h"
#include "Arduino_JSON.h"
#include <time.h>


#define CHECK_IN_EVERY_SECS 180


#define ONBOARD_LED 2


#define TOUCH_THRESHOLD 30
#define TOUCH1 4 // GPIO 4 for touch 1
#define TOUCH2 27 // GPIO 27 for touch 2


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1




#define ssid "Jui er net"
#define password "passwordbolbonaa"

String base_url= "https://cdn-api.co-vin.in/api/v2/appointment/sessions/public/calendarByDistrict";
#define APD_DIST_CODE 710
#define COOCH_DIST_CODE 783



bool VACCINE_AVAILABLE= false;

/*****************************
********* For sound **********
******************************/
int freq = 2000;
int channel = 0;
int resolution = 8;





Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long utcOffsetInSeconds = 19800;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


int date= 0, month= 0, year= 0;
int hours= 3, minutes= 0, seconds= 0;
char days[7][4]= {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
int today= 0;
char ampm[3]= "am";
time_t rawtime;
struct tm *ti;


void Time() {
  timeClient.update();

  time_t rawtime = timeClient.getEpochTime();
  ti = localtime (&rawtime);
  year= ti->tm_year + 1900;
  month= ti->tm_mon+1;
  date= ti->tm_mday;
  Serial.print(date);
  Serial.print("  ");
  Serial.print(month);
  Serial.print("  ");
  Serial.print(year);
  Serial.print("  \n");
}

String get_time() {
  timeClient.update();
  hours= timeClient.getHours()>12? timeClient.getHours()-12 : timeClient.getHours();
  minutes= timeClient.getMinutes();
  seconds= timeClient.getSeconds();
  ampm[0]= timeClient.getHours()>12? 'p' : 'a';
  return String(hours)+":"+String(minutes)+":"+String(seconds)+" "+String(ampm);
}

String get_date() {
  timeClient.update();
  time_t rawtime = timeClient.getEpochTime();
  ti = localtime (&rawtime);
  year= ti->tm_year + 1900;
  month= ti->tm_mon+1;
  date= ti->tm_mday;
  return String(date)+"-"+String(month)+"-"+String(year);
}

String make_url(String dist_code) {
  return base_url+"?district_id="+dist_code+"&date="+get_date();
}


/*******************************************
***** Show WARNING through onboard led *****
*******************************************/
void warn() {
  digitalWrite(ONBOARD_LED, HIGH);
  delay(100);
  digitalWrite(ONBOARD_LED, LOW);
  delay(100);
  digitalWrite(ONBOARD_LED, HIGH);
  delay(100);
  digitalWrite(ONBOARD_LED, LOW);
  delay(200);
}

/*******************************************
***** Show ERROR through onboard led *****
*******************************************/

void error() {
  while(1) {
    digitalWrite(ONBOARD_LED, HIGH);
    delay(500);
    digitalWrite(ONBOARD_LED, LOW);
    delay(100);
  }
}


int show_waiting_shift= 0;
void show_waiting(int clr= 0) {
  int x= SCREEN_WIDTH -10;
  int y= SCREEN_HEIGHT -10;

  int radius= 10;

  display.fillCircle(x, y, radius, BLACK);
  if(clr){
    show_waiting_shift=0;
    display.display();
    return;
  }

  for(int r=radius-show_waiting_shift; r>0; r-=5){
    display.drawCircle(x, y, r, WHITE);
  }
  show_waiting_shift = ++show_waiting_shift % 5;
  
  display.display();
}

void beep_weird() {
  ledcWriteTone(channel, 2000);
  delay(250);
  ledcWriteTone(channel, 1000);
  delay(200);
  ledcWriteTone(channel, 3000);
  delay(250);
  ledcWriteTone(channel, 700);
  delay(150);
  ledcWriteTone(channel, 2500);
  delay(150);
  ledcWriteTone(channel, 1500);
  delay(250);
  ledcWriteTone(channel, 400);
  delay(150);
  ledcWriteTone(channel, 1500);
  delay(200);

  ledcWriteTone(channel, 0);
  
}

void beep_check() {
  for(int i=100; i<4000; i+=200) {
    ledcWriteTone(channel, i);
    delay(25);
  }
  for(int i=4000; i>100; i-=250) {
    ledcWriteTone(channel, i);
    delay(15);
  }
  ledcWriteTone(channel, 0);
}

void contiuous_beep() {
  while(true) {
    display.fillRect(0, SCREEN_HEIGHT-15, SCREEN_WIDTH, 15, BLACK);
    display.fillRect(0, SCREEN_HEIGHT-15, SCREEN_WIDTH/2+2, 15, WHITE);
    display.display();
    for(int i=100; i<1000; i+=100) {
      ledcWriteTone(channel, i);
      delay(50);
    }
    display.fillRect(0, SCREEN_HEIGHT-15, SCREEN_WIDTH, 15, BLACK);
    display.fillRect(SCREEN_WIDTH/2-2, SCREEN_HEIGHT-15, SCREEN_WIDTH/2+2, 15, WHITE);
    display.display();
    for(int i=3000; i>2000; i-=100) {
      ledcWriteTone(channel, i);
      delay(50);
    }
  }
}





void check_vaccin_availablity(int dist_code) {
  if ((WiFi.status() == WL_CONNECTED)) {
 
    HTTPClient http;

    String url= make_url(String(dist_code));

    http.begin(url);
    int httpCode = http.GET();
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
//        Serial.println(httpCode);/
//        Serial.println(payload);/

          if(httpCode == 200) {
            JSONVar dat = JSON.parse(payload);

            if (JSON.typeof(dat) == "undefined") {
              Serial.println("Parsing data Failed");
              return;
            }
          
            if(!dat.hasOwnProperty("centers")){
              Serial.println("No centers field");
              return;
            }

            JSONVar centers_arr= dat["centers"];

            for(int center_id=0; center_id<centers_arr.length(); center_id++) {
              JSONVar center= centers_arr[center_id];
              

              if(!center.hasOwnProperty("sessions")) {
                Serial.println("No Sessions field");
                continue;
              }

              JSONVar sessions_arr= center["sessions"];

              for(int session_id=0; session_id<sessions_arr.length(); session_id++) {

                 JSONVar session= sessions_arr[session_id];

                 if(!session.hasOwnProperty("min_age_limit")) {
                  Serial.println("No min_age_limit field");
                  continue;
                }

                if(!session.hasOwnProperty("available_capacity_dose1")) {
                  Serial.println("No available_capacity_dose1 field");
                  continue;
                }

                int available_capacity_dose1= session["available_capacity_dose1"];
                int min_age_limit= session["min_age_limit"];

//                Serial.println(available_capacity_dose1);
//                Serial.println(min_age_limit);

                if(available_capacity_dose1 > 0 && min_age_limit <26) {
                  VACCINE_AVAILABLE= true;
                }
                
              }
              
            }
            
          }
          else {
            Serial.println("Got non-200 code...");
          }

      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end();
  }
}


void show_progressbar(int percent, int x= 0, int y= SCREEN_HEIGHT-10, int width= SCREEN_WIDTH) {
  display.fillRect(x, y+2, width, 4, BLACK);
  display.drawRect(x, y, width, 8, WHITE);
  display.fillRect(x+2, y+2, (width-4)*percent/100, 4, WHITE);
  display.display();
}


void setup() {
  
  Serial.begin(115200);
  pinMode(ONBOARD_LED,OUTPUT);
  delay(10);

  /**************************************
  ********* Set up display **************
  **************************************/
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    error();
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  /**************************************
  ********* Connect to WIFI *************
  **************************************/

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  display.setCursor(0,SCREEN_HEIGHT/4);
  display.print("Connecting WIFI");
  display.display();

  while(WiFi.status() != WL_CONNECTED) {
    warn();
    Serial.print(".");

    display.print(".");
    display.display();
  }
  display.println(" ");
  display.println("WIFI Connected");
  display.display();

  /*******************************************
  ********** Setup for NTP *******************
  *******************************************/

  timeClient.begin();

  display.println("NTP started");
  display.display();


  
  /*******************************************
  ********* Setup fTone PWM ******************
  *******************************************/

  ledcSetup(channel, freq, resolution);
  ledcAttachPin(12, channel);

  beep_check();


  /******************************************
  ********** END of setup *******************
  ******************************************/
  for(int i=0; i<30; i++) {
    show_waiting();
    delay(50);
  }
  show_waiting(1);

  
}

void loop() {
//  show_waiting(1);

//  display.fillRect(0,0,SCREEN_WIDTH, 10, WHITE);
//  display.clearDisplay();
  display.fillRect(0,0,SCREEN_WIDTH, SCREEN_HEIGHT-10, BLACK);
  display.drawRect(0, 10, SCREEN_WIDTH, 2, WHITE);
  display.setCursor(5,0);
  display.print(get_time());
  display.display();

  display.setCursor(0, SCREEN_HEIGHT/4);
  
  check_vaccin_availablity(APD_DIST_CODE);
  if(VACCINE_AVAILABLE) {
    display.println("Vaccine found APD");
    display.display();
    contiuous_beep();
  }
  else {
    display.println("NO Vaccine APD");
    display.display();
  }
  
  check_vaccin_availablity(COOCH_DIST_CODE);
  if(VACCINE_AVAILABLE) {
    display.println("Vaccine found COOCH");
    display.display();
    contiuous_beep();
  }
  else {
    display.println("NO Vaccine COOCH");
    display.display();
  }
  
  
  for(int i=0; i<=100; i+=10) {
    show_progressbar(i);
    delay(10);
  }
  for(int i=100; i>0; i--) {
    show_progressbar(i);
    delay(CHECK_IN_EVERY_SECS*10);
  }
}

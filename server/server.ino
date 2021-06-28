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
#include <time.h>

HTTPClient http;

#define ONBOARD_LED 2


#define TOUCH_THRESHOLD 30
#define TOUCH1 4 // GPIO 4 for touch 1
#define TOUCH2 27 // GPIO 27 for touch 2


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1




#define ssid "Jui er net"
#define password "passwordbolbonaa"







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
  int x= SCREEN_WIDTH -13;
  int y= SCREEN_HEIGHT -13;

  int radius= 10;

  display.fillCircle(x, y, radius+3, BLACK);
  if(clr){
    show_waiting_shift=0;
    return;
  }

  for(int r=radius+show_waiting_shift; r>0; r-=3){
    display.drawCircle(x, y, r, WHITE);
  }
  show_waiting_shift = ++show_waiting_shift % 3;
  
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
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin("http://jsonplaceholder.typicode.com/comments?id=10"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 
  for(int i=0; i<30; i++) {
    show_waiting();
    delay(50);
  }
  show_waiting(1);
}

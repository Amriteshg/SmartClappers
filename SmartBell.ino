//Code for Smart Clappers

#include <Arduino.h>
#include <string>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "time.h"
#include "FirebaseDatabase.h"
#include <SoftwareSerial.h>

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

const char *ssid = "";
const char *password = "";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

#define BUZZER_PIN 13
#define LED 2

#define bellRingTime 4000

bool bellRung = false;
int belltime = 0;

FirebaseDatabase db;

std::string NormalTable;
std::string InternalTable;
std::string CA4Table;
std::string SemesterTable;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  db.FBSetup(FIREBASE_HOST, FIREBASE_AUTH);
  // Normal Routine
  db.initTimeStr(NormalTable, "/Routine/Normal");
    // Internal Routine
  db.initTimeStr(InternalTable, "/Routine/Internal");
    // CA Routine
  db.initTimeStr(CA4Table, "/Routine/CA4");
    // Semester Routine
  db.initTimeStr(SemesterTable, "/Routine/Semester");

  // Configure Time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  Serial.println("Time Sync...");
  
  time_t now;
  
  while (true) {
    now = time(nullptr);
    if (now > 0) {
      break;
    }
    delay(1000);
	
  }
  
  Serial.println("Time synchronized");

  Serial.println("Initialization Done...");
  
  delay(3000);
}

void loop() {
  //Time Variable
  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
	
    return;
  }
  
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  db.ProcessTimeSlots();
  
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  
  // Display 12hr Time Format
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");
  
  int current_time = timeinfo.tm_hour + timeinfo.tm_min;
  
  if (db.getTime() == current_time){
	bellRung = true;
	belltime = millis();
	//to be replaced with a buzzer
	digitalWrite(LED, HIGH);
  }

  if (bellRung && millis() - belltime >= bellRingTime) {
    bellRung = false;
    //to be replaced with a buzzer
	digitalWrite(LED, LOW);
  }

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
  
}

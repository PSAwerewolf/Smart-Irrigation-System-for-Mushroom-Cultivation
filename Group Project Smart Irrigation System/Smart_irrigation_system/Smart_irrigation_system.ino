

#include "dht.h"
#include <Arduino_LSM6DS3.h>
#include <Firebase_Arduino_WiFiNINA.h>
//#include <FirebaseError.h>
//#include "FirebaseArduino.h"
#include <WiFiNINA.h>

#include <ArduinoMqttClient.h>

#define FIREBASE_HOST "irrigationsys-f31ac-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "xo074Yxzgw7YQdC8NPH93keVLHYxUQyHbRuQmpbx"
#define DATABASE_URL "smartirrigationsystem-a330f-default-rtdb.asia-southeast1.firebasedatabase.app"
//#define WIFI_SSID "Elvis"
//#define WIFI_PASSWORD "12345678"

//#define WIFI_SSID "Dialog 4G 983"
//#define WIFI_PASSWORD "QwertY#1234"

#define WIFI_SSID "STUDENTS"
#define WIFI_PASSWORD ""

//#define WIFI_SSID "harish"
//#define WIFI_PASSWORD "12345678"

FirebaseData firebaseData;

String path = "/Data";
String jsonStr;

dht DHT;
int Humidity_pin = A1;
int Moisture_pin = A0;
int Rain_pin = A2;
int Motor_pin = 7;
int Rain_dig = 5;

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting to WiFi...");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //pinMode(Rain_pin,OUTPUT);

  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);

  pinMode(Motor_pin,OUTPUT);
  //Firebase.reconnectWiFi(true);

  //Firebase.begin(FIREBASE_HOST, DATABASE_URL);

  //Check if Firebase connection is successful
  //if (Firebase.success()) {
  //  Serial.println("Firebase connected");
  //} else {
   // Serial.println("Firebase connection failed");
  //}

}

void motorOnOff(){

  bool motor = Firebase.getBool(firebaseData,"/motor_state");
  Serial.print("Motor_state :");
  Serial.println(motor);
  if (motor == 0){
    digitalWrite(Motor_pin,HIGH);
  }else{
    digitalWrite(Motor_pin,LOW);
  }
}

void pushdataDatabase(float Hum,float Temp,float SoilM,String RainL){

  if (Firebase.setFloat(firebaseData,"/Humidity", Hum)) {
      Serial.println(firebaseData.dataPath() + " = " + Hum);
    }
    if (Firebase.setFloat(firebaseData,"/Temperature", Temp)) {
      Serial.println(firebaseData.dataPath() + " = " + Temp);
    }
    if (Firebase.setFloat(firebaseData, "/Soil_Moisture", SoilM)) {
      Serial.println(firebaseData.dataPath() + " = " + SoilM);
    }
     if (Firebase.setString(firebaseData,"/Rain_data", RainL)) {
      
      Serial.println(firebaseData.dataPath() + " = " + RainL);
    } 
    else{    
      Serial.println("Firebase Not Connected   " + firebaseData.errorReason());
    }
}

float Humidity_sensor_hum(){
  DHT.read11(Humidity_pin);

  float Hum = DHT.humidity;
  float temp = DHT.temperature;

  Serial.print("Humidity :");
  Serial.print(DHT.humidity);
  Serial.print("%\n");
  

  

  return Hum;
}

float Humidity_sensor_temp(){
  DHT.read11(Humidity_pin);

  float Hum = DHT.humidity;
  float temp = DHT.temperature;

 
  

  Serial.print("Temperature :");
  Serial.print(DHT.temperature);
  Serial.print("C\n");
  delay(5000);

  return temp;
}

float Moisture_sensor(){
  int Moisture = analogRead(Moisture_pin);
  Serial.print("Soil Moisture :");
  Serial.println(Moisture);
  delay(2000);

  return Moisture;
}

String Rain_sensor(){
  int Rain = analogRead(Rain_sensor);
  int Rain_digit = digitalRead(Rain_dig);
  int outputValue = map(Rain, 0, 1023, 255, 0);

  Serial.print("Rain :");
  Serial.println(Rain);
  Serial.println(Rain_digit);
  delay(2000);
  String rain_msg;

  if(Rain > 250){
    rain_msg = "Sunny";
  }else if(Rain <250 && Rain > 50){
    rain_msg = "Cloudy";
  }else{
    rain_msg = "Rainy";
  }
  Serial.println(rain_msg);
  return rain_msg;
}
void loop() {
  float SoilM = Moisture_sensor();
  float Hum =Humidity_sensor_hum();
  float Temp =Humidity_sensor_temp();
  String RainL = Rain_sensor();
  motorOnOff();

  SoilM =(1024 - SoilM)/10;
  //pushdataDatabase(Hum,Temp,SoilM,RainL);
  pushdataDatabase(Hum,Temp,SoilM,RainL);
  
}

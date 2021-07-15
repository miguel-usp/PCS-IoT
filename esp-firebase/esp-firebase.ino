#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Keypad.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Wi-Fi Gauss"
#define WIFI_PASSWORD "miguelerebeca"

#define API_KEY "AIzaSyBOneWagWEe-JLdeFuAKjsafHx3WRJNgs0"
#define DATABASE_URL "https://projeto-pcs-default-rtdb.firebaseio.com"

#define USER_EMAIL "miguel.velasques@usp.br"
#define USER_PASSWORD "pcsiot2021"

#define TAM_PASS 10

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
    delay(200);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setStreamCallback(fbdo, streamCallback, streamTimeoutCallback);
  Firebase.beginStream(fbdo, "/portas/-MefAucAJHPw7IHZ4ga8/senha");
}

void loop()
{

}

void streamCallback(StreamData data)
{
  if (data.dataType() == "int")
    Serial.println(data.intData());
}

void streamTimeoutCallback(bool timeout)
{
  if(timeout){
    Serial.println("Stream timeout, resume streaming...");
  }  
}

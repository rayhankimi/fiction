#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Anak kos"
#define WIFI_PASS "22445588"

#define API_KEY "AIzaSyDCi4PCgn12zaD1yjJhtUW0u-_WRBEUU0Q"
#define DATABASE_URL "pringgaxi-default-rtdb.asia-southeast1.firebasedatabase.app"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
String uid;
String databasePath;
String statusPath;

const int LDRPin = 27;
const int ledPin = 14;
const float gama = 0.7;
const float rl10 = 50;
bool deter = false;

unsigned long prevMillis = 0;
unsigned long timerDelay = 180000;
int count = 0;

void sendBool(String path, bool kehalangGkTuh){
    if (Firebase.RTDB.setBool(&fbdo, path.c_str(), kehalangGkTuh)){
    Serial.print("Writing value: ");
    Serial.print (kehalangGkTuh);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);

    
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(1000);
    }
    Serial.println();
    Serial.println("Connected to Wi-Fi");
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    
    if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signed up successfully");
    signupOK = true;
     } else {
    Serial.printf("Firebase sign up failed: %s\n", config.signer.signupError.message.c_str());
    }

    fbdo.setResponseSize(4096);
    config.token_status_callback = tokenStatusCallback;
    config.max_token_generation_retry = 5;
    Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);

    Serial.println("Getting User UID");
    while ((auth.token.uid) == "") {
      Serial.print('.');
       delay(1000);
  }
    // Print user UID
    uid = auth.token.uid.c_str();
    Serial.print("User UID: ");
    Serial.println(uid);

    databasePath = "/UsersData/" + uid;
    statusPath = statusPath + "/deter";
}

void loop() {
    delay(100);
    int nilaiLDR = analogRead(LDRPin);
    nilaiLDR = map(nilaiLDR, 4095, 0, 1024, 0);
    float volt = nilaiLDR / 1024.0 * 5;
    float res = 2000 * volt / (1 - volt / 5);
    float lux = pow(rl10 * 1e3 * pow(10.0, gama) / res, (1.0 / gama));
    
    if (lux < 1000){
        deter = true;
    } else{
        deter = false;
    }

    if (Firebase.ready() && (millis() - prevMillis > timerDelay || prevMillis == 0)){
        prevMillis = millis();
        sendBool(statusPath, deter);
    } else{
        Serial.println("Firebase not ready");
        Serial.println("REASON: " + fbdo.errorReason());
        delay(3000);
    }
}

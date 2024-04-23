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

String uid;

String databasePath;
String ldrPath;

unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;

const int ldrPin = 33;
const float gama = 0.7;
const float rl10 = 50;

void initWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void sendFloat(String path, float value){
    if (Firebase.RTDB.setInt(&fbdo, path.c_str(), value)){
        Serial.println("LDR Value is : ");
        Serial.println(value);
        Serial.print(" On Path : ");
        Serial.println(path);
        Serial.println("PASSED");
        Serial.println("PATH " + fbdo.dataPath());
        Serial.println("TYPE " + fbdo.dataType());
    } else {
        Serial.println("Set int data failed");
        Serial.println("Error: " + fbdo.errorReason());
    }
}
void setup() {
    Serial.begin(115200);
    initWifi();

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    config.token_status_callback = tokenStatusCallback;

    Firebase.begin(&config, &auth);

    if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    }
    else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }
    config.token_status_callback = tokenStatusCallback;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    
    databasePath = "/UsersData/" + uid;
    ldrPath = databasePath + "/LDR";
}

void loop(){
    int ldrVal = analogRead(ldrPin);
    
    
    if (Firebase.ready() &&  (millis() - sendDataPrevMillis > 9999 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // Get latest sensor readings
    sendFloat(ldrPath, ldrVal);
  }
}

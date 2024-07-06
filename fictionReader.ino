#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define WIFI_SSID "WifiSSID"
#define WIFI_PASS "password"

#define API_KEY "yOurSecRetaPiKeY"
#define DATABASE_URL "YourFirebaseRTDB-rtdb.asia-southeast1.firebasedatabase.app"


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;
String databasePath;
String ldrPath;

unsigned long sendDataPrevMillis = 0;
unsigned long lastTrafficUpdate1 = 0;
unsigned long lastTrafficUpdate2 = 0;

bool sensorState = false;
int trafficRate = 1;
int interval = 1000; //EVERYTHING IN FIREBASE WILL REFRESH FOR EVERY MS YOU PUT IN INTERVAL

String noTraffic = "Jln Lancar";
String light = "Mct Ringan";
String medium = "Mct Sedang";
String heavy = "Mct Tinggi";
String severe = "Mct Parah";
String extreme = "Mct Eksrim";

String messageNoTraffic = "-";
String messageLight = "-";
String messageMedium = "Waspada";
String messageHeavy = "Hindari";
String messageSevere = "Hindari";
String messageExtreme = "Gunakan Jln Lain";

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

    lcd.init();
    lcd.backlight();
}


void loop() {
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getInt(&fbdo, "/UsersData/LDR")) {
      if (fbdo.dataType() == "int") {
        int ldrValue = fbdo.intData();
        if (ldrValue < 2000) {
          if (sensorState == true) {
            sensorState = false;
            Serial.println("No Traffic Detected");
          }
          if (trafficRate > 4){
          unsigned long currentMillis1 = micros();
          if (currentMillis1 - lastTrafficUpdate1 > 0) {
            lastTrafficUpdate1 = currentMillis1;
            trafficRate = trafficRate - 3; }
        }
        } else {
          if (sensorState == false) {
            sensorState = true;
            Serial.println("Sensor Blocked, Traffic Rate may Arise");
          }
          // Tambahkan delay sebelum menambah traffic rate agar sesuai dengan kebutuhan (10 detik)
          unsigned long currentMillis2 = micros();
          if (currentMillis2 - lastTrafficUpdate2 > 0) {
            lastTrafficUpdate2 = currentMillis2;
            trafficRate = trafficRate + 1; 
          }
        }
      }
    } else {
      Serial.println(fbdo.errorReason());
    }
  }


    // Menggunakan rentang nilai untuk menentukan traffic rate
    if (trafficRate >= 1 && trafficRate <= 30)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(noTraffic);
        lcd.setCursor(12,0);
        lcd.print(trafficRate);
        lcd.setCursor(0, 1);
        lcd.print(messageNoTraffic);
    }
    else if (trafficRate >= 31 && trafficRate <= 60)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(light);
        lcd.setCursor(12,0);
        lcd.print(trafficRate);
        lcd.setCursor(0, 1);
        lcd.print(messageLight);
    }
    else if (trafficRate >= 61 && trafficRate <= 120)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(medium);
        lcd.setCursor(12,0);
        lcd.print(trafficRate);
        lcd.setCursor(0, 1);
        lcd.print(messageMedium);
    }
    else if (trafficRate >= 121 && trafficRate <= 240)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(heavy);
        lcd.setCursor(12,0);
        lcd.print(trafficRate);
        lcd.setCursor(0, 1);
        lcd.print(messageHeavy);
    }
  else if (trafficRate >= 241 && trafficRate <= 600)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(severe);
        lcd.setCursor(12,0);
        lcd.print(trafficRate);
        lcd.setCursor(0, 1);
        lcd.print(messageSevere);
    }
    else if (trafficRate >= 601)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(extreme);
        lcd.setCursor(12,0);
        lcd.print(trafficRate);
        lcd.setCursor(0, 1);
        lcd.print(messageExtreme);
    }
    else
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error");
        lcd.setCursor(0, 1);
        lcd.print("Error");
    }
    delay(1000); // Delay tambahan untuk stabilisasi tampilan LCD
}

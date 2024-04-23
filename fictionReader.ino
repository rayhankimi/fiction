#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define WIFI_SSID "Anak kos"
#define WIFI_PASS "22445588"

#define API_KEY "AIzaSyDCi4PCgn12zaD1yjJhatUW0u-_WRBEUU0Q"
#define DATABASE_URL "pringgaxi-default-rtdb.asia-southeast1.firebasedatabase.app"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;
String databasePath;
String ldrPath;

unsigned long sendDataPrevMillis = 0;
unsigned long lastTrafficUpdate = 0;

bool sensorState = false;
int trafficRate = 1;
int interval = 10000; //EVERYTHING IN FIREBASE WILL REFRESH FOR EVERY MS YOU PUT IN INTERVAL

String noTraffic = "Jalan Lancar";
String light = "Macet Ringan";
String medium = "Macet Sedang";
String heavy = "Macet Berat";
String extreme = "Macet Parah";

String messageNoTraffic = "Silahkan Lewat";
String messageLight = "Tetap Waspada";
String messageMedium = "Waspada";
String messageHeavy = "Hindari";
String messageExtreme = "Gnkn Jln Lain";

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
  if (Firebase.ready() && (millis() - sendDataPrevMillis > interval || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getInt(&fbdo, "/UsersData/LDR")) {
      if (fbdo.dataType() == "int") {
        int ldrValue = fbdo.intData();
        if (ldrValue < 2000) {
          if (sensorState == true) {
            sensorState = false;
            Serial.println("No Traffic Detected");
          }
        } else {
          if (sensorState == false) {
            sensorState = true;
            Serial.println("Sensor Blocked, Traffic Rate may Arise");
          }
          // Tambahkan delay sebelum menambah traffic rate agar sesuai dengan kebutuhan (10 detik)
          unsigned long currentMillis = millis();
          if (currentMillis - lastTrafficUpdate >= 10000) {
            lastTrafficUpdate = currentMillis;
            trafficRate++; // Tambahkan 1 setiap 10 detik
          }
        }
      }
    } else {
      Serial.println(fbdo.errorReason());
    }
  }


    // Menggunakan rentang nilai untuk menentukan traffic rate
    if (trafficRate >= 1 && trafficRate <= 3)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(noTraffic);
        lcd.setCursor(0, 1);
        lcd.print(messageNoTraffic);
    }
    else if (trafficRate >= 4 && trafficRate <= 6)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(light);
        lcd.setCursor(0, 1);
        lcd.print(messageLight);
    }
    else if (trafficRate >= 7 && trafficRate <= 8)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(medium);
        lcd.setCursor(0, 1);
        lcd.print(messageMedium);
    }
    else if (trafficRate >= 9 && trafficRate <= 11)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(heavy);
        lcd.setCursor(0, 1);
        lcd.print(messageHeavy);
    }
    else if (trafficRate >= 12)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(extreme);
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
    Serial.print("Traffic Rate :");
    Serial.println(trafficRate);
    delay(1000); // Delay tambahan untuk stabilisasi tampilan LCD
}


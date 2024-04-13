#define ldrPin 2
int ledPin = 14;
const float gama = 0.7;
const float rl10 = 50;

unsigned long previousMillis = 0;  
unsigned long interval = 1000;     
unsigned long previousMessageMillis = 0;  
const unsigned long messageInterval = 10000;  // Message Sent Interval
const unsigned long kemacetanInterval = 10000;  // Interval of incrementation in traffic level
int waktuPerDetik = 0;             // time per second
int tingkatKemacetan = 1;          // Traffic Level
bool sensorTerhalang = false;      // State of the sensor

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(ledPin, OUTPUT);
}

void loop() {
  delay(10);
  int nilaiLDR = analogRead(ldrPin);
  nilaiLDR = map(nilaiLDR, 4095, 0, 1024, 0);
  float voltase = nilaiLDR / 1024.0 * 5;
  float resistansi = 2000 * voltase / (1 - voltase / 5);
  float kecerahan = pow(rl10 * 1e3 * pow(10.0, gama) / resistansi, (1.0 / gama)); 

  unsigned long currentMillis = millis();  // get time
  if (kecerahan < 1000) {
    if (!sensorTerhalang) {
      sensorTerhalang = true;  // status change in sensor
      Serial.println("Sensor terhalang");
    }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;  
      waktuPerDetik++;                 
      

      if (currentMillis - previousMessageMillis >= messageInterval) {
        previousMessageMillis = currentMillis;  
        Serial.print("Sensor terhalang selama: ");
        Serial.print(waktuPerDetik);   
        Serial.println(" detik");

    
        if (waktuPerDetik % 10 == 0) {
          tingkatKemacetan++; 
          Serial.print("Tingkat kemacetan: ");
          Serial.println(tingkatKemacetan);
        }
      }
    }
  } else {
    if (sensorTerhalang) {
      sensorTerhalang = false; 
      Serial.println("Sensor tidak terhalang");
      previousMillis = currentMillis;  
      waktuPerDetik = 0; 
      tingkatKemacetan = 1;
    }
  }
}

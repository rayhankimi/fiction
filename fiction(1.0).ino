#define ldrPin 2
int ledPin = 14;
const float gama = 0.7;
const float rl10 = 50;

unsigned long previousMillis = 0;  // Waktu sebelumnya dalam milidetik
unsigned long interval = 1000;     // Interval waktu per detik
unsigned long previousMessageMillis = 0;  // Waktu sebelumnya pesan dikirim
const unsigned long messageInterval = 10000;  // Interval pengiriman pesan (10 detik)
const unsigned long kemacetanInterval = 10000;  // Interval penambahan tingkat kemacetan (10 detik)
int waktuPerDetik = 0;             // Waktu per detik
int tingkatKemacetan = 1;          // Tingkat kemacetan
bool sensorTerhalang = false;      // Status sensor terhalang

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
  float kecerahan = pow(rl10 * 1e3 * pow(10.0, gama) / resistansi, (1.0 / gama)); // Perhatikan penggunaan angka desimal untuk tipe double

  unsigned long currentMillis = millis();  // Ambil waktu sekarang

  if (kecerahan < 1000) {
    if (!sensorTerhalang) {
      sensorTerhalang = true;  // Ubah status sensor menjadi terhalang
      Serial.println("Sensor terhalang");
    }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;  // Atur waktu sebelumnya menjadi waktu sekarang
      waktuPerDetik++;                 // Tambahkan waktu per detik satu per detik
      
      // Cek apakah sudah waktunya mengirim pesan
      if (currentMillis - previousMessageMillis >= messageInterval) {
        previousMessageMillis = currentMillis;  // Atur waktu sebelumnya pesan dikirim
        Serial.print("Sensor terhalang selama: ");
        Serial.print(waktuPerDetik);   // Tampilkan waktu per detik yang bertambah
        Serial.println(" detik");

        // Tampilkan tingkat kemacetan setiap 10 detik
        if (waktuPerDetik % 10 == 0) {
          tingkatKemacetan++;  // Tambahkan tingkat kemacetan setiap 10 detik
          Serial.print("Tingkat kemacetan: ");
          Serial.println(tingkatKemacetan);
        }
      }
    }
  } else {
    if (sensorTerhalang) {
      sensorTerhalang = false;  // Ubah status sensor menjadi tidak terhalang
      Serial.println("Sensor tidak terhalang");
      previousMillis = currentMillis;  // Reset waktu sebelumnya
      waktuPerDetik = 0;  // Reset waktu per detik
      tingkatKemacetan = 1; // Reset tingkat kemacetan
    }
  }
}

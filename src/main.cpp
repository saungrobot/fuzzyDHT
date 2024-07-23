#include <Arduino.h>
#include <Adafruit_Sensor.h> // Tambahkan library Adafruit Sensor
#include <DHT.h> // Tambahkan pustaka DHT
#include <Wire.h> // Tambahkan library Wire
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL69Prhmtje"
#define BLYNK_TEMPLATE_NAME "IoT"
#define BLYNK_PRINT Serial

#define DHTPIN D4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);


char auth[] = "mk0JrqsmyCLZ6ElAgxw4dL4-rkGLokDp"; // Ganti dengan token Blynk Anda
char ssid[] = "YOUR_SSID"; // Ganti dengan SSID WiFi Anda
char pass[] = "YOUR_PASSWORD"; // Ganti dengan password WiFi Anda

// Deklarasi variabel input dan ouput fuzzy
float suhu_z, kelembaban_z, pwm_z;
// Deklarasi fungsi pembulatan
int roundsuhu;
int roundkelembaban;

/* Bagian 1: Definisi Type Bentukan Variabel Fuzzy */
typedef struct suhu {
  float sangat_dingin;
  float dingin;
  float normal;
  float panas;
  float sangat_panas;
};
typedef struct kelembaban {
  float sangat_kering;
  float kering;
  float sedang;
  float basah;
  float sangat_basah;
};


/* Bagian 2 : Definisi Fungsi Himpunan Fuzzy */
float trap_mf (float a, float b, float c, float d, float x) {
  /* a,b,c, dan d adalah parameter trapesium */
  /* x adalah besaran yang ingin diketahui Fungsi Keanggotaan/Membership
    Function-nya */
  return max(min(min(((x - a) / (b - a)), ((d - x) / (d - c))), 1), 0);
}
float tri_mf(float a, float b, float c, float x) {
  /* a,b, dan c adalah parameter segitiga */
  /* x adalah besaran yang ingin diketahui Fungsi Keanggotaan/Membership
    Function-nya */
  return max(min(((x - a) / (b - a)), ( (c - x) / (c - b))), 0);
}




void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(D8, OUTPUT);

  // Blynk.begin(auth, ssid, pass); // Inisialisasi Blynk

  lcd.init();  // Inisialisasi LCD
  lcd.backlight(); // Nyalakan backlight LCD

  

 
}


void loop() {
   delay(2000); // Wait 2 seconds before next loop
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  roundkelembaban = round(h);
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  roundsuhu = round(t);


  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float kelembabanValue = roundkelembaban;
  // Read temperature as Celsius (the default)
  float suhuValue = roundsuhu;
  

  /* Bagian 3 : Input Crips */
  suhu Suhu;
  kelembaban Kelembaban;

    /* Bagian 4 : Fungsi Keanggotaan Himpunan Fuzzy dan Domainnya*/
  Suhu.sangat_dingin = trap_mf(0, 0, 5, 10, suhuValue);
  Suhu.dingin = trap_mf(5, 10, 20, 25, suhuValue);
  Suhu.normal = trap_mf(20, 25, 30, 35, suhuValue);
  Suhu.panas = trap_mf(30, 35, 40, 45, suhuValue);
  Suhu.sangat_panas = trap_mf(40, 45, 50, 50, suhuValue);

  Kelembaban.sangat_kering = trap_mf(0, 0, 10, 20, kelembabanValue);
  Kelembaban.kering = trap_mf(10, 20, 30, 40, kelembabanValue);
  Kelembaban.sedang = trap_mf(30, 40, 60, 70, kelembabanValue);
  Kelembaban.basah = trap_mf(60, 70, 80, 90, kelembabanValue);
  Kelembaban.sangat_basah = trap_mf(80, 90, 100, 100, kelembabanValue);

//Membuat fungsi keanggotaan output pwm kecepatan
// Fuzzy output sets for Fan Speed
float sangatPelan =0;
float pelan =25;
float sedangKecepatan =50;
float cepat =75;
float sangatCepat =100; 


  float bobot1 = 0, bobot2 = 0, bobot3 = 0, bobot4 = 0, bobot5 = 0;
  float bobot6 = 0, bobot7 = 0, bobot8 = 0, bobot9 = 0, bobot10 = 0;
  float bobot11 = 0, bobot12 = 0, bobot13 = 0, bobot14 = 0, bobot15 = 0;
  float bobot16 = 0, bobot17 = 0, bobot18 = 0, bobot19 = 0, bobot20 = 0;
  float bobot21 = 0, bobot22 = 0, bobot23 = 0, bobot24 = 0, bobot25 = 0;

/* Bagian 5 : Membuat Fuzzy Rule Base dan Proses Implikasi (MIN) */
 // (1) Jika Suhu Sangat Dingin dan Kelembaban Sangat Kering maka DP Lama
  if (Suhu.sangat_dingin > 0 && Kelembaban.sangat_kering > 0 ) {
    bobot1 = min(Suhu.sangat_dingin, Kelembaban.sangat_kering);
  }
  // (2) Jika Suhu Sangat Dingin dan Kelembaban Kering maka DP Menengah
  if (Suhu.sangat_dingin > 0 && Kelembaban.kering > 0 ) {
    bobot2 = min(Suhu.sangat_dingin, Kelembaban.kering);
  }
  // (3) Jika Suhu Sangat Dingin dan Kelembaban Sedang maka DP Singkat
  if (Suhu.sangat_dingin > 0 && Kelembaban.sedang > 0 ) {
    bobot3 = min(Suhu.sangat_dingin, Kelembaban.sedang);
  }
  // (4) Jika Suhu Sangat Dingin dan Kelembaban Basah maka DP Sangat Singkat
  if (Suhu.sangat_dingin > 0 && Kelembaban.basah > 0 ) {
    bobot4 = min(Suhu.sangat_dingin, Kelembaban.basah);
  }
  // (5) Jika Suhu Sangat Dingin dan Kelembaban Sangat Basah maka DP Sangat Singkat
  if (Suhu.sangat_dingin > 0 && Kelembaban.sangat_basah > 0 ) {
    bobot5 = min(Suhu.sangat_dingin, Kelembaban.sangat_basah);
  }
  // (6) Jika Suhu Dingin dan Kelembaban Sangat Kering maka DP Lama
  if (Suhu.dingin > 0 && Kelembaban.sangat_kering > 0 ) {
    bobot6 = min(Suhu.dingin, Kelembaban.sangat_kering);
  }
  // (7) Jika Suhu Dingin dan Kelembaban Kering maka DP Lama
  if (Suhu.dingin > 0 && Kelembaban.kering > 0 ) {
    bobot7 = min(Suhu.dingin, Kelembaban.kering);
  }
  // (8) Jika Suhu Dingin dan Kelembaban Sedang maka DP Menengah
  if (Suhu.dingin > 0 && Kelembaban.sedang > 0 ) {
    bobot8 = min(Suhu.dingin, Kelembaban.sedang);
  }
  // (9) Jika Suhu Dingin dan Kelembaban Basah maka DP Singkat
  if (Suhu.dingin > 0 && Kelembaban.basah > 0 ) {
    bobot9 = min(Suhu.dingin, Kelembaban.basah);
  }
  // (10) Jika Suhu Dingin dan Kelembaban Sangat Basah maka DP Sangat Singkat
  if (Suhu.dingin > 0 && Kelembaban.sangat_basah > 0 ) {
    bobot10 = min(Suhu.dingin, Kelembaban.sangat_basah);
  }
  // (11) Jika Suhu Normal dan Kelembaban Sangat Kering maka DP Sangat Lama
  if (Suhu.normal > 0 && Kelembaban.sangat_kering > 0 ) {
    bobot11 = min(Suhu.normal, Kelembaban.sangat_kering);
  }
  // (12) Jika Suhu Normal dan Kelembaban Kering maka DP Lama
  if (Suhu.normal > 0 && Kelembaban.kering > 0 ) {
    bobot12 = min(Suhu.normal, Kelembaban.kering);
  }
  // (13) Jika Suhu Normal dan Kelembaban Sedang maka DP Menengah
  if (Suhu.normal > 0 && Kelembaban.sedang > 0 ) {
    bobot13 = min(Suhu.normal, Kelembaban.sedang);
  }
  // (14) Jika Suhu Normal dan Kelembaban Basah maka DP Menengah
  if (Suhu.normal > 0 && Kelembaban.basah > 0 ) {
    bobot14 = min(Suhu.normal, Kelembaban.basah);
  }
  // (15) Jika Suhu Normal dan Kelembaban Sangat Basah maka DP Singkat
  if (Suhu.normal > 0 && Kelembaban.sangat_basah > 0 ) {
    bobot15 = min(Suhu.normal, Kelembaban.sangat_basah);
  }
  // (16) Jika Suhu Panas dan Kelembaban Sangat Kering maka DP Sangat Lama
  if (Suhu.panas > 0 && Kelembaban.sangat_kering > 0 ) {
    bobot16 = min(Suhu.panas, Kelembaban.sangat_kering);
  }
  // (17) Jika Suhu Panas dan Kelembaban Kering maka DP Sangat Lama
  if (Suhu.panas > 0 && Kelembaban.kering > 0 ) {
    bobot17 = min(Suhu.panas, Kelembaban.kering);
  }
  // (18) Jika Suhu Panas dan Kelembaban Sedang maka DP Lama
  if (Suhu.panas > 0 && Kelembaban.sedang > 0 ) {
    bobot18 = min(Suhu.panas, Kelembaban.sedang);
  }
  // (19) Jika Suhu Panas dan Kelembaban Basah maka DP Lama
  if (Suhu.panas > 0 && Kelembaban.basah > 0 ) {
    bobot19 = min(Suhu.panas, Kelembaban.basah);
  }
  // (20) Jika Suhu Panas & Kelembaban Sangat Basah maka DP Menengah
  if (Suhu.panas > 0 && Kelembaban.sangat_basah > 0 ) {
    bobot20 = min(Suhu.panas, Kelembaban.sangat_basah);
  }
  // (21) Jika Suhu Sangat Panas dan Kelembaban Sangat Kering maka DP Sangat Lama
  if (Suhu.sangat_panas > 0 && Kelembaban.sangat_kering > 0 ) {
    bobot21 = min(Suhu.sangat_panas, Kelembaban.sangat_kering);
  }
  // (22) Jika Suhu Sangat Panas dan Kelembaban Kering maka DP Sangat Lama
  if (Suhu.sangat_panas > 0 && Kelembaban.kering > 0 ) {
    bobot22 = min(Suhu.sangat_panas, Kelembaban.kering);
  }
  // (23) Jika Suhu Sangat Panas dan Kelembaban Sedang maka DP Sangat Lama
  if (Suhu.sangat_panas > 0 && Kelembaban.sedang > 0 ) {
    bobot23 = min(Suhu.sangat_panas, Kelembaban.sedang);
  }
  // (24) Jika Suhu Sangat Panas dan Kelembaban Basah maka DP Lama
  if (Suhu.sangat_panas > 0 && Kelembaban.basah > 0 ) {
    bobot24 = min(Suhu.sangat_panas, Kelembaban.basah);
  }
  // (25) Jika Suhu Sangat Panas dan Kelembaban Sangat Basah maka DP Lama
  if (Suhu.sangat_panas > 0 && Kelembaban.sangat_basah > 0 ) {
    bobot25 = min(Suhu.sangat_panas, Kelembaban.sangat_basah);
  }
  
  // ************************
  /* Bagian 6 : Defuzzyfikasi Rata-Rata Terbobot*/
  float weightedsum = (bobot1 * lama) + (bobot2 * menengah) + (bobot3 * singkat)
                      + (bobot4 * sangat_singkat) + (bobot5 * sangat_singkat) + (bobot6 * lama) +
                      (bobot7 * lama) + (bobot8 * menengah) + (bobot9 * singkat) + (bobot10 *
                          sangat_singkat) + (bobot11 * sangat_lama) + (bobot12 * lama) + (bobot13 *
                              menengah) + (bobot14 * menengah) + (bobot15 * singkat) + (bobot16 *
                                  sangat_lama) + (bobot17 * sangat_lama) + (bobot18 * lama) + (bobot19 * lama)
                      + (bobot20 * menengah) + (bobot21 * sangat_lama) + (bobot22 * sangat_lama) +
                      (bobot23 * sangat_lama) + (bobot24 * lama) + (bobot25 * lama);
  float sumofweight = bobot1 + bobot2 + bobot3 + bobot4 + bobot5 + bobot6 +
                      bobot7 + bobot8 + bobot9 + bobot10 + bobot11 + bobot12 + bobot13 + bobot14 +
                      bobot15 + bobot16 + bobot17 + bobot18 + bobot19 + bobot20 + bobot21 + bobot22
                      + bobot23 + bobot24 + bobot25;
  pwm_z = weightedsum / sumofweight;

 /* Bagian 7 : Display Serial Monitor*/
  Serial.println("\n\n\nInputs:");
  Serial.print("Kelembaban: ");
  Serial.println(kelembabanValue);
  Serial.print(" %, ");
  Serial.print("Suhu: ");
  Serial.println(suhuValue);
  Serial.print(" *C, ");
  Serial.print("PWM: ");
  Serial.println(pwm_z);
  Serial.print(" m/s");
  Serial.println(" ");
  
  
  // Konversi pwm_z ke nilai PWM 0-255
  int pwmValue = map(pwm_z, 0, 100, 0, 255);
  Serial.print("Nilai PWM: ");
  Serial.println(pwmValue);
  
  // Mengatur kecepatan kipas menggunakan PWM pada pin D8
  analogWrite(D8, pwmValue);
  
  // Blynk.run(); // Jalankan Blynk
  
  // // Kirim data ke Blynk
  // Blynk.virtualWrite(V0, kelembabanValue); // Kirim kelembaban ke virtual pin V0
  // Blynk.virtualWrite(V1, suhuValue); // Kirim suhu ke virtual pin V1
  // Blynk.virtualWrite(V2, kecepatanValue); // Kirim kecepatan kipas ke virtual pin V2

  // Tampilkan kelembaban dan suhu di LCD satu baris
  lcd.setCursor(0, 0);
  lcd.print("Rh:");
  lcd.print(kelembabanValue);
  lcd.print("T:");
  lcd.print(suhuValue);

  //lcd kecepatan kipas dan nilai pwm
  lcd.setCursor(0, 1); 
  lcd.print("Kec:");
  lcd.print(kecepatanValue);
  lcd.print("PWM:");
  lcd.print(pwmValue);
}
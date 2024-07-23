// Deklarasi Sensor DHT
#include <DHT.h>
#include <Adafruit_Sensor.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// Deklarasi LCD I2C
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
// Deklarasi variabel input dan ouput fuzzy
float suhu_z, kelembaban_z, durasi_z;
// Deklarasi fungsi pembulatan
int roundsuhu;
int roundkelembaban;
// Deklarasi pin output ke LED
int pindurasi = 4;

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
  pinMode(pindurasi, OUTPUT); // Set pindurasi sebagai OUTPUT
  Serial.begin(9600); // Menjalankan komunikasi serial dengan baud rate 9600
  dht.begin(); // Menjalankan sensor DHT
  lcd.init(); // Inisialisasi LCD I2C
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Suhu");
  lcd.setCursor(0, 1);
  lcd.print("Kelembaban");
  lcd.setCursor(0, 2);
  lcd.print("Durasi");
  lcd.setCursor(0, 3);
  lcd.print("Status");
  lcd.setCursor(11, 0);
  lcd.print(":");
  lcd.setCursor(11, 1);
  lcd.print(":");
  lcd.setCursor(11, 2);
  lcd.print(":");
  lcd.setCursor(11, 3);
  lcd.print(":");
  lcd.setCursor(18, 0);
  lcd.print(" C");
  lcd.setCursor(18, 1);
  lcd.print(" %");
  lcd.setCursor(18, 2);
  lcd.print(" s");
  delay(500);
}

void loop() {
  // Program di bawah ini agar sensor DHT11 membaca suhu dan kelembaban
  suhu_z = dht.readTemperature();
  roundsuhu = round(suhu_z); // Melakukan operasi pembulatan nilai suhu yang terbaca
  lcd.setCursor(13, 0);
  lcd.print(roundsuhu);
  
  kelembaban_z = dht.readHumidity();
  roundkelembaban = round(kelembaban_z); // Melakukan operasi pembulatan nilai kelembaban yang terbaca
  lcd.setCursor(13, 1);
  lcd.print(roundkelembaban);
  
  /* Bagian 3 : Input Crips */
  suhu Suhu;
  kelembaban Kelembaban;
  
  /* Bagian 4 : Fungsi Keanggotaan Himpunan Fuzzy dan Domainnya*/
  / Membuat fungsi keanggotaan input suhu /
  Suhu.sangat_dingin = tri_mf(0, 0, 10, roundsuhu);
  Suhu.dingin = tri_mf(5, 10, 20, roundsuhu);
  Suhu.normal = tri_mf(15, 20, 30, roundsuhu);
  Suhu.panas = tri_mf(25, 30, 37, roundsuhu);
  Suhu.sangat_panas = tri_mf(30, 40, 40, roundsuhu);
  / Membuat fungsi keanggotaan input kelembaban /
  Kelembaban.sangat_kering = trap_mf(0, 0, 10, 20, roundkelembaban);
  Kelembaban.kering = trap_mf(10, 20, 30, 40, roundkelembaban);
  Kelembaban.sedang = trap_mf(30, 40, 60, 70, roundkelembaban);
  Kelembaban.basah = trap_mf(60, 70, 80, 90, roundkelembaban);
  Kelembaban.sangat_basah = trap_mf(80, 90, 100, 100, roundkelembaban);
  
  / Membuat fungsi keanggotaan output durasi penyiraman /
  float sangat_singkat = 0 ;
  float singkat = 6 ;
  float menengah = 12 ;
  float lama = 18 ;
  float sangat_lama = 25 ;
  
  float bobot1 = 0, bobot2 = 0, bobot3 = 0, bobot4 = 0, bobot5 = 0;
  float bobot6 = 0, bobot7 = 0, bobot8 = 0, bobot9 = 0, bobot10 = 0;
  float bobot11 = 0, bobot12 = 0, bobot13 = 0, bobot14 = 0, bobot15 = 0;
  float bobot16 = 0, bobot17 = 0, bobot18 = 0, bobot19 = 0, bobot20 = 0;
  float bobot21 = 0, bobot22 = 0, bobot23 = 0, bobot24 = 0, bobot25 = 0;
  /* Bagian 5 : Membuat Fuzzy Rule Base dan Proses Implikasi (MIN) */
  / DP = durasi penyiraman /
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
  durasi_z = weightedsum / sumofweight;

  
  /* Bagian 7 : Display Serial Monitor*/
  Serial.print("Suhu : ");
  Serial.print(roundsuhu);
  Serial.print(" *C, ");
  Serial.print("Kelembaban : ");
  Serial.print(roundkelembaban);
  Serial.print(" %, ");
  Serial.print("Durasi: ");
  Serial.print(durasi_z);
  Serial.print(" s");
  Serial.println(" ");
  
  /* Bagian 8 : Display LCD I2C 20x4 */
  if (durasi_z > 0) {
    digitalWrite(pindurasi, HIGH);
    lcd.setCursor(13, 2);
    lcd.print(durasi_z);
    lcd.setCursor(13, 3);
    lcd.print("Aktif");
    delay(durasi_z * 1000);
    digitalWrite(pindurasi, LOW);
    lcd.setCursor(13, 3);
    lcd.print(" ");
  }
  lcd.setCursor(13, 3);
  lcd.print("Mati");
  delay(2000);
}
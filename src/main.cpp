#include <Arduino.h>
#include <Fuzzy.h>
#include <Adafruit_Sensor.h> // Tambahkan library Adafruit Sensor
#include <DHT.h> // Tambahkan pustaka DHT
#include <Wire.h> // Tambahkan library Wire
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL69Prhmtje"
#define BLYNK_TEMPLATE_NAME "IoT"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

#define DHTPIN D4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);



// #define BLYNK_AUTH_TOKEN "mk0JrqsmyCLZ6ElAgxw4dL4-rkGLokDp"

char auth[] = "mk0JrqsmyCLZ6ElAgxw4dL4-rkGLokDp"; // Ganti dengan token Blynk Anda
char ssid[] = "YOUR_SSID"; // Ganti dengan SSID WiFi Anda
char pass[] = "YOUR_PASSWORD"; // Ganti dengan password WiFi Anda

// Create a Fuzzy object
Fuzzy *fuzzy = new Fuzzy();

// Fuzzy input sets for Humidity
FuzzySet *sangatKering = new FuzzySet(0, 0, 10, 20);
FuzzySet *kering = new FuzzySet(10, 20, 30, 40);
FuzzySet *sedangKelembaban = new FuzzySet(30, 40, 60, 70);
FuzzySet *basah = new FuzzySet(60, 70, 80, 90);
FuzzySet *sangatBasah = new FuzzySet(80, 90, 100, 100);

// Fuzzy input sets for Temperature
FuzzySet *sangatDingin = new FuzzySet(0, 0, 5, 10);
FuzzySet *dingin = new FuzzySet(5, 10, 20, 25);
FuzzySet *normal = new FuzzySet(20, 25, 30, 35);
FuzzySet *panas = new FuzzySet(30, 35, 40, 45);
FuzzySet *sangatPanas = new FuzzySet(40, 45, 50, 50);

// Fuzzy output sets for Fan Speed
FuzzySet *sangatPelan = new FuzzySet(0, 0, 0, 25);
FuzzySet *pelan = new FuzzySet(0, 25, 25, 50);
FuzzySet *sedangKecepatan = new FuzzySet(25, 50, 50, 75);
FuzzySet *cepat = new FuzzySet(50, 75, 75, 100);
FuzzySet *sangatCepat = new FuzzySet(75, 100, 100, 100);


void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(D8, OUTPUT);

  // Blynk.begin(auth, ssid, pass); // Inisialisasi Blynk

  lcd.init();  // Inisialisasi LCD
  lcd.backlight(); // Nyalakan backlight LCD

  // Setup Fuzzy Inputs
  FuzzyInput *kelembaban = new FuzzyInput(1);
  kelembaban->addFuzzySet(sangatKering);
  kelembaban->addFuzzySet(kering);
  kelembaban->addFuzzySet(sedangKelembaban);
  kelembaban->addFuzzySet(basah);
  kelembaban->addFuzzySet(sangatBasah);
  fuzzy->addFuzzyInput(kelembaban);
  
  FuzzyInput *suhu = new FuzzyInput(2);
  suhu->addFuzzySet(sangatDingin);
  suhu->addFuzzySet(dingin);
  suhu->addFuzzySet(normal);
  suhu->addFuzzySet(panas);
  suhu->addFuzzySet(sangatPanas);
  fuzzy->addFuzzyInput(suhu);
  
  // Setup Fuzzy Output
  FuzzyOutput *kecepatan = new FuzzyOutput(1);
  kecepatan->addFuzzySet(sangatPelan);
  kecepatan->addFuzzySet(pelan);
  kecepatan->addFuzzySet(sedangKecepatan);
  kecepatan->addFuzzySet(cepat);
  kecepatan->addFuzzySet(sangatCepat);
  fuzzy->addFuzzyOutput(kecepatan);
  
  // Define Fuzzy Rules
  FuzzyRuleAntecedent *ifSangatKeringAndSangatDingin = new FuzzyRuleAntecedent();
  ifSangatKeringAndSangatDingin->joinWithAND(sangatKering, sangatDingin);
  FuzzyRuleConsequent *thenSangatPelan = new FuzzyRuleConsequent();
  thenSangatPelan->addOutput(sangatPelan);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, ifSangatKeringAndSangatDingin, thenSangatPelan);
  fuzzy->addFuzzyRule(fuzzyRule1);
  
  FuzzyRuleAntecedent *ifKeringAndDingin = new FuzzyRuleAntecedent();
  ifKeringAndDingin->joinWithAND(kering, dingin);
  FuzzyRuleConsequent *thenPelan = new FuzzyRuleConsequent();
  thenPelan->addOutput(pelan);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, ifKeringAndDingin, thenPelan);
  fuzzy->addFuzzyRule(fuzzyRule2);

  FuzzyRuleAntecedent *ifSedangKelembabanAndNormal = new FuzzyRuleAntecedent();
  ifSedangKelembabanAndNormal->joinWithAND(sedangKelembaban, normal);
  FuzzyRuleConsequent *thenSedangKecepatan = new FuzzyRuleConsequent();
  thenSedangKecepatan->addOutput(sedangKecepatan);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, ifSedangKelembabanAndNormal, thenSedangKecepatan);
  fuzzy->addFuzzyRule(fuzzyRule3);

  FuzzyRuleAntecedent *ifBasahAndPanas = new FuzzyRuleAntecedent();
  ifBasahAndPanas->joinWithAND(basah, panas);
  FuzzyRuleConsequent *thenCepat = new FuzzyRuleConsequent();
  thenCepat->addOutput(cepat);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, ifBasahAndPanas, thenCepat);
  fuzzy->addFuzzyRule(fuzzyRule4);

  FuzzyRuleAntecedent *ifSangatBasahAndSangatPanas = new FuzzyRuleAntecedent();
  ifSangatBasahAndSangatPanas->joinWithAND(sangatBasah, sangatPanas);
  FuzzyRuleConsequent *thenSangatCepat = new FuzzyRuleConsequent();
  thenSangatCepat->addOutput(sangatCepat);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, ifSangatBasahAndSangatPanas, thenSangatCepat);
  fuzzy->addFuzzyRule(fuzzyRule5);

 
}


void loop() {
   delay(2000); // Wait 2 seconds before next loop
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();


  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float kelembabanValue = h;
  // Read temperature as Celsius (the default)
  float suhuValue = t;

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("°C ");
  
  Serial.println("\n\n\nInputs:");
  Serial.print("Kelembaban: ");
  Serial.println(kelembabanValue);
  Serial.print("Suhu: ");
  Serial.println(suhuValue);
  
  fuzzy->setInput(1, kelembabanValue);
  fuzzy->setInput(2, suhuValue);
  fuzzy->fuzzify();
  
  float kecepatanValue = fuzzy->defuzzify(1);
  
  Serial.print("Kecepatan Kipas: ");
  Serial.println(kecepatanValue);
  
  // Konversi kecepatan ke nilai PWM 0-255
  int pwmValue = map(kecepatanValue, 0, 100, 0, 255);
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
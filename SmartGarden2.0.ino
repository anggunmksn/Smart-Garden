// Blynk
#define BLYNK_TEMPLATE_ID "TMPL6PUU-biAJ"
#define BLYNK_TEMPLATE_NAME "Smart Garden"
#define BLYNK_AUTH_TOKEN "IlI-IkNMptFSlVBKgWFrl4gzZQ7WUC61"

// WiFi
char ssid[] = "Mas Anto";
char pass[] = "myhoney1971";
char auth[] = BLYNK_AUTH_TOKEN;

// memasukkan library
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "MQ135.h"
#include "DHT.h"

// definisikan pinout
// DHT11
#define pinDHT11 27
#define DHTTYPE DHT11
DHT dht(pinDHT11, DHTTYPE); 
// MQ135
#define pinMQ135 33
MQ135 mq135_sensor(pinMQ135);
// FC-28
#define pinFC28 34
// Buzzer
#define pinBuzzer 32
// Relay
#define pinRelay 14

// Blynk virtual pins
#define VPIN_TEMPERATURE V0
#define VPIN_HUMIDITY V1
#define VPIN_AIR_QUALITY V2
#define VPIN_SOIL_HUMIDITY V3
#define VPIN_WATER_PUMP V4

BlynkTimer timer;

void setup() {
    // inisialisasi serial
    Serial.begin(115200);
    // setup dht11
    dht.begin();
    // setup mq135
    pinMode(pinMQ135, INPUT);
    // setup kelembaban tanah
    pinMode(pinFC28, INPUT);
    // setup buzzer
    pinMode(pinBuzzer, OUTPUT);
    digitalWrite(pinBuzzer, LOW);
    // setup relay
    pinMode(pinRelay, OUTPUT);
    digitalWrite(pinRelay, HIGH);
    // setup blynk
    Blynk.begin(auth, ssid, pass);
    // Setup a function to be called every second
    timer.setInterval(1000L, bacaSensor);
}

void bacaSensor() {
    // baca sensor dht11
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // delay
    delay(1000);

    // baca sensor mq135
    int MQ135_Value = analogRead(pinMQ135);
    float MQ135_percentage = map(MQ135_Value, 0, 4095, 0, 100);

    Serial.print("MQ135 Value: ");
    Serial.print(MQ135_Value);
    Serial.print(" - Percentage: ");
    Serial.print(MQ135_percentage);
    Serial.println(" %");

    // logika untuk mengendalikan buzzer berdasarkan kualitas udara
    if (MQ135_percentage <= 40) {
    digitalWrite(pinBuzzer, LOW);
    Serial.println("Air Quality: Bersih");
    } else {
    digitalWrite(pinBuzzer, HIGH);
    Serial.println("Air Quality: Kotor");
    }

    // delay
    delay(1000);

    // baca sensor kelembaban tanah
    int FC28_Value = analogRead(pinFC28);  // Baca nilai dari sensor kelembaban tanah
    float FC28_percentage = map(FC28_Value, 0, 4095, 100, 0);

    Serial.print("Soil Moisture Value: ");
    Serial.print(FC28_Value);
    Serial.print(" - Percentage: ");
    Serial.print(FC28_percentage);
    Serial.println(" %");

    if (FC28_percentage <= 40) {
      Serial.println("Tanah Kering");
    } else {
      Serial.println("Tanah Lembab");
    }

    // delay
    delay(1000);

    // kirim data ke blynk
    Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);
    Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
    Blynk.virtualWrite(VPIN_AIR_QUALITY, MQ135_percentage);
    Blynk.virtualWrite(VPIN_SOIL_HUMIDITY, FC28_percentage);
  
    // terima perintah dari blynk
    BLYNK_WRITE(V4);

    // delay
    delay(1000);
}

// terima perintah dari blynk untuk mengendalikan pompa air
BLYNK_WRITE(V4) {
    int pinValue = param.asInt();
    if (pinValue == 1) {
      digitalWrite(pinRelay, LOW);
      Serial.println("Water pump Nyala");
    } else if (pinValue == 0) {
      digitalWrite(pinRelay, HIGH);
      Serial.println("Water pump Mati");
    }
    return;
}

void loop() {
    Blynk.run();
    timer.run();
}
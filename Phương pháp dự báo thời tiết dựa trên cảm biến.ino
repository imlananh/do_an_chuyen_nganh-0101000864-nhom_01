#include <DHT.h>
#include <WiFi.h>

// DHT Configuration
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// GPIO Configuration
#define RELAY1 2
#define RELAY2 15
#define LED_AUTO_MANUAL 4
#define BUTTON_AUTO_MANUAL 5
#define SOIL_SENSOR_PIN 34 // Soil moisture sensor pin

float Tc;  // Leaf temperature (manual input)
float Ta;  // Air temperature (from DHT11)
float VPD = 1.5;  // VPD value (can be changed)
float VPG = 2.0;  // VPG value (can be changed)

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Initialize pins
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(LED_AUTO_MANUAL, OUTPUT);
  pinMode(BUTTON_AUTO_MANUAL, INPUT);
}

// Function to read soil moisture
int readSoilMoisture() {
  int value = analogRead(SOIL_SENSOR_PIN);
  return value;
}

void loop() {
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Calculate dew point
  float dewPoint = temperature - (100 - humidity) / 5.0;

  // Display information on Serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C  Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Dew Point: ");
  Serial.print(dewPoint);
  Serial.println("°C");

  // Rain forecast based on dew point
  if (dewPoint >= temperature - 1 && dewPoint <= temperature + 1) {
    Serial.println("Warning: Dew point is close to temperature. There is a possibility of condensation and rain.");
  } else if (dewPoint < temperature - 2) {
    Serial.println("The air is too dry for condensation. Unlikely to rain.");
  } else {
    Serial.println("No significant weather change expected.");
  }

  // Read soil moisture
  int soilMoisture = readSoilMoisture();
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);

  delay(5000);  // Wait for 5 seconds before next reading
}

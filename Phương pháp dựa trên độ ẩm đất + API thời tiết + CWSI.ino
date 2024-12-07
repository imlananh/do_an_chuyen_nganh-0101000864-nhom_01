#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Cấu hình Wi-Fi
const char* ssid = "HUAWEIY9PRIME2019";
const char* password = "yesido08";

// Cấu hình DHT
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Cấu hình GPIO
#define RELAY1 2
#define RELAY2 15
#define LED_AUTO_MANUAL 4
#define BUTTON_AUTO_MANUAL 5
#define SOIL_SENSOR_PIN 34

// Biến trạng thái
bool autoMode = true;   // Chế độ Auto mặc định
bool pumpState = false; // Trạng thái bơm

// Hàm kết nối Wi-Fi
void connectToWiFi() {
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected!");
}

// Hàm đọc dữ liệu từ DHT
void readDHTData(float& temperature, float& humidity) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
    }
}

// Hàm đọc dữ liệu độ ẩm đất
int readSoilMoisture() {
    return analogRead(SOIL_SENSOR_PIN); // Đọc dữ liệu từ cảm biến
}

// Hàm đọc dữ liệu thời tiết từ API
String fetchWeatherData() {
    HTTPClient http;
    String url = String url = "http://api.weatherapi.com/v1/current.json?key=70776dedcb452d7cfebda26b6b167e53&q=10.7987,106.6369&aqi=no";  // Sử dụng API key của bạn

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        return http.getString();
    } else {
        Serial.print("Error fetching weather data. HTTP Code: ");
        Serial.println(httpCode);
        return "";
    }
}

// Hàm tính toán VPD (Vapor Pressure Deficit)
float calculateVPD(float temperature, float humidity) {
    float es = 0.6108 * exp((17.27 * temperature) / (temperature + 237.3)); // Áp suất hơi bão hòa
    float ea = es * (humidity / 100.0);                                     // Áp suất hơi thực tế
    return es - ea;                                                        // Chênh lệch áp suất hơi
}

// Hàm tính toán CWSI
float calculateCWSI(float Tc, float Ta, float VPD) {
    float Ta_L = 0.5 + 0.2 * VPD;  // Đường cơ sở thấp (giả định)
    float Ta_U = 1.0 + 0.3 * VPD;  // Đường cơ sở cao (giả định)
    return ((Tc - Ta) - Ta_L) / (Ta_U - Ta_L);  // Công thức tính CWSI
}

// Hàm logic mờ cho tưới tiêu
int fuzzyLogic(float CWSI, int soilMoisture) {
    if (CWSI < 0.2 && soilMoisture > 600) {
        return 0;  // Không tưới
    } else if ((CWSI >= 0.2 && CWSI < 0.6) || (soilMoisture > 300 && soilMoisture <= 600)) {
        return 1;  // Tưới vừa phải
    } else {
        return 2;  // Tưới nhiều
    }
}

// Điều khiển bơm nước
void controlWaterPump(int pumpCommand) {
    if (pumpCommand == 0 && pumpState) {  // Nếu không cần tưới và bơm đang bật
        Serial.println("Turning off water pump...");
        digitalWrite(RELAY1, LOW);  // Tắt bơm
        pumpState = false;          // Cập nhật trạng thái bơm
    } else if (pumpCommand > 0 && !pumpState) {  // Nếu cần tưới và bơm đang tắt
        Serial.println("Activating water pump...");
        digitalWrite(RELAY1, HIGH);  // Bật bơm
        pumpState = true;            // Cập nhật trạng thái bơm
    }
}

void setup() {
    // Khởi tạo Serial
    Serial.begin(115200);

    // Khởi tạo DHT
    dht.begin();

    // Khởi tạo GPIO
    pinMode(RELAY1, OUTPUT);
    pinMode(LED_AUTO_MANUAL, OUTPUT);
    pinMode(BUTTON_AUTO_MANUAL, INPUT_PULLUP);

    // Kết nối Wi-Fi
    connectToWiFi();

    Serial.println("System initialized!");
}

void loop() {
    // Đọc cảm biến độ ẩm đất
    int soilMoisture = readSoilMoisture();

    // Đọc nút Auto/Manual
    static bool lastButtonState = HIGH;
    bool currentButtonState = digitalRead(BUTTON_AUTO_MANUAL);

    if (lastButtonState == HIGH && currentButtonState == LOW) {
        autoMode = !autoMode;
        digitalWrite(LED_AUTO_MANUAL, autoMode ? HIGH : LOW);
        Serial.print("Mode changed to: ");
        Serial.println(autoMode ? "Auto" : "Manual");
    }
    lastButtonState = currentButtonState;

    if (autoMode) {
        // Lấy dữ liệu thời tiết từ API
        String weatherData = fetchWeatherData();
        if (weatherData != "") {
            // Phân tích JSON và lấy nhiệt độ và độ ẩm
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, weatherData);

            float temperature = doc["current"]["temp_c"];
            float humidity = doc["current"]["humidity"];

            Serial.print("Weather Temperature: ");
            Serial.print(temperature);
            Serial.print("C, Humidity: ");
            Serial.print(humidity);
            Serial.println("%");

            // Tính toán VPD
            float VPD = calculateVPD(temperature, humidity);

            // Nhập nhiệt độ tán lá từ người dùng
            Serial.println("Enter leaf temperature (Tc): ");
            while (Serial.available() == 0) {
                delay(100); // Chờ người dùng nhập
            }
            float Tc = Serial.parseFloat();

            // Tính CWSI
            float CWSI = calculateCWSI(Tc, temperature, VPD);

            // Logic mờ cho tưới tiêu
            int pumpCommand = fuzzyLogic(CWSI, soilMoisture);

            // Điều khiển bơm nước
            controlWaterPump(pumpCommand);
        } else {
            Serial.println("Failed to get weather data.");
        }
    } else {
        Serial.println("Manual mode active. Waiting for user control...");
    }

    delay(5000); // Chờ 5 giây trước khi lặp lại
}

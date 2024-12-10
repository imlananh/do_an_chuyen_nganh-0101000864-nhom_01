//Phương pháp dựa trên cây trồng + API
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Cấu hình DHT
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Cấu hình GPIO
#define RELAY1 2
#define RELAY2 15
#define LED_AUTO_MANUAL 4
#define BUTTON_AUTO_MANUAL 5
#define SOIL_SENSOR_PIN 34 // Chân cảm biến độ ẩm đất

// Thông tin mạng Wi-Fi
const char* ssid = "HUAWEIY9PRIME2019";
const char* password = "yesido08";

// API key OpenWeatherMap và địa chỉ API
const String API_KEY = "70776dedcb452d7cfebda26b6b167e53";
const float LAT = 10.8231;  // Vĩ độ
const float LON = 106.6297; // Kinh độ

float Tc;  // Nhiệt độ tán lá (nhập tay)
float Ta;  // Nhiệt độ không khí (đọc từ DHT11)
float VPD = 1.5;  // Giá trị VPD (có thể thay đổi)
float VPG = 2.0;  // Giá trị VPG (có thể thay đổi)

// Hàm đọc dữ liệu độ ẩm đất
int readSoilMoisture() {
  int value = analogRead(SOIL_SENSOR_PIN); // Đọc giá trị cảm biến độ ẩm
  return value;
}

// Hàm tính toán CWSI
float calculateCWSI(float Tc, float Ta) {
  float a = 1.0, b = 0.5;  // Các hằng số trong công thức
  float Ta_L = a + b * VPD;  // Đường cơ sở thấp
  float Ta_U = a + b * VPG;  // Đường cơ sở cao
  return ((Tc - Ta) - Ta_L) / (Ta_U - Ta_L);  // Công thức tính CWSI
}

// Hàm logic mờ cho tưới tiêu (dựa trên CWSI và độ ẩm đất)
int fuzzyLogic(float CWSI, int soilMoisture, float tempForecast, float humidityForecast, bool isRain) {
  // Kiểm tra dự báo thời tiết
  if (isRain) {
    return 0;  // Nếu có mưa, không cần tưới
  }

  if (tempForecast > 30.0 || CWSI > 0.6 || soilMoisture < 300) {
    return 2;  // Tưới nhiều nếu nhiệt độ cao hoặc CWSI > 0.6
  } else if ((CWSI >= 0.2 && CWSI < 0.6) || (soilMoisture >= 300 && soilMoisture <= 600)) {
    return 1;  // Tưới vừa phải
  } else {
    return 0;  // Không tưới nếu không cần thiết
  }
}

// Hàm lấy dữ liệu dự báo thời tiết từ OpenWeatherMap
void getWeatherForecast(float& tempForecast, float& humidityForecast, bool& isRain) {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + String(LAT) + "&lon=" + String(LON) + "&units=metric&appid=" + API_KEY;
  
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    // Phân tích dữ liệu JSON trả về
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    tempForecast = doc["list"][0]["main"]["temp"];  // Nhiệt độ dự báo
    humidityForecast = doc["list"][0]["main"]["humidity"];  // Độ ẩm dự báo
    isRain = doc["list"][0]["weather"][0]["main"].as<String>() == "Rain";  // Kiểm tra khả năng mưa
  } else {
    Serial.println("Error in API request");
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  dht.begin();  // Khởi tạo cảm biến DHT11
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(LED_AUTO_MANUAL, OUTPUT);  // Đèn báo chế độ tự động / thủ công
  pinMode(BUTTON_AUTO_MANUAL, INPUT_PULLUP);  // Nút chuyển chế độ tự động / thủ công

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Nhập nhiệt độ tán lá (Tc) từ người dùng qua Serial Monitor
  Serial.println("Enter leaf temperature (Tc):");
  while (Serial.available() == 0) {
    // Chờ người dùng nhập giá trị
  }
  Tc = Serial.parseFloat();  // Đọc giá trị nhập từ Serial

  // Đọc nhiệt độ không khí từ cảm biến DHT11
  Ta = dht.readTemperature();
  if (isnan(Ta)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    return;
  }

  // Đọc độ ẩm đất
  int soilMoisture = readSoilMoisture();

  // In thông tin
  Serial.print("Leaf temperature (Tc): ");
  Serial.println(Tc);
  Serial.print("Air temperature (Ta): ");
  Serial.println(Ta);
  Serial.print("Soil moisture: ");
  Serial.println(soilMoisture);

  // Tính toán CWSI
  float CWSI = calculateCWSI(Tc, Ta);
  Serial.print("CWSI: ");
  Serial.println(CWSI);

  // Lấy dự báo thời tiết
  float tempForecast, humidityForecast;
  bool isRain;
  getWeatherForecast(tempForecast, humidityForecast, isRain);

  Serial.print("Forecasted temperature: ");
  Serial.println(tempForecast);
  Serial.print("Forecasted humidity: ");
  Serial.println(humidityForecast);
  Serial.print("Rain forecast: ");
  Serial.println(isRain ? "Yes" : "No");

  // Quyết định tưới tiêu
  int action = fuzzyLogic(CWSI, soilMoisture, tempForecast, humidityForecast, isRain);
  if (action == 0) {
    Serial.println("No watering needed.");
    digitalWrite(RELAY1, LOW);  // Tắt bơm nước
    digitalWrite(RELAY2, LOW);
  } else if (action == 1) {
    Serial.println("Watering moderately.");
    digitalWrite(RELAY1, HIGH); // Bật bơm một phần
    digitalWrite(RELAY2, LOW);
  } else {
    Serial.println("Watering heavily.");
    digitalWrite(RELAY1, HIGH); // Bật cả hai relay
    digitalWrite(RELAY2, HIGH);
  }

  delay(5000);  // Chờ 5 giây trước khi đọc lại
}

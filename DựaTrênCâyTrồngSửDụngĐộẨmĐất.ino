//Phương pháp dựa trên cây trồng Sử dụng độ ẩm đất + CWSI, không xét thời tiết
#include <DHT.h>
#include <WiFi.h>

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
int fuzzyLogic(float CWSI, int soilMoisture) {
  if (CWSI < 0.2 && soilMoisture > 600) {
    return 0;  // Không tưới
  } else if ((CWSI >= 0.2 && CWSI < 0.6) || (soilMoisture > 300 && soilMoisture <= 600)) {
    return 1;  // Tưới một lượng vừa phải
  } else {
    return 2;  // Tưới nhiều
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();  // Khởi tạo cảm biến DHT11
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(LED_AUTO_MANUAL, OUTPUT);  // Đèn báo chế độ tự động / thủ công
  pinMode(BUTTON_AUTO_MANUAL, INPUT_PULLUP);  // Nút chuyển chế độ tự động / thủ công
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

  // Quyết định tưới tiêu
  int action = fuzzyLogic(CWSI, soilMoisture);
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


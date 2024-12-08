
/*************************************************************
  Download latest ERa library here:
    https://github.com/eoh-jsc/era-lib/releases/latest
    https://www.arduino.cc/reference/en/libraries/era
    https://registry.platformio.org/libraries/eoh-ltd/ERa/installation

    ERa website:                https://e-ra.io
    ERa blog:                   https://iotasia.org
    ERa forum:                  https://forum.eoh.io
    Follow us:                  https://www.fb.com/EoHPlatform
 *************************************************************/

// Enable debug console
// Set CORE_DEBUG_LEVEL = 3 first
#define ERA_DEBUG

#define DEFAULT_MQTT_HOST "mqtt1.eoh.io"

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "437cb40d-826f-4b7d-b0b9-f720066331c2"

#define SOIL_VERY_WET 3000    // Đất đủ ẩm, dừng tưới (giá trị cao)
#define SOIL_VERY_DRY 0      // Đất khô, cần tưới (giá trị thấp)

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>
#include "DHT.h"
#include <TimeLib.h>
#include <time.h>

const char ssid[] = "Le Gia System Lau1 2.5Ghz";  // Tên Wi-Fi của bạn
const char pass[] = "b123456789";  // Mật khẩu Wi-Fi của bạn

#define VN_TIMEZONE 7
#define TIMEZONE_OFFSET VN_TIMEZONE * 3600
#define NTP_SERVER "pool.ntp.org"

// Uncomment loại cảm biến bạn sử dụng, nếu DHT11 thì uncomment DHT11 và comment DHT22
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Kết nối
// DHT       | ESP32
//----------------
// VCC(1)    |  3.3V
// DATA(2)   |  D3
// NC(3)     |  x
// GND(4)    |  GND

// Kết nối chân 1 của DHT với 3.3V
// Nối trở 10k giữa chân 1 và chân 2
#define DHTPIN 14      // Chân DATA nối với chân D1
// Khởi tạo cảm biến
DHT dht(DHTPIN, DHTTYPE);
ERaTimer timer;
#define sensorPin 25    // Phao
#define floatSwitchPin 27  //Cam bien luu luong

#define led  4      //
#define button 5      // Auto/Manual

#define led1 16
#define button1 18     //Cong tac nao? ----> bao tinh trang hoatj dong DC tuoi

#define led2 17
#define button2 19      // Bao hoat dong DC bom

#define relay 2
#define relay1 15

#define SAMPLE_COUNT 25        // Tăng lên 25 mẫu
#define SAMPLE_DELAY 300       // Tăng lên 300ms  
#define THRESHOLD_HIGH 20      // Cần 20/25 mẫu HIGH
#define THRESHOLD_LOW 5 
#define DEBOUNCE_DELAY 3000    // Tăng lên 3 giây

#define MOTOR_PUMP 15    // Chân điều khiển relay cho động cơ bơm
#define MOTOR_WATER 2    // Chân điều khiển relay cho động cơ tưới

#define MOTOR_ON HIGH    // Trạng thái bật động cơ
#define MOTOR_OFF LOW    // Trạng thái tắt động cơ

volatile unsigned int pulseCount = 0;
float flowRate = 0.0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;
unsigned long oldTime = 0;

int value = 0;//giá trị ban đầu cảm biến mực nước
/* This function print uptime every second */
int a = 0;  // Giờ bật đèn
int b = 0;  // Phút bật đèn
int c = 0;  // Ngày
int d = 0;   // Tháng
int e = 0; // Năm
int f = 0;   // Số phút đèn sáng sau khi bật
int g = 0;   // Khoảng cách phút trước khi đèn sáng lại
int h = 0; // Số lần cần lặp lại (h = 0 thì không lặp lại)
int i = 0; 
int j = 0; 
int k = 0; 
int l = 0; 

boolean buttonState = HIGH;       // Trạng thái của nút nhấn (HIGH là không nhấn, LOW là nhấn)
boolean lastButtonState = HIGH;   // Trạng thái trước đó của nút nhấn
boolean toggleState = LOW;        // Trạng thái hiện tại của đèn (LOW là tắt, HIGH là bật)

boolean buttonState1 = HIGH;       // Trạng thái của nút nhấn (HIGH là không nhấn, LOW là nhấn)
boolean lastButtonState1 = HIGH;   // Trạng thái trước đó của nút nhấn
boolean toggleState1 = LOW;        // Trạng thái hiện tại của đèn (LOW là tắt, HIGH là bật)

boolean buttonState2 = HIGH;       // Trạng thái của nút nhấn (HIGH là không nhấn, LOW là nhấn)
boolean lastButtonState2 = HIGH;   // Trạng thái trước đó của nút nhấn
boolean toggleState2 = LOW;        // Trạng thái hiện tại của đèn (LOW là tắt, HIGH là bật)

int reading;
void timerEvent() {
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);  //Dung de lam gi?
}
ERA_WRITE(V1) { a = param.getInt(); }  // Giờ
ERA_WRITE(V2) { b = param.getInt(); }  // Phút  
ERA_WRITE(V3) { c = param.getInt(); }  // Ngày
ERA_WRITE(V4) { d = param.getInt(); }  // Tháng
ERA_WRITE(V5) { e = param.getInt(); }  // Năm
ERA_WRITE(V6) { f = param.getInt(); }  // Số phút tưới
ERA_WRITE(V7) { g = param.getInt(); }  // Khoảng tưới lại
ERA_WRITE(V8) { h = param.getInt(); }  // Số lần tưới
ERA_WRITE(V23){ l = param.getInt(); } // Lưu lượng

ERA_WRITE(V17) {
 struct tm timeinfo;
 if (!getLocalTime(&timeinfo)) {
     Serial.println("Không thể lấy thông tin thời gian");
     return;
 }
 
 i = param.getInt();
 if(i==1||i==0){
   toggleState = !toggleState;  
   ERa.virtualWrite(V18, toggleState);
   digitalWrite(led, toggleState);
 }

 if (toggleState == LOW) { // Chế độ Auto
   // Đảm bảo tắt tất cả đèn Manual và trạng thái
   digitalWrite(led, LOW);      // LED physical cho Auto/Manual
   digitalWrite(led1, LOW);     // LED physical cho bơm tưới 
   digitalWrite(led2, LOW);     // LED physical cho bơm nước
   digitalWrite(relay, LOW);    // Relay bơm tưới
   digitalWrite(relay1, LOW);   // Relay bơm nước
   
   // Reset trạng thái các nút bấm
   toggleState1 = LOW;
   toggleState2 = LOW;
   
   // Reset tất cả các giá trị hiển thị trên ERA về 0
   ERa.virtualWrite(V17, 0);  // Trạng thái chế độ Manual = 0
   ERa.virtualWrite(V18, 0);  // Chỉ thị Auto/Manual = 0  
   ERa.virtualWrite(V19, 0);  // Đèn Manual động cơ tưới = 0
   ERa.virtualWrite(V20, 0);  // Trạng thái bơm tưới = 0
   ERa.virtualWrite(V21, 0);  // Đèn Manual động cơ bơm = 0
   ERa.virtualWrite(V22, 0);  // Trạng thái bơm nước = 0
   
   Serial.println("Chế độ hiện tại: Auto - Tất cả đã reset về 0");

   // Kiểm tra có lịch đặt thời gian chưa
   if (!checkScheduleSet()) {
       return; // Chưa có lịch thì thoát
   }

   // Kiểm tra đúng thời gian đặt
   if ((timeinfo.tm_year + 1900) == e || e == -1) {
     if ((timeinfo.tm_mon + 1) == d || d == -1) {
       if (timeinfo.tm_mday == c || c == -1) {
         if (timeinfo.tm_hour == a || a == -1) {
           if (timeinfo.tm_min == b) {
             if(reading == HIGH && (4095-value) <= 1000) {
               digitalWrite(relay, HIGH);
               digitalWrite(led1, HIGH);
               ERa.virtualWrite(V20, HIGH);
             }
           }
         }
       }
     }
   }

   // Kiểm tra lưu lượng
   if(totalMilliLitres >= l){
     digitalWrite(relay, LOW);
     digitalWrite(led1, LOW);
     ERa.virtualWrite(V20, LOW);
     delay(3000);
     totalMilliLitres = 0;
   }

   // Logic dừng tưới sau thời gian f phút
   if (timeinfo.tm_min == b + f) {
     digitalWrite(relay, LOW);
     digitalWrite(led1, LOW);
     ERa.virtualWrite(V20, LOW);

     // Cập nhật chu kỳ tưới tiếp theo nếu còn
     if (h > 0) {
       b = b + g;  // Tăng thời gian theo khoảng tưới
       if(b > 59){
         b = b - 60;  // Reset phút về 0-59
         a++;         // Tăng giờ
       }
       h--;  // Giảm số lần tưới còn lại
     }
   }

   // Xử lý mực nước thấp
   if(reading == LOW){
       digitalWrite(relay1, HIGH);
       digitalWrite(led2, HIGH);
       ERa.virtualWrite(V22, HIGH);
       // Tắt bơm tưới khi không có nước
       digitalWrite(relay, LOW);
       digitalWrite(led1, LOW);
       ERa.virtualWrite(V20, LOW);
   }

   // Kiểm tra độ ẩm đất
   if((4095-value) > 1000){   
      digitalWrite(relay, LOW);
      digitalWrite(led1, LOW);
      ERa.virtualWrite(V20, LOW);
   }

 } else if (toggleState == HIGH) { // Chế độ Manual  
   // Reset tất cả về trạng thái ban đầu
   digitalWrite(relay, LOW);    // Tắt relay bơm tưới
   digitalWrite(relay1, LOW);   // Tắt relay bơm nước
   digitalWrite(led1, LOW);     // Tắt LED bơm tưới
   digitalWrite(led2, LOW);     // Tắt LED bơm nước
   
   // Reset tất cả trạng thái trên ERA về 0
   ERa.virtualWrite(V19, 0);  // Đèn Manual động cơ tưới = 0
   ERa.virtualWrite(V20, 0);  // Trạng thái bơm tưới = 0
   ERa.virtualWrite(V21, 0);  // Đèn Manual động cơ bơm = 0 
   ERa.virtualWrite(V22, 0);  // Trạng thái bơm nước = 0

   Serial.println("Chế độ hiện tại: Manual - Đã reset tất cả về 0");
 }
}
ERA_WRITE(V19) {
  j = param.getInt();
  if (toggleState == HIGH){
  if(j==1||j==0){
    toggleState1 = !toggleState1; 
    digitalWrite(led1, toggleState1);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V20, toggleState1);
  }

 // Thực hiện chuyển đổi chế độ
        if (toggleState1 == LOW) {
            // Chế độ Auto
            Serial.println("Tắt bơm");
            // Thêm các hành động khi chuyển sang chế độ Auto ở đây (nếu cần)
            digitalWrite(relay, LOW);
        } else if (toggleState1 == HIGH) {
            // Chế độ Manual
            Serial.println("Bật bơm");
            // Thêm các hành động khi chuyển sang chế độ Manual ở đây (nếu cần
            digitalWrite(relay, HIGH);
        }
}
}
ERA_WRITE(V21) {
  k = param.getInt();
  if (toggleState == HIGH){
  if(k==1||k==0){
    toggleState2 = !toggleState2; 
    digitalWrite(led2, toggleState2);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V22, toggleState2);
  }
}
 // Thực hiện chuyển đổi chế độ
        if (toggleState2 == LOW) {
            // Chế độ Auto
            Serial.println("Tắt bơm");
            // Thêm các hành động khi chuyển sang chế độ Auto ở đây (nếu cần)
            digitalWrite(relay1, LOW);
        } else if (toggleState2 == HIGH) {
            // Chế độ Manual
            Serial.println("Bật bơm");
            // Thêm các hành động khi chuyển sang chế độ Manual ở đây (nếu cần
            digitalWrite(relay1, HIGH);
        }
}

void th(){
  float h = dht.readHumidity();
  // Đọc giá trị nhiệt độ C (mặc định)
  float t = dht.readTemperature();
  // Đọc giá trị nhiệt độ F(isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Kiểm tra quá trình đọc thành công hay không
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // IN thông tin ra màn hình
  Serial.print("Do am: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Nhiet do: ");
  Serial.print(t);
  Serial.println(" *C ");
  
  ERa.virtualWrite(V24, h); // Gửi giá trị lưu lượng đến biểu đồ Era
  ERa.virtualWrite(V25, t); // Gửi giá trị lưu lượng đến biểu đồ Blynk
}
void doamdat() {
    value = analogRead(34); 
    int soilMoisture = 4095 - value;
    ERa.virtualWrite(V9, soilMoisture);
    
    Serial.print("Do am dat = ");
    Serial.print(soilMoisture);
    
    // Kiểm tra và tự động tưới ở chế độ Auto
    if(toggleState == LOW) { // Chế độ Auto
        if(reading == HIGH) { // Nếu có nước trong bồn
            if(soilMoisture <= SOIL_VERY_DRY) { // Đất khô thì tưới 
                digitalWrite(MOTOR_WATER, MOTOR_ON);
                digitalWrite(led1, HIGH);
                ERa.virtualWrite(V20, HIGH);
                Serial.println("Dat kho, bat dau tuoi!");
            }
            else if(soilMoisture >= SOIL_VERY_WET) { // Đất đủ ẩm thì dừng
                digitalWrite(MOTOR_WATER, MOTOR_OFF);
                digitalWrite(led1, LOW);
                ERa.virtualWrite(V20, LOW);
                Serial.println("Dat du am, dung tuoi");
            }
        }
    }
    delay(1000);
}
void pulseCounter() {
  pulseCount++;
}

// Thêm biến đếm thời gian không có flow
unsigned long noFlowStartTime = 0;
#define NO_FLOW_TIMEOUT 5000  // 5 giây không có flow = hết nước
void sendFlowData() {
  unsigned long currentTime = millis();

  if (currentTime - oldTime > 1000) {

    // Nếu không có xung trong 1 giây
        if (pulseCount == 0) {
            if (noFlowStartTime == 0) {
                noFlowStartTime = currentTime;
            }
            // Nếu không có flow quá lâu -> có thể hết nước
            else if (currentTime - noFlowStartTime > NO_FLOW_TIMEOUT) {
                Serial.println("Cảnh báo: Có thể đã hết nước!");
                // Thêm xử lý khi hết nước ở đây
            }
        } else {
            noFlowStartTime = 0;  // Reset nếu có flow
        }
        
    flowRate = ((1000.0 / (currentTime - oldTime)) * pulseCount) / 7.5;       //CT nay tu dau ra?????
    oldTime = currentTime;

    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.print(" L/min\t");
    Serial.print("Total: ");
    Serial.print(totalMilliLitres);
    Serial.println(" mL");

    pulseCount = 0;

    ERa.virtualWrite(V10, flowRate); // Gửi giá trị lưu lượng đến ERa
    ERa.virtualWrite(V11, totalMilliLitres); // Gửi giá trị lưu lượng tổng đến ERa
  }
}

// Biến lưu trạng thái ổn định cuối cùng
static int lastStableReading = LOW;
static unsigned long lastReadTime = 0;

int readFloatSwitchWithFilter() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastReadTime < DEBOUNCE_DELAY) {
        return lastStableReading;
    }
    
    // Đọc nhiều mẫu để xác nhận trạng thái thực của phao
    int readingCount = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        readingCount += digitalRead(floatSwitchPin);
        delay(SAMPLE_DELAY);
    }
    
    int currentReading;
    if (readingCount >= THRESHOLD_HIGH) {
        currentReading = HIGH;  // Phao đã dựng lên hoàn toàn
    } else if (readingCount <= THRESHOLD_LOW) {
        currentReading = LOW;   // Phao đã hạ xuống hoàn toàn
    } else {
        currentReading = lastStableReading; // Giữ trạng thái cũ nếu chưa xác định rõ
    }
    
    lastReadTime = currentTime;
    lastStableReading = currentReading;
    return currentReading;
}

void phao() {
   // Đọc trạng thái của bộ lọc
   reading = readFloatSwitchWithFilter();
   
   ERa.virtualWrite(V6, reading);
   
   if (reading == HIGH) {
        Serial.println("Mực nước đầy, tắt bơm.");
        ERa.virtualWrite(V26, 1); // 1 = Phao cao
   } else {
        Serial.println("Mực nước thấp, bật bơm.");
        ERa.virtualWrite(V26, 0); // 0 = Phao thấp
   }
}

void setupTime() {
    // Cấu hình timezone và server NTP
    configTime(TIMEZONE_OFFSET, 0, NTP_SERVER);
    
    // Đợi đồng bộ thời gian
    Serial.println("Đang đồng bộ thời gian...");
    int retry = 0;
    while (time(nullptr) < 1000000000L && retry < 10) {
        Serial.print(".");
        delay(1000);
        retry++;
    }
    Serial.println("\nĐã đồng bộ thời gian!");
    
    // In thời gian hiện tại để kiểm tra
    printLocalTime();
}

// Thêm biến toàn cục để kiểm tra đã có lịch đặt chưa
bool hasSchedule = false;

// Hàm kiểm tra xem đã có lịch đặt chưa
bool checkScheduleSet() {
    // Kiểm tra các thông số cài đặt
    if (a != 0 || b != 0 || c != 0 || d != 0 || e != 0) {
        hasSchedule = true;
        return true;
    }
    hasSchedule = false;
    return false;
}

void printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Không thể lấy thông tin thời gian");
        return;
    }
    
    ERa.virtualWrite(V12, timeinfo.tm_hour);
    ERa.virtualWrite(V13, timeinfo.tm_min);
    ERa.virtualWrite(V14, timeinfo.tm_mday);
    ERa.virtualWrite(V15, timeinfo.tm_mon + 1);
    ERa.virtualWrite(V16, timeinfo.tm_year + 1900);

    if (toggleState == LOW) { // Chế độ Auto
        static bool pumpRunning = false;
        static bool waterRunning = false;
        int soilMoisture = 4095 - value;

        // Kiểm tra xem đã có lịch đặt chưa
        if (!checkScheduleSet()) {
            // Nếu chưa có lịch, tắt hết các thiết bị
            digitalWrite(MOTOR_PUMP, MOTOR_OFF);
            digitalWrite(MOTOR_WATER, MOTOR_OFF);
            digitalWrite(led1, LOW);
            digitalWrite(led2, LOW);
            ERa.virtualWrite(V20, LOW);
            ERa.virtualWrite(V22, LOW);
            return; // Thoát khỏi hàm
        }

        // LOGIC BƠM NƯỚC - theo thời gian đặt
        if (timeinfo.tm_hour == a && timeinfo.tm_min == b &&
            timeinfo.tm_mday == c && (timeinfo.tm_mon + 1) == d && 
            (timeinfo.tm_year + 1900) == e) {
            
            if (reading == LOW && !pumpRunning) {
                digitalWrite(MOTOR_PUMP, MOTOR_ON);
                digitalWrite(led2, HIGH);
                ERa.virtualWrite(V22, HIGH);
                pumpRunning = true;
                Serial.println("Đúng thời gian đặt, bắt đầu bơm nước");
            }
        }

        // Điều kiện dừng bơm
        if (pumpRunning && (reading == HIGH || totalMilliLitres >= l)) {
            digitalWrite(MOTOR_PUMP, MOTOR_OFF);
            digitalWrite(led2, LOW);
            ERa.virtualWrite(V22, LOW);
            pumpRunning = false;
            totalMilliLitres = 0;
            Serial.println("Dừng bơm - bể đầy hoặc đủ lưu lượng");
        }

        // LOGIC TƯỚI CÂY - theo độ ẩm và các thông số tưới
        if (soilMoisture <= SOIL_VERY_DRY && !waterRunning && reading == HIGH) {
            // Kiểm tra các thông số tưới đã được đặt
            if (f > 0 && g > 0 && h > 0) {
                digitalWrite(MOTOR_WATER, MOTOR_ON);
                digitalWrite(led1, HIGH);
                ERa.virtualWrite(V20, HIGH);
                waterRunning = true;
                Serial.println("Đất khô, bắt đầu tưới theo lịch");
            }
        }
        
        if (waterRunning && (soilMoisture >= SOIL_VERY_WET || reading == LOW)) {
            digitalWrite(MOTOR_WATER, MOTOR_OFF);
            digitalWrite(led1, LOW);
            ERa.virtualWrite(V20, LOW);
            waterRunning = false;
            Serial.println("Dừng tưới - đất đủ ẩm hoặc hết nước");
        }
    }
}

void check_value() {
    if((4095-value) > 1000) {   
        digitalWrite(relay, LOW);
        digitalWrite(led1, LOW);
        ERa.virtualWrite(V20, LOW);
    }
}

void check_button() {
    boolean currentState = digitalRead(button);
    if (currentState == LOW && lastButtonState == HIGH) {
        delay(50); // Chống dội 
        if (digitalRead(button) == LOW) {
            toggleState = !toggleState;
            
            // Đồng bộ với ERA
            ERa.virtualWrite(V17, !toggleState); // Gửi trạng thái lên ERA
            ERa.virtualWrite(V18, toggleState);
            digitalWrite(led, toggleState);

            // Reset các trạng thái giống như khi điều khiển từ ERA
            if (toggleState == LOW) { // Auto
                toggleState1 = LOW;
                toggleState2 = LOW;
                digitalWrite(led1, LOW);
                digitalWrite(led2, LOW);
                digitalWrite(relay, LOW);
                digitalWrite(relay1, LOW);
                ERa.virtualWrite(V20, LOW);
                ERa.virtualWrite(V22, LOW);
                Serial.println("Chuyển sang chế độ Auto");
            } else { // Manual
                digitalWrite(relay, LOW);
                digitalWrite(relay1, LOW);
                digitalWrite(led1, LOW);
                digitalWrite(led2, LOW);
                ERa.virtualWrite(V20, LOW);
                ERa.virtualWrite(V22, LOW);
                Serial.println("Chuyển sang chế độ Manual");
            }
        }
    }
    lastButtonState = currentState;
}

void check_button1() {
    boolean currentState1 = digitalRead(button1);
    delay(20);
    
    if (currentState1 == LOW && lastButtonState1 == HIGH && toggleState == HIGH && reading == HIGH) {
        toggleState1 = !toggleState1;
        digitalWrite(led1, toggleState1);
        ERa.virtualWrite(V19, toggleState1);
        ERa.virtualWrite(V20, toggleState1);
        
        if (toggleState1 == LOW) {
            digitalWrite(relay, LOW);
            Serial.println("Chế độ hiện tại: Bơm1 tắt");
        } else if (toggleState1 == HIGH) {
            digitalWrite(relay, HIGH);
            Serial.println("Chế độ hiện tại: Bơm1 bật");
        }
    }
    if(reading == LOW) {
        digitalWrite(relay, LOW);
        digitalWrite(led1, LOW);
        ERa.virtualWrite(V20, LOW);
    }
    lastButtonState1 = currentState1;
}

void check_button2() {
    boolean currentState2 = digitalRead(button2);
    delay(20);
    
    if (currentState2 == LOW && lastButtonState2 == HIGH && toggleState == HIGH && reading == LOW) {
        toggleState2 = !toggleState2;
        digitalWrite(led2, toggleState2);
        ERa.virtualWrite(V21, toggleState2);
        ERa.virtualWrite(V22, toggleState2);
        
        if (toggleState2 == LOW) {
            digitalWrite(relay1, LOW);
            Serial.println("Chế độ hiện tại: Bơm2 tắt");
        } else if (toggleState2 == HIGH) {
            digitalWrite(relay1, HIGH);
            Serial.println("Chế độ hiện tại: Bơm2 bật");
        }
    }
    if(reading == HIGH) {
        digitalWrite(relay1, LOW);
        digitalWrite(led2, LOW);
        ERa.virtualWrite(V22, LOW);
    }
    lastButtonState2 = currentState2;
}

void setup() {
    Serial.begin(115200);
    ERa.begin(ssid, pass);
    Serial.println("DHTxx test!");
    dht.begin();
    pinMode(sensorPin, INPUT);
    pinMode(floatSwitchPin, INPUT_PULLUP);
    pinMode(led, OUTPUT);
    pinMode(button, INPUT_PULLUP);
    pinMode(led1, OUTPUT);
    pinMode(button1, INPUT_PULLUP);
    pinMode(led2, OUTPUT);
    pinMode(button2, INPUT_PULLUP);
    pinMode(relay, OUTPUT);
    pinMode(relay1, OUTPUT);
    
    // Thiết lập chân điều khiển động cơ
    pinMode(MOTOR_PUMP, OUTPUT);   // Chân điều khiển động cơ bơm
    pinMode(MOTOR_WATER, OUTPUT);  // Chân điều khiển động cơ tưới
    
    // Đảm bảo động cơ tắt khi khởi động
    digitalWrite(MOTOR_PUMP, MOTOR_OFF);
    digitalWrite(MOTOR_WATER, MOTOR_OFF);

    attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
    timer.setInterval(1000L, sendFlowData);
    timer.setInterval(1000L, printLocalTime);
    configTime(7 * 3600, 0, "pool.ntp.org");

    // Reset các trạng thái sau khi khởi động
    hasSchedule = false;
    digitalWrite(MOTOR_PUMP, MOTOR_OFF);
    digitalWrite(MOTOR_WATER, MOTOR_OFF);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
}

void loop() {
    ERa.run();
    timer.run();
    th();
    doamdat();
    phao();
    check_button();
    check_button1();
    check_button2();
}

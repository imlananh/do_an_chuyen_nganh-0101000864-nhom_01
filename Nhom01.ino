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
#include <Preferences.h>
#include <ERa.h>

const char ssid[] = "HUAWEIY9PRIME2019";  // Tên Wi-Fi của bạn
const char pass[] = "yesido08";  // Mật khẩu Wi-Fi của bạn

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
//unsigned long accumulatedMilliLitres = 0;
unsigned long currentFlowMilliLitres = 0;  // Lưu lượng hiện tại của lần tưới
unsigned long accumulatedFlowMilliLitres = 0;  // Tổng lưu lượng tích lũy
bool flowLimitReached = false;


Preferences preferences;

int value = 0;//giá trị ban đầu cảm biến mực nước
/* This function print uptime every second */
int a = 0;  // Giờ bật đèn
int b = 0;  // Phút bật đèn
int c = 0;  // Ngày
int d = 0;   // Tháng
int e = 0; // Năm
int i = 0; 
int j = 0; 
int k = 0; 
int l = 0; 

bool isWateringComplete = false;
bool wateringInProgress = false;
// Biến cho chế độ Auto
bool pumpCycleStarted = false;  // Đánh dấu đã bắt đầu chu kỳ bơm

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
// Sửa các ERA_WRITE để lưu giá trị
ERA_WRITE(V1) { 
    a = param.getInt(); 
    preferences.putInt("hour", a);
}

ERA_WRITE(V2) { 
    b = param.getInt(); 
    preferences.putInt("minute", b);
}

ERA_WRITE(V3) { 
    c = param.getInt(); 
    preferences.putInt("day", c);
}

ERA_WRITE(V4) { 
    d = param.getInt(); 
    preferences.putInt("month", d);
}

ERA_WRITE(V5) { 
    e = param.getInt(); 
    preferences.putInt("year", e);
}



ERA_WRITE(V23) { 
    l = param.getInt(); 
    preferences.putInt("flow", l);
}

ERA_WRITE(V17) {
   struct tm timeinfo;
   if (!getLocalTime(&timeinfo)) {
       Serial.println("Không thể lấy thông tin thời gian");
       return;
   }

   i = param.getInt();
   if(i==1||i==0) {
       toggleState = !toggleState;  
       
       ERa.virtualWrite(V18, toggleState);  
       digitalWrite(led, toggleState);
       
       toggleState1 = LOW;
       toggleState2 = LOW;
       digitalWrite(led1, LOW); 
       digitalWrite(led2, LOW);
       digitalWrite(relay, LOW);
       digitalWrite(relay1, LOW);
       
       ERa.virtualWrite(V19, 0);
       ERa.virtualWrite(V20, 0);
       ERa.virtualWrite(V21, 0);
       ERa.virtualWrite(V22, 0);
   }

   if (toggleState == LOW) {  // Chế độ Auto
       Serial.println("Chế độ hiện tại: Auto");
       if (!checkScheduleSet()) {
           return;
       }

       // Kiểm tra thời điểm bắt đầu chu kỳ bơm
       if ((timeinfo.tm_year + 1900) == e || e == -1) {
           if ((timeinfo.tm_mon + 1) == d || d == -1) {
               if (timeinfo.tm_mday == c || c == -1) {
                   if (timeinfo.tm_hour == a || a == -1) {
                       if (timeinfo.tm_min == b) {
                           pumpCycleStarted = true;  // Đánh dấu bắt đầu chu kỳ
                           Serial.println("Đã đến thời điểm bắt đầu chu kỳ bơm");
                           if(reading == LOW) {
                               digitalWrite(relay1, HIGH);
                               digitalWrite(led2, HIGH);
                               ERa.virtualWrite(V22, HIGH);
                           }
                       }
                   }
               }
           }
       }

       // Logic điều khiển bơm - sau thời điểm bắt đầu
       if (pumpCycleStarted) {
           if (reading == LOW) {  // Nếu hết nước
               digitalWrite(relay1, HIGH);
               digitalWrite(led2, HIGH);
               ERa.virtualWrite(V22, HIGH);
               Serial.println("Phát hiện hết nước - BẬT bơm");
               // Tạm dừng tưới khi không có nước
               digitalWrite(relay, LOW);
               digitalWrite(led1, LOW);
               ERa.virtualWrite(V20, LOW);
           } else {  // Nếu đủ nước
               digitalWrite(relay1, LOW);
               digitalWrite(led2, LOW);
               ERa.virtualWrite(V22, LOW);
               Serial.println("Phát hiện đủ nước - TẮT bơm");
               
               // Kiểm tra điều kiện tưới
               if (totalMilliLitres < l && (4095-value) <= 1000) {
                   digitalWrite(relay, HIGH);
                   digitalWrite(led1, HIGH);
                   ERa.virtualWrite(V20, HIGH);
                   Serial.println("Tiếp tục tưới - chưa đạt lưu lượng");
               }
           }
       }

       // Kiểm tra điều kiện dừng tưới  
       if (totalMilliLitres >= l) {
           digitalWrite(relay, LOW);
           digitalWrite(led1, LOW);
           ERa.virtualWrite(V20, LOW);
           Serial.printf("Đạt lưu lượng đặt %d ml - Dừng tưới hoàn toàn\n", l);
       }

   } else { // Chế độ Manual
       Serial.println("Chế độ hiện tại: Manual");
       pumpCycleStarted = false;  // Reset flag khi chuyển sang Manual
   }
}

ERA_WRITE(V19) {
    if (toggleState == HIGH) {
        j = param.getInt();
        if(j == 1 || j == 0) {
            digitalWrite(relay, j);
            digitalWrite(led1, j);
            ERa.virtualWrite(V20, j);
        }
    }
}

ERA_WRITE(V21) {
    if (toggleState == HIGH) {
        k = param.getInt();
        if(k == 1 || k == 0) {
            digitalWrite(relay1, k);
            digitalWrite(led2, k);
            ERa.virtualWrite(V22, k);
        }
    }
}

void resetSchedule() {
    preferences.clear();
    a = b = c = d = e = l = 0;
    
    // Cập nhật lên ERA
    ERa.virtualWrite(V1, 0);
    ERa.virtualWrite(V2, 0);
    ERa.virtualWrite(V3, 0);
    ERa.virtualWrite(V4, 0);
    ERa.virtualWrite(V5, 0);
    ERa.virtualWrite(V23, 0);
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
  ERa.virtualWrite(V25, t); // Gửi giá trị lưu lượng đến biểu đồ Era
}

void doamdat() {
    value = analogRead(34); 
    int soilMoisture = 4095 - value;
    ERa.virtualWrite(V9, soilMoisture);
    
    Serial.print("Do am dat = ");
    Serial.print(soilMoisture);
    
    if (toggleState == LOW && pumpCycleStarted) {  // Chế độ Auto và đã qua thời điểm bắt đầu
        if (reading == HIGH) {  // Có nước trong bồn
            if (totalMilliLitres < l) {  // Chưa đạt lưu lượng
                if (soilMoisture <= SOIL_VERY_DRY) {  // Đất khô
                    digitalWrite(MOTOR_WATER, MOTOR_ON);
                    digitalWrite(led1, HIGH);
                    ERa.virtualWrite(V20, HIGH);
                    Serial.println("Dat kho, bat dau tuoi!");
                }
                else if (soilMoisture >= SOIL_VERY_WET) {  // Đất đủ ẩm
                    digitalWrite(MOTOR_WATER, MOTOR_OFF);
                    digitalWrite(led1, LOW);
                    ERa.virtualWrite(V20, LOW);
                    Serial.println("Dat du am, dung tuoi");
                }
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
        // Kiểm tra flow
        if (pulseCount == 0) {
            if (noFlowStartTime == 0) {
                noFlowStartTime = currentTime;
            }
            else if (currentTime - noFlowStartTime > NO_FLOW_TIMEOUT) {
                Serial.println("Cảnh báo: Có thể đã hết nước!");
            }
        } else {
            noFlowStartTime = 0;
        }
        
        // Tính lưu lượng hiện tại (ml)
        flowRate = ((1000.0 / (currentTime - oldTime)) * pulseCount) / 7.5;
        flowMilliLitres = (int)((flowRate / 60) * 1000);
        currentFlowMilliLitres += flowMilliLitres;  // Cộng vào lưu lượng hiện tại

        // Kiểm tra nếu đạt lưu lượng đặt
        if (currentFlowMilliLitres >= l) {
            // Đánh dấu đã đạt lưu lượng
            flowLimitReached = true;
            // Cộng vào tổng lưu lượng
            accumulatedFlowMilliLitres += l;
            
            // Lưu tổng vào bộ nhớ
            preferences.putULong("totalFlow", accumulatedFlowMilliLitres);
            
            // Reset lưu lượng hiện tại
            currentFlowMilliLitres = 0;
            
            // Dừng bơm khi đạt lưu lượng
            digitalWrite(relay, MOTOR_OFF);
            digitalWrite(led1, MOTOR_OFF);
            ERa.virtualWrite(V20, MOTOR_OFF);
            
            Serial.printf("Đạt lưu lượng đặt. Tổng: %lu ml\n", accumulatedFlowMilliLitres);
        }
        pulseCount = 0;

        // Cập nhật lên ERA
        ERa.virtualWrite(V10, currentFlowMilliLitres);  // Lưu lượng hiện tại
        ERa.virtualWrite(V11, accumulatedFlowMilliLitres);  // Tổng tích lũy
    }
}

// Thêm hàm reset lưu lượng hiện tại khi bắt đầu tưới mới


// Thêm hàm reset tổng lưu lượng (tùy chọn)
void resetTotalFlow() {
    accumulatedFlowMilliLitres = 0;
    preferences.putULong("totalFlow", 0);
    ERa.virtualWrite(V11, 0);
    Serial.println("Reset tổng lưu lượng về 0");
}

// Thêm hàm reset tổng lưu lượng (tùy chọn)
void resetAccumulatedFlow() {
    accumulatedFlowMilliLitres = 0;
    preferences.putULong("totalFlow", 0);
    ERa.virtualWrite(V11, 0);
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

bool hasSchedule = false;

bool checkScheduleSet() {
    if (a != 0 || b != 0 || c != 0 || d != 0 || e != 0) {
        hasSchedule = true;
        return true;
    }
    hasSchedule = false;
    Serial.println("Chưa có lịch tưới được đặt!");
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
            digitalWrite(MOTOR_WATER, MOTOR_ON);
            digitalWrite(led1, HIGH);
            ERa.virtualWrite(V20, HIGH);
            waterRunning = true;
            Serial.println("Đất khô, bắt đầu tưới theo lịch");
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

// Thêm biến thời gian cho việc cập nhật
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 100; // Cập nhật mỗi 100ms

// Thêm biến để theo dõi chu kỳ tưới
unsigned long lastWateringTime = 0;  // Thời điểm tưới lần cuối
int currentCycle = 0;                // Số lần đã tưới
bool isWatering = false;             // Trạng thái đang tưới
int nextWateringHour = 0;            // Giờ tưới tiếp theo
int nextWateringMinute = 0;          // Phút tưới tiếp theo

// Hàm tính thời gian tưới tiếp theo
void calculateNextWateringTime() {
    nextWateringHour = a;
    
    // Xử lý khi phút vượt quá 59
    while (nextWateringMinute > 59) {
        nextWateringMinute -= 60;
        nextWateringHour++;
    }
    
    // Xử lý khi giờ vượt quá 23
    if (nextWateringHour > 23) {
        nextWateringHour -= 24;
    }
}

void handleAutoWatering() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Không thể lấy thông tin thời gian");
        return;
    }

    // Kiểm tra điều kiện tưới
    if (toggleState == LOW && reading == HIGH) {  // Chế độ Auto và có nước
        // Đúng thời gian tưới
        if (timeinfo.tm_hour == a && timeinfo.tm_min == b && !isWatering) {
            startNewWatering();
            isWatering = true;
        }

        // DỪNG BƠM TƯỚI HOÀN TOÀN khi đạt lưu lượng
        if (isWatering && totalMilliLitres >= l) {
            stopWatering();
            isWatering = false;
            //Nếu độ ẩm đất vẫn thấp, không kích hoạt động cơ nữa
            if ((4095-value) <= SOIL_VERY_DRY) {
              Serial.println("Độ ẩm đất vẫn thấp, không tưới thêm");
            }
        }
    }
}

// Hàm bắt đầu tưới
void startNewWatering() {
    if ((4095-value) <= SOIL_VERY_DRY) {
        digitalWrite(relay, HIGH);
        digitalWrite(led1, HIGH);
        ERa.virtualWrite(V20, HIGH);
        totalMilliLitres = 0;
        currentFlowMilliLitres = 0;  // Reset lưu lượng hiện tại
        ERa.virtualWrite(V10, 0);    // Cập nhật lên ERA
        Serial.println("Bắt đầu tưới mới - Reset lưu lượng");
    }
}

// Hàm dừng tưới
void stopWatering() {
    digitalWrite(relay, LOW);
    digitalWrite(led1, LOW);
    ERa.virtualWrite(V20, LOW);
    Serial.printf("Dừng tưới - Lưu lượng: %d mL\n", totalMilliLitres);
}

void setup() {
   Serial.begin(115200);
   
   // Khởi tạo NVS
   preferences.begin("scheduler", false);

   flowLimitReached = false;  // Khởi tạo flag

   
   // Đọc các giá trị đã lưu
   a = preferences.getInt("hour", 0);
   b = preferences.getInt("minute", 0);
   c = preferences.getInt("day", 0);
   d = preferences.getInt("month", 0);
   e = preferences.getInt("year", 0);
   l = preferences.getInt("flow", 0);
   
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
   
   pinMode(MOTOR_PUMP, OUTPUT);
   pinMode(MOTOR_WATER, OUTPUT);
   
   toggleState = LOW;
   toggleState1 = LOW;
   toggleState2 = LOW;
   pumpCycleStarted = false;
   
   digitalWrite(led, LOW);
   digitalWrite(led1, LOW);
   digitalWrite(led2, LOW);
   digitalWrite(relay, LOW);
   digitalWrite(relay1, LOW);
   digitalWrite(MOTOR_PUMP, MOTOR_OFF);
   digitalWrite(MOTOR_WATER, MOTOR_OFF);
   
    // Reset tất cả giá trị trên ERA về 0
   ERa.virtualWrite(V17, 0);  // LED chế độ Manual
   ERa.virtualWrite(V18, 0);  // Trạng thái Auto/Manual
   ERa.virtualWrite(V19, 0);  // LED Manual bơm tưới
   ERa.virtualWrite(V20, 0);  // Trạng thái bơm tưới
   ERa.virtualWrite(V21, 0);  // LED Manual bơm nước
   ERa.virtualWrite(V22, 0);  // Trạng thái bơm nước

   // Cập nhật giá trị đã lưu lên ERA
   ERa.virtualWrite(V1, a);
   ERa.virtualWrite(V2, b);
   ERa.virtualWrite(V3, c);
   ERa.virtualWrite(V4, d);
   ERa.virtualWrite(V5, e);
   ERa.virtualWrite(V23, l);

   attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
   timer.setInterval(1000L, sendFlowData);
   timer.setInterval(1000L, printLocalTime);
   configTime(7 * 3600, 0, "pool.ntp.org");

   accumulatedFlowMilliLitres = preferences.getULong("totalFlow", 0);
   ERa.virtualWrite(V11, accumulatedFlowMilliLitres);

}

// Hàm cập nhật trạng thái động cơ lên ERA
void updateMotorStatus() {
   unsigned long currentTime = millis();
   if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
       lastUpdateTime = currentTime;

       // Cập nhật trạng thái động cơ bơm nước
       bool pumpStatus = digitalRead(relay1);
       ERa.virtualWrite(V22, pumpStatus);
       digitalWrite(led2, pumpStatus);
       
       // Cập nhật trạng thái động cơ tưới
       bool waterStatus = digitalRead(relay);
       ERa.virtualWrite(V20, waterStatus);
       digitalWrite(led1, waterStatus);
   }
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
   updateMotorStatus();
}


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

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>
#include "DHT.h"
#include <TimeLib.h>

const char ssid[] = "PhuongAnh";  // Tên Wi-Fi của bạn
const char pass[] = "1133445566A";  // Mật khẩu Wi-Fi của bạn


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
#define sensorPin 25
#define floatSwitchPin 27

#define led  4
#define button 5

#define led1 16
#define button1 18

#define led2 17
#define button2 19

#define relay 2
#define relay1 15

//cảm biến lưu lượng
volatile unsigned int pulseCount = 0;
float flowRate = 0.0;
unsigned int flowMilliLitres = 0; //dung tích
unsigned long totalMilliLitres = 0; //tổng dung tích đã chảy qua sensor
unsigned long oldTime = 0;

//set time chô cảm biến
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


//Sử dụng framework đkhien thiết bị
//hỗ trợ giao tiếp qua các kênh ảo (virtual channels)

int reading; //khai báo biến toàn cục
void timerEvent() { //thien các hành động theo chu kỳ (gọi bởi bộ định thời - timer)
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
    //hàm ghi nhật ký do framework cung cấp, dùng debug or theo dõi state ctrinh
    //ERmili khả năng trả về số ms , kể từ khi tbi bắt đầu chạy
    //erms /1000L -> chuyển time ms -> s
    //uptime -> in ra chuỗi định dạng, thể hiện time chạy của tbi (uptime) theo s
}


//ERA-WRITE : là macro / hàm callback của framework để xử lý các thay đổi gtri từ các kênh ảo (virtual pin)
//khi gtri trên kênh tương ứng đc cập nhật, hàm trong era-write sẽ đc gọi
//ERA_WRITE(Vx) - tên kênh ảo


// các kênh ảo địa diện cho các nút, thanh trượt, ô nhập liệu tương ứng , nơi ng dùng gửi gtri đến tbi đkhien
//các gtri dùng để dkhien tbi (đèn, động cơ)

ERA_WRITE(V1) {     //tên kênh ảo
  a = param.getInt();   //lấy gtri từ giao diện /ứng dụng - gán vào biến cục bộ - toàn cục
}
ERA_WRITE(V2) {     // gán gtri từ kênh ảo V2 cho biến b
  b = param.getInt();
}
ERA_WRITE(V3) {
  c = param.getInt();
}
ERA_WRITE(V4) {
  d = param.getInt();
}
ERA_WRITE(V5) {
  e = param.getInt();
}
ERA_WRITE(V6) {
  f = param.getInt();
}
ERA_WRITE(V7) {
  g = param.getInt();
}
ERA_WRITE(V8) {
  h = param.getInt();
}
ERA_WRITE(V23) {
  l = param.getInt();
}


//kênh ảo V17 , thien bật tắt đèn dựa trên gtri đầu vào,cập nhật state đèn trong system
ERA_WRITE(V17) { //calback đc kích hoạt khi có dlieu gửi đến V17
  struct tm timeinfo; //lưu trữ ttin về time (năm/thang/ngày/h/p/s)
  i = param.getInt(); //lấy gtri từ V17 gán cho i
  if(i==1||i==0){  //ktr gtri i =1/0 , dùng để bật / tắt auto mode
    toggleState = !toggleState;  //1 biến boolean , đc use chuyển đổi trạng thái(toggle) (1/0)
    ERa.virtualWrite(V18, toggleState); //gửi state của toggle -> V18, cập nhật UI ứng dụng, state mới
    digitalWrite(led, toggleState);  // Áp dụng trạng thái đèn mới , thien bật /tắt real bằng ghi trạng thái mới vào chân đkhien đèn
  }

  //chương trình thực hiện để đưa system vào Auto Mode
  //toggleState trạng thái hiện tại của hệ thống
 if (toggleState == LOW) {
            // Chế độ Auto
             toggleState1 = LOW;
            digitalWrite(led1, toggleState1);
            digitalWrite(relay, LOW);
            toggleState2 = LOW;
            digitalWrite(led2, toggleState2);
            digitalWrite(relay1, LOW); 
            Serial.println("Chế độ hiện tại: Auto");

   // kích hoạt tbi khi đạt time - dkdien cảm biên phù hợp
   //hẹn giờ
  if ((timeinfo.tm_year + 1900) == e && (timeinfo.tm_mon + 1) == d && timeinfo.tm_mday == c 
      && timeinfo.tm_min == b && timeinfo.tm_hour == a 
      || -1 == e && (timeinfo.tm_mon + 1) == d && timeinfo.tm_mday == c 
      && timeinfo.tm_min == b && timeinfo.tm_hour == a) {
        if(reading == HIGH && (4095-value) <= 1000){
    digitalWrite(relay, HIGH); //bật relay kích hoạt tbi
    digitalWrite(led1, HIGH);  // Áp dụng trạng thái đèn mới, báo hiệu relay đang on
    ERa.virtualWrite(V20, HIGH);  // Áp dụng trạng thái đèn mới cập nhật ui để đồng bộ
        }
  }

  if (a == -1 && c == -1 && d == -1 && e == -1 && timeinfo.tm_min == b 
      || c == -1 && d == -1 && e == -1 && a == timeinfo.tm_hour && b == timeinfo.tm_min) {
     if(reading == HIGH && (4095-value) <= 1000){
    digitalWrite(relay, HIGH);
    digitalWrite(led1, HIGH);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V20, HIGH);  // Áp dụng trạng thái đèn mới
        }
  }
  if(totalMilliLitres>=l){
    delay(3000);
    totalMilliLitres=0;
  }
  // Tắt LED sau f phút
  if (timeinfo.tm_min == b + f || totalMilliLitres>=l) {
    digitalWrite(relay, LOW);
    digitalWrite(led1, LOW);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V20, LOW);  // Áp dụng trạng thái đèn mới
    // Nếu `h > 0`, lặp lại việc bật đèn
    if (h > 0) {
      b = b + g;  // Cập nhật thời gian phút cho lần bật đèn tiếp theo
      if(b>59){
        b=0;
        a++;
      }
      h--;  // Giảm số lần lặp lại
    }
  }

if(reading == LOW){
    digitalWrite(relay1, HIGH);
    digitalWrite(led2, HIGH);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V22, HIGH);  // Áp dụng trạng thái đèn mới
    digitalWrite(relay, LOW);
    digitalWrite(led1, LOW);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V20, LOW);  // Áp dụng trạng thái đèn mới
}
if((4095-value) > 1000){   
   digitalWrite(relay, LOW);
   digitalWrite(led1, LOW);  // Áp dụng trạng thái đèn mới
   ERa.virtualWrite(V20, LOW);  // Áp dụng trạng thái đèn mới
}
}

         if (toggleState == HIGH) {
            // Chế độ Manual
             Serial.println("Chế độ hiện tại: Manual");
            // Thêm các hành động khi chuyển sang chế độ Manual ở đây (nếu cần
            digitalWrite(relay, LOW);
            digitalWrite(relay1, LOW);
            digitalWrite(led1, LOW);
            digitalWrite(led2, LOW);
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
void doamdat(){
  value = analogRead(34); //đọc giá trị từ một cảm biến analog được kết nối với chân 34 
  ERa.virtualWrite(V9,4095-value); // Reverse
  Serial.print("Do am dat = ");
  Serial.print(4095-value); //Giá trị cảm biến được in ra màn hình
  Serial.println();
  delay(1000);
}
void pulseCounter() {
  pulseCount++;
}

void sendFlowData() {
  unsigned long currentTime = millis();

  if (currentTime - oldTime > 1000) {
    flowRate = ((1000.0 / (currentTime - oldTime)) * pulseCount) / 7.5;
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
void phao(){
   // Đọc trạng thái của cảm biến phao
   reading = digitalRead(floatSwitchPin);
   ERa.virtualWrite(V6, reading); // Gửi giá trị lưu lượng đến biểu đồ Era
  
      if (reading == HIGH) {
        Serial.println("Mực nước đầy, tắt bơm.");
      } else {
        Serial.println("Mực nước thấp, bật bơm.");
      }
    }
void printLocalTime() {
  struct tm timeinfo;
 // Điều kiện kiểm tra thời gian hiện tại
 

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Không thể lấy thông tin thời gian");
    return;
  }
   ERa.virtualWrite(V12, timeinfo.tm_hour);//Gửi giá trị thời gian thực (giờ) đến ERa
  ERa.virtualWrite(V13, timeinfo.tm_min);//Gửi giá trị thời gian thực (phút) đến ERa
  ERa.virtualWrite(V14, timeinfo.tm_mday);//Gửi giá trị thời gian thực (ngày) đến ERa
  ERa.virtualWrite(V15, timeinfo.tm_mon + 1);//Gửi giá trị thời gian thực (tháng được tính từ 0) đến ERa
  ERa.virtualWrite(V16, timeinfo.tm_year + 1900);//Gửi giá trị thời gian thực (giờ được tính từ 1900) đến ERa
if (toggleState == LOW){
  if ((timeinfo.tm_year + 1900) == e && (timeinfo.tm_mon + 1) == d && timeinfo.tm_mday == c 
      && timeinfo.tm_min == b && timeinfo.tm_hour == a 
      || -1 == e && (timeinfo.tm_mon + 1) == d && timeinfo.tm_mday == c 
      && timeinfo.tm_min == b && timeinfo.tm_hour == a) {
        if(reading == HIGH && (4095-value) <= 1000){
    digitalWrite(relay, HIGH);
    digitalWrite(led1, HIGH);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V20, HIGH);  // Áp dụng trạng thái đèn mới
        }
  }

  if (a == -1 && c == -1 && d == -1 && e == -1 && timeinfo.tm_min == b 
      || c == -1 && d == -1 && e == -1 && a == timeinfo.tm_hour && b == timeinfo.tm_min) {
     if(reading == HIGH && (4095-value) <= 1000){
    digitalWrite(relay, HIGH);
    digitalWrite(led1, HIGH);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V20, HIGH);  // Áp dụng trạng thái đèn mới
        }
  }
  if(totalMilliLitres>=l){
    delay(3000);
    totalMilliLitres=0;
  }
  // Tắt LED sau f phút
  if (timeinfo.tm_min == b + f || totalMilliLitres>=l) {
    digitalWrite(relay, LOW);
    digitalWrite(led1, LOW);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V20, LOW);  // Áp dụng trạng thái đèn mới
    // Nếu `h > 0`, lặp lại việc bật đèn
    if (h > 0) {
      b = b + g;  // Cập nhật thời gian phút cho lần bật đèn tiếp theo
      if(b>59){
        b=0;
        a++;
      }
      h--;  // Giảm số lần lặp lại
    }
  }

if(reading == LOW){
    digitalWrite(relay1, HIGH);
    digitalWrite(led2, HIGH);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V22, HIGH);  // Áp dụng trạng thái đèn mới
    digitalWrite(relay, LOW);
    digitalWrite(led1, LOW);  // Áp dụng trạng thái đèn mới
    ERa.virtualWrite(V20, LOW);  // Áp dụng trạng thái đèn mới
}
if((4095-value) > 1000){   
   digitalWrite(relay, LOW);
   digitalWrite(led1, LOW);  // Áp dụng trạng thái đèn mới
   ERa.virtualWrite(V20, LOW);  // Áp dụng trạng thái đèn mới
}
}
}
void check_button() {
    boolean currentState = digitalRead(button);
     delay(20);
    
    if (currentState == LOW && lastButtonState == HIGH) {
        // Nếu nút nhấn được nhấn
        toggleState = !toggleState;  // Chuyển đổi trạng thái đèn
        digitalWrite(led, toggleState);  // Áp dụng trạng thái đèn mới
        ERa.virtualWrite(V17, toggleState);  // Gửi trạng thái đèn về ERa
        ERa.virtualWrite(V18, toggleState);  // Gửi trạng thái đèn về ERa
    
    
        // Thực hiện chuyển đổi chế độ
        if (toggleState == LOW) {
            // Chế độ Auto
             toggleState1 = LOW;
            digitalWrite(led1, toggleState1);
            digitalWrite(relay, LOW);
            toggleState2 = LOW;
            digitalWrite(led2, toggleState2);
            digitalWrite(relay1, LOW); 
            Serial.println("Chế độ hiện tại: Auto");
        } else if (toggleState == HIGH ) {
            digitalWrite(relay, LOW);
            digitalWrite(relay1, LOW);
            digitalWrite(led1, LOW);
            digitalWrite(led2, LOW);
            // Chế độ Manual
             Serial.println("Chế độ hiện tại: Manual");
           
        }
    }

    lastButtonState = currentState;

}
void check_button1() {
    boolean currentState1 = digitalRead(button1);
     delay(20);
    
          if (currentState1 == LOW && lastButtonState1 == HIGH && toggleState == HIGH && reading == HIGH) {
        // Nếu nút nhấn được nhấn
        toggleState1 = !toggleState1;  // Chuyển đổi trạng thái đèn
        digitalWrite(led1, toggleState1);  // Áp dụng trạng thái đèn mới
        ERa.virtualWrite(V19, toggleState1);  // Gửi trạng thái đèn về ERa
        ERa.virtualWrite(V20, toggleState1);  // Gửi trạng thái đèn về ERa
    
    
        // Thực hiện chuyển đổi chế độ
        if (toggleState1 == LOW ) {
            // Chế độ Auto
            digitalWrite(relay, LOW);
            Serial.println("Chế độ hiện tại: Bơm1 tắt");
        } else if (toggleState1 == HIGH ) {
          digitalWrite(relay, HIGH);
            // Chế độ Manual
             Serial.println("Chế độ hiện tại: Bơm1 bật");
           
        }
    }
if(reading == LOW)
{
  digitalWrite(relay, LOW);
  digitalWrite(led1, LOW);
  ERa.virtualWrite(V20, LOW);  // Gửi trạng thái đèn về ERa
}
    lastButtonState1 = currentState1;

}
void check_button2() {
    boolean currentState2 = digitalRead(button2);
     delay(20);
    
    if (currentState2 == LOW && lastButtonState2 == HIGH && toggleState == HIGH && reading == LOW) {
        // Nếu nút nhấn được nhấn
        toggleState2 = !toggleState2;  // Chuyển đổi trạng thái đèn
        digitalWrite(led2, toggleState2);  // Áp dụng trạng thái đèn mới
        ERa.virtualWrite(V21, toggleState2);  // Gửi trạng thái đèn về ERa
        ERa.virtualWrite(V22, toggleState2);  // Gửi trạng thái đèn về ERa
    
    
        // Thực hiện chuyển đổi chế độ
        if (toggleState2 == LOW) {
          digitalWrite(relay1, LOW);
            // Chế độ Auto
            Serial.println("Chế độ hiện tại: Bơm2 tắt");
        } else if (toggleState2 == HIGH) {
          digitalWrite(relay1, HIGH);
            // Chế độ Manual
            Serial.println("Chế độ hiện tại: Bơm2 bật");
           
        }
    }
    if(reading == HIGH)
{
  digitalWrite(relay1, LOW);
    digitalWrite(led2, LOW);
    ERa.virtualWrite(V22, LOW);  // Gửi trạng thái đèn về ERa
}
    lastButtonState2 = currentState2;

}
void setup() {
    /* Setup debug console */
    Serial.begin(115200);

    ERa.begin(ssid, pass);
     Serial.println("DHTxx test!");
  // Bắt đầu đọc dữ liệu
    dht.begin();
  pinMode(sensorPin, INPUT);
  pinMode(floatSwitchPin, INPUT_PULLUP);  // Không dùng pull-up nội, sử dụng pull-up ngoài
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(led2, OUTPUT);
  pinMode(button2, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(relay1, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  timer.setInterval(1000L, sendFlowData); // Gửi dữ liệu lưu lượng mỗi 1 giây
  timer.setInterval(1000L, printLocalTime);// Gửi dữ liệu thời gian thực mỗi 1 giây
  configTime(7 * 3600, 0, "pool.ntp.org");//Thiết lập máy chủ NTP với một múi giờ cụ thể và sử dụng máy chủ NTP từ pool.ntp.org.
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

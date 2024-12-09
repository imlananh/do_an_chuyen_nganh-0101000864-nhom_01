# Hệ Thống Tưới Cây Tự Động Theo Thời Gian Dựa Vào Dự Báo Thời Tiết

## Giới Thiệu
Dự án này là một Hệ Thống Tưới Cây Tự Động Theo Thời Gian Dựa Vào Dự Báo Thời Tiết và nền tảng E-Ra IoT. Hệ thống có thể tự động tưới cây dựa trên độ ẩm đất, lưu lượng nước và các thiết lập thời gian, đồng thời cho phép giám sát và điều khiển từ xa thông qua giao diện web.

## Tính Năng
- **Hai Chế Độ Hoạt Động:**
  - Chế độ Auto: Tự động tưới theo lịch và điều kiện cài đặt
  - Chế độ Manual: Điều khiển thủ công qua nút nhấn hoặc app

- **Giám Sát Thông Số:**
  - Độ ẩm đất
  - Nhiệt độ và độ ẩm không khí
  - Lưu lượng nước tưới
  - Mực nước trong bể chứa

- **Điều Khiển Thông Minh:**
  - Lập lịch tưới tự động
  - Đặt lưu lượng nước cho mỗi lần tưới
  - Thiết lập khoảng thời gian và số lần tưới
  - Tự động dừng khi đạt lưu lượng hoặc độ ẩm đủ

## Phần Cứng
- ESP32 Development Board
- Cảm biến độ ẩm đất
- Cảm biến DHT11 (nhiệt độ và độ ẩm)
- Cảm biến lưu lượng YF-S201
- Cảm biến mực nước (phao)
- 2 relay để điều khiển bơm
- Động cơ R385
- Nút nhấn và đèn LED chỉ thị

## Cài Đặt
1. **Yêu Cầu Phần Cứng:**
   - Kết nối các cảm biến và thiết bị theo sơ đồ
   - Cấp nguồn 12V cho động cơ bơm
   - Cấp nguồn cho ESP32

2. **Cài Đặt Phần Mềm:**
   ```cpp
   // Cài đặt thư viện
   - ERa library
   - DHT sensor library
   - Time library
   - Preferences library
   ```

3. **Cấu Hình:**
   ```cpp
   // Thông tin WiFi
   const char ssid[] = "YOUR_WIFI_SSID";
   const char pass[] = "YOUR_WIFI_PASSWORD";

   // Token ERA
   #define ERA_AUTH_TOKEN "YOUR_ERA_TOKEN"
   ```

## Sử Dụng
1. **Chế Độ Auto:**
   - Thiết lập thời gian tưới
   - Cài đặt lưu lượng nước
   - Hệ thống sẽ tự động vận hành

2. **Chế Độ Manual:**
   - Sử dụng nút nhấn hoặc app để điều khiển
   - Có thể bật/tắt riêng từng bơm

## Giám Sát
- Theo dõi qua giao diện E-Ra:
  - Trạng thái hoạt động
  - Lưu lượng nước tưới ra
  - Tổng lưu lượng được tưới
  - Các thông số môi trường

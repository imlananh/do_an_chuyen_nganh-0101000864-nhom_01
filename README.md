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

### Nguyên Lý Hoạt Động

1. **Chế Độ Auto:**
   * Hoạt động hoàn toàn tự động với hai động cơ độc lập
   * **Động cơ bơm:**
     - Hoạt động theo thời gian đặt trước
     - Tự động bơm khi phao báo hết nước
     - Tự động dừng khi phao báo đầy nước
   * **Động cơ tưới:**
     - Hoạt động theo lưu lượng đặt (VD: 3000ml)
     - Tự động tạm dừng khi hết nước
     - Tự động tiếp tục khi có nước trở lại
     - Dừng hoàn toàn khi đạt đủ lưu lượng đặt

2. **Chế Độ Manual:**
   * Điều khiển trực tiếp qua:
     - Nút nhấn trên board mạch
     - Giao diện E-Ra
   * **Cơ chế an toàn:**
     - Động cơ bơm tự động dừng khi phao báo đầy
     - Động cơ tưới tự động dừng khi phao báo hết nước
   * Có thể bật/tắt riêng từng động cơ

### Các Bước Sử Dụng

1. **Chế Độ Auto:**
   * Thiết lập thời gian tưới
   * Cài đặt lưu lượng nước
   * Hệ thống sẽ tự động vận hành

2. **Chế Độ Manual:**
   * Sử dụng nút nhấn hoặc app để điều khiển
   * Có thể bật/tắt riêng từng bơm

### Giám Sát
* Theo dõi qua giao diện E-Ra:
   * Trạng thái hoạt động
   * Lưu lượng nước tưới ra
   * Tổng lưu lượng được tưới
   * Các thông số môi trường
## Về Dự Báo Thời Tiết
Trong quá trình phát triển, chúng tôi đã nghiên cứu và thử nghiệm ba phương pháp bổ sung:

### 1. Phương Pháp Dự Báo Thời Tiết Dựa Trên Cảm Biến
* **Nguyên lý hoạt động:**
  - Thu thập dữ liệu từ cảm biến DHT11
  - Tính toán điểm sương
  - Dự đoán khả năng mưa dựa trên độ ẩm và nhiệt độ
* **Ưu điểm:**
  - Không cần kết nối internet
  - Phản ứng nhanh với thay đổi thời tiết
  - Chi phí thấp
* **Hạn chế:**
  - Độ chính xác phụ thuộc vào cảm biến
  - Phạm vi dự báo ngắn hạn

### 2. Phương Pháp Dựa Trên Cây Trồng + API
* **Nguyên lý hoạt động:**
  - Kết hợp dữ liệu cây trồng với API thời tiết
  - Sử dụng OpenWeatherMap cho dự báo
  - Logic mờ ra quyết định tưới
* **Ưu điểm:**
  - Dự báo chính xác dài hạn
  - Tích hợp nhiều nguồn dữ liệu
  - Tối ưu hóa lịch tưới
* **Hạn chế:**
  - Yêu cầu kết nối internet ổn định
  - Phụ thuộc dịch vụ bên thứ ba
  - Chi phí vận hành cao hơn

### 3. Phương Pháp Dựa Trên Cây Trồng (CWSI)
* **Nguyên lý hoạt động:**
  - Sử dụng chỉ số căng thẳng nước (CWSI)
  - Đo độ ẩm đất trực tiếp
  - Không phụ thuộc thời tiết
* **Ưu điểm:**
  - Phản ánh trực tiếp nhu cầu cây trồng
  - Hoạt động độc lập
  - Độ tin cậy cao
* **Hạn chế:**
  - Không dự đoán được điều kiện tương lai
  - Yêu cầu cài đặt thông số cụ thể cho từng loại cây

### Kết Luận Đánh Giá
Sau khi thử nghiệm các phương pháp, chúng tôi nhận thấy mỗi phương pháp đều có những ưu điểm riêng. Tuy nhiên, phương pháp tưới theo thời gian kết hợp với nền tảng IoT được chọn là giải pháp chính vì:
- Dễ triển khai và sử dụng
- Chi phí hợp lý
- Độ ổn định cao
- Khả năng mở rộng và tích hợp trong tương lai

Các phương pháp khác sẽ được tiếp tục nghiên cứu và phát triển để tích hợp vào hệ thống trong các phiên bản nâng cấp sau.

# Hệ Thống Tưới Cây Tự Động Theo Thời Gian Dựa Vào Dự Báo Thời Tiết

## Thành Viên Nhóm

| STT | Họ và tên | MSSV | Lớp | Vai trò |
|-----|-----------|------|------|----------|
| 1 | Đặng Hoàng Phương Uyên | 2254810214 | 22ĐHTT05 | Nhóm Trưởng |
| 2 | Phạm Trần Lan Anh | 2254810094 | 22ĐHTT02 | Thành Viên |
| 3 | Đàm Thị Ngọc Minh | 2254810098 | 22ĐHTT02 | Thành Viên |
| 4 | Trần Đình Anh Duy | 2254810246 | 22ĐHTT05 | Thành Viên |
| 5 | Văn Hồng Quân | 2254810255 | 22ĐHTT06 | Thành Viên |

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
- Relay Module (2 channel)
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
   - WiFi library
   - TimeLib library
   - Preferences library
   - HTTPClient library
   - ArduinoJson library
   ```

3. **Cấu Hình:**
   ```cpp
   // Thông tin WiFi
   const char ssid[] = "YOUR_WIFI_SSID";
   const char pass[] = "YOUR_WIFI_PASSWORD";

   // Token ERA
   #define ERA_AUTH_TOKEN "YOUR_ERA_TOKEN"
   ```

## Hướng Dẫn Sử Dụng

### 1. Khởi động hệ thống
- Kết nối nguồn điện cho hệ thống
- Kiểm tra kết nối WiFi
- Kiểm tra giao diện E-Ra đã sẵn sàng

### 2. Lựa chọn chế độ hoạt động
#### Chế độ Auto
- Đặt lịch thời gian cho động cơ bơm:
  + Chọn giờ, phút, ngày tháng năm mong muốn
  + Hệ thống sẽ bắt đầu bơm tại thời điểm này
- Đặt lưu lượng nước cho động cơ tưới:
  + Nhập lượng nước cần tưới (ví dụ: 3000ml)
  + Hệ thống sẽ tự động dừng khi đạt đủ lưu lượng

#### Chế độ Manual
- Chuyển sang chế độ Manual qua app hoặc nút nhấn
- Điều khiển trực tiếp các động cơ theo nhu cầu
- Theo dõi hoạt động qua đèn LED chỉ thị

### 3. Giám sát hoạt động
- Quan sát các thông số trên giao diện:
  + Trạng thái động cơ
  + Lưu lượng nước
  + Các thông số môi trường

### 4. Điều chỉnh khi cần
- Thay đổi thời gian hoặc lưu lượng nước
- Chuyển đổi giữa các chế độ
- Can thiệp thủ công nếu cần thiết

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

## Liên Hệ

Nếu bạn có bất kỳ cần Tài khoản đăng nhập và mật khẩu giao diện E-Ra, vui lòng liên hệ với chúng tôi qua email: [2254810246@vaa.edu.vn](mailto:2254810246@vaa.edu.vn)

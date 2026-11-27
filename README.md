# Cờ Tướng OOP

## 1. Giới thiệu

**Cờ Tướng OOP** là dự án trò chơi Cờ Tướng được xây dựng bằng **C++17** theo phương pháp lập trình hướng đối tượng. Dự án sử dụng **SFML 2.6.1** để tạo cửa sổ, hiển thị bàn cờ và quân cờ, nhận thao tác bàn phím/chuột, đồng thời phát nhạc nền và âm thanh khi di chuyển.

Mã nguồn được chia thành các thành phần quản lý trạng thái và luật chơi, bộ máy chơi với người dùng, giao diện đồ họa và các lớp quân cờ. Thiết kế này giúp mỗi phần có nhiệm vụ rõ ràng, thuận tiện cho việc bảo trì và mở rộng.

## 2. Tính năng chính

- **Hai chế độ chơi:** Đấu với máy (PvE) và Đấu 2 người trên cùng một thiết bị (PvP), với các mức độ thử thách dành cho chế độ đấu với máy.
- **Luật Cờ Tướng:** Tướng và Sĩ đi trong cung; Tượng đi chéo hai ô, không qua sông và bị chặn bởi quân ở mắt Tượng; Mã bị cản bởi quân ở chân Mã; Xe đi ngang/dọc và không xuyên quân; Pháo cần đúng một quân làm ngòi khi bắt; Tốt không đi lùi và chỉ được đi ngang sau khi qua sông.
- **An toàn cho Tướng:** Không chấp nhận nước đi khiến Tướng phe mình bị chiếu hoặc để hai Tướng lộ mặt trực diện trên cùng một cột. Ván đấu kết thúc khi đối phương bị chiếu bí hoặc không còn nước đi hợp lệ.
- **Xử hòa:** Ván được xử hòa khi một thế cờ cùng bên đi xuất hiện đủ ba lần hoặc khi cả hai bên đều không còn quân tấn công có khả năng chiếu bí.
- **Lịch sử nước đi:** Hiển thị ký hiệu Hán–Việt theo phe đang đi: Tấn dùng `.`, Bình dùng `-`, Thoái dùng `/`; ví dụ `B7.1`, `P2-5`, `M7/3`.
- **Đồng hồ theo lượt:** Mỗi lượt có 2 phút, hiển thị theo định dạng `02:00`. Khi hết giờ, lượt hiện tại được bỏ qua và quyền đi chuyển sang đối phương; đồng hồ lượt mới được đặt lại 2 phút.
- **Hỗ trợ trực quan:** Tô sáng các nước đi hợp lệ của quân đang chọn; hỗ trợ chọn quân và di chuyển bằng chuột hoặc bàn phím.
- **Lưu và tải ván:** Nhấn **F5** hoặc dùng nút lưu để tạo bản lưu, sau đó có thể mở lại ván từ danh sách tải ván.
- **Âm thanh và cài đặt:** Có nhạc nền, âm thanh nước đi/bắt quân, lựa chọn ngôn ngữ và các tùy chỉnh trong giao diện.

## 3. Cấu trúc thư mục

```text
.
├── assets/       # Bàn cờ, quân cờ, phông chữ và âm thanh
├── saves/        # Ván đã lưu và thiết lập người dùng
├── src/
│   ├── ai/       # Bộ máy lựa chọn nước đi khi đấu với máy
│   ├── core/     # Bàn cờ, nước đi, luật và trạng thái ván
│   ├── pieces/   # Lớp cơ sở và các lớp quân cờ cụ thể
│   ├── ui/       # Giao diện SFML và xử lý tương tác
│   └── main.cpp  # Điểm khởi chạy chương trình
└── CMakeLists.txt
```

Các quân cờ được mô hình hóa bằng lớp riêng. Lớp cơ sở trừu tượng `QuanCo` định nghĩa giao diện sinh nước đi; các lớp như Tướng, Sĩ, Tượng, Mã, Xe, Pháo và Tốt hiện thực hành vi tương ứng. `Board` quản lý vị trí quân và kiểm tra nước đi, còn `Game` điều phối lượt, trạng thái và lịch sử ván.

## 4. Cài đặt và biên dịch trên Visual Studio 2022

### Yêu cầu

- Windows 10/11.
- Visual Studio 2022 với workload **Desktop development with C++** và thành phần hỗ trợ **CMake**.
- Kết nối Internet trong lần cấu hình đầu để CMake tải SFML 2.6.1 bằng `FetchContent`.

### Mở và chạy dự án

1. Mở Visual Studio 2022, chọn **File → Open → Folder...** và chọn thư mục gốc chứa `CMakeLists.txt`.
2. Chờ Visual Studio cấu hình CMake và tải SFML nếu đây là lần đầu.
3. Chọn cấu hình **x64-Release** (hoặc **x64-Debug**) và chọn target khởi chạy **XiangqiOOP.exe**.
4. Chọn **Build → Build All** để biên dịch.
5. Chạy bằng **Ctrl+F5** hoặc nút Start trong Visual Studio.

Sau khi build thành công, bước hậu xử lý của CMake tự chép thư mục `assets` đến cạnh tệp thực thi và tạo thư mục `saves` tại vị trí build. Đường dẫn lưu ván và cài đặt của project được cấu hình về thư mục `saves/` ở thư mục nguồn.

### Biên dịch bằng dòng lệnh

Có thể dùng CMake với bộ sinh dự án Visual Studio 2022 từ Developer PowerShell:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Tệp thực thi thường được tạo tại `build/Release/XiangqiOOP.exe`. CMake sẽ tự tải SFML 2.6.1 trong bước cấu hình và thực hiện sao chép tài nguyên sau khi build.

## 5. Điều khiển

- **Chuột:** Chọn quân và chọn ô đích được đánh dấu là nước đi hợp lệ.
- **W, A, S, D** hoặc **phím mũi tên:** Di chuyển con trỏ trên bàn cờ.
- **Enter / Space:** Chọn quân hoặc xác nhận ô đích.
- **F5:** Lưu nhanh ván đang chơi.
- **Esc:** Mở menu.
- **H:** Mở hướng dẫn trong khi chơi.

Ván lưu thủ công được quản lý trong `saves/manual/`; thiết lập giao diện được ghi trong `saves/settings.txt`.
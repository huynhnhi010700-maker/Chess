# CẬP NHẬT GIAO DIỆN / LƯU VÁN

## Các sửa đổi

- Bỏ dòng chữ nhỏ `WASD / Enter / Chuột` dưới màn hình đấu.
- Lưu F5 và nút `LƯU VÁN` dùng chung hàm `saveCurrent()`; có thông báo thành công/thất bại 3,5 giây ở bảng thông tin bên phải.
- Nơi lưu cố định: `XiangqiOOP/saves/last_game.txt` (thư mục nguồn, **không phải** `out/build/x64-debug/saves`). Khi kết thúc, bản lưu lịch sử nằm trong `XiangqiOOP/saves/history`.
- Khi hết 30 giây hoặc ván kết thúc, mở trang kết quả lấy nền ảnh đỏ `assets/autumn_pixel.png`. 1,8 giây đầu chỉ có ảnh; sau đó hiện kết quả thắng/thua; sau 3,5 giây hiện **Chơi lại / Menu chính**.
- Khi đấu máy, kết quả hiện **BẠN ĐÃ THẮNG** / **BẠN ĐÃ THUA**. Đấu 2 người hiện **QUÂN ĐỎ THẮNG / QUÂN ĐEN THẮNG**.
- Khi chọn 2 người, ba nút độ khó bị vô hiệu hóa và hiện trạng thái khóa; chỉ khi đấu máy mới chọn được độ khó.
- Lưu/tải game và lưu cài đặt đều dùng chung đường dẫn ổn định trong project. Khi tải ván chơi dở, tiếp tục thời gian đã lưu.

## Cách cập nhật trong Visual Studio

1. Sao lưu cả thư mục project cũ (đặc biệt `saves`).
2. Giải nén ZIP, mở **chính thư mục `XiangqiOOP` có `CMakeLists.txt`** bằng File > Open > Folder.
3. Build > Build All; chọn `XiangqiOOP.exe` (không chọn `sfml-*.dll`), nhấn Ctrl+F5.
4. Trong ván đấu, nhấn F5 hoặc bấm LƯU VÁN; xem thông báo phía phải.
5. Kiểm tra `XiangqiOOP/saves/last_game.txt`, về Menu > TẢI VÁN CŨ để tải.
6. Nếu muốn giữ ván cũ, chép `saves/*.txt` và `saves/history/*.txt` từ project cũ vào `saves` của project mới.

## Lưu ý kiểm thử

Phần lõi C++ có thể kiểm tra bằng GCC. Giao diện SFML cần build/kiểm thử trên Windows với Visual Studio + SFML 2.6.1.

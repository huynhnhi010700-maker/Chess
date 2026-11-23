# Cờ Tướng Pixel – C++ OOP & SFML 2.6.1

## Chức năng
- Menu pixel-art giống các mẫu: đấu máy / 2 người, chọn mức độ khó trên menu, ván mới, tải ván cũ, hướng dẫn, cài đặt.
- Chuyển tiếng Việt/English bằng nút góc phải hoặc Settings.
- Nhạc nền tự tạo, âm thanh đi/ăn quân, bật tắt độc lập, lưu cài đặt tại `saves/settings.txt`.
- **30 giây mỗi lượt**; sau nước đi thời gian đặt lại 30 giây cho bên tiếp theo. Hết thời gian thua.
- Điều khiển chuột, WASD, phím mũi tên, Enter; hiển thị các nước đi hợp lệ.
- Lưu game F5, tự lưu ván, danh sách chọn ván đã lưu.

## Cài đặt
Visual Studio 2022/2026: File > Open > Folder, chọn XiangqiOOP. Chờ CMake tải SFML lần đầu, chọn **XiangqiOOP.exe** trong startup item (không chọn DLL). Nhấn Ctrl+F5.

CMake tự copy assets và những thư viện runtime cần thiết; không tải DLL từ các trang web DLL không rõ nguồn. Khi gặp lỗi OpenAL32.dll hãy copy từ SFML extlibs/bin/x64 vào cạnh XiangqiOOP.exe.

## OOP
Mỗi loại quân có class riêng trong `src/pieces`; `QuanCo` là class trừu tượng. Luật và trạng thái game ở `core`; AI ở `ai`; SFML UI trong `ui`. Không định nghĩa struct.

## Lưu ý kiểm thử
Logic game đã được dùng từ phiên bản trước; giao diện bản này cần build và kiểm tra trên Windows/Visual Studio để xác thực đầy đủ. Menu **Dễ/Vừa/Khó** có ba mức tìm nước đi khác nhau (Dễ: chọn nước ưu tiên quân ăn; Vừa: tìm kiếm 2 tầng; Khó: tìm kiếm 3 tầng trên tối đa 12 ứng viên). Mức Khó có thể gây khựng khung hình trong khi máy suy nghĩ vì AI vẫn chạy trên luồng chính. Bản nhạc nền được tạo mới, không dùng nhạc có bản quyền.

Font chữ: nạp Consolas sẵn có của Windows rồi hiển thị theo tỉ lệ bitmap thấp; không kèm theo tệp phông chữ.


## Giao dien menu
Menu, cai dat, huong dan va tai van cu su dung duy nhat assets/autumn_pixel.png (anh cay do/nui/lua cua nguoi dung). Khong su dung nen xanh pixel_landscape.png cua game Caro. Sau khi cap nhat hay Build > Build All va chay XiangqiOOP.exe moi.


## NEW: Player names, match names and Xiangqi move notation

- New Game opens a setup screen: enter Red player name, Black player name (2-player only), and the match save name. Click a field to focus; type Unicode text. Backspace deletes, Tab switches fields, Enter starts.
- The right panel shows one row per pair of moves, in the Red and Black columns. Example `P2-5`: Cannon on file 2 moves sideways to file 5; `P2+1` means Cannon advances 1 rank; `x` marks a capture. Files are counted from the current side's right in the traditional Xiangqi convention. For pieces that move diagonally, the destination number refers to the destination file.
- F5 / Save creates a named save slot under `saves/` and also refreshes `last_game.txt`; later saves update the same slot. Names are stored as UTF-8 metadata in XIANGQI_OOP_V2 files. Older XIANGQI_OOP_V1 files remain readable.
- On a two-player finish, the results screen shows the winner's entered name.
- If your old ZIP project has saves, copy the contents of its `saves/` folder into the new project's `saves/` folder before running.

**Note:** SFML interface requires Windows/Visual Studio to run; logic source was checked with GCC, but the complete SFML UI has not been compiled here.


### Quy tac luu van (cap nhat)
- Chi luu khi bam F5 hoac nut LUU VAN. File nam tai `saves/manual/`.
- Khong tu luu khi bat dau, di quan, thoat menu hoac ket thuc.
- Van thang/thua/het gio khong duoc tai lai va ban luu dang choi cua van do duoc xoa khi ket thuc.
- Danh sach TAI VAN CU chi quet `saves/manual/`, bo qua autosave cu, README, settings va history.
- Nut Replay tai man hinh mo dau chi tiep tuc van thu cong moi nhat; neu chua co thi mo form tao van moi.

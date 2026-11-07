# HCMUS LAN Chat & File Sharing

Ung dung tro chuyen va truyen tap tin ngang hang (Peer-to-Peer) trong mang LAN su dung C++17, Windows Sockets 2 (ws2_32) va Qt6 Widgets.
Do an mon Mang may tinh (CSC10008) - Truong Dai hoc Khoa hoc Tu nhien, Dai hoc Quoc gia TP.HCM.

---

## 1. Yeu cau he thong (Prerequisites)

- He dieu hanh: Windows 10 hoac Windows 11 (64-bit).
- Moi truong bien dich: MSYS2 voi toolchain UCRT64:
  - GCC / G++ (phien ban 11 tro len, ho tro C++17)
  - CMake (phien ban 3.20 tro len)
  - Ninja build system
  - Qt6 Base (`mingw-w64-ucrt-x86_64-qt6-base`)
- Thu vien mang he thong: Windows Sockets 2 (`ws2_32.lib` - co san tren Windows).

Cai dat cac goi can thiet qua MSYS2 UCRT64 terminal (neu chua co):
```powershell
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-qt6-base
```

---

## 2. Huong dan cai dat va bien dich (Installation & Build)

1. Mo PowerShell hoac Terminal tai thu muc goc cua du an:
```powershell
cd C:\Users\LEGION\Downloads\Socket-project
```

2. Khoi tao cau hinh bien dich voi CMake va Ninja:
```powershell
cmake -B build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

3. Thuc hien bien dich:
```powershell
cmake --build build
```

Sau khi bien dich hoan tat, file thuc thi `lan_chat.exe` se duoc tao trong thu muc `build/`.

---

## 3. Huong dan khoi chay (How to Run)

Khoi chay ung dung tu thu muc goc cua du an:
```powershell
.\build\lan_chat.exe
```

Khi khoi chay:
- Ung dung tu dong khoi dong Winsock (`WSAStartup`).
- Mo mot cong TCP lang nghe (mac dinh tu cong 5000 tro len).
- Tu dong phat broadcast UDP tren cong 9999 de tim kiem cac peer khac trong mang LAN.
- Hien thi cua so giao dien chinh Qt6.

---

## 4. Huong dan kiem thu (How to Test)

### 4.1. Kiem thu nhieu client tren cung mot may tinh (Local Loopback)
Mo 2 hoac 3 cua so PowerShell rieng biet va cung chay:
```powershell
# Cua so 1:
.\build\lan_chat.exe

# Cua so 2:
.\build\lan_chat.exe

# Cua so 3:
.\build\lan_chat.exe
```
He thong se tu dong cap phat cac cong TCP khac nhau (5000, 5001, 5002...) va phat hien lan nhau qua card Loopback adapter.

### 4.2. Kiem thu trong mang LAN thuc te
1. Ket noi 2 hoac nhieu may tinh vao cung mot mang Wi-Fi hoac mang day LAN.
2. Dam bao Windows Defender Firewall cho phep ung dung truyen thong qua cong UDP 9999 va dai cong TCP 5000-5050 (hoac chon "Allow access" khi co pop-up Windows Firewall).
3. Khoi chay `lan_chat.exe` tren tung may. Cac may se tu dong xuat hien trong danh sach Online Peers cua nhau sau toi da 3 giay.
4. Chon ban chat tren danh sach de bat dau gui tin nhan van ban hoac nhan nut "Send File" de truyen tap tin.

### 4.3. Kiem thu va bat goi tin bang Wireshark
1. Mo Wireshark va chon card mang can quan sat:
   - Chon `Npcap Loopback Adapter` neu kiem thu nhieu client tren 1 may tinh.
   - Chon `Wi-Fi` hoac `Ethernet` neu kiem thu giua cac may trong mang LAN.
2. Nhap bo loc hien thi (Display Filter):
```text
udp.port == 9999 || tcp.port >= 5000
```
3. Cac diem kiem chung tren luong goi tin:
   - Goi UDP: Xac nhan xuat hien goi `HELLO|<ten>|<port>|<status>` dinh ky moi 3 giay toi `255.255.255.255:9999`.
   - Goi TCP Handshake: Quan sat qua trinh bat tay 3 buoc (SYN, SYN-ACK, ACK) khi nhan vao mot peer de mo hoi thoai.
   - Goi TCP Data: Quan sat cac goi tin PSH chua du lieu bat tay `AUTH|...`, tin nhan `MSG|<len>|<text>`, va header file `FILE|<name>|<size>|` kem cac khoi nhi phan 64KB.

---

## 5. Tai lieu tham khao (Documentation)

- Bang phan cong nhiem vu (Markdown): [docs/PHAN_CONG_NHIEM_VU.md](docs/PHAN_CONG_NHIEM_VU.md)
- Bang phan cong nhiem vu (PDF): [docs/Phan_Cong_Nhiem_Vu_Do_An_Socket.pdf](docs/Phan_Cong_Nhiem_Vu_Do_An_Socket.pdf)
- Tai lieu kien truc he thong va huong dan ky thuat (Markdown): [docs/TAI_LIEU_KIEN_TRUC_VA_HUONG_DAN_KY_THUAT.md](docs/TAI_LIEU_KIEN_TRUC_VA_HUONG_DAN_KY_THUAT.md)
- Tai lieu kien truc he thong va huong dan ky thuat (PDF): [docs/Tai_Lieu_Kien_Truc_Va_Huong_Dan_Ky_Thuat.pdf](docs/Tai_Lieu_Kien_Truc_Va_Huong_Dan_Ky_Thuat.pdf)

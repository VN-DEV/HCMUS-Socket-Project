# HCMUS LAN Chat & File Sharing

Ung dung tro chuyen va truyen tap tin ngang hang (Peer-to-Peer) trong mang noi bo (LAN) su dung lap trinh Socket mang goc (Winsock2) tren ngon ngu C++17 ket hop giao dien do hoa Qt6 Widgets.

Do an mon hoc Mang may tinh (CSC10008) - Truong Dai hoc Khoa hoc Tu nhien, Dai hoc Quoc gia TP.HCM.

---

## 1. Danh sach Thanh vien Nhom

Tat ca thanh vien co vai tro va ty le dong gop nhu nhau (33.3% moi nguoi):

1. Nguyen Manh Tri
   - Linh vuc phu trach: Giao dien nguoi dung (Qt6 Widgets UI), Thiet ke Giao thuc Tang Ung dung, Dong goi Du lieu & Co che Peer Discovery (UDP Broadcast, Heartbeat, Timeout).
2. Do Duy Truong
   - Linh vuc phu trach: He thong TCP Listening Server, Quan ly Ket noi Tin nhan 1-1, Xu ly Message Framing chong dinh/cat goi.
3. Mai Huu Tri
   - Linh vuc phu trach: Module Truyen File theo khoi 64KB (TCP Chunked Transfer), Dieu phoi Phien chat (Chat Manager), Quan ly Lich su Tin nhan va So tin nhan chua doc.

Chi tiet phan cong nhiem vu xem tai:
- Tai lieu Markdown: [docs/PHAN_CONG_NHIEM_VU.md](docs/PHAN_CONG_NHIEM_VU.md)
- Ban in PDF: [docs/Phan_Cong_Nhiem_Vu_Do_An_Socket.pdf](docs/Phan_Cong_Nhiem_Vu_Do_An_Socket.pdf)

---

## 2. Kien truc He thong va Giao thuc

Ung dung hoat dong theo mo hinh Pure Peer-to-Peer (khong server trung gian), ket hop ca hai giao thuc transport:

### 2.1. Peer Discovery (UDP Broadcast)
- Moi nut dinh ky 3 giay phat goi tin quang ba toi `255.255.255.255:9999`:
  ```
  HELLO|<display_name>|<tcp_port>|<status>
  ```
- Cac nut lang nghe tren cong 9999 se cap nhat danh sach online peers.
- Neu sau 10 giay khong nhan duoc tin hieu tu mot peer, peer do tu dong bi danh dau offline va xoa khoi danh ba.
- Ho tro cap nhat trang thai tuc thi: `online` hoac `away`.

### 2.2. Messaging va File Sharing (TCP Socket)
- Ket noi 1-1 truc tiep giua hai peer qua cong TCP (5000+).
- Quy trinh bat tay xac thuc ngay khi ket noi:
  ```
  AUTH|<display_name>|<tcp_port>|
  ```
- Phau khung tin nhan van ban (Length-prefixed Message Framing) de chong hien tuong dinh goi / cat goi trong TCP:
  ```
  MSG|<length_in_bytes>|<utf8_text_content>
  ```
- Phau khung truyen tap tin theo khoi (Chunked File Transfer):
  ```
  FILE|<filename>|<filesize_in_bytes>|<binary_stream_in_64kb_chunks>
  ```
- Thong bao ngat ket noi:
  ```
  BYE|
  ```

Chi tiet kien truc, mo hinh da luong va huong dan ky thuat xem tai:
- Tai lieu Markdown: [docs/TAI_LIEU_KIEN_TRUC_VA_HUONG_DAN_KY_THUAT.md](docs/TAI_LIEU_KIEN_TRUC_VA_HUONG_DAN_KY_THUAT.md)
- Ban in PDF: [docs/Tai_Lieu_Kien_Truc_Va_Huong_Dan_Ky_Thuat.pdf](docs/Tai_Lieu_Kien_Truc_Va_Huong_Dan_Ky_Thuat.pdf)

---

## 3. He thong Cong cu va Cong nghe (Toolchain)

- Ngon ngu: C++17 (su dung cac tinh nang std::thread, std::mutex, std::atomic, std::string).
- Trinh bien dich: GCC 16.2.0 (MinGW-w64 UCRT64).
- He thong Build:
  - CMake (3.20+): Quan ly cau hinh bien dich tap trung qua file `CMakeLists.txt`.
  - Ninja: He thong thuc thi build toc do cao, bien dich song song cac luong CPU.
- Giao dien: Qt6 Widgets (`QMainWindow`, `QSplitter`, `QListWidget`, `QTextBrowser`, `QProgressBar`).
- Mang: Windows Sockets 2 (`ws2_32`) - Native Win32 Socket API theo dung chuan yeu cau de bai.
- Kiem thu goi tin: Wireshark (bat goi tin UDP 9999 va phien TCP tren card LAN hoac Loopback).

---

## 4. Cau truc Thu muc Du an

```text
Socket-project/
|-- CMakeLists.txt              # Cau hinh bien dich CMake
|-- README.md                   # Tai lieu huong dan tong quan
|-- .gitignore                  # Cac tep bo qua trong Git
|-- docs/                       # Tai lieu dac ta va phan cong do an
|   |-- PHAN_CONG_NHIEM_VU.md
|   |-- Phan_Cong_Nhiem_Vu_Do_An_Socket.pdf
|   |-- TAI_LIEU_KIEN_TRUC_VA_HUONG_DAN_KY_THUAT.md
|   |-- Tai_Lieu_Kien_Truc_Va_Huong_Dan_Ky_Thuat.pdf
|-- downloads/                  # Thu muc tu dong luu cac file duoc nhan ve
|-- src/
    |-- main.cpp                # Diem khoi dau: WSAStartup, khoi dong Qt UI
    |-- models/
    |   |-- Peer.h              # Cau truc du lieu Peer
    |   |-- ChatMessage.h       # Cau truc du lieu Tin nhan
    |-- protocol/
    |   |-- Protocol.h          # Khai bao dong goi va giai ma goi tin
    |   |-- Protocol.cpp        # Hien thuc split, serialize, deserialize
    |-- network/
    |   |-- UdpDiscovery.h      # Khai bao bo phat hien ngang hang UDP
    |   |-- UdpDiscovery.cpp    # Luong phat, luong thu, luong timeout
    |   |-- TcpServer.h         # Khai bao TCP Server lang nghe
    |   |-- TcpServer.cpp       # Hien thuc socket bind, listen, accept
    |   |-- TcpConnection.h     # Khai bao ket noi TCP 1-1
    |   |-- TcpConnection.cpp   # Hien thuc gui/nhan auth, msg, file chunks
    |-- chat/
    |   |-- ChatManager.h       # Khai bao bo dieu phoi phien chat, unread count
    |   |-- ChatManager.cpp     # Hien thuc quan ly ket noi, lich su tin nhan
    |-- ui/
        |-- MainWindow.h        # Khai bao cua so giao dien chinh Qt6
        |-- MainWindow.cpp      # Hien thuc bo cuc giao dien va xu ly su kien
```

---

## 5. Huong dan Bien dich va Cai dat

### 5.1. Yeu cau he thong
- Windows 10/11 (64-bit).
- Môi truong MSYS2 UCRT64 da cai dat: GCC, CMake, Ninja, Qt6-Base.

### 5.2. Cac buoc bien dich

1. Mo PowerShell tai thu muc du an:
   ```powershell
   cd C:\Users\LEGION\Downloads\Socket-project
   ```

2. Khoi tao thu muc build voi CMake va Ninja:
   ```powershell
   cmake -B build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
   ```

3. Thuc hien bien dich:
   ```powershell
   cmake --build build
   ```

4. Khoi chay ung dung:
   ```powershell
   .\build\lan_chat.exe
   ```

---

## 6. Huong dan Kiem thu (Testing)

### 6.1. Chay thu nghiem nhieu client tren cung mot may tinh
Mo 2 hoac 3 cua so Terminal va chay dong thoi:
```powershell
# Cua so 1:
.\build\lan_chat.exe

# Cua so 2:
.\build\lan_chat.exe
```
Ung dung se tu dong mo cac cong TCP khac nhau (5000, 5001...) va phat hien nhau thong qua co che UDP broadcast tren card Loopback.

### 6.2. Bat va phan tich goi tin bang Wireshark
1. Chon card mang `Npcap Loopback Adapter` (neu test 1 may) hoac `Wi-Fi / Ethernet` (neu test mang LAN).
2. Nhap bo loc:
   ```text
   udp.port == 9999 || tcp.port >= 5000
   ```
3. Quan sat goi tin UDP `HELLO|...`, qua trinh bat tay 3 buoc TCP (SYN, SYN-ACK, ACK), va cac goi tin PSH chua du lieu `AUTH|...`, `MSG|...`, `FILE|...`.

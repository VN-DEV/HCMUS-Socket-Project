# BANG PHAN CONG NHIEM VU VA HUONG DAN TRIEN KHAI DO AN
## DE TAI: LAN CHAT & FILE SHARING (P2P SOCKET PROGRAMMING)
**Mon hoc:** CSC10008 - Mang may tinh | Truong DH Khoa hoc Tu nhien, DHQG-HCM
**Repository:** https://github.com/VN-DEV/HCMUS-Socket-Project

---

## 1. DANH SACH THANH VIEN NHOM

1. **Nguyen Manh Tri** (Nhom truong)
2. **Do Duy Truong** (Thanh vien)
3. **Mai Huu Tri** (Thanh vien)

---

## 2. BANG PHAN CONG NHIEM VU CHI TIET

| Thanh vien | Module dam nhiem | File ma nguon phu trach | Noi dung cong viec cu the |
| :--- | :--- | :--- | :--- |
| **Nguyen Manh Tri**<br/>(Nhom truong) | **1. Base Architecture & Protocol**<br/>**2. UDP Peer Discovery & Status** | `src/models/Peer.h`<br/>`src/protocol/Protocol.h`<br/>`src/protocol/Protocol.cpp`<br/>`src/network/UdpDiscovery.h`<br/>`src/network/UdpDiscovery.cpp`<br/>`src/main.cpp` | - Khoi tao va duy tri he thong CMakeLists.txt, Winsock startup trong main.cpp.<br/>- Hien thuc serialize/parse cac goi tin: HELLO, AUTH, MSG, FILE, BYE.<br/>- Viet luong broadcast UDP moi 3s qua cong 9999 (SO_BROADCAST).<br/>- Viet luong nhan UDP lang nghe cong 9999 (SO_REUSEADDR).<br/>- Xu ly timeout 10 giay xoa peer offline.<br/>- Dong bo trang thai online va away. |
| **Do Duy Truong** | **3. TCP Network & 1-1 Chat** | `src/network/TcpServer.h`<br/>`src/network/TcpServer.cpp`<br/>`src/network/TcpConnection.h`<br/>`src/network/TcpConnection.cpp` | - Mo cong TCP lang nghe cac ket noi den, chay luong acceptLoop chap nhan client.<br/>- Xu ly tu dong thu cong tiep theo (5001, 5002...) neu cong 5000 bi chiem.<br/>- Ket noi chu dong toi ban chat bang connect() qua IP:Port.<br/>- Gui goi bat tay dinh danh AUTH sau khi vua ket noi.<br/>- Viet luong doc receiverLoop tach cac goi tin theo chuan framing: MSG, BYE.<br/>- Xu ly bat dong bo gui tin nhan qua socket, tranh dính goi TCP. |
| **Mai Huu Tri** | **4. P2P File Transfer**<br/>**5. Chat Manager & UI** | `src/network/TcpConnection.cpp`<br/>`src/chat/ChatManager.h`<br/>`src/chat/ChatManager.cpp`<br/>`src/ui/MainWindow.h`<br/>`src/ui/MainWindow.cpp` | - Gui file tren Worker Thread rieng (sendFileAsync) de khong lam nghen chat.<br/>- Chia file thanh chunk 64KB, gui header FILE va cap nhat tien trinh %.<br/>- Nhan file vao thu muc downloads/, xoa file dang do neu dut mang.<br/>- Quan ly danh sach ket noi active, lich su chat va unread count.<br/>- Hoan thien giao dien do hoa Qt (danh sach peer, khung chat, nut gui file). |
| **Ca 3 thanh vien** | **6. Testing, Report & Demo** | `docs/`, `tests/` | - Chup anh phan tich Wireshark: UDP Broadcast va TCP 3-way handshake.<br/>- Viet bao cao ky thuat (Report 5-10 trang) theo yeu cau muc 7.2.<br/>- Chay thu nghiem 2+ may tren mang LAN de quay video demo 10 phut. |

---

## 3. QUY CHUAN GIAO THUC TANG UNG DUNG (APPLICATION FRAMING)

Moi thanh vien can tuan thu nghiem ngat quy chuan dong goi du lieu de he thong ghep noi khong bi loi:

1. **UDP Presence Broadcast:**
   `HELLO|<name>|<tcp_port>|<status>`
   *Vi du:* `HELLO|Alice|5001|online`

2. **TCP Authentication Handshake:**
   `AUTH|<name>|<tcp_port>|`
   *Vi du:* `AUTH|Alice|5001|`

3. **TCP Message Framing:**
   `MSG|<length>|<utf-8 text>`
   *Vi du:* `MSG|5|Hello`

4. **TCP File Transfer Header:**
   `FILE|<filename>|<size_in_bytes>|`
   *Vi du:* `FILE|slides.pdf|2048000|`

5. **TCP Disconnect:**
   `BYE|`

---

## 4. QUY TRINH PULL / PUSH CODE CHO CAC THANH VIEN

### Buoc 1: Clone repo ve may
```powershell
git clone https://github.com/VN-DEV/HCMUS-Socket-Project.git
cd HCMUS-Socket-Project
```

### Buoc 2: Bien dich du an thu nghiem (Dam bao moi truong san sang)
```powershell
cmake -B build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

### Buoc 3: Lam viec tren branch rieng truoc khi gop vao main
```powershell
# Tao branch moi theo ten cua minh
git checkout -b feature-tcp-chat     # Vi du cho Truong
git checkout -b feature-file-transfer # Vi du cho Huu Tri

# Sau khi code xong va build thanh cong:
git add .
git commit -m "feat: Implement TCP connection receiver loop"
git push origin <ten-branch>
```
Nhom truong (Nguyen Manh Tri) se review va merge vao branch `main`.

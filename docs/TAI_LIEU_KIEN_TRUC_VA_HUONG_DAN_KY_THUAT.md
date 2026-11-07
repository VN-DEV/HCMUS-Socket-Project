# TAI LIEU KIEN TRUC HE THONG VA HUONG DAN KY THUAT
## MON HOC: MANG MAY TINH (CSC10008) - HCMUS
## DE TAI: UNG DUNG TRO CHUYEN VA CHIA SE TEP TIN NGANG HANG TRONG MANG LAN

---

### MUC LUC
1. Tong quan du an va Muc tieu ky thuat
2. Kien truc tong the he thong (Peer-to-Peer Architecture)
3. Mo hinh da luong (Multi-threading Model) va Dong bo du lieu
4. Dac ta chi tiet Giao thuc Tang Ung dung (Application Layer Protocols)
5. He thong Cong cu va Cong nghe su dung (Tools & Technology Stack)
6. To chuc Cau truc Ma nguon (Source Tree Organization)
7. Huong dan Thiet lap Moi truong va Bien dich (Build Instructions)
8. Huong dan Kiem thu va Bat goi tin bang Wireshark
9. Quy uoc Quan ly Ma nguon va Dong gop (Git Workflow)

---

### 1. TONG QUAN DU AN VA MUC TIEU KY THUAT
Du an nham muc dich hien thuc mot he thong truyen thong thoi gian thuc trong mang noi bo (Local Area Network - LAN) theo mo hinh ngang hang thuan tuy (Pure Peer-to-Peer), khong phu thuoc vao bat ky may chu trung tam (Centralized Server) nao.

Cac yeu cau ky thuat cot loi:
- Tu dong phat hien cac nut mang (Peer Discovery) su dung co che UDP Broadcast dinh ky.
- Duy tri danh sach cac nut dang hoat dong (Presence Management) thong qua co che Heartbeat va kiem soat Timeout 10 giay.
- Thiet lap ket noi tin cay 1-1 su dung TCP Socket de trao doi tin nhan van ban va truyen tap tin.
- Giai quyet triet de van de dinh goi (Packet Sticking) va cat goi (Packet Fragmentation) trong luong du lieu TCP bang co che phan khung (Message Framing / Length-prefixed Framing).
- Truyen tap tin kich thuoc bat ky duoi dang cac khoi nhi phan (Chunking 64 KB), bao dam khong lam tran bo dem (RAM) va hien thi tien do thoi gian thuc tren giao dien.
- Giao dien nguoi dung truc quan su dung Qt6 Widgets.

---

### 2. KIEN TRUC TONG THE HE THONG

He thong hoat dong theo kien truc lai giua UDP va TCP (Hybrid Transport Layer Architecture):
1. UDP (User Datagram Protocol):
   - Dong vai tro la kenh phat tin hieu quang ba (Broadcast Channel).
   - Su dung cong co dinh 9999 de cac peer tim thay nhau ma khong can cau hinh dia chi IP truoc.
   - Tan dung uu diem toc do cao, khong ton chi phi duy tri phien ket noi cua UDP.
2. TCP (Transmission Control Protocol):
   - Dong vai tro la kenh truyen thong du lieu tin cay (Reliable Data Channel).
   - Moi client dong thoi dong vai tro vua la Server (lang nghe ket noi den tren cong 5000+) vua la Client (chu dong tao ket noi toi cac peer khac).
   - Dam bao du lieu tin nhan va noi dung file duoc truyen nguyen ven, dung thu tu, co xac nhan ACK o tang transport.

So do hoat dong tong quat:
- Buoc 1: Client A khoi dong -> Mo cong TCP lang nghe (vi du 5001) -> Bat dau broadcast HELLO|A|5001|online toi 255.255.255.255:9999.
- Buoc 2: Client B tren cung mang LAN nhan duoc goi UDP cua A -> Them A vao danh ba online peers -> Broadcast goi HELLO|B|5002|online cua minh.
- Buoc 3: Client A nhan duoc goi UDP cua B -> Cap nhat B vao danh ba online peers.
- Buoc 4: Khi nguoi dung o Client A muon gui tin nhan hoac file cho B -> Client A tao socket TCP ket noi toi IP_B:5002 -> Gui bat tay xac thuc AUTH|A|5001| -> Gui noi dung tin nhan (MSG) hoac goi tin file (FILE).

---

### 3. MO HINH DA LUONG (MULTI-THREADING MODEL)

De giao dien nguoi dung (Qt UI) luon phan hoi muot ma va cac thao tac mang khong bi chan (Non-blocking I/O), he thong su dung kien truc da luong chuyen biet:

1. Luong chinh (Main / GUI Thread):
   - Chay vong lap su kien Qt (QApplication::exec()).
   - Xu ly cac tuong tac cua nguoi dung: go tin nhan, nhan nut, chon ban chat, mo hop thoai chon file.
   - Cap nhat danh sach online peers, khung chat va thanh tien do file.
   - Tuyet doi khong thuc hien cac lenh blocking socket (nhu recv, connect, send file lon) tren luong nay.

2. Luong phat Broadcast UDP (Broadcast Sender Thread):
   - Chay ngam trong UdpDiscovery.
   - Vong lap ngu 3 giay va gui goi HELLO qua socket UDP broadcast.

3. Luong thu Broadcast UDP (Broadcast Listener Thread):
   - Chay ngam trong UdpDiscovery, chan tai lenh recvfrom() tren cong 9999.
   - Khi co goi HELLO den, giai ma va cap nhat vao bang danh ba peer (dung mutex bao ve).

4. Luong kiem tra Timeout (Heartbeat Timeout Thread):
   - Chay ngam trong UdpDiscovery, dinh ky 1 giay quet danh ba peer.
   - Neu (now - lastSeen) > 10 giay, xoa peer khoi danh sach online va thong bao cho UI cap nhat.

5. Luong chap nhan ket noi TCP (TCP Acceptor Thread):
   - Chay ngam trong TcpServer, chan tai lenh accept() tren cong TCP mo cua may.
   - Khi co peer ket noi den, tao doi tuong TcpConnection moi de xu ly phien lam viec.

6. Luong thu du lieu TCP (TCP Receiver Thread):
   - Moi ket noi TcpConnection so huu 1 luong thu rieng.
   - Chan tai lenh recv() de doc va giai ma cac khung du lieu (AUTH, MSG, FILE, BYE).
   - Khi nhan tin nhan hoac nhan du lieu file, phat tin hieu chuyen du lieu an toan ve luong GUI qua co che Signal/Slot hoac QMetaObject::invokeMethod.

7. Luong gui tap tin (File Sender Worker Thread):
   - Khi nguoi dung gui file, mot luong doc lap duoc tao ra de doc file tu o cung va goi send() tung khoi 64 KB, cap nhat phan tram ma khong lam dung phien chat.

---

### 4. DAC TA CHI TIET GIAO THUC TANG UNG DUNG

Giao thuc su dung ky tu gach dung '|' (pipe) lam ky tu phan tach cac truong (Field Delimiter), ma hoa van ban UTF-8.

#### 4.1. Giao thuc UDP Presence & Heartbeat
- Cu phap:
  `HELLO|<display_name>|<tcp_port>|<status>`
- Cac truong:
  + HELLO: Tu khoa nhan dien ban tin quang ba.
  + display_name: Ten hien thi cua nguoi dung (vi du: Alice, Bob).
  + tcp_port: Cong TCP ma peer nay mo de nhan chat/file (vi du: 5000, 5001).
  + status: Trang thai cua peer ("online" hoac "away").
- Chu ky phat: 3 giay / lan.
- Co che Timeout: Neu sau 10 giay khong nhan duoc goi HELLO tu mot peer, peer do duoc coi la offline va go bo khoi danh sach hien thi.

#### 4.2. Giao thuc TCP Authentication Handshake
- Cu phap:
  `AUTH|<my_name>|<my_tcp_port>|`
- Muc dich:
  Ngay khi ket noi TCP duoc thiet lap thanh cong giua hai peer, ben ket noi gui goi tin AUTH de ben nhan biet chinh xac danh tinh va so cong lang nghe cua nguoi vua ket noi.

#### 4.3. Giao thuc TCP Message Framing (Chong dinh goi / cat goi)
- Cu phap:
  `MSG|<length>|<utf8_text_content>`
- Cac truong:
  + MSG: Ma lenh xac dinh goi tin nhan van ban.
  + length: Do dai noi dung tin nhan tinh theo so luong byte.
  + utf8_text_content: Chuoi van ban UTF-8 co dung `length` byte.
- Co che xu ly dong du lieu (Stream Processing):
  Vi TCP la luong byte lien tuc (Byte Stream), nhieu tin nhan co the bi gop vao mot lan doc hoac mot tin nhan bi chia thanh nhieu lan doc.
  Ben nhan doc tien to "MSG|", sau do doc do dai "length|", sau do lap goi recv() cho den khi nhan du dung `length` byte.

#### 4.4. Giao thuc TCP File Transfer
- Cu phap Header:
  `FILE|<file_name>|<file_size_in_bytes>|`
- Quy trinh truyen:
  1. Ben gui gui chuoi header "FILE|<file_name>|<file_size>|".
  2. Ben gui lien tiep doc file tu o dia thanh tung khoi co kich thuoc 64 KB (65,536 bytes) va goi send() qua socket.
  3. Ben nhan doc header, tao file moi trong thu muc `downloads/<file_name>`, doc tuan tu cac byte nhi phan tu socket va ghi xuong o dia cho den khi du `file_size_in_bytes`.
  4. Ca ben gui va ben nhan deu tinh toan:
     `percent = (bytes_transferred * 100) / file_size`
     va cap nhat thanh tien do (QProgressBar).

#### 4.5. Giao thuc TCP Disconnect
- Cu phap:
  `BYE|`
- Muc dich: Thong bao chu dong dong phien trao doi truoc khi ngat socket.

---

### 5. HE THONG CONG CU VA CONG NGHE SU DUNG

#### 5.1. Ngon ngu lap trinh C++ (Tieu chuan C++17)
- C++17 cung cap cac tinh nang hien dai giup quan ly bo nho an toan, ho tro lap trinh da luong chuan:
  + `std::thread`, `std::mutex`, `std::lock_guard`, `std::atomic` giup he thong chay on dinh khong bi data race.
  + `std::string`, `std::vector`, `std::map`, `std::function` giup ma nguon gon gang, hien dai.

#### 5.2. Trinh bien dich GCC (MinGW-w64 UCRT64)
- Su dung GCC 16.2.0 tren nen UCRT (Universal C Runtime) cua Windows.
- UCRT tuong thich toi da voi he dieu hanh Windows 10/11, ho tro day du cac ham UTF-8 va Windows API.

#### 5.3. He thong quan ly cau hinh Build: CMake
- CMake la cong cu quan ly tien trinh bien dich doc lap voi IDE va trinh bien dich.
- File `CMakeLists.txt` mo ta toan bo cac dependency, cac tap tin ma nguon, cac co bien dich (-Wall, -Wextra, -O2), va chi dinh thu vien can lien ket (`Qt6::Widgets`, `ws2_32`).
- CMake giup bat ky thanh vien nao trong nhom deu co the tao ra ban build chuan xac chi voi mot dong lenh duy nhat.

#### 5.4. He thong thuc thi Build: Ninja
- Ninja la cong cu thuc thi lenh bien dich tap trung vao toc do toi da (Speed-focused Build System).
- So sanh Ninja voi Make truyen thong:
  + Make: Doc file Makefile, tinh toan do thi phu thuoc (dependency graph) rat cham khi du an co nhieu file.
  + Ninja: Duoc thiet ke de nhan file mo ta da tinh toan truoc tu CMake (`build.ninja`). Ninja thuc thi bien dich song song (Parallel Compilation) tren toan bo cac core CPU voi toc do cuc nhanh, giam thieu thoi gian cho doi khi build ma nguon.

#### 5.5. Thu vien Giao dien: Qt6 Widgets
- Cung cap he thong component giao dien manh me: `QMainWindow`, `QSplitter`, `QListWidget`, `QTextBrowser`, `QLineEdit`, `QPushButton`, `QProgressBar`.
- Co che Signal & Slot cho phep ket noi cac su kien bat dong bo giua cac luong mot cach an toan.

#### 5.6. Thu vien Mang: Windows Sockets 2 (ws2_32)
- Theo dung yeu cau hoc thuat cua mon hoc Mang may tinh: Lap trinh truc tiep tren Native Socket API (Winsock2) thay vi su dung lop wrap cap cao (nhu QTcpSocket).
- Giup sinh vien hieu sau ban chat cac loi goi he thong: `WSAStartup`, `socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`, `select`, `closesocket`, `WSACleanup`.

#### 5.7. Cong cu bat goi tin: Wireshark
- Dung de quan sat thuc te cac goi tin UDP Broadcast (cong 9999) va cac phien TCP Handshake (SYN, SYN-ACK, ACK), luong du lieu TCP Segment (MSG, FILE) di qua card mang.

---

### 6. TO CHUC CAU TRUC MA NGUON

```
Socket-project/
|-- CMakeLists.txt              # File cau hinh bien dich CMake
|-- README.md                   # Tai lieu huong dan tong quan va su dung
|-- .gitignore                  # Cac tep tin va thu muc can bo qua khi commit Git
|-- docs/                       # Thu muc chua tai lieu du an
|   |-- PHAN_CONG_NHIEM_VU.md   # Bang phan cong chi tiet cho 3 thanh vien
|   |-- Phan_Cong_Nhiem_Vu_Do_An_Socket.pdf
|   |-- TAI_LIEU_KIEN_TRUC_VA_HUONG_DAN_KY_THUAT.md
|   |-- Tai_Lieu_Kien_Truc_Va_Huong_Dan_Ky_Thuat.pdf
|-- downloads/                  # Thu muc tu dong luu cac file duoc nhan ve
|-- src/
    |-- main.cpp                # Diem khoi dau: WSAStartup, khoi dong Qt App
    |-- models/
    |   |-- Peer.h              # Cau truc thong tin Peer (name, ip, port, status, lastSeen)
    |   |-- ChatMessage.h       # Cau truc thong tin tin nhan
    |-- protocol/
    |   |-- Protocol.h          # Khai bao dong goi va giai ma goi tin giao thuc
    |   |-- Protocol.cpp        # Hien thuc split, build/parse UDP & TCP packet
    |-- network/
    |   |-- UdpDiscovery.h      # Khai bao bo phat hien ngang hang UDP Broadcast
    |   |-- UdpDiscovery.cpp    # Hien thuc 3 luong: sender, listener, timeout
    |   |-- TcpServer.h         # Khai bao server TCP lang nghe ket noi den
    |   |-- TcpServer.cpp       # Hien thuc socket bind, listen, accept loop
    |   |-- TcpConnection.h     # Khai bao ket noi TCP 1-1 voi mot peer
    |   |-- TcpConnection.cpp   # Hien thuc gui/nhan auth, tin nhan va truyen file
    |-- chat/
    |   |-- ChatManager.h       # Khai bao bo dieu phoi phien chat, unread count
    |   |-- ChatManager.cpp     # Hien thuc quan ly ket noi, lich su tin nhan
    |-- ui/
        |-- MainWindow.h        # Khai bao cua so giao dien chinh Qt6
        |-- MainWindow.cpp      # Hien thuc bo cuc giao dien va xu ly su kien
```

---

### 7. HUONG DAN THIET LAP MOI TRUONG VA BIEN DICH

#### 7.1. Yeu cau he thong
- He dieu hanh: Windows 10 hoac Windows 11 (64-bit).
- Bo cong cu MSYS2 voi UCRT64 da cai dat:
  + GCC / G++ (phien ban 13 tro len)
  + CMake (phien ban 3.20 tro len)
  + Ninja (phien ban 1.10 tro len)
  + Qt6 Base (`mingw-w64-ucrt-x86_64-qt6-base`)

#### 7.2. Cac buoc bien dich

1. Mo PowerShell hoac Terminal tai thu muc goc cua du an:
   ```powershell
   cd C:\Users\LEGION\Downloads\Socket-project
   ```

2. Khoi tao thu muc build voi CMake va trinh sinh Ninja:
   ```powershell
   cmake -B build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
   ```

3. Thuc hien bien dich:
   ```powershell
   cmake --build build
   ```

4. Chay ung dung:
   ```powershell
   .\build\lan_chat.exe
   ```

---

### 8. HUONG DAN KIEM THU VA BAT GOI TIN BANG WIRESHARK

De phuc vu viec bao cao nghiem thu mon hoc, sinh vien can ghi hinh va bat goi tin chung minh he thong hoat dong dung giao thuc.

1. Chuan bi Wireshark:
   - Khoi dong Wireshark tren may tinh.
   - Chon card mang can bat:
     + Neu chay tren mang LAN thuc te: Chon card Wi-Fi hoac Ethernet.
     + Neu chay thu nghiem nhieu client tren 1 may tinh: Chon card "Npcap Loopback Adapter".

2. Bo loc bat goi tin (Display Filter):
   - De quan sat tat ca cac goi tin cua ung dung:
     `udp.port == 9999 || tcp.port >= 5000`
   - De chi quan sat cac goi broadcast hien dien:
     `udp.port == 9999`
   - De chi quan sat cac goi trao doi tin nhan va truyen file:
     `tcp.port >= 5000 && ip.addr == <dia_chi_ip_peer>`

3. Cac dau hieu kiem chung trong Wireshark:
   - Goi UDP: Xuat hien dinh ky 3 giay mot goi directed toi dia chi `255.255.255.255`, noi dung bat dau bang chuoi `HELLO|...`.
   - Goi TCP: Khi bat dau nhan tin hoac gui file, quan sat thay qua trinh bat tay 3 buoc (Three-Way Handshake: SYN -> SYN-ACK -> ACK), tiep theo la goi tin PSH-ACK mang du lieu `AUTH|...` va `MSG|...` hoac `FILE|...`.

---

### 9. QUY UOC QUAN LY MA NGUON VA DONG GOP (GIT WORKFLOW)

- Nhanh chinh (Branch): `main` luon giu trang thai bien dich on dinh.
- Quy trinh lam viec:
  1. Moi thanh vien tao branch rieng khi thuc hien tinh nang:
     `git checkout -b feature/<ten_tinh_nang>`
  2. Commit ma nguon thuong xuyen voi thong diep ro rang, chuyen nghiep:
     `git commit -m "feat(network): implement udp broadcast receiver thread"`
  3. Tuyet doi khong su dung cac ky tu bieu tuong cam xuc (emoji) trong commit message, chu thich ma nguon va tai lieu.
  4. Day len remote va tao Pull Request de cac thanh vien cung review truoc khi gop vao `main`.


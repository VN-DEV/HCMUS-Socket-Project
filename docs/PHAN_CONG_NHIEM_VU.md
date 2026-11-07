# BANG PHAN CONG NHIEM VU VA KE HOACH THUC HIEN DO AN
## MON HOC: MANG MAY TINH (CSC10008) - HCMUS
## DE TAI: UNG DUNG CHAT VA TRUYEN FILE TRONG MANG LAN THEO MO HINH PEER-TO-PEER (P2P)

---

### THONG TIN CHUNG
- Truong: Dai hoc Khoa hoc Tu nhien - Dai hoc Quoc gia TP.HCM
- Khoa: Cong nghe Thong tin
- Mon hoc: Mang may tinh (CSC10008)
- De tai: Xay dung ung dung tro chuyen va chia se file ngang hang trong mang LAN
- Ngon ngu va Nen tang: C++17, Qt6 Widgets, Windows Sockets (Winsock2), CMake, Ninja
- Kho ma nguon (GitHub): https://github.com/VN-DEV/HCMUS-Socket-Project

---

### DANH SACH THANH VIEN DANG KY

1. Nguyen Manh Tri
   - Vai tro: Thanh vien
   - Linh vuc phu trach: Giao thuc mang & Co che Peer Discovery (UDP)

2. Do Duy Truong
   - Vai tro: Thanh vien
   - Linh vuc phu trach: Mang TCP & Xu ly Ket noi Tin nhan (TCP Messaging)

3. Mai Huu Tri
   - Vai tro: Thanh vien
   - Linh vuc phu trach: Giao dien nguoi dung (Qt6 UI) & Module Truyen File (TCP File Transfer)

---

### PHAN CONG CHI TIET TUNG THANH VIEN

#### THANH VIEN 1: NGUYEN MANH TRI
Phu trach module: Giao thuc, Dong goi du lieu & Peer Discovery (UDP)

1. Thiet ke va dinh nghia giao thuc tang ung dung (src/protocol/Protocol.h, src/protocol/Protocol.cpp):
   - Xay dung bo dinh dang goi tin UDP: HELLO|<name>|<tcp_port>|<status>
   - Xay dung bo dinh dang goi tin TCP Bat tay: AUTH|<name>|<tcp_port>|
   - Xay dung bo dinh dang goi tin TCP Tin nhan: MSG|<length>|<text>
   - Xay dung bo dinh dang goi tin TCP File Header: FILE|<filename>|<size>|
   - Xay dung cac ham parser de tach du lieu (split) theo ky tu phan cach '|' an toan, chong tran bo dem.

2. Phat trien module Peer Discovery bang UDP Broadcast (src/network/UdpDiscovery.h, src/network/UdpDiscovery.cpp):
   - Tao socket UDP (AF_INET, SOCK_DGRAM), kich hoat tuy chon SO_BROADCAST va SO_REUSEADDR.
   - Hien thuc Luong Phat (Broadcast Sender Thread): Dinh ky moi 3 giay gui broadcast toi 255.255.255.255:9999 thong bao su hien dien va trang thai (online/away).
   - Hien thuc Luong Thu (Broadcast Listener Thread): Lang nghe tren cong 9999, tiep nhan goi HELLO, cap nhat hoac them moi vao danh ba peer.
   - Hien thuc Luong Kiem tra Timeout (Heartbeat Timeout Thread): Dinh ky 1 giay quet danh ba, peer nao khong phat broadcast sau 10 giay se bi go khoi danh sach online.

3. Quan ly trang thai nguoi dung (src/models/Peer.h):
   - Chuyen doi trang thai Online / Away va phat broadcast cap nhat ngay lap tuc cho cac peer khac.

4. Diem khoi tao ung dung (src/main.cpp):
   - Khoi dong Winsock (WSAStartup) va giai phong (WSACleanup).
   - Tich hop he thong log va ho tro bat goi tin bang Wireshark tren card mang LAN / Loopback.

---

#### THANH VIEN 2: DO DUY TRUONG
Phu trach module: TCP Server & Dong bo Ket noi Tin nhan (TCP Messaging)

1. Phat trien TCP Listening Server (src/network/TcpServer.h, src/network/TcpServer.cpp):
   - Tao socket TCP (AF_INET, SOCK_STREAM, IPPROTO_TCP), bat SO_REUSEADDR.
   - Bind vao cong chi dinh (mac dinh 5000), co co che thu cong lien tiep (5001, 5002...) neu cong dang ban (ho tro chay nhieu instance tren cung 1 may tinh de test).
   - Chuyen socket sang che do listen(SOMAXCONN).
   - Phat trien Luong Accept (TCP Acceptor Thread): Goi accept() don cac ket noi den va chuyen socket ket noi cho module quan ly.

2. Phat trien module TCP Connection (src/network/TcpConnection.h, src/network/TcpConnection.cpp):
   - Hien thuc co che Bat tay xac thuc (Authentication Handshake): Khi ket noi thanh cong, chu dong gui AUTH|<myName>|<myTcpPort>| de hai ben biet danh tinh va luu lai so cong TCP cua nhau.
   - Hien thuc Luong Thu Du Lieu (Receiver Worker Thread): Doc du lieu theo framing cua giao thuc.
   - Xu ly Framing chong dinh goi (Packet Sticking) va cat goi (Packet Fragmentation): Doc ky tu phan cach, doc do dai length truoc, sau do doc chinh xac du byte noi dung.
   - Goi callback onMessageReceived de ban tin nhan ve tang quan ly phien chat.
   - Phat hien ngat ket noi (recv() tra ve 0 hoac SOCKET_ERROR) va don dep socket sach se, kich hoat callback onDisconnected.

3. Dong bo gui tin nhan an toan da luong (Thread-Safety):
   - Su dung std::mutex dam bao chi co duy nhat 1 luong duoc phep goi send() tren cung mot socket tai mot thoi diem, tranh xung dot khi vua gui tin nhan vua truyen file.

---

#### THANH VIEN 3: MAI HUU TRI
Phu trach module: Truyen File Chunked (TCP) & Giao dien nguoi dung (Qt6 UI)

1. Phat trien module Truyen File theo khoi qua TCP (src/network/TcpConnection.cpp, src/chat/ChatManager.cpp):
   - Luong Gui File (File Sender Worker Thread): Mo file nhi phan (binary mode), doc kich thuoc file, gui goi tin header FILE|<filename>|<filesize>|.
   - Chia nho file thanh tung khoi 64 KB (Chunking) va gui tuan tu qua socket.
   - Tinh toan tien do gui phan tram (%) va kich hoat callback onProgress de cap nhat len giao dien.
   - Luong Nhan File (File Receiver): Khi nhan header FILE, tu dong tao tap tin trong thu muc downloads/, tiep tuc doc dung so byte cua file va ghi vao dia.
   - Xu ly bao loi khi file bi huy hoac mat ket noi giua chung.

2. Quan ly Phien chat va Lich su Tin nhan (src/chat/ChatManager.h, src/chat/ChatManager.cpp):
   - Quan ly danh sach cac ket noi dang hoat dong voi tung peer (std::map<std::string, TcpConnection*>).
   - Luu tru lich su tin nhan theo tung peer trong RAM.
   - Xu ly dem so tin nhan chua doc (Unread Badge / Count) khi co tin nhan den tu peer khac voi peer dang mo hoi thoai hien tai.

3. Thiet ke va lap trinh Giao dien do hoa (src/ui/MainWindow.h, src/ui/MainWindow.cpp):
   - Su dung Qt6 Widgets voi bo cuc 2 cot (QSplitter):
     + Cot trai: Ho so nguoi dung (ten, IP, cong TCP), Dropdown doi trang thai (Online/Away), Danh sach ban be online (QListWidget) kem so tin nhan chua doc (Unread Count).
     + Cot phai: Khung lich su tro chuyen (QTextBrowser) hien thi ro nguoi gui, gio gui va noi dung; Khung thong bao tien trinh truyen/nhan file (QProgressBar); O nhap tin nhan (QLineEdit) va cac nut chuc nang (Send, Send File).
   - Chuyen tiep an toan tin hieu giua cac luong mang Winsock sang luong GUI cua Qt (su dung Qt Signals & Slots hoac QMetaObject::invokeMethod) tranh gay treo giao dien.

---

### KE HOACH TIEN DO THUC HIEN (TIMELINE)

| Tuan | Muc tieu can dat | Thanh vien phu trach | San pham ban giao |
| :--- | :--- | :--- | :--- |
| Tuan 1 (08/11 - 15/11) | Khoi tao du an, cau hinh CMake & Ninja, viet khung ma nguon, thiet ke giao thuc chi tiet | Ca 3 thanh vien | Repo GitHub chuan, ban build khung thanh cong |
| Tuan 2 (16/11 - 22/11) | Hoan thien UDP Discovery (broadcast & timeout) va TCP Server (listen & accept) | Nguyen Manh Tri, Do Duy Truong | Danh sach peer cap nhat tu dong qua broadcast |
| Tuan 3 (23/11 - 29/11) | Hoan thien module gui nhan tin nhan TCP va xu ly framing chong dinh goi | Do Duy Truong, Mai Huu Tri | Chat 1-1 qua TCP hoat dong on dinh |
| Tuan 4 (30/11 - 06/12) | Phat trien truyen file chunked 64KB va ghep noi giao dien Qt6 Widgets hoan chinh | Mai Huu Tri, Nguyen Manh Tri | Gui duoc file, hien thi tien do % tren giao dien |
| Tuan 5 (07/12 - 14/12) | Kiem thu toan dien (Stress test, nhieu client tren LAN, bat goi Wireshark), viet bao cao | Ca 3 thanh vien | Bao cao PDF, file .pcapng bat goi tin, video demo |

---

### TIEU CHI DANH GIA DONG GOP
- Tat ca thanh vien dong gop ngang nhau (33.3% moi nguoi).
- Ma nguon duoc quan ly thong qua GitHub Issues va Pull Requests tren branch rieng truoc khi merge vao main.
- Moi commit tuan thu quy tac ro rang, ghi chu thich day du, khong su dung emoji.

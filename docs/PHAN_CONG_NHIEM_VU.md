# BANG PHAN CONG NHIEM VU DO AN
## MON HOC: MANG MAY TINH (CSC10008) - HCMUS
## DE TAI: UNG DUNG TRO CHUYEN VA CHIA SE TEP TIN TRONG MANG LAN THEO MO HINH PEER-TO-PEER (P2P)

---

### 1. THONG TIN CHUNG
- Truong: Dai hoc Khoa hoc Tu nhien - Dai hoc Quoc gia TP.HCM
- Khoa: Cong nghe Thong tin
- Mon hoc: Mang may tinh (CSC10008)
- De tai: Xay dung ung dung tro chuyen va chia se file ngang hang trong mang LAN
- Ngon ngu va Nen tang: C++17, Qt6 Widgets, Windows Sockets 2 (ws2_32), CMake, Ninja
- Kho ma nguon (GitHub): https://github.com/VN-DEV/HCMUS-Socket-Project

---

### 2. DANH SACH THANH VIEN VA TY LE DONG GOP

Tat ca thanh vien co vai tro va ty le dong gop dong deu nhau (33.3% moi nguoi):

1. Nguyen Manh Tri
   - Vai tro: Thanh vien
   - Linh vuc phu trach chinh: Giao dien nguoi dung (Qt6 UI), Giao thuc mang & Peer Discovery (UDP)
   - Ty le dong gop: 33.3%

2. Do Duy Truong
   - Vai tro: Thanh vien
   - Linh vuc phu trach chinh: He thong TCP Listening Server & Xu ly Ket noi Tin nhan (TCP Messaging)
   - Ty le dong gop: 33.3%

3. Mai Huu Tri
   - Vai tro: Thanh vien
   - Linh vuc phu trach chinh: Module Truyen File Chunked 64KB (TCP) & Dieu phoi Phien Chat (Chat Manager)
   - Ty le dong gop: 33.3%

---

### 3. CHI TIET PHAN CONG NHIEM VU TUNG THANH VIEN

#### 3.1. THANH VIEN: NGUYEN MANH TRI (33.3%)
Phu trach module: Giao dien nguoi dung (Qt6 Widgets), Giao thuc Mang & Peer Discovery (UDP)

1. Thiet ke va Lap trinh Giao dien do hoa (src/ui/MainWindow.h, src/ui/MainWindow.cpp):
   - Xay dung bo cuc giao dien truc quan 2 cot su dung QSplitter:
     + Cot trai: Ho so nguoi dung (hien thi ten, IP, port TCP), dropdown lua chon trang thai (Online / Away), danh sach cac ban chat online (QListWidget) kem so tin nhan chua doc (Unread Count).
     + Cot phai: Tieu de nguoi dang chat, khung lich su tro chuyen thoi gian thuc (QTextBrowser), thanh tien trinh truyen file (QProgressBar) kem nhan thong bao %, o nhap noi dung tin nhan (QLineEdit) va cac nut gui tin nhan, nut mo hop thoai chon file (QFileDialog).
   - Ket noi cac su kien giao dien (Signals & Slots) voi backend: bat su kien click danh sach peer, go Enter/bam Send gui tin nhan, bam Send File mo hop thoai.
   - Dam bao tinh an toan da luong giua cac luong socket Winsock va luong giao dien Qt (Main Thread).

2. Thiet ke va Dinh nghia Giao thuc Tang Ung dung (src/protocol/Protocol.h, src/protocol/Protocol.cpp):
   - Xay dung bo dinh dang va phan tich goi tin UDP: HELLO|<name>|<tcp_port>|<status>
   - Xay dung bo dinh dang va phan tich goi tin TCP Bat tay: AUTH|<name>|<tcp_port>|
   - Xay dung bo dinh dang goi tin TCP Tin nhan: MSG|<length>|<text>
   - Xay dung bo dinh dang goi tin TCP File Header: FILE|<filename>|<size>|
   - Xay dung ham split() cat chuoi an toan theo ky tu phan cach '|' ngan ngua tran bo dem.

3. Phat trien Module Peer Discovery qua UDP Broadcast (src/network/UdpDiscovery.h, src/network/UdpDiscovery.cpp):
   - Tao socket UDP, cau hinh tuy chon SO_BROADCAST va SO_REUSEADDR.
   - Luong Phat (broadcastLoop): Dinh ky 3 giay phat goi HELLO toi 255.255.255.255:9999 de quang ba su hien dien.
   - Luong Thu (listenLoop): Lang nghe cong 9999, tiep nhan goi HELLO va cap nhat danh sach online peers.
   - Luong Kiem tra Timeout (timeoutLoop): Dinh ky 1 giay quet danh ba, xoa peer sau 10 giay khong co tin hieu.
   - Cap nhat trang thai Online / Away va phat thong bao broadcast tuc thoi khi thay doi trang thai.

4. Diem khoi dau ung dung (src/main.cpp):
   - Khoi tao Winsock (WSAStartup) va giai phong tai nguyen mang (WSACleanup).
   - Khoi chay ung dung giao dien QApplication.

---

#### 3.2. THANH VIEN: DO DUY TRUONG (33.3%)
Phu trach module: TCP Server & Dong bo Ket noi Tin nhan (TCP Messaging)

1. Phat trien TCP Listening Server (src/network/TcpServer.h, src/network/TcpServer.cpp):
   - Tao socket TCP (AF_INET, SOCK_STREAM, IPPROTO_TCP), bat SO_REUSEADDR.
   - Bind vao cong chi dinh (mac dinh 5000), tu dong thu cac cong lien tiep (5001, 5002...) neu bi trung khi chay nhieu instance tren cung 1 may.
   - Chuyen socket sang che do listen(SOMAXCONN).
   - Luong Chap nhan Ket noi (acceptLoop): Goi accept() don cac ket noi TCP den va chuyen socket ket noi cho module quan ly thong qua callback onNewConnection.

2. Phat trien Quan ly Ket noi TCP (src/network/TcpConnection.h, src/network/TcpConnection.cpp):
   - Hien thuc co che Bat tay xac thuc (Authentication Handshake): Gui goi AUTH|<myName>|<myTcpPort>| ngay sau khi ket noi thanh cong de hai ben ghi nhan danh tinh cua nhau.
   - Luong Thu Du Lieu (receiverLoop): Doc du lieu theo quy chuan framing tu socket TCP.
   - Xu ly Framing chong dinh goi (Packet Sticking) va cat goi (Packet Fragmentation): Doc phan tien to MSG, trich xuat do dai length, sau do lap goi recv() cho den khi doc du dung so byte cua tin nhan truoc khi ban len tang ung dung.
   - Goi callback onMessageReceived de chuyen tin nhan ve he thong.
   - Nhan dien ngat ket noi (recv tra ve <= 0 hoac SOCKET_ERROR) va don dep socket an toan, kich hoat callback onDisconnected.

3. Dong bo Gui Du lieu An toan Da luong (Thread-Safety):
   - Su dung std::mutex dam bao chi co duy nhat 1 luong duoc goi send() tren cung mot socket tai mot thoi diem, ngan ngua pha vo luong byte stream khi gui dong thoi tin nhan va du lieu file.

---

#### 3.3. THANH VIEN: MAI HUU TRI (33.3%)
Phu trach module: Truyen File Chunked 64KB (TCP) & Dieu phoi Phien Chat (Chat Manager)

1. Phat trien Module Truyen Tap tin theo khoi qua TCP (src/network/TcpConnection.cpp):
   - Luong Gui File (sendFileWorker): Mo tap tin nhi phan, lay kich thuoc file, gui header FILE|<filename>|<filesize>|.
   - Chia nho file thanh cac khoi 64 KB (65,536 bytes) va goi send() tuan tu qua socket.
   - Tinh toan tien do phan tram: percent = (sentBytes * 100) / fileSize va kich hoat callback onProgress de cap nhat len thanh tien trinh giao dien theo thoi gian thuc.
   - Luong Nhan File: Doc header FILE, tao file moi trong thu muc downloads/, tiep tuc doc dung so byte cua file va ghi xuong o dia.
   - Xu ly xoa file tam neu qua trinh truyen file bi gian doan hoac gap loi ket noi.

2. Phat trien Bo Dieu phoi Phien chat (src/chat/ChatManager.h, src/chat/ChatManager.cpp):
   - Quan ly danh sach cac doi tuong ket noi TCP dang hoat dong voi tung peer (std::map<std::string, TcpConnection*>).
   - Luu tru lich su tat ca tin nhan trong RAM theo tung peer (std::map<std::string, std::vector<ChatMessage>>).
   - Xu ly chuyen doi ban chat khi nguoi dung chon peer tren giao dien (selectPartner).
   - Quan ly bo dem so tin nhan chua doc (Unread Count): Khi nhan tin nhan tu peer khac voi nguoi dang mo hoi thoai hien tai, tu dong tang bien dem va kich hoat callback onUnreadCountChanged de cap nhat so hien thi tren danh sach ban be.

---

### 4. TIEU CHI DANH GIA DONG GOP VA QUY UOC LAM VIEC
- Tat ca 3 thanh vien dong gop dong deu 33.3% khoi luong cong viec cua do an.
- Ma nguon duoc quan ly tap trung tren GitHub, phat trien tren cac nhanh (branch) rieng va merge vao main qua Pull Request.
- Moi commit tuan thu quy tac ro rang, ghi chu thich day du, khong su dung bat ky ky tu bieu tuong cam xuc (emoji) nao.


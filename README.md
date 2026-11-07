# HCMUS LAN Chat & File Sharing

A peer-to-peer (P2P) local area network communication and file transfer application built with C++17, native Windows Sockets 2 (ws2_32), and Qt6 Widgets.
Developed for the CSC10008 Computer Networks course at VNU-HCM University of Science.

---

## 1. Prerequisites

- Operating System: Windows 10 or Windows 11 (64-bit).
- Build Environment: MSYS2 with UCRT64 toolchain:
  - GCC / G++ (version 11 or higher, with C++17 support)
  - CMake (version 3.20 or higher)
  - Ninja build system
  - Qt6 Base (`mingw-w64-ucrt-x86_64-qt6-base`)
- Network Library: Windows Sockets 2 (`ws2_32.lib` - bundled with Windows).

Install required packages via the MSYS2 UCRT64 terminal:
```powershell
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-qt6-base
```

---

## 2. Installation & Build Instructions

1. Open PowerShell or a terminal at the project root directory:
```powershell
cd C:\Users\LEGION\Downloads\Socket-project
```

2. Generate the build configuration using CMake with the Ninja generator:
```powershell
cmake -B build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

3. Compile the executable:
```powershell
cmake --build build
```

Upon successful compilation, the executable `lan_chat.exe` is generated in the `build/` directory.

---

## 3. How to Run

Launch the application executable from the project root:
```powershell
.\build\lan_chat.exe
```

Upon launch:
- The application initializes the Windows Sockets layer (`WSAStartup`).
- Binds and listens on an available TCP port (default starts at 5000).
- Starts broadcasting UDP presence packets on port 9999 to automatically discover local peers.
- Displays the main Qt6 graphical user interface.

---

## 4. How to Test

### 4.1. Multi-Instance Testing on a Single Machine (Local Loopback)
Open two or more separate PowerShell windows and run:
```powershell
# Instance 1:
.\build\lan_chat.exe

# Instance 2:
.\build\lan_chat.exe

# Instance 3:
.\build\lan_chat.exe
```
The application dynamically selects distinct listening TCP ports (5000, 5001, 5002...) and automatically discovers active peers via the loopback network adapter.

### 4.2. Testing across a Physical Local Area Network (LAN)
1. Connect two or more physical machines to the same Wi-Fi or wired Ethernet LAN.
2. Ensure Windows Defender Firewall permits incoming and outgoing traffic on UDP port 9999 and TCP port range 5000-5050 (or select "Allow access" on the Windows Firewall prompt).
3. Start `lan_chat.exe` on each machine. Devices will automatically appear in each other's Online Peers list within 3 seconds.
4. Select a peer from the list to start real-time text messaging or click "Send File" to transfer files.

### 4.3. Packet Inspection with Wireshark
1. Launch Wireshark and select the appropriate network interface:
   - Select `Npcap Loopback Adapter` for single-machine multi-instance testing.
   - Select `Wi-Fi` or `Ethernet` for multi-machine LAN testing.
2. Apply the capture / display filter:
```text
udp.port == 9999 || tcp.port >= 5000
```
3. Verification checkpoints:
   - UDP Packets: Periodic `HELLO|<name>|<port>|<status>` packets broadcast every 3 seconds to `255.255.255.255:9999`.
   - TCP Handshake: Standard three-way handshake (SYN, SYN-ACK, ACK) upon initiating a chat session.
   - TCP Data Stream: `AUTH|...` authentication packets, length-prefixed `MSG|<len>|<text>` chat messages, and `FILE|<name>|<size>|` headers followed by 64 KB raw binary chunks.

---

## 5. Documentation & References

- Task Assignment Document (Markdown): [docs/PHAN_CONG_NHIEM_VU.md](docs/PHAN_CONG_NHIEM_VU.md)
- Task Assignment Document (PDF): [docs/Phan_Cong_Nhiem_Vu_Do_An_Socket.pdf](docs/Phan_Cong_Nhiem_Vu_Do_An_Socket.pdf)
- Technical Architecture & Developer Guide (Markdown): [docs/TAI_LIEU_KIEN_TRUC_VA_HUONG_DAN_KY_THUAT.md](docs/TAI_LIEU_KIEN_TRUC_VA_HUONG_DAN_KY_THUAT.md)
- Technical Architecture & Developer Guide (PDF): [docs/Tai_Lieu_Kien_Truc_Va_Huong_Dan_Ky_Thuat.pdf](docs/Tai_Lieu_Kien_Truc_Va_Huong_Dan_Ky_Thuat.pdf)

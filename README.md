# HCMUS LAN Chat & File Sharing

A peer-to-peer (P2P) local area network communication and file transfer application built with C++ and Qt6.
Developed for the CSC10008 Computer Networks course at VNU-HCM University of Science.

## Team Members

1. Nguyen Manh Tri (Team Lead)
2. Do Duy Truong
3. Mai Huu Tri

## Architecture & Transport Protocols

### 1. Peer Discovery (UDP)
- Periodic presence broadcast every 3 seconds on fixed UDP port 9999.
- Automatic online peers list maintenance with 10-second timeout.
- Real-time online/away status synchronization.

### 2. Direct Messaging & File Sharing (TCP)
- Direct 1-to-1 TCP connections between peers.
- Application layer message framing:
  - MSG|<length>|<utf-8 text>
  - FILE|<filename>|<size_in_bytes>|<raw bytes>
  - AUTH|<name>|<port>|
  - BYE|
- Dedicated worker threads for file transfer to avoid blocking interactive chat.
- Automatic partial file deletion upon transmission interruption.

## Directory Structure

```text
src/
├── chat/        # Session coordinator, chat history, and unread counters
├── models/      # Data structures (Peer, ChatMessage)
├── network/     # Native Winsock2 UDP discovery, TCP server, and TCP connections
├── protocol/    # Packet serialization, deserialization, and message framing
└── ui/          # Qt6 GUI widgets (main window, peer list, chat history)
docs/            # Technical documentation and task assignment guide
downloads/       # Storage directory for incoming transferred files
tests/           # Test scripts and verification utilities
```

## Prerequisites

- C++17 compliant compiler (GCC/G++ 11+ via MSYS2 UCRT64 or MSVC)
- CMake 3.16 or higher
- Ninja build system
- Qt6 (Widgets component)
- Windows Sockets 2 (ws2_32)

## Build Instructions

```powershell
cmake -B build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

## Running Multiple Instances (Local Testing)

```powershell
# Instance 1
.\build\lan_chat.exe --name Alice --port 5001

# Instance 2
.\build\lan_chat.exe --name Bob --port 5002
```

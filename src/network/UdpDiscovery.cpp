#include "UdpDiscovery.h"
#include "protocol/Protocol.h"
#include <iostream>

/*
 * ============================================================================
 * TẬP TIN: src/network/UdpDiscovery.cpp
 * NGƯỜI PHỤ TRÁCH: Nguyễn Mạnh Trí
 * 
 * HƯỚNG DẪN HIỆN THỰC CHI TIẾT:
 * 
 * 1. Hàm start():
 *    - Tạo sendSocket: socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP).
 *    - Bật cờ SO_BROADCAST bằng setsockopt.
 *    - Tạo recvSocket, bật cờ SO_REUSEADDR bằng setsockopt.
 *    - Bind recvSocket vào cổng udpPort (9999) với địa chỉ INADDR_ANY.
 *    - Đặt m_running = true và khởi chạy 3 luồng: broadcastThread, listenThread, timeoutThread.
 * 
 * 2. Hàm stop():
 *    - Đặt m_running = false.
 *    - Đóng sendSocket và recvSocket bằng closesocket().
 *    - Chờ (join) cả 3 luồng kết thúc nếu joinable.
 *    - Dùng lock_guard<mutex> để xóa sạch map m_peers.
 * 
 * 3. Hàm broadcastLoop():
 *    - Vòng lặp while(m_running):
 *      + Tạo gói tin bằng Protocol::buildUdpHello(m_myName, m_myTcpPort, m_myStatus).
 *      + Gửi qua sendto() tới "255.255.255.255" và "127.255.255.255".
 *      + Ngủ 3 giây: std::this_thread::sleep_for(std::chrono::seconds(3)).
 * 
 * 4. Hàm listenLoop():
 *    - Vòng lặp while(m_running):
 *      + Nhận gói qua recvfrom().
 *      + Bóc tách bằng Protocol::parseUdpHello().
 *      + Lấy địa chỉ IP người gửi bằng inet_ntop().
 *      + Bỏ qua nếu là chính máy mình phát ra (trùng port và trùng IP).
 *      + Dùng lock_guard<mutex> để thêm hoặc cập nhật peer vào m_peers với lastSeen = now().
 * 
 * 5. Hàm timeoutLoop():
 *    - Vòng lặp while(m_running):
 *      + Ngủ 1 giây.
 *      + Dùng lock_guard<mutex> duyệt m_peers, xóa các peer có (now - lastSeen > 10s).
 * ============================================================================
 */

UdpDiscovery::UdpDiscovery() {}

UdpDiscovery::~UdpDiscovery() {
    stop();
}

void UdpDiscovery::setStatus(const std::string& newStatus) {
    m_myStatus = newStatus;
}

std::vector<Peer> UdpDiscovery::getOnlinePeers() {
    std::lock_guard<std::mutex> lock(m_peersMutex);
    std::vector<Peer> result;
    for (const auto& pair : m_peers) {
        result.push_back(pair.second);
    }
    return result;
}

bool UdpDiscovery::start(const std::string& myName, int myTcpPort, const std::string& status, int udpPort) {

    m_myName = myName;
    m_myTcpPort = myTcpPort;
    m_myStatus = status;
    m_udpPort = udpPort;

    m_sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    m_recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    BOOL bOpt = true;
    setsockopt(m_sendSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&bOpt, sizeof(bOpt));
    BOOL rOpt = true;
    setsockopt(m_recvSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&rOpt, sizeof(rOpt));

    sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(m_udpPort);
    recvAddr.sin_addr.s_addr = INADDR_ANY;
    bind(m_recvSocket, (sockaddr*)&recvAddr, sizeof(recvAddr));

    m_running = true;

    m_broadcastThread = std::thread(&UdpDiscovery::broadcastLoop, this);
    m_listenThread = std::thread(&UdpDiscovery::listenLoop, this);
    m_timeoutThread = std::thread(&UdpDiscovery::timeoutLoop, this);

    return true;
}

void UdpDiscovery::stop() {
    m_running = false;

    // TODO: Nguyễn Mạnh Trí sẽ hiện thực đóng socket và join các luồng tại đây
    if (m_sendSocket != INVALID_SOCKET) {
        closesocket(m_sendSocket);
        m_sendSocket = INVALID_SOCKET;
    }
    if (m_recvSocket != INVALID_SOCKET) {
        closesocket(m_recvSocket);
        m_recvSocket = INVALID_SOCKET;
    }

    if (m_broadcastThread.joinable()) m_broadcastThread.join();
    if (m_listenThread.joinable()) m_listenThread.join();
    if (m_timeoutThread.joinable()) m_timeoutThread.join();

    std::lock_guard<std::mutex> lock(m_peersMutex);
    m_peers.clear();
}

void UdpDiscovery::broadcastLoop() {
    // TODO: Nguyễn Mạnh Trí sẽ hiện thực vòng lặp broadcast gửi HELLO mỗi 3 giây tại đây
    sockaddr_in broadcastAddr;
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(m_udpPort);
    broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    while (m_running == true){
        std::string packet = Protocol::buildUdpHello(m_myName, m_myTcpPort, m_myStatus);
        sendto(m_sendSocket, packet.c_str(),(int)packet.length(),0, (sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void UdpDiscovery::listenLoop() {
    // TODO: Nguyễn Mạnh Trí sẽ hiện thực vòng lặp recvfrom nhận HELLO và cập nhật danh bạ tại đây
    while (m_running == true){
        char buffer[1024];
        sockaddr_in senderAddr;
        int senderLen = sizeof(senderAddr);

        int bytesRecv = recvfrom(m_recvSocket, buffer, sizeof(buffer) - 1 , 0, (sockaddr*)&senderAddr, &senderLen);
        if (bytesRecv <= 0){
            break;
        }
        buffer[bytesRecv] = '\0';
    

    char senderIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(senderAddr.sin_addr), senderIp, sizeof(senderIp));
    std::string peerName, peerStatus;
    int peerTcpPort = 0;
    if (!Protocol::parseUdpHello( buffer, peerName, peerTcpPort, peerStatus)){
        continue;
    }

    if (peerTcpPort == m_myTcpPort && (std::string(senderIp) == "127.0.0.1" || peerName == m_myName)){
        continue;
    }

    {
    std::lock_guard<std::mutex> lock(m_peersMutex);
    Peer peer;
    peer.name = peerName;
    peer.ip = senderIp;
    peer.tcpPort = peerTcpPort;
    peer.status = peerStatus;
    peer.lastSeen = std::chrono::steady_clock::now(); // Cập nhật mốc thời gian nhận gói
    m_peers[peer.key()] = peer; // peer.key() là "IP:Port"
    }
}
}

void UdpDiscovery::timeoutLoop() {
    // TODO: Nguyễn Mạnh Trí sẽ hiện thực vòng lặp kiểm tra timeout 10 giây xóa peer tại đây
   
    while (m_running == true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(m_peersMutex);
        for (auto it = m_peers.begin(); it != m_peers.end(); ){
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.lastSeen).count();
            if (elapsed > 10){
                it = m_peers.erase(it);
            } else {
                ++it;
            }
        }
    }
    


}

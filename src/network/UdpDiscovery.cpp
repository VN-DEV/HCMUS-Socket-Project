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

    // TODO: Nguyễn Mạnh Trí sẽ hiện thực khởi tạo socket và bật 3 luồng tại đây
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
}

void UdpDiscovery::listenLoop() {
    // TODO: Nguyễn Mạnh Trí sẽ hiện thực vòng lặp recvfrom nhận HELLO và cập nhật danh bạ tại đây
}

void UdpDiscovery::timeoutLoop() {
    // TODO: Nguyễn Mạnh Trí sẽ hiện thực vòng lặp kiểm tra timeout 10 giây xóa peer tại đây
}

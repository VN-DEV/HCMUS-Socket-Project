#include "UdpDiscovery.h"
#include "protocol/Protocol.h"
#include <iostream>

/*
 * ============================================================================
 * FILE: src/network/UdpDiscovery.cpp
 * NGUOI PHU TRACH: Nguyen Manh Tri
 * 
 * HUONG DAN HIEN THUC CHI TIET CHO CAC HAM:
 * 
 * 1. Ham start():
 *    - Tao sendSocket bang socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP).
 *    - Bat co SO_BROADCAST bang setsockopt.
 *    - Tao recvSocket, bat co SO_REUSEADDR, bind vao cong udpPort (9999) voi INADDR_ANY.
 *    - Bat co running = true va tao 3 thread: broadcastThread, listenThread, timeoutThread.
 * 
 * 2. Ham stop():
 *    - Dat running = false.
 *    - closesocket cho ca 2 socket.
 *    - join() cac thread neu joinable.
 *    - Xoa map m_peers (nho dung lock_guard<mutex>).
 * 
 * 3. Ham broadcastLoop():
 *    - Vong lap while(running):
 *      + Tao chuoi packet bang Protocol::buildUdpHello(m_myName, m_myTcpPort, m_myStatus).
 *      + Gui den 255.255.255.255 va 127.255.255.255 bang sendto().
 *      + Ngu 3 giay: std::this_thread::sleep_for(std::chrono::seconds(3)).
 * 
 * 4. Ham listenLoop():
 *    - Vong lap while(running):
 *      + Dung recvfrom() doc goi tin tu recvSocket.
 *      + Dung Protocol::parseUdpHello() boc tach thong tin.
 *      + Lay IP nguoi gui qua inet_ntop().
 *      + Bo qua neu goi tin do chinh may minh gui ra (trung IP va trung port).
 *      + Dung lock_guard<mutex> cap nhat peer vao map m_peers voi lastSeen = now().
 * 
 * 5. Ham timeoutLoop():
 *    - Vong lap while(running):
 *      + Ngu 1 giay.
 *      + Dung lock_guard<mutex> duyet qua m_peers, xoa cac peer co (now - lastSeen > 10 giay).
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

    // TODO: Nguyen Manh Tri se hien thuc khoi tao socket va khoi chay 3 luong tai day
    return true;
}

void UdpDiscovery::stop() {
    m_running = false;

    // TODO: Nguyen Manh Tri se hien thuc dong socket va join cac luong tai day
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
    // TODO: Nguyen Manh Tri se hien thuc vong lap broadcast moi 3 giay tai day
}

void UdpDiscovery::listenLoop() {
    // TODO: Nguyen Manh Tri se hien thuc vong lap recvfrom lang nghe tai day
}

void UdpDiscovery::timeoutLoop() {
    // TODO: Nguyen Manh Tri se hien thuc vong lap kiem tra timeout 10s tai day
}

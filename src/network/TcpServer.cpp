#include "TcpServer.h"
#include <iostream>

/*
 * ============================================================================
 * TẬP TIN: src/network/TcpServer.cpp
 * NGƯỜI PHỤ TRÁCH: Đỗ Duy Trường
 * 
 * HƯỚNG DẪN HIỆN THỰC CHI TIẾT:
 * 
 * 1. Hàm start(preferredPort):
 *    - Tạo socket TCP: m_serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP).
 *    - Bật cờ SO_REUSEADDR bằng setsockopt().
 *    - Vòng lặp thử bind từ preferredPort đến preferredPort + 50 (hỗ trợ test nhiều instance).
 *    - Gọi listen(m_serverSock, SOMAXCONN).
 *    - Đặt m_running = true và bật luồng: m_acceptThread = std::thread(&TcpServer::acceptLoop, this).
 * 
 * 2. Hàm stop():
 *    - Đặt m_running = false.
 *    - closesocket(m_serverSock).
 *    - Chờ luồng kết thúc (join) nếu joinable.
 * 
 * 3. Hàm acceptLoop():
 *    - Vòng lặp while(m_running):
 *      + Gọi accept(m_serverSock, (sockaddr*)&clientAddr, &addrLen).
 *      + Lấy địa chỉ IP client bằng inet_ntop().
 *      + Nếu onNewConnection != nullptr, gọi onNewConnection(clientSock, clientIp).
 * ============================================================================
 */

TcpServer::TcpServer() {}

TcpServer::~TcpServer() {
    stop();
}

bool TcpServer::start(int preferredPort) {
    m_listeningPort = preferredPort;

    // TODO: Đỗ Duy Trường sẽ hiện thực tạo socket, bind, listen và acceptThread tại đây
    return true;
}

void TcpServer::stop() {
    m_running = false;

    // TODO: Đỗ Duy Trường sẽ hiện thực đóng socket và join acceptThread tại đây
    if (m_serverSock != INVALID_SOCKET) {
        closesocket(m_serverSock);
        m_serverSock = INVALID_SOCKET;
    }
    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }
}

void TcpServer::acceptLoop() {
    // TODO: Đỗ Duy Trường sẽ hiện thực vòng lặp accept() đón kết nối mới tại đây
}

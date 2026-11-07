#include "TcpServer.h"
#include <iostream>

/*
 * ============================================================================
 * FILE: src/network/TcpServer.cpp
 * NGUOI PHU TRACH: Do Duy Truong
 * 
 * HUONG DAN HIEN THUC:
 * 
 * 1. Ham start(preferredPort):
 *    - Tao socket TCP: m_serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP).
 *    - Bat co SO_REUSEADDR bang setsockopt.
 *    - Vong lap thu bind tu preferredPort den preferredPort + 50 (phong truong hop chay nhieu instance tren 1 PC).
 *    - Goi listen(m_serverSock, SOMAXCONN).
 *    - Bat co m_running = true va khoi chay m_acceptThread = std::thread(&TcpServer::acceptLoop, this).
 * 
 * 2. Ham stop():
 *    - Dat m_running = false.
 *    - closesocket(m_serverSock).
 *    - join() luong m_acceptThread neu joinable.
 * 
 * 3. Ham acceptLoop():
 *    - Vong lap while(m_running):
 *      + Goi accept(m_serverSock, (sockaddr*)&clientAddr, &addrLen).
 *      + Lay IP client qua inet_ntop.
 *      + Neu onNewConnection != nullptr, goi callback onNewConnection(clientSock, clientIp).
 * ============================================================================
 */

TcpServer::TcpServer() {}

TcpServer::~TcpServer() {
    stop();
}

bool TcpServer::start(int preferredPort) {
    m_listeningPort = preferredPort;

    // TODO: Do Duy Truong se hien thuc tao socket, bind, listen va acceptThread tai day
    return true;
}

void TcpServer::stop() {
    m_running = false;

    // TODO: Do Duy Truong se hien thuc dong m_serverSock va join acceptThread tai day
    if (m_serverSock != INVALID_SOCKET) {
        closesocket(m_serverSock);
        m_serverSock = INVALID_SOCKET;
    }
    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }
}

void TcpServer::acceptLoop() {
    // TODO: Do Duy Truong se hien thuc vong lap accept() tai day
}

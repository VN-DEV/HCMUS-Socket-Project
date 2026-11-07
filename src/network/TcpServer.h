#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <atomic>
#include <functional>

/*
 * ============================================================================
 * FILE: src/network/TcpServer.h
 * NGUOI PHU TRACH: Do Duy Truong
 * 
 * MO TA:
 * Class nay mo mot cong TCP lang nghe cac ket noi den tu cac ban be trong mang LAN.
 * 
 * NGUYEN LY HOAT DONG (MUC 5.1 TRONG DE BAI):
 * 1. TCP Acceptor Thread: Chay vong lap accept() de don cac ket noi TCP moi.
 * 2. Khi co mot peer ket noi toi, tao mot socket ket noi rieng cho peer do
 *    va thong bao qua callback onNewConnection de khoi tao phien chat.
 * ============================================================================
 */

class TcpServer {
public:
    TcpServer();
    ~TcpServer();

    // Khoi dong TCP server tren cong preferredPort (mac dinh 5000)
    // Neu cong bi trung khi chay nhieu client tren 1 may, tu dong thu cong tiep theo (5001, 5002...)
    bool start(int preferredPort = 5000);

    // Dung TCP server, dong socket lang nghe va ket thuc luong accept
    void stop();

    // Lay so cong TCP thuc te dang lang nghe
    int getPort() const { return m_listeningPort; }

    // Callback thong bao khi co ket noi moi tu mot peer
    std::function<void(SOCKET clientSock, const std::string& clientIp)> onNewConnection;

private:
    // Luong don ket noi TCP (TCP Acceptor Thread)
    void acceptLoop();

    SOCKET m_serverSock = INVALID_SOCKET;
    int m_listeningPort = 0;
    std::atomic<bool> m_running{false};
    std::thread m_acceptThread;
};

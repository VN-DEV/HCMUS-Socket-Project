#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <atomic>
#include <functional>

/*
 * ============================================================================
 * TẬP TIN: src/network/TcpServer.h
 * NGƯỜI PHỤ TRÁCH: Đỗ Duy Trường
 * 
 * MÔ TẢ:
 * Class mở cổng TCP để lắng nghe và chấp nhận các kết nối nhắn tin đến từ bạn bè
 * trong mạng LAN.
 * 
 * CƠ CHẾ HOẠT ĐỘNG (MỤC 5.1 TRONG ĐỀ BÀI):
 * 1. TCP Acceptor Thread: Luồng chạy ngầm đứng đợi accept() các kết nối mới.
 * 2. Khi có peer kết nối tới, accept() trả về một socket riêng và thông báo
 *    qua callback onNewConnection để hệ thống bắt đầu phiên trao đổi.
 * ============================================================================
 */

class TcpServer {
public:
    TcpServer();
    ~TcpServer();

    // Khởi động TCP server trên cổng preferredPort (mặc định 5000)
    // Nếu cổng bị bận (khi test nhiều client trên 1 máy), tự động thử các cổng tiếp theo (5001, 5002...)
    bool start(int preferredPort = 5000);

    // Dừng TCP server, đóng socket lắng nghe và kết thúc luồng accept
    void stop();

    // Lấy số cổng TCP thực tế đang lắng nghe
    int getPort() const { return m_listeningPort; }

    // Callback thông báo lên hệ thống khi có kết nối mới từ một peer
    std::function<void(SOCKET clientSock, const std::string& clientIp)> onNewConnection;

private:
    // Luồng đón kết nối TCP (TCP Acceptor Thread)
    void acceptLoop();

    SOCKET m_serverSock = INVALID_SOCKET;
    int m_listeningPort = 0;
    std::atomic<bool> m_running{false};
    std::thread m_acceptThread;
};

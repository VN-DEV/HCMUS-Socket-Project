#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

/*
 * ============================================================================
 * TẬP TIN: src/network/TcpConnection.h
 * NGƯỜI PHỤ TRÁCH: Đỗ Duy Trường (Chat 1-1) & Mai Hữu Trí (Truyền File)
 * 
 * MÔ TẢ:
 * Class quản lý một kết nối TCP trực tiếp 1-1 với một bạn chat:
 * - Gửi/nhận tin nhắn chat thời gian thực.
 * - Gửi/nhận file nhị phân trên luồng riêng (không nghẽn chat).
 * - Bóc tách gói tin theo đúng chuẩn framing (AUTH, MSG, FILE, BYE).
 * 
 * CƠ CHẾ HOẠT ĐỘNG (MỤC 3.3, 3.4 & 5.2 TRONG ĐỀ BÀI):
 * 1. receiverLoop (Luồng nhận): Chạy ngầm liên tục đọc byte stream từ socket.
 * 2. sendFileWorker (Luồng gửi file): Chạy trên luồng riêng, chia nhỏ file thành
 *    các khối chunk 64KB để truyền và phát tín hiệu tiến trình %.
 * 3. sendMutex: Khóa chống gửi chồng chéo dữ liệu giữa luồng chat và luồng file.
 * ============================================================================
 */

class TcpConnection {
public:
    TcpConnection(SOCKET sock, const std::string& peerIp);
    ~TcpConnection();

    // Khởi động luồng nhận dữ liệu (receiverLoop)
    void start();

    // Đóng kết nối an toàn
    void close();

    // Gửi gói tin bắt tay định danh AUTH|<myName>|<myTcpPort>|
    bool sendAuth(const std::string& myName, int myTcpPort);

    // Gửi tin nhắn chat MSG|<length>|<text>
    bool sendMsg(const std::string& text);

    // Gửi file không làm nghẽn chat (chạy trên luồng riêng)
    void sendFileAsync(const std::string& filePath,
                       std::function<void(int percent)> onProgress,
                       std::function<void(bool success, const std::string& err)> onFinished);

    bool isConnected() const { return m_connected; }
    std::string getPeerIp() const { return m_peerIp; }
    int getPeerPort() const { return m_peerPort; }
    std::string getPeerName() const { return m_peerName; }
    std::string getPeerKey() const { return m_peerIp + ":" + std::to_string(m_peerPort); }

    // Callbacks thông báo sự kiện lên ChatManager / UI
    std::function<void(const std::string& name, int port)> onAuthenticated;
    std::function<void(const std::string& text)> onMessageReceived;
    std::function<void(const std::string& fileName, int percent)> onFileRecvProgress;
    std::function<void(const std::string& fileName, const std::string& savePath)> onFileRecvFinished;
    std::function<void()> onDisconnected;

private:
    // Luồng nhận dữ liệu (TCP Receiver Thread)
    void receiverLoop();

    // Luồng worker truyền file
    void sendFileWorker(std::string filePath,
                        std::function<void(int percent)> onProgress,
                        std::function<void(bool success, const std::string& err)> onFinished);

    SOCKET m_sock = INVALID_SOCKET;
    std::string m_peerIp;
    int m_peerPort = 0;
    std::string m_peerName = "Unknown";
    std::atomic<bool> m_connected{false};

    std::thread m_recvThread;
    std::mutex m_sendMutex; // Khóa bảo vệ hàm send() tránh xung đột dữ liệu
};

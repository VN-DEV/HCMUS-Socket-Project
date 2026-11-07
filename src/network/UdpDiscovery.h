#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include "models/Peer.h"

/*
 * ============================================================================
 * TẬP TIN: src/network/UdpDiscovery.h
 * NGƯỜI PHỤ TRÁCH: Nguyễn Mạnh Trí
 * 
 * MÔ TẢ:
 * Class đảm nhận tính năng Peer Discovery (tự động phát hiện bạn bè trong mạng LAN)
 * và quản lý trạng thái Online Status bằng UDP Broadcast.
 * 
 * CƠ CHẾ HOẠT ĐỘNG (MỤC 3.1 & 3.2 TRONG ĐỀ BÀI):
 * 1. Luồng phát (broadcastLoop): Định kỳ 3 giây gửi gói tin
 *    HELLO|<name>|<tcpPort>|<status> tới địa chỉ broadcast 255.255.255.255 qua cổng 9999.
 * 2. Luồng nghe (listenLoop): Mở socket UDP bind cổng 9999 (bật SO_REUSEADDR),
 *    nhận gói tin từ các peer khác và cập nhật danh sách online peers.
 * 3. Luồng kiểm tra timeout (timeoutLoop): Định kỳ 1 giây kiểm tra danh bạ, peer nào
 *    quá 10 giây không có tín hiệu broadcast sẽ tự động bị xóa khỏi danh sách.
 * ============================================================================
 */

class UdpDiscovery {
public:
    UdpDiscovery();
    ~UdpDiscovery();

    // Khởi động tiến trình discovery: tạo socket, bind cổng udpPort (mặc định 9999) và bật 3 luồng
    bool start(const std::string& myName, int myTcpPort, const std::string& status = "online", int udpPort = 9999);

    // Dừng toàn bộ tiến trình: đóng các socket và kết thúc (join) các luồng an toàn
    void stop();

    // Cập nhật trạng thái bản thân ("online" hoặc "away")
    void setStatus(const std::string& newStatus);

    // Lấy danh sách các bạn bè đang online (được bảo vệ an toàn đa luồng bằng mutex)
    std::vector<Peer> getOnlinePeers();

private:
    // Các hàm vòng lặp chạy trên luồng riêng (Worker Threads)
    void broadcastLoop(); // Luồng gửi broadcast mỗi 3s
    void listenLoop();    // Luồng lắng nghe gói UDP
    void timeoutLoop();   // Luồng kiểm tra timeout 10s

    // Thông tin của bản thân
    std::string m_myName;
    int m_myTcpPort = 0;
    std::string m_myStatus = "online";
    int m_udpPort = 9999;

    // Sockets và cờ trạng thái
    SOCKET m_sendSocket = INVALID_SOCKET;
    SOCKET m_recvSocket = INVALID_SOCKET;
    std::atomic<bool> m_running{false};

    // Các luồng chạy song song
    std::thread m_broadcastThread;
    std::thread m_listenThread;
    std::thread m_timeoutThread;

    // Danh sách lưu trữ bạn bè và khóa bảo vệ luồng (mutex)
    std::map<std::string, Peer> m_peers; // Khóa là chuỗi "IP:Port"
    std::mutex m_peersMutex;
};

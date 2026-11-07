#pragma once

#include <string>
#include <chrono>

/*
 * ============================================================================
 * TẬP TIN: src/models/Peer.h
 * NGƯỜI PHỤ TRÁCH: Nguyễn Mạnh Trí
 * 
 * MÔ TẢ:
 * Cấu trúc dữ liệu Peer đại diện cho một nút (người dùng/máy tính) trong mạng
 * ngang hàng LAN.
 * 
 * CÁC TRƯỜNG DỮ LIỆU:
 * 1. name: Tên hiển thị của người dùng (ví dụ: "Minh", "Quân").
 * 2. ip: Địa chỉ IPv4 của peer trong mạng LAN (ví dụ: "192.168.1.15").
 * 3. tcpPort: Cổng TCP mà peer này mở để lắng nghe kết nối nhắn tin và truyền file.
 * 4. status: Trạng thái hoạt động ("online" hoặc "away").
 * 5. lastSeen: Mốc thời gian nhận gói tin UDP broadcast gần nhất, dùng để tính
 *    timeout 10 giây (quá 10s không thấy tín hiệu thì coi như offline).
 * 6. unreadCount: Số tin nhắn chưa đọc từ peer này khi người dùng chưa chọn chat.
 * ============================================================================
 */

struct Peer {
    std::string name;
    std::string ip;
    int tcpPort = 0;
    std::string status = "online";
    std::chrono::steady_clock::time_point lastSeen = std::chrono::steady_clock::now();
    int unreadCount = 0;

    // Hàm tiện ích: Trả về chuỗi định danh duy nhất "IP:Port" để làm khóa trong std::map
    std::string key() const {
        return ip + ":" + std::to_string(tcpPort);
    }
};

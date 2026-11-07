#pragma once

#include <string>
#include <chrono>

/*
 * ============================================================================
 * FILE: src/models/Peer.h
 * NGUOI PHU TRACH: Nguyen Manh Tri
 * 
 * MO TA:
 * File nay dinh nghia cau truc du lieu Peer, dai dien cho mot nut (nguoi dung)
 * trong mang ngang hang LAN.
 * 
 * CAC TRUONG DU LIEU CAN QUAN LY:
 * 1. name: Ten hien thi cua peer (vi du: "Alice", "Bob").
 * 2. ip: Dia chi IPv4 cua peer trong mang LAN (vi du: "192.168.1.15").
 * 3. tcpPort: Cong TCP ma peer nay dang mo de lang nghe ket noi chat.
 * 4. status: Trang thai cua peer ("online" hoac "away").
 * 5. lastSeen: Moc thoi gian nhan duoc goi tin UDP broadcast lan cuoi,
 *    dung de tinh toan timeout 10 giay.
 * 6. unreadCount: So tin nhan chua doc tu peer nay khi chua mo tab chat.
 * ============================================================================
 */

struct Peer {
    std::string name;
    std::string ip;
    int tcpPort = 0;
    std::string status = "online";
    std::chrono::steady_clock::time_point lastSeen = std::chrono::steady_clock::now();
    int unreadCount = 0;

    // Ham tien ich: Tao chuoi dinh danh duy nhat "IP:Port" de lam khoa trong std::map
    std::string key() const {
        return ip + ":" + std::to_string(tcpPort);
    }
};

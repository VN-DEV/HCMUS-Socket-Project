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
 * FILE: src/network/UdpDiscovery.h
 * NGUOI PHU TRACH: Nguyen Manh Tri
 * 
 * MO TA:
 * Class nay dam nhiem toan bo tinh nang Peer Discovery (Tu dong phat hien ban be)
 * va Online Status trong mang cuc bo (LAN) bang UDP Broadcast.
 * 
 * NGUYEN LY HOAT DONG (MUC 3.1 & 3.2 TRONG DE BAI):
 * 1. Luong broadcast (broadcastLoop): Dinh ky 3s mot lan, gui goi tin
 *    HELLO|<name>|<tcpPort>|<status> toi cong 9999 (255.255.255.255).
 * 2. Luong lang nghe (listenLoop): Mo socket UDP bind cong 9999 voi co SO_REUSEADDR,
 *    nhan goi tin tu cac may khac va cap nhat danh sach online peers.
 * 3. Luong kiem tra timeout (timeoutLoop): Cứ 1s kiem tra mot lan, peer nao qua
 *    10 giay khong co tin hieu broadcast se bi xoa khoi danh sach.
 * ============================================================================
 */

class UdpDiscovery {
public:
    UdpDiscovery();
    ~UdpDiscovery();

    // Khoi dong tien trinh discovery: tao socket, bind cong udpPort (mac dinh 9999) va bat 3 luong
    bool start(const std::string& myName, int myTcpPort, const std::string& status = "online", int udpPort = 9999);

    // Dung toan bo tien trinh: dong cac socket va join cac luong
    void stop();

    // Cap nhat trang thai cua ban than ("online" hoac "away")
    void setStatus(const std::string& newStatus);

    // Lay danh sach cac ban be dang online (an toan da luong voi mutex)
    std::vector<Peer> getOnlinePeers();

private:
    // Cac ham chay tren luong rieng (Worker Threads)
    void broadcastLoop(); // Luong phat goi tin moi 3s
    void listenLoop();    // Luong nhan goi tin UDP
    void timeoutLoop();   // Luong kiem tra va xoa peer qua 10s timeout

    // Thong tin cua may minh
    std::string m_myName;
    int m_myTcpPort = 0;
    std::string m_myStatus = "online";
    int m_udpPort = 9999;

    // Sockets va co trang thai
    SOCKET m_sendSocket = INVALID_SOCKET;
    SOCKET m_recvSocket = INVALID_SOCKET;
    std::atomic<bool> m_running{false};

    // Cac luong chay song song
    std::thread m_broadcastThread;
    std::thread m_listenThread;
    std::thread m_timeoutThread;

    // Danh sach ban be va khoa bao ve da luong
    std::map<std::string, Peer> m_peers; // key la "IP:Port"
    std::mutex m_peersMutex;
};

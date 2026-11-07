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
 * FILE: src/network/TcpConnection.h
 * NGUOI PHU TRACH: Do Duy Truong (Chat 1-1) & Mai Huu Tri (Truyen File)
 * 
 * MO TA:
 * Class nay quan ly mot ket noi TCP truc tiep voi mot peer cu the:
 * - Gui/nhan tin nhan thoi gian thuc.
 * - Gui/nhan file khong nghen chat (Worker Thread rieng).
 * - Boc tach goi tin theo chuan framing (AUTH, MSG, FILE, BYE).
 * 
 * NGUYEN LY HOAT DONG (MUC 3.3, 3.4 & 5.2 TRONG DE BAI):
 * 1. receiverLoop (Luong nhan): Chay ngam doc lien tuc tu socket, nhan tin nhan va file.
 * 2. sendFileWorker (Luong gui file): Chay tren luong rieng, chia file thanh cac
 *    chunk 64KB truyen qua socket va bao tien trinh %.
 * 3. sendMutex: Khoa dong bo de dam bao goi tin nhan khong bi chen lan vao giua du lieu file.
 * ============================================================================
 */

class TcpConnection {
public:
    TcpConnection(SOCKET sock, const std::string& peerIp);
    ~TcpConnection();

    // Khoi dong luong nhan du lieu (receiverLoop)
    void start();

    // Dong ket noi an toan
    void close();

    // Gui goi tin bat tay dinh danh AUTH|<myName>|<myTcpPort>|
    bool sendAuth(const std::string& myName, int myTcpPort);

    // Gui tin nhan chat MSG|<length>|<text>
    bool sendMsg(const std::string& text);

    // Gui file khong lam nghen chat (chay tren luong rieng)
    void sendFileAsync(const std::string& filePath,
                       std::function<void(int percent)> onProgress,
                       std::function<void(bool success, const std::string& err)> onFinished);

    bool isConnected() const { return m_connected; }
    std::string getPeerIp() const { return m_peerIp; }
    int getPeerPort() const { return m_peerPort; }
    std::string getPeerName() const { return m_peerName; }
    std::string getPeerKey() const { return m_peerIp + ":" + std::to_string(m_peerPort); }

    // Callbacks bao len cho ChatManager / UI
    std::function<void(const std::string& name, int port)> onAuthenticated;
    std::function<void(const std::string& text)> onMessageReceived;
    std::function<void(const std::string& fileName, int percent)> onFileRecvProgress;
    std::function<void(const std::string& fileName, const std::string& savePath)> onFileRecvFinished;
    std::function<void()> onDisconnected;

private:
    // Luong nhan du lieu (TCP Receiver Thread)
    void receiverLoop();

    // Luong worker gui file
    void sendFileWorker(std::string filePath,
                        std::function<void(int percent)> onProgress,
                        std::function<void(bool success, const std::string& err)> onFinished);

    SOCKET m_sock = INVALID_SOCKET;
    std::string m_peerIp;
    int m_peerPort = 0;
    std::string m_peerName = "Unknown";
    std::atomic<bool> m_connected{false};

    std::thread m_recvThread;
    std::mutex m_sendMutex; // Khoa bao ve send() tranh xung dot giua tin nhan va file
};

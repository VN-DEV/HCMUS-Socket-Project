#include "TcpConnection.h"
#include "protocol/Protocol.h"
#include <iostream>
#include <fstream>
#include <vector>

/*
 * ============================================================================
 * FILE: src/network/TcpConnection.cpp
 * NGUOI PHU TRACH: Do Duy Truong (Chat & Ket noi) & Mai Huu Tri (Truyen File)
 * 
 * HUONG DAN HIEN THUC CHI TIET:
 * 
 * 1. Ham sendAuth(myName, myTcpPort) [Do Duy Truong]:
 *    - Goi Protocol::buildTcpAuth(myName, myTcpPort).
 *    - Khoa sendMutex va goi send().
 * 
 * 2. Ham sendMsg(text) [Do Duy Truong]:
 *    - Goi Protocol::buildTcpMsg(text).
 *    - Khoa sendMutex va goi send().
 * 
 * 3. Ham receiverLoop() [Do Duy Truong]:
 *    - Vong lap doc tu socket:
 *      + Doc lenh truoc dau '|' (AUTH, MSG, FILE, BYE).
 *      + Neu AUTH: Doc ten va port, cap nhat m_peerName, goi onAuthenticated.
 *      + Neu MSG: Doc do dai len, lap recv() doc du len byte, goi onMessageReceived.
 *      + Neu FILE [Mai Huu Tri]: Doc ten file va filesize, mo file downloads/<filename>,
 *        lap doc cac chunk 64KB ghi vao file, goi onFileRecvProgress va onFileRecvFinished.
 *      + Neu BYE hoac recv() tra ve <= 0: Dong ket noi, goi onDisconnected.
 * 
 * 4. Ham sendFileAsync(...) & sendFileWorker(...) [Mai Huu Tri]:
 *    - Tao thread chay sendFileWorker.
 *    - sendFileWorker: Mo file binary, lay fileSize, gui Protocol::buildTcpFileHeader().
 *    - Vong lap doc file theo chunk 64KB, khoa sendMutex va send() tung chunk.
 *    - Tinh percent = (sentBytes * 100) / fileSize va goi onProgress(percent).
 *    - Gui xong goi onFinished(true, "").
 * ============================================================================
 */

TcpConnection::TcpConnection(SOCKET sock, const std::string& peerIp)
    : m_sock(sock), m_peerIp(peerIp) {
    m_connected = (m_sock != INVALID_SOCKET);
}

TcpConnection::~TcpConnection() {
    close();
}

void TcpConnection::start() {
    if (m_sock == INVALID_SOCKET) return;
    m_recvThread = std::thread(&TcpConnection::receiverLoop, this);
}

void TcpConnection::close() {
    if (!m_connected.exchange(false)) return;

    if (m_sock != INVALID_SOCKET) {
        closesocket(m_sock);
        m_sock = INVALID_SOCKET;
    }

    if (m_recvThread.joinable() && std::this_thread::get_id() != m_recvThread.get_id()) {
        m_recvThread.join();
    }
}

bool TcpConnection::sendAuth(const std::string& myName, int myTcpPort) {
    // TODO: Do Duy Truong se hien thuc gui goi tin bat tay tai day
    return true;
}

bool TcpConnection::sendMsg(const std::string& text) {
    // TODO: Do Duy Truong se hien thuc gui tin nhan theo framing MSG|<len>|<text> tai day
    return true;
}

void TcpConnection::sendFileAsync(const std::string& filePath,
                                 std::function<void(int percent)> onProgress,
                                 std::function<void(bool success, const std::string& err)> onFinished) {
    // TODO: Mai Huu Tri se khoi chay luong sendFileWorker tai day
    std::thread sender(&TcpConnection::sendFileWorker, this, filePath, onProgress, onFinished);
    sender.detach();
}

void TcpConnection::sendFileWorker(std::string filePath,
                                  std::function<void(int percent)> onProgress,
                                  std::function<void(bool success, const std::string& err)> onFinished) {
    // TODO: Mai Huu Tri se hien thuc doc file theo chunk 64KB va gui qua socket tai day
}

void TcpConnection::receiverLoop() {
    // TODO: Do Duy Truong & Mai Huu Tri se hien thuc luong nhan MSG va FILE tai day
}


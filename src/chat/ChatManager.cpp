#include "ChatManager.h"
#include <iostream>

/*
 * ============================================================================
 * FILE: src/chat/ChatManager.cpp
 * NGUOI PHU TRACH: Mai Huu Tri
 * 
 * HUONG DAN HIEN THUC:
 * 
 * 1. Ham selectPartner(peerKey):
 *    - Gan m_currentPartnerKey = peerKey.
 *    - Reset m_unreadCounts[peerKey] = 0.
 *    - Goi onUnreadCountChanged(peerKey, 0).
 * 
 * 2. Ham getOrCreateConnection(ip, port):
 *    - Kiem tra trong m_connections da co socket voi key "IP:port" chua.
 *    - Neu chua co hoac bi ngat:
 *      + Tao socket, goi connect() toi ip:port.
 *      + Tao TcpConnection(sock, ip).
 *      + Gui bat tay sendAuth(m_myName, m_myTcpPort).
 *      + Gan cac callback (onMessageReceived, onFileRecvProgress, v.v.).
 *      + Luu vao m_connections va start().
 * 
 * 3. Xu ly unreadCount khi nhan tin nhan:
 *    - Trong callback onMessageReceived tu TcpConnection:
 *      + Luu ChatMessage vao m_chatHistories[peerKey].
 *      + Neu peerKey != m_currentPartnerKey:
 *        m_unreadCounts[peerKey]++;
 *        Goi onUnreadCountChanged(peerKey, m_unreadCounts[peerKey]);
 *        Goi onNotification("Tin nhan moi tu " + peerName);
 * ============================================================================
 */

ChatManager::ChatManager() {}

ChatManager::~ChatManager() {
    std::lock_guard<std::mutex> lock(m_chatMutex);
    for (auto& pair : m_connections) {
        if (pair.second) {
            pair.second->close();
            delete pair.second;
        }
    }
    m_connections.clear();
}

void ChatManager::setMyInfo(const std::string& name, int tcpPort) {
    m_myName = name;
    m_myTcpPort = tcpPort;
}

void ChatManager::selectPartner(const std::string& peerKey) {
    std::lock_guard<std::mutex> lock(m_chatMutex);
    m_currentPartnerKey = peerKey;
    m_unreadCounts[peerKey] = 0;
    if (onUnreadCountChanged) {
        onUnreadCountChanged(peerKey, 0);
    }
}

TcpConnection* ChatManager::getOrCreateConnection(const std::string& ip, int port) {
    // TODO: Mai Huu Tri se hien thuc kiem tra va ket noi toi peer tai day
    return nullptr;
}

bool ChatManager::sendMessage(const std::string& text) {
    // TODO: Mai Huu Tri se hien thuc gui tin nhan toi ban chat hien tai tai day
    return true;
}

void ChatManager::sendFile(const std::string& filePath,
                           std::function<void(int percent)> onProgress,
                           std::function<void(bool success, const std::string& err)> onFinished) {
    // TODO: Mai Huu Tri se hien thuc goi sendFileAsync qua TcpConnection tai day
}

std::vector<ChatMessage> ChatManager::getHistory(const std::string& peerKey) {
    std::lock_guard<std::mutex> lock(m_chatMutex);
    auto it = m_chatHistories.find(peerKey);
    if (it != m_chatHistories.end()) {
        return it->second;
    }
    return {};
}

int ChatManager::getUnreadCount(const std::string& peerKey) {
    std::lock_guard<std::mutex> lock(m_chatMutex);
    auto it = m_unreadCounts.find(peerKey);
    if (it != m_unreadCounts.end()) {
        return it->second;
    }
    return 0;
}

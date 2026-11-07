#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <functional>
#include "models/Peer.h"
#include "models/ChatMessage.h"
#include "network/TcpConnection.h"

/*
 * ============================================================================
 * FILE: src/chat/ChatManager.h
 * NGUOI PHU TRACH: Mai Huu Tri
 * 
 * MO TA:
 * Class nay dong vai tro la trung tam dieu phoi (Session Coordinator) cua ung dung:
 * - Quan ly danh sach cac ket noi TCP dang mo voi tung ban chat.
 * - Luu tru lich su tin nhan theo tung peer.
 * - Quan ly so tin nhan chua doc (unread count) khi nhan tin tu peer khong phai
 *   la nguoi dang mo chat hien tai (theo muc 3.3 de bai).
 * ============================================================================
 */

class ChatManager {
public:
    ChatManager();
    ~ChatManager();

    // Chon ban chat hien tai (khi click vao danh sach peer tren GUI)
    void selectPartner(const std::string& peerKey);
    std::string getCurrentPartnerKey() const { return m_currentPartnerKey; }

    // Ket noi toi peer neu chua co ket noi
    TcpConnection* getOrCreateConnection(const std::string& ip, int port);

    // Gui tin nhan toi ban chat hien tai
    bool sendMessage(const std::string& text);

    // Gui file toi ban chat hien tai
    void sendFile(const std::string& filePath,
                  std::function<void(int percent)> onProgress,
                  std::function<void(bool success, const std::string& err)> onFinished);

    // Lay lich su chat voi 1 peer
    std::vector<ChatMessage> getHistory(const std::string& peerKey);

    // Lay so tin nhan chua doc tu 1 peer
    int getUnreadCount(const std::string& peerKey);

    // Thiet lap thong tin cua ban than
    void setMyInfo(const std::string& name, int tcpPort);

    // Callbacks bao len tang Giao dien (UI)
    std::function<void(const std::string& peerKey, const ChatMessage& msg)> onNewMessageReceived;
    std::function<void(const std::string& peerKey, int count)> onUnreadCountChanged;
    std::function<void(const std::string& notice)> onNotification;

private:
    std::string m_myName = "User";
    int m_myTcpPort = 0;
    std::string m_currentPartnerKey;

    std::mutex m_chatMutex;
    std::map<std::string, TcpConnection*> m_connections;          // Key: "IP:Port"
    std::map<std::string, std::vector<ChatMessage>> m_chatHistories; // Key: "IP:Port"
    std::map<std::string, int> m_unreadCounts;                   // Key: "IP:Port"
};

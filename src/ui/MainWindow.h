#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <map>
#include <vector>
#include <string>

#include "network/UdpDiscovery.h"

/*
 * ============================================================================
 * TẬP TIN: src/ui/MainWindow.h
 * NGƯỜI PHỤ TRÁCH: Nguyễn Mạnh Trí
 * 
 * MÔ TẢ:
 * Cửa sổ giao diện chính:
 * - Cột Sidebar bên trái: Phong cách Discord Dark Theme (#2B2D31), bo góc 12px.
 * - Khung trò chuyện bên phải: Bong bóng tin nhắn bo tròn dạng viên thuốc chuẩn
 *   Messenger (Messenger Rounded Pill Bubbles):
 *   + Tin gửi đi (phải): Nằm sát lề phải, bo tròn 18px dạng viên thuốc ôm sát chữ.
 *   + Tin nhận về (trái): Nằm sát lề trái kèm avatar tròn, nền tối bo tròn 18px.
 * ============================================================================
 */

struct UiChatMessage {
    QString sender;
    QString text;
    QString timeStr;
    bool isMine;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // Cập nhật thông tin bản thân hiển thị trên thanh User Profile mini
    void setMyProfile(const QString& name, const QString& ip, int port);

    // Thêm hoặc cập nhật một peer vào danh sách bạn bè online
    void addOrUpdatePeer(const QString& name, const QString& ip, int port, const QString& status, int unreadCount = 0);

    // Xóa một peer khỏi danh sách khi bị timeout 10s
    void removePeer(const QString& peerKey);

    // Thêm một tin nhắn vào khung chat theo dạng bong bóng Messenger
    void appendChatMessage(const QString& sender, const QString& text, const QString& timeStr, bool isMine);

    // Cập nhật tiến độ truyền nhận file (%)
    void setFileProgress(int percent, const QString& statusText);

    // Ẩn thanh tiến độ truyền file
    void hideFileProgress();

private slots:
    void onSendMessageClicked();
    void onSendFileClicked();
    void onPeerSelected(QListWidgetItem* item);
    void onRefreshPeerList();
    void onToggleTheme();

private:
    void setupUi();
    void applyStyleSheet();
    void initMockData();
    void renderMessageBubble(const QString& sender, const QString& text, const QString& timeStr, bool isMine);
    void scrollToBottom();

        // Các thành phần cột trái (Sidebar Modern Card)
    QLabel* m_myProfileLabel = nullptr;      // Tên hiển thị (LEGION)
    QLabel* m_myEndpointLabel = nullptr;     // Cổng TCP (Port: 5001)
    QComboBox* m_statusCombo = nullptr;      // Dropdown Trực tuyến / Bận
    QLabel* m_peerCountLabel = nullptr;      // Tiêu đề: Bạn bè trong mạng LAN (0)
    QListWidget* m_peerListWidget = nullptr; // Danh sách bạn bè

    // Các thành phần cột phải (Chat Area)
    QLabel* m_partnerHeaderLabel = nullptr;  // Tiêu đề: "Chưa chọn người nhận"
    QLabel* m_partnerStatusLabel = nullptr;  // Phụ đề hướng dẫn
    QLabel* m_connectedBadge = nullptr;      // Nhãn viên thuốc "Đã kết nối"
    QPushButton* m_disconnectBtn = nullptr;  // Nút "Ngắt kết nối" (đỏ nhạt)
    QPushButton* m_themeBtn = nullptr;       // Nut chuyen Dark/Light mode

    QScrollArea* m_chatScrollArea = nullptr;
    QWidget* m_scrollContent = nullptr;
    QVBoxLayout* m_messagesLayout = nullptr;

    QProgressBar* m_fileProgressBar = nullptr;
    QLabel* m_fileProgressLabel = nullptr;
    QWidget* m_fileProgressContainer = nullptr;

    void onDisconnectClicked(); // Thêm slot này

    QPushButton* m_sendFileBtn = nullptr;    // Nút tròn "+" đính kèm file
    QLineEdit* m_messageInput = nullptr;     // Ô gõ tin nhắn bo viên thuốc
    QPushButton* m_sendBtn = nullptr;        // Nút tròn gửi tin
    bool m_isDarkMode = false;

    QString m_currentPeerKey;
    QString m_currentPeerName;
    QString m_myName = "";
    int m_myTcpPort = 0;

    UdpDiscovery m_UdpDiscovery; // Quản lý phát hiện peer qua UDP broadcast
    QTimer* m_refreshTimer = nullptr; // Timer tự động refresh danh sách peer mỗi 5s

    // Lưu trữ lịch sử tin nhắn dạng danh sách cấu trúc cho từng bạn chat (Key: "IP:Port")
    std::map<QString, std::vector<UiChatMessage>> m_chatHistories;

    // Socket TCP giữ cổng để không cho cửa sổ khác chọn trùng
    SOCKET m_tcpListenSock = INVALID_SOCKET;
    void startTcpListener(int port);
    void stopTcpListener();
};

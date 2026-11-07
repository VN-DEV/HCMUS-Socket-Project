#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <map>
#include <vector>
#include <string>

/*
 * ============================================================================
 * TẬP TIN: src/ui/MainWindow.h
 * NGƯỜI PHỤ TRÁCH: Nguyễn Mạnh Trí
 * 
 * MÔ TẢ:
 * Cửa sổ giao diện đồ họa chính thiết kế theo phong cách Discord Dark Theme:
 * - Cột trái (Sidebar #2B2D31): Danh mục Direct Messages (Online Peers) và
 *   thanh User Profile mini cố định ở đáy (#232428).
 * - Cột phải (Chat Area #313338): Header kênh chat, danh sách tin nhắn phẳng
 *   (Discord Message Rows), khung tiến độ file dạng Embed, và thanh nhập liệu
 *   nổi (#383A40) kèm nút Blurple (#5865F2).
 * ============================================================================
 */

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

    // Thêm một tin nhắn vào khung chat theo định dạng Discord Message Row
    void appendChatMessage(const QString& sender, const QString& text, const QString& timeStr, bool isMine);

    // Cập nhật tiến độ truyền nhận file (%)
    void setFileProgress(int percent, const QString& statusText);

    // Ẩn thanh tiến độ truyền file
    void hideFileProgress();

private slots:
    void onSendMessageClicked();
    void onSendFileClicked();
    void onPeerSelected(QListWidgetItem* item);
    void onStatusChanged(int index);

private:
    void setupUi();
    void applyDiscordStyleSheet();
    void initMockData();

    // Các thành phần cột trái (Sidebar Discord)
    QLabel* m_myAvatarLabel = nullptr;
    QLabel* m_myProfileLabel = nullptr;
    QLabel* m_myEndpointLabel = nullptr;
    QComboBox* m_statusCombo = nullptr;
    QListWidget* m_peerListWidget = nullptr;

    // Các thành phần cột phải (Chat Area Discord)
    QLabel* m_partnerHeaderLabel = nullptr;
    QLabel* m_partnerStatusLabel = nullptr;
    QTextBrowser* m_chatHistoryView = nullptr;
    QProgressBar* m_fileProgressBar = nullptr;
    QLabel* m_fileProgressLabel = nullptr;
    QWidget* m_fileProgressContainer = nullptr;
    QLineEdit* m_messageInput = nullptr;
    QPushButton* m_sendBtn = nullptr;
    QPushButton* m_sendFileBtn = nullptr;

    // Quản lý trạng thái nội bộ
    QString m_currentPeerKey;
    QString m_currentPeerName;
    QString m_myName = "Nguyen Manh Tri";
    int m_myTcpPort = 5000;

    // Lưu trữ lịch sử tin nhắn dạng HTML cho từng bạn chat (Key: "IP:Port")
    std::map<QString, QString> m_chatHtmlHistories;
};

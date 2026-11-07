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
 * Cửa sổ giao diện đồ họa chính (Qt6 Widgets):
 * - Cột trái: Hồ sơ cá nhân (Tên, IP, Cổng TCP), Dropdown trạng thái (Online/Away),
 *   Danh sách các nút bạn bè trong mạng LAN (Online Peers) kèm số tin chưa đọc.
 * - Cột phải: Thanh tiêu đề người đang chat, Khung hiển thị tin nhắn dạng bong bóng
 *   (Chat Bubbles), Khung tiến độ truyền nhận file (QProgressBar), Thanh nhập liệu.
 * ============================================================================
 */

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // Cập nhật thông tin bản thân hiển thị trên góc trái
    void setMyProfile(const QString& name, const QString& ip, int port);

    // Thêm hoặc cập nhật một peer vào danh sách bạn bè online
    void addOrUpdatePeer(const QString& name, const QString& ip, int port, const QString& status, int unreadCount = 0);

    // Xóa một peer khỏi danh sách khi bị timeout
    void removePeer(const QString& peerKey);

    // Thêm một tin nhắn vào khung chat (isMine: true = tin của mình, false = tin của bạn)
    void appendChatMessage(const QString& sender, const QString& text, const QString& timeStr, bool isMine);

    // Cập nhật tiến độ truyền nhận file (%)
    void setFileProgress(int percent, const QString& statusText);

    // Ẩn thanh tiến độ truyền file khi xong hoặc bị hủy
    void hideFileProgress();

private slots:
    void onSendMessageClicked();
    void onSendFileClicked();
    void onPeerSelected(QListWidgetItem* item);
    void onStatusChanged(int index);

private:
    void setupUi();
    void applyStyleSheet();
    void initMockData(); // Dữ liệu mẫu giúp giao diện hiển thị trực quan ngay lập tức

    // Các thành phần giao diện cột trái (Sidebar)
    QLabel* m_myProfileLabel = nullptr;
    QLabel* m_myEndpointLabel = nullptr;
    QComboBox* m_statusCombo = nullptr;
    QListWidget* m_peerListWidget = nullptr;

    // Các thành phần giao diện cột phải (Chat Area)
    QLabel* m_partnerHeaderLabel = nullptr;
    QLabel* m_partnerStatusLabel = nullptr;
    QTextBrowser* m_chatHistoryView = nullptr;
    QProgressBar* m_fileProgressBar = nullptr;
    QLabel* m_fileProgressLabel = nullptr;
    QWidget* m_fileProgressContainer = nullptr;
    QLineEdit* m_messageInput = nullptr;
    QPushButton* m_sendBtn = nullptr;
    QPushButton* m_sendFileBtn = nullptr;

    // Quản lý trạng thái nội bộ của GUI
    QString m_currentPeerKey;
    QString m_myName = "Nguyen Manh Tri";
    int m_myTcpPort = 5000;

    // Lưu trữ lịch sử tin nhắn dạng HTML cho từng bạn chat (Key: "IP:Port")
    std::map<QString, QString> m_chatHtmlHistories;
};

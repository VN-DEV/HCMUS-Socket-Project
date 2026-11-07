#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>

/*
 * ============================================================================
 * FILE: src/ui/MainWindow.h
 * NGUOI PHU TRACH: Mai Huu Tri & Nguyen Manh Tri
 * 
 * MO TA:
 * Cua so giao dien do hoa chinh cua chuong trinh viet bang Qt6 Widgets:
 * - Ben trai: Ho so ca nhan, doi trang thai (Online/Away), danh sach Online Peers.
 * - Ben phai: Khung lich su chat thoi gian thuc, thanh tien trinh gui/nhan file %.
 * - Ben duoi: O nhap tin nhan, nut gui tin nhan va nut chon file gui (QFileDialog).
 * ============================================================================
 */

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSendMessageClicked();
    void onSendFileClicked();
    void onPeerSelected(QListWidgetItem* item);
    void onStatusChanged(int index);

private:
    void setupUi();

    // Cac widget giao dien chinh
    QListWidget* m_peerListWidget = nullptr;
    QComboBox* m_statusCombo = nullptr;
    QLabel* m_partnerHeaderLabel = nullptr;
    QTextBrowser* m_chatHistoryView = nullptr;
    QLineEdit* m_messageInput = nullptr;
    QPushButton* m_sendBtn = nullptr;
    QPushButton* m_sendFileBtn = nullptr;
    QProgressBar* m_fileProgressBar = nullptr;
    QLabel* m_fileProgressLabel = nullptr;
};


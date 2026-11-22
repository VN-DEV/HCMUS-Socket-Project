#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QFileInfo>
#include "SetupDialog.h"
#include <QScrollBar>
#include "network/UdpDiscovery.h"

/*
 * ============================================================================
 * TẬP TIN: src/ui/MainWindow.cpp
 * NGƯỜI PHỤ TRÁCH: Nguyễn Mạnh Trí
 * 
 * MÔ TẢ:
 * Hiện thực giao diện đồ họa chuẩn phong cách Discord Sidebar + Messenger Chat Bubbles:
 * - Cột Sidebar bên trái: Discord Dark Theme (#2B2D31), bo góc 12px.
 * - Khung trò chuyện bên phải: QScrollArea chứa các bong bóng tin nhắn bo tròn
 *   chuẩn Messenger (Messenger Rounded Pill Bubbles):
 *   + Tin nhắn gửi đi (phải): Nằm sát lề phải, bong bóng đỏ san hô (#D33A2C) bo tròn
 *     18px ôm sát nội dung chữ (giống hệt ảnh chụp Messenger).
 *   + Tin nhắn nhận về (trái): Nằm sát lề trái kèm avatar tròn ở góc dưới, bong bóng
 *     xám tối (#383A40) bo tròn 18px ôm sát chữ.
 * ============================================================================
 */

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("HCMUS LAN Chat & File Sharing (CSC10008)");
    resize(1060, 720);
    setMinimumSize(850, 560);

    setupUi();
    applyStyleSheet();

    //m_UdpDiscovery.start(m_myName.toStdString(), m_myTcpPort);

    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &MainWindow::onRefreshPeerList);
    m_refreshTimer->start(5000); // Refresh every 5 seconds
    //initMockData();
}

MainWindow::~MainWindow() {
    stopTcpListener();
    m_UdpDiscovery.stop();
}

void MainWindow::onRefreshPeerList(){
    auto peers = m_UdpDiscovery.getOnlinePeers();
    
    // 1. Thêm hoặc cập nhật bạn bè đang online
    std::set<QString> activeKeys;
    for (const auto& p : peers){
        QString key = QString::fromStdString(p.ip) + ":" + QString::number(p.tcpPort);
        activeKeys.insert(key);
        addOrUpdatePeer(QString::fromStdString(p.name), QString::fromStdString(p.ip), p.tcpPort, QString::fromStdString(p.status));
    }

    // 2. Tự động xóa những bạn bè đã offline (quá 10s không còn tín hiệu)
    for (int i = m_peerListWidget->count() - 1; i >= 0; --i) {
        QString itemKey = m_peerListWidget->item(i)->data(Qt::UserRole).toString();
        if (activeKeys.find(itemKey) == activeKeys.end()) {
            removePeer(itemKey);
        }
    }
}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    QHBoxLayout* rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(14);

    // ========================================================================
    // 1. CỘT TRÁI (SIDEBAR THẺ TRẮNG BO GÓC)
    // ========================================================================
    QWidget* leftPanel = new QWidget(this);
    leftPanel->setObjectName("leftPanel");
    leftPanel->setFixedWidth(290);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(12);

    // Thẻ thông tin cá nhân (User Card)
    QFrame* userCard = new QFrame(leftPanel);
    userCard->setObjectName("userCard");
    QVBoxLayout* userCardLayout = new QVBoxLayout(userCard);
    userCardLayout->setContentsMargins(14, 12, 14, 12);
    userCardLayout->setSpacing(10);

    // Dòng 1: Tên xanh dương bên trái, Port bên phải
    QHBoxLayout* userRow1 = new QHBoxLayout();
    m_myProfileLabel = new QLabel(m_myName, userCard);
    m_myProfileLabel->setObjectName("myProfileName");

    m_myEndpointLabel = new QLabel("Port: " + QString::number(m_myTcpPort), userCard);
    m_myEndpointLabel->setObjectName("myProfilePort");

    userRow1->addWidget(m_myProfileLabel);
    userRow1->addStretch(1);
    userRow1->addWidget(m_myEndpointLabel);
    userCardLayout->addLayout(userRow1);

    // Dòng 2: Trạng thái và Dropdown QComboBox
    QHBoxLayout* userRow2 = new QHBoxLayout();
    QLabel* statusLabel = new QLabel("Trạng thái:", userCard);
    statusLabel->setObjectName("statusTitle");

    m_statusCombo = new QComboBox(userCard);
    m_statusCombo->setObjectName("statusCombo");
    m_statusCombo->addItem("Trực tuyến");
    m_statusCombo->addItem("Bận");

    userRow2->addWidget(statusLabel);
    userRow2->addSpacing(8);
    userRow2->addWidget(m_statusCombo, 1);
    userCardLayout->addLayout(userRow2);

    leftLayout->addWidget(userCard);

    // Tiêu đề danh sách bạn bè
    m_peerCountLabel = new QLabel("Bạn bè trong mạng LAN  <font color='#0084FF'><b>(0)</b></font>", leftPanel);
    m_peerCountLabel->setObjectName("peerCountLabel");
    leftLayout->addWidget(m_peerCountLabel);

    // Thẻ danh sách bạn bè
    m_peerListWidget = new QListWidget(leftPanel);
    m_peerListWidget->setObjectName("peerListWidget");
    leftLayout->addWidget(m_peerListWidget, 1);

    rootLayout->addWidget(leftPanel);

    // ========================================================================
    // 2. CỘT PHẢI (CHAT AREA)
    // ========================================================================
    QWidget* rightPanel = new QWidget(this);
    rightPanel->setObjectName("rightPanel");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    // Header thẻ trắng trên cùng
    QFrame* chatHeader = new QFrame(rightPanel);
    chatHeader->setObjectName("chatHeaderCard");
    chatHeader->setFixedHeight(66);
    QHBoxLayout* headerLayout = new QHBoxLayout(chatHeader);
    headerLayout->setContentsMargins(18, 10, 18, 10);

    QVBoxLayout* titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(6);
    m_partnerHeaderLabel = new QLabel("Chưa chọn người nhận", chatHeader);
    m_partnerHeaderLabel->setObjectName("chatPartnerTitle");

    m_partnerStatusLabel = new QLabel("Nhấp vào bạn bè bên trái để bắt đầu nhắn tin hoặc gửi file", chatHeader);
    m_partnerStatusLabel->setObjectName("chatPartnerSubtitle");

    titleLayout->addWidget(m_partnerHeaderLabel);
    titleLayout->addWidget(m_partnerStatusLabel);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch(1);

    // Nhãn viên thuốc "Đã kết nối"
    m_connectedBadge = new QLabel("Đã kết nối", chatHeader);
    m_connectedBadge->setObjectName("connectedBadge");
    m_connectedBadge->setVisible(false);
    m_connectedBadge->setAlignment(Qt::AlignCenter);
    m_connectedBadge->setFixedHeight(34); // Đặt chiều cao 34px
    headerLayout->addWidget(m_connectedBadge);
    headerLayout->addSpacing(8);

    // Nút "Ngắt kết nối" (màu đỏ)
    m_disconnectBtn = new QPushButton("Ngắt kết nối", chatHeader);
    m_disconnectBtn->setObjectName("disconnectBtn");
    m_disconnectBtn->setCursor(Qt::PointingHandCursor);
    m_disconnectBtn->setFixedHeight(34); // Đặt chiều cao 34px to ngang nút kế bên
    headerLayout->addWidget(m_disconnectBtn);
    headerLayout->addSpacing(8);

    // Nút Theme tròn
    m_themeBtn = new QPushButton("🌙", chatHeader);
    m_themeBtn->setObjectName("themeBtn");
    m_themeBtn->setFixedSize(34, 34);
    m_themeBtn->setCursor(Qt::PointingHandCursor);
    headerLayout->addWidget(m_themeBtn);

    rightLayout->addWidget(chatHeader);

    // Khung cuộn hiển thị tin nhắn (Thẻ trắng bo góc 16px)
    m_chatScrollArea = new QScrollArea(rightPanel);
    m_chatScrollArea->setObjectName("chatScrollArea");
    m_chatScrollArea->setWidgetResizable(true);
    m_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scrollContent = new QWidget();
    m_scrollContent->setObjectName("chatScrollContent");
    m_messagesLayout = new QVBoxLayout(m_scrollContent);
    m_messagesLayout->setContentsMargins(20, 16, 20, 16);
    m_messagesLayout->setSpacing(8);
    m_messagesLayout->addStretch(1);

    m_chatScrollArea->setWidget(m_scrollContent);
    rightLayout->addWidget(m_chatScrollArea, 1);

    // Thanh tiến độ truyền file
    m_fileProgressContainer = new QWidget(rightPanel);
    m_fileProgressContainer->setObjectName("fileEmbed");
    QVBoxLayout* progressLayout = new QVBoxLayout(m_fileProgressContainer);
    progressLayout->setContentsMargins(14, 8, 14, 8);
    m_fileProgressLabel = new QLabel("Đang gửi file...", m_fileProgressContainer);
    m_fileProgressBar = new QProgressBar(m_fileProgressContainer);
    m_fileProgressBar->setFixedHeight(12);
    m_fileProgressBar->setTextVisible(false);
    progressLayout->addWidget(m_fileProgressLabel);
    progressLayout->addWidget(m_fileProgressBar);
    m_fileProgressContainer->setVisible(false);
    rightLayout->addWidget(m_fileProgressContainer);



    // Thanh nhập liệu đáy
    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(10);

    // Nút tròn "+" đính kèm file
    m_sendFileBtn = new QPushButton("+", rightPanel);
    m_sendFileBtn->setObjectName("attachBtn");
    m_sendFileBtn->setFixedSize(38, 38);
    m_sendFileBtn->setCursor(Qt::PointingHandCursor);

    // Ô nhập tin nhắn bo viên thuốc
    m_messageInput = new QLineEdit(rightPanel);
    m_messageInput->setObjectName("messageInput");
    m_messageInput->setPlaceholderText("Nhập tin nhắn... (Nhấn Enter để gửi)");
    m_messageInput->setFixedHeight(42);



    // Nút tròn gửi tin nhắn
    m_sendBtn = new QPushButton("➤", rightPanel);
    m_sendBtn->setObjectName("sendBtn");
    m_sendBtn->setFixedSize(40, 40);
    m_sendBtn->setCursor(Qt::PointingHandCursor);

    m_messageInput->setEnabled(false); // Lúc này m_messageInput vẫn là nullptr!
    m_messageInput->setPlaceholderText("Nhấp vào bạn bè bên trái để bắt đầu chat...");
    m_sendBtn->setEnabled(false);

    inputLayout->addWidget(m_sendFileBtn);
    inputLayout->addWidget(m_messageInput, 1);
    inputLayout->addWidget(m_sendBtn);
    connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::onToggleTheme);

    rightLayout->addLayout(inputLayout);
    rootLayout->addWidget(rightPanel, 1);

    // Kết nối các sự kiện
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendMessageClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendMessageClicked);
    connect(m_sendFileBtn, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);
    connect(m_peerListWidget, &QListWidget::itemClicked, this, &MainWindow::onPeerSelected);

    connect(m_disconnectBtn, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
}

void MainWindow::applyStyleSheet() {
    QString qss = R"(
        QWidget {
            font-family: 'Segoe UI', Arial, sans-serif;
            color: #050505;
        }

        #centralWidget {
            background-color: #F0F2F5;
        }

        /* 1. Thẻ thông tin cá nhân */
        #userCard {
            background-color: #FFFFFF;
            border: 1px solid #E4E6EB;
            border-radius: 14px;
        }

        #myProfileName {
            font-size: 16px;
            font-weight: 800;
            color: #0084FF;
        }

        #myProfilePort {
            font-size: 12px;
            font-weight: 600;
            color: #65676B;
        }

        #statusTitle {
            font-size: 13px;
            color: #65676B;
            font-weight: 500;
        }

        #statusCombo {
            background-color: #FFFFFF;
            border: 1px solid #CCD0D5;
            border-radius: 8px;
            padding: 4px 8px;
            font-size: 12.5px;
            font-weight: 600;
            color: #050505;
        }

        #peerCountLabel {
            font-size: 13px;
            font-weight: 700;
            color: #050505;
            padding-left: 2px;
        }

        /* 2. Danh sách bạn bè */
        #peerListWidget {
            background-color: #FFFFFF;
            border: 1px solid #E4E6EB;
            border-radius: 14px;
            padding: 6px;
            outline: none;
        }

        #peerListWidget::item {
            border-radius: 10px;
            padding: 8px 10px;
            margin-bottom: 4px;
        }

        #peerListWidget::item:hover {
            background-color: #F2F4F7;
        }

        #peerListWidget::item:selected {
            background-color: #EBF5FF;
            border: 1px solid #90CDF4;
        }
        
        #connectedBadge {
            background-color: #EBF5FF;
            color: #0084FF;
            border-radius: 17px;
            padding: 0 16px;
            font-size: 13px;
            font-weight: 700;
        }

        #disconnectBtn {
            background-color: #FDE8E8;
            color: #E02424;
            border: none;
            border-radius: 17px;
            padding: 0 16px;
            font-size: 13px;
            font-weight: 700;
        }

        #disconnectBtn:hover {
            background-color: #FBD5D5;
        }

        /* 3. Header khu vực Chat */
        #chatHeaderCard {
            background-color: #FFFFFF;
            border: 1px solid #E4E6EB;
            border-radius: 14px;
        }

        #chatPartnerTitle {
            font-size: 16px;
            font-weight: 700;
            color: #050505;
        }

        #chatPartnerSubtitle {
            font-size: 12px;
            color: #65676B;
        }

        #connectedBadge {
            background-color: #EBF5FF;
            color: #0084FF;
            border-radius: 12px;
            padding: 4px 12px;
            font-size: 12px;
            font-weight: 700;
        }

        #disconnectBtn {
            background-color: #FDE8E8;
            color: #E02424;
            border: none;
            border-radius: 14px;
            padding: 6px 14px;
            font-size: 12px;
            font-weight: 700;
        }

        #disconnectBtn:hover {
            background-color: #FBD5D5;
        }

        #themeBtn {
            background-color: #F0F2F5;
            border: none;
            border-radius: 17px;
            font-size: 14px;
        }

        #themeBtn:hover {
            background-color: #E4E6EB;
        }

        /* 4. Khung nội dung trò chuyện */
        #chatScrollArea {
            background-color: #FFFFFF;
            border: 1px solid #E4E6EB;
            border-radius: 16px;
        }

        #chatScrollContent {
            background-color: #FFFFFF;
        }

        /* 5. Bong bóng tin nhắn Messenger */
        #myBubble {
            background-color: #0084FF;
            color: #FFFFFF;
            border-radius: 18px;
            padding: 9px 16px;
            font-size: 14px;
            font-weight: 500;
        }

        #peerBubble {
            background-color: #E4E6EB;
            color: #050505;
            border-radius: 18px;
            padding: 9px 16px;
            font-size: 14px;
            font-weight: 500;
        }

        #systemMessageLabel {
            color: #65676B;
            font-size: 12px;
            font-weight: 600;
            padding: 6px 0;
        }

        /* 6. Thanh nhập liệu đáy */
        #attachBtn {
            background-color: #F0F2F5;
            color: #65676B;
            border: none;
            border-radius: 19px;
            font-size: 20px;
            font-weight: 600;
        }

        #attachBtn:hover {
            background-color: #E4E6EB;
        }

        #messageInput {
            background-color: #F0F2F5;
            border: 1.5px solid transparent;
            border-radius: 21px;
            padding: 0 16px;
            font-size: 13.5px;
            color: #050505;
        }

        #messageInput:focus {
            background-color: #FFFFFF;
            border: 1.5px solid #0084FF;
        }

        #sendBtn {
            background-color: #0084FF;
            color: #FFFFFF;
            border: none;
            border-radius: 20px;
            font-size: 15px;
            font-weight: bold;
        }

        #sendBtn:hover {
            background-color: #0073E6;
        }
    )";
    setStyleSheet(qss);
}

void MainWindow::initMockData() {
    addOrUpdatePeer("LEGION_2", "172.16.33.41", 5002, "online", 0);
}

void MainWindow::renderMessageBubble(const QString& sender, const QString& text, const QString& timeStr, bool isMine) {
    QWidget* rowWidget = new QWidget(m_scrollContent);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(0, 2, 0, 2);
    rowLayout->setSpacing(8);

    QLabel* bubble = new QLabel(text, rowWidget);
    bubble->setWordWrap(true);
    bubble->setMaximumWidth(480);
    bubble->setTextInteractionFlags(Qt::TextSelectableByMouse);
    bubble->setAttribute(Qt::WA_StyledBackground, true);

    if (isMine) {
        // Tin gửi đi: Căn sát lề phải, bong bóng xanh dương viên thuốc #0084FF
        rowLayout->addStretch(1);
        bubble->setObjectName("myBubble");
        rowLayout->addWidget(bubble);
    } else {
        // Tin nhận về: Căn lề trái, bong bóng xám nhạt #E4E6EB
        bubble->setObjectName("peerBubble");
        rowLayout->addWidget(bubble);
        rowLayout->addStretch(1);
    }

    m_messagesLayout->insertWidget(m_messagesLayout->count() - 1, rowWidget);
}

void MainWindow::scrollToBottom() {
    QTimer::singleShot(40, this, [this]() {
        if (m_chatScrollArea && m_chatScrollArea->verticalScrollBar()) {
            m_chatScrollArea->verticalScrollBar()->setValue(m_chatScrollArea->verticalScrollBar()->maximum());
        }
    });
}

void MainWindow::setMyProfile(const QString& name, const QString& ip, int port) {
    m_myName = name;
    m_myTcpPort = port;
    setWindowTitle(QString("HCMUS P2P LAN Chat & File Sharing - %1 (Port: %2)").arg(name).arg(port));
    if (m_myProfileLabel) m_myProfileLabel->setText(name);
    if (m_myEndpointLabel) m_myEndpointLabel->setText("Port: " + QString::number(port));

    // Giữ cổng TCP thực tế trên Windows
    startTcpListener(port);

    m_UdpDiscovery.stop();
    m_UdpDiscovery.start(m_myName.toStdString(), m_myTcpPort);
}

void MainWindow::addOrUpdatePeer(const QString& name, const QString& ip, int port, const QString& status, int unreadCount) {
    QString key = ip + ":" + QString::number(port);

    // Kiểm tra nếu đã có trong danh sách thì bỏ qua không tạo trùng
    for (int i = 0; i < m_peerListWidget->count(); ++i) {
        QListWidgetItem* item = m_peerListWidget->item(i);
        if (item->data(Qt::UserRole).toString() == key) {
            return;
        }
    }

    // Tạo Item bạn bè mới
    QListWidgetItem* newItem = new QListWidgetItem(m_peerListWidget);
    newItem->setData(Qt::UserRole, key);
    newItem->setData(Qt::UserRole + 1, name);
    newItem->setData(Qt::UserRole + 2, status);
    newItem->setSizeHint(QSize(260, 56)); // Chiều cao 56px thoáng đãng

    QWidget* itemWidget = new QWidget();
    itemWidget->setStyleSheet("background: transparent;");
    QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
    itemLayout->setContentsMargins(8, 4, 8, 4);
    itemLayout->setSpacing(10);

    // Chấm tròn xanh
    QLabel* dot = new QLabel("●", itemWidget);
    dot->setStyleSheet("color: #23A55A; font-size: 14px; background: transparent;");
    itemLayout->addWidget(dot);

    // Layout chứa 2 dòng chữ
    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setContentsMargins(0, 0, 0, 0); // Xóa lề mặc định
    textLayout->setSpacing(4); // Khoảng cách 4px giúp 2 dòng tách biệt rõ ràng

    QLabel* nameLabel = new QLabel(name, itemWidget);
    nameLabel->setFixedHeight(18); // Khóa chiều cao tên
    nameLabel->setStyleSheet("font-weight: 700; font-size: 13.5px; color: #050505; background: transparent;");

    QLabel* endpointLabel = new QLabel(key, itemWidget);
    endpointLabel->setFixedHeight(16); // Khóa chiều cao IP
    endpointLabel->setStyleSheet("font-size: 11.5px; color: #65676B; background: transparent;");

    textLayout->addWidget(nameLabel);
    textLayout->addWidget(endpointLabel);
    itemLayout->addLayout(textLayout, 1);

    m_peerListWidget->setItemWidget(newItem, itemWidget);

    // Cập nhật số đếm trên tiêu đề
    if (m_peerCountLabel) {
        m_peerCountLabel->setText(QString("Bạn bè trong mạng LAN  <font color='#0084FF'><b>(%1)</b></font>").arg(m_peerListWidget->count()));
    }
}

void MainWindow::removePeer(const QString& peerKey) {
    for (int i = 0; i < m_peerListWidget->count(); ++i) {
        QListWidgetItem* item = m_peerListWidget->item(i);
        if (item->data(Qt::UserRole).toString() == peerKey) {
            delete m_peerListWidget->takeItem(i);
            break;
        }
    }
    if (m_peerCountLabel) {
        m_peerCountLabel->setText(QString("Bạn bè trong mạng LAN  <font color='#0084FF'><b>(%1)</b></font>").arg(m_peerListWidget->count()));
    }

    // NẾU NGƯỜI VỪA THOÁT ĐÚNG LÀ NGƯỜI ĐANG TRÒ CHUYỆN:
    if (peerKey == m_currentPeerKey) {
        m_currentPeerKey.clear();
        m_currentPeerName.clear();

        m_partnerHeaderLabel->setText("Chưa chọn người nhận");
        m_partnerStatusLabel->setText("Người bạn này đã ngắt kết nối");
        if (m_connectedBadge) {
            m_connectedBadge->setVisible(false);
        }

        // Khóa ô nhập và nút gửi, không cho gửi tiếp
        m_messageInput->setEnabled(false);
        m_messageInput->setPlaceholderText("Người bạn này đã thoát. Hãy chọn bạn khác...");
        m_sendBtn->setEnabled(false);

        // Thông báo chữ đỏ trong khung chat
        QLabel* disconnectNotice = new QLabel("Người bạn này đã ngắt kết nối khỏi phòng chat.", m_scrollContent);
        disconnectNotice->setObjectName("systemMessageLabel");
        disconnectNotice->setAlignment(Qt::AlignCenter);
        disconnectNotice->setStyleSheet("color: #E02424; font-weight: 600; font-size: 12px; padding: 6px 0;");
        m_messagesLayout->insertWidget(m_messagesLayout->count() - 1, disconnectNotice);
        scrollToBottom();
    }
}

void MainWindow::appendChatMessage(const QString& sender, const QString& text, const QString& timeStr, bool isMine) {
    // Lưu vào lịch sử tin nhắn
    m_chatHistories[m_currentPeerKey].push_back({sender, text, timeStr, isMine});

    // Vẽ bong bóng tin nhắn lên giao diện
    renderMessageBubble(sender, text, timeStr, isMine);
    scrollToBottom();
}

void MainWindow::setFileProgress(int percent, const QString& statusText) {
    if (m_fileProgressContainer) m_fileProgressContainer->setVisible(true);
    if (m_fileProgressLabel) m_fileProgressLabel->setText(statusText);
    if (m_fileProgressBar) m_fileProgressBar->setValue(percent);
}

void MainWindow::hideFileProgress() {
    if (m_fileProgressContainer) {
        m_fileProgressContainer->setVisible(false);
    }
}

void MainWindow::onSendMessageClicked() {
    QString text = m_messageInput->text().trimmed();
    if (text.isEmpty()) return;

    QString now = QDateTime::currentDateTime().toString("hh:mm");
    appendChatMessage(m_myName, text, now, true);
    m_messageInput->clear();
    m_messageInput->setFocus();
}

void MainWindow::onSendFileClicked() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select File to Send",
        "",
        "All Files (*.*)"
    );

    if (filePath.isEmpty()) return;

    QFileInfo info(filePath);
    QString fileName = info.fileName();
    qint64 fileSize = info.size();

    // Hiển thị Embed tiến trình gửi file kiểu Discord bo góc 10px
    setFileProgress(0, QString("Uploading %1 (%2 KB)...").arg(fileName).arg(fileSize / 1024));

    QTimer* timer = new QTimer(this);
    int* currentProgress = new int(0);

    connect(timer, &QTimer::timeout, this, [this, timer, currentProgress, fileName]() {
        *currentProgress += 15;
        if (*currentProgress >= 100) {
            setFileProgress(100, QString("Uploaded %1 successfully!").arg(fileName));
            timer->stop();
            timer->deleteLater();
            delete currentProgress;

            // Thêm một thông báo file vào dòng chat
            QString now = QDateTime::currentDateTime().toString("hh:mm");
            appendChatMessage(m_myName, QString("[File đính kèm] %1").arg(fileName), now, true);

            // Tự động ẩn Embed sau 2 giây
            QTimer::singleShot(2000, this, &MainWindow::hideFileProgress);
        } else {
            setFileProgress(*currentProgress, QString("Uploading %1 (%2%)...").arg(fileName).arg(*currentProgress));
        }
    });

    timer->start(150);
}

void MainWindow::onPeerSelected(QListWidgetItem* item) {
    m_messageInput->setEnabled(true);
    m_messageInput->setPlaceholderText("Nhập tin nhắn... (Nhấn Enter để gửi)");
    m_messageInput->setFocus();
    m_sendBtn->setEnabled(true);

    if (!item) return;

    m_currentPeerKey = item->data(Qt::UserRole).toString();
    m_currentPeerName = item->data(Qt::UserRole + 1).toString();
    QString status = item->data(Qt::UserRole + 2).toString();

    // 1. Cập nhật Header đúng như Hình 3
    m_partnerHeaderLabel->setText("Trò chuyện cùng " + m_currentPeerName);
    m_partnerStatusLabel->setText(m_currentPeerKey + " • Trực tuyến");
    if (m_connectedBadge) {
        m_connectedBadge->setVisible(true); // Hiện nhãn "Đã kết nối"
    }

    // 2. Xóa các widget tin nhắn cũ trên màn hình (chừa lại stretch ở cuối)
    while (m_messagesLayout->count() > 1) {
        QLayoutItem* layoutItem = m_messagesLayout->takeAt(0);
        if (layoutItem->widget()) {
            delete layoutItem->widget();
        }
        delete layoutItem;
    }

    // 3. Thêm dòng thông báo hệ thống ở giữa: "Đã kết nối với <Tên>."
    QLabel* sysMsg = new QLabel(QString("Đã kết nối với %1.").arg(m_currentPeerName), m_scrollContent);
    sysMsg->setObjectName("systemMessageLabel");
    sysMsg->setAlignment(Qt::AlignCenter);
    m_messagesLayout->insertWidget(m_messagesLayout->count() - 1, sysMsg);

    // 4. Nạp lại lịch sử trò chuyện của người bạn này
    const auto& history = m_chatHistories[m_currentPeerKey];
    for (const auto& msg : history) {
        renderMessageBubble(msg.sender, msg.text, msg.timeStr, msg.isMine);
    }
    scrollToBottom();

    m_messageInput->setEnabled(true);
    m_messageInput->setPlaceholderText("Nhập tin nhắn... (Nhấn Enter để gửi)");
    m_sendBtn->setEnabled(true);
}

void MainWindow::onToggleTheme() {
    m_isDarkMode = !m_isDarkMode;
    
    if (m_isDarkMode) {
        m_themeBtn->setText("☀️");
        // Đổi sang bảng màu Dark Mode
        setStyleSheet(R"(
            QWidget { font-family: 'Segoe UI', Arial, sans-serif; color: #DBDEE1; }
            #centralWidget { background-color: #1E1F22; }
            #userCard, #chatHeaderCard, #chatScrollArea, #chatScrollContent, #peerListWidget { 
                background-color: #2B2D31; border: 1px solid #1F2023; border-radius: 14px; 
            }
            #myProfileName { font-size: 16px; font-weight: 800; color: #5865F2; }
            #myProfilePort, #statusTitle, #chatPartnerSubtitle { font-size: 12px; color: #949BA4; }
            #statusCombo { background-color: #313338; border: 1px solid #1F2023; border-radius: 8px; color: #DBDEE1; padding: 4px 8px; }
            #peerCountLabel { font-size: 13px; font-weight: 700; color: #DBDEE1; }
            #peerListWidget::item { border-radius: 10px; padding: 8px 10px; margin-bottom: 4px; }
            #peerListWidget::item:hover { background-color: #35373C; }
            #peerListWidget::item:selected { background-color: #404249; border: 1px solid #5865F2; }
            #chatPartnerTitle { font-size: 16px; font-weight: 700; color: #FFFFFF; }
            #connectedBadge { background-color: #233644; color: #0084FF; border-radius: 12px; padding: 4px 12px; font-weight: 700; }
            #disconnectBtn { background-color: #3C2224; color: #F23F43; border: none; border-radius: 14px; padding: 6px 14px; font-weight: 700; }
            #themeBtn, #attachBtn { background-color: #383A40; color: #DBDEE1; border: none; border-radius: 17px; }
            #messageInput { background-color: #383A40; border: 1.5px solid transparent; border-radius: 21px; padding: 0 16px; color: #FFFFFF; }
            #messageInput:focus { border: 1.5px solid #5865F2; }
            #sendBtn { background-color: #5865F2; color: #FFFFFF; border: none; border-radius: 20px; font-size: 15px; }
            #myBubble { background-color: #5865F2; color: #FFFFFF; border-radius: 18px; padding: 9px 16px; font-size: 14px; }
            #peerBubble { background-color: #383A40; color: #DBDEE1; border-radius: 18px; padding: 9px 16px; font-size: 14px; }
            #systemMessageLabel { color: #949BA4; font-size: 12px; font-weight: 600; padding: 6px 0; }
        )");
    } else {
        m_themeBtn->setText("🌙");
        applyStyleSheet(); // Trở về bảng màu Light Mode gốc
    }
}

void MainWindow::onDisconnectClicked() {
    // 1. Dừng Discovery ngắt phát broadcast
    stopTcpListener();
    m_UdpDiscovery.stop();

    // 2. Đưa màn hình chat về trạng thái ban đầu (Chưa chọn người nhận)
    m_currentPeerKey.clear();
    m_currentPeerName.clear();
    m_partnerHeaderLabel->setText("Chưa chọn người nhận");
    m_partnerStatusLabel->setText("Nhấp vào bạn bè bên trái để bắt đầu nhắn tin hoặc gửi file");
    if (m_connectedBadge) {
        m_connectedBadge->setVisible(false);
    }

    // 3. Xóa các bong bóng chat trên màn hình
    while (m_messagesLayout->count() > 1) {
        QLayoutItem* layoutItem = m_messagesLayout->takeAt(0);
        if (layoutItem->widget()) {
            delete layoutItem->widget();
        }
        delete layoutItem;
    }

    // 4. Xóa danh sách bạn bè
    m_peerListWidget->clear();
    if (m_peerCountLabel) {
        m_peerCountLabel->setText("Bạn bè trong mạng LAN  <font color='#0084FF'><b>(0)</b></font>");
    }

    // 5. Ẩn cửa sổ MainWindow và mở lại SetupDialog
    this->hide();

    SetupDialog setupDlg;
    if (setupDlg.exec() == QDialog::Accepted) {
        QString newName = setupDlg.getUserName();
        int newPort = setupDlg.getTcpPort();

        // Cập nhật thông tin mới và hiện lại cửa sổ chat
        setMyProfile(newName, "127.0.0.1", newPort);
        this->show();
    } else {
        // Nếu người dùng bấm "Thoát" ở hộp thoại thì tắt hẳn ứng dụng
        this->close();
    }
}

void MainWindow::startTcpListener(int port) {
    stopTcpListener();
    m_tcpListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_tcpListenSock != INVALID_SOCKET) {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
        bind(m_tcpListenSock, (sockaddr*)&addr, sizeof(addr));
        listen(m_tcpListenSock, SOMAXCONN);
    }
}

void MainWindow::stopTcpListener() {
    if (m_tcpListenSock != INVALID_SOCKET) {
        closesocket(m_tcpListenSock);
        m_tcpListenSock = INVALID_SOCKET;
    }
}
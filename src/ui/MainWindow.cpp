#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QFileInfo>
#include <QScrollBar>

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
    initMockData();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    // Layout gốc đệm 10px tạo hiệu ứng các panel nổi bo góc
    QHBoxLayout* rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(10, 10, 10, 10);
    rootLayout->setSpacing(10);

    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(0);

    // ========================================================================
    // 1. CỘT TRÁI (DISCORD SIDEBAR #2B2D31 - BO GÓC 12px)
    // ========================================================================
    QWidget* leftPanel = new QWidget(this);
    leftPanel->setObjectName("discordSidebar");
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(10, 12, 10, 10);
    leftLayout->setSpacing(8);

    // Tiêu đề danh mục Direct Messages
    QLabel* dmHeader = new QLabel("DIRECT MESSAGES", leftPanel);
    dmHeader->setObjectName("discordDmHeader");
    leftLayout->addWidget(dmHeader);

    // Danh sách các peer online (bo góc từng item)
    m_peerListWidget = new QListWidget(leftPanel);
    m_peerListWidget->setObjectName("discordPeerList");
    leftLayout->addWidget(m_peerListWidget, 1);

    // Thanh User Profile mini dạng card bo góc 10px cố định ở đáy (#232428)
    QWidget* userFooter = new QWidget(leftPanel);
    userFooter->setObjectName("discordUserFooter");
    QHBoxLayout* userFooterLayout = new QHBoxLayout(userFooter);
    userFooterLayout->setContentsMargins(10, 8, 10, 8);
    userFooterLayout->setSpacing(10);

    // Avatar tròn viết tắt 2 chữ cái đầu (bo tròn 17px)
    m_myAvatarLabel = new QLabel("NT", userFooter);
    m_myAvatarLabel->setObjectName("discordAvatarBadge");
    m_myAvatarLabel->setFixedSize(34, 34);
    m_myAvatarLabel->setAlignment(Qt::AlignCenter);

    // Tên và thông số mạng
    QVBoxLayout* userTextLayout = new QVBoxLayout();
    userTextLayout->setSpacing(1);
    m_myProfileLabel = new QLabel(m_myName, userFooter);
    m_myProfileLabel->setObjectName("discordUserName");

    m_myEndpointLabel = new QLabel("Port: " + QString::number(m_myTcpPort), userFooter);
    m_myEndpointLabel->setObjectName("discordUserSubtext");
    userTextLayout->addWidget(m_myProfileLabel);
    userTextLayout->addWidget(m_myEndpointLabel);

    userFooterLayout->addWidget(m_myAvatarLabel);
    userFooterLayout->addLayout(userTextLayout, 1);

    leftLayout->addWidget(userFooter);

    mainSplitter->addWidget(leftPanel);

    // ========================================================================
    // 2. CỘT PHẢI (MESSENGER BUBBLES IN DISCORD CHAT AREA #313338 - BO GÓC 12px)
    // ========================================================================
    QWidget* rightPanel = new QWidget(this);
    rightPanel->setObjectName("discordChatArea");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // Thanh Header kênh chat ở đỉnh bo góc
    QWidget* chatHeaderWidget = new QWidget(rightPanel);
    chatHeaderWidget->setObjectName("discordChatHeader");
    chatHeaderWidget->setFixedHeight(52);
    QHBoxLayout* headerLayout = new QHBoxLayout(chatHeaderWidget);
    headerLayout->setContentsMargins(18, 0, 18, 0);

    m_partnerHeaderLabel = new QLabel("@ Select a friend to start messaging", chatHeaderWidget);
    m_partnerHeaderLabel->setObjectName("discordChannelTitle");

    m_partnerStatusLabel = new QLabel("", chatHeaderWidget);
    m_partnerStatusLabel->setObjectName("discordChannelSubtext");

    headerLayout->addWidget(m_partnerHeaderLabel);
    headerLayout->addSpacing(12);
    headerLayout->addWidget(m_partnerStatusLabel);
    headerLayout->addStretch();
    rightLayout->addWidget(chatHeaderWidget);

    // Khung cuộn hiển thị bong bóng chat Messenger (QScrollArea)
    m_chatScrollArea = new QScrollArea(rightPanel);
    m_chatScrollArea->setObjectName("chatScrollArea");
    m_chatScrollArea->setWidgetResizable(true);
    m_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_scrollContent = new QWidget();
    m_scrollContent->setObjectName("chatScrollContent");
    m_messagesLayout = new QVBoxLayout(m_scrollContent);
    m_messagesLayout->setContentsMargins(20, 16, 20, 16);
    m_messagesLayout->setSpacing(6); // Khoảng cách nhỏ gọn giữa các bong bóng kiểu Messenger
    m_messagesLayout->addStretch(1); // Đẩy các tin nhắn lên trên hoặc giữ cuộn mượt mà

    m_chatScrollArea->setWidget(m_scrollContent);
    rightLayout->addWidget(m_chatScrollArea, 1);

    // Khung tiến độ truyền file (Embed card #2B2D31, bo góc 10px, viền Blurple)
    m_fileProgressContainer = new QWidget(rightPanel);
    m_fileProgressContainer->setObjectName("discordFileEmbed");
    QVBoxLayout* progressLayout = new QVBoxLayout(m_fileProgressContainer);
    progressLayout->setContentsMargins(16, 10, 16, 10);
    progressLayout->setSpacing(6);

    m_fileProgressLabel = new QLabel("Uploading file...", m_fileProgressContainer);
    m_fileProgressLabel->setObjectName("discordEmbedTitle");

    m_fileProgressBar = new QProgressBar(m_fileProgressContainer);
    m_fileProgressBar->setObjectName("discordProgressBar");
    m_fileProgressBar->setRange(0, 100);
    m_fileProgressBar->setValue(0);
    m_fileProgressBar->setTextVisible(true);

    progressLayout->addWidget(m_fileProgressLabel);
    progressLayout->addWidget(m_fileProgressBar);
    m_fileProgressContainer->setVisible(false);
    rightLayout->addWidget(m_fileProgressContainer);

    // Thanh nhập liệu nổi bo tròn 12px (#383A40)
    QWidget* inputContainer = new QWidget(rightPanel);
    inputContainer->setObjectName("discordInputContainer");
    QVBoxLayout* inputContainerLayout = new QVBoxLayout(inputContainer);
    inputContainerLayout->setContentsMargins(16, 0, 16, 16);

    QWidget* inputPill = new QWidget(inputContainer);
    inputPill->setObjectName("discordInputPill");
    QHBoxLayout* pillLayout = new QHBoxLayout(inputPill);
    pillLayout->setContentsMargins(6, 6, 8, 6);
    pillLayout->setSpacing(8);

    m_sendFileBtn = new QPushButton("+ File", inputPill);
    m_sendFileBtn->setObjectName("discordAttachBtn");
    m_sendFileBtn->setCursor(Qt::PointingHandCursor);

    m_messageInput = new QLineEdit(inputPill);
    m_messageInput->setObjectName("discordMessageInput");
    m_messageInput->setPlaceholderText("Message @peer... (Press Enter to send)");

    m_sendBtn = new QPushButton("Send", inputPill);
    m_sendBtn->setObjectName("discordSendBtn");
    m_sendBtn->setCursor(Qt::PointingHandCursor);

    pillLayout->addWidget(m_sendFileBtn);
    pillLayout->addWidget(m_messageInput, 1);
    pillLayout->addWidget(m_sendBtn);

    inputContainerLayout->addWidget(inputPill);
    rightLayout->addWidget(inputContainer);

    mainSplitter->addWidget(rightPanel);
    mainSplitter->setSizes({270, 770});

    rootLayout->addWidget(mainSplitter);

    // Kết nối các sự kiện tương tác
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendMessageClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendMessageClicked);
    connect(m_sendFileBtn, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);
    connect(m_peerListWidget, &QListWidget::itemClicked, this, &MainWindow::onPeerSelected);
}

void MainWindow::applyStyleSheet() {
    QString qss = R"(
        /* Nền tối sâu Discord của toàn bộ cửa sổ */
        #centralWidget {
            background-color: #1E1F22;
        }

        QWidget {
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13.5px;
            color: #DBDEE1;
        }

        /* 1. Sidebar bên trái (Discord #2B2D31) */
        #discordSidebar {
            background-color: #2B2D31;
            border-radius: 12px;
            border: 1px solid #1F2023;
        }

        #discordDmHeader {
            font-size: 11px;
            font-weight: 700;
            color: #949BA4;
            letter-spacing: 0.5px;
            padding: 8px 10px 4px 10px;
            background-color: transparent;
        }

        #discordPeerList {
            background-color: transparent;
            border: none;
            outline: none;
            padding: 2px;
        }

        #discordPeerList::item {
            height: 40px;
            padding: 6px 12px;
            margin-bottom: 3px;
            border-radius: 8px;
            color: #949BA4;
            font-weight: 500;
        }

        #discordPeerList::item:hover {
            background-color: #35373C;
            color: #DBDEE1;
        }

        #discordPeerList::item:selected {
            background-color: #404249;
            color: #FFFFFF;
            font-weight: 600;
        }

        /* 2. Thanh User Profile mini ở góc đáy */
        #discordUserFooter {
            background-color: #232428;
            border-radius: 10px;
            border: 1px solid #1A1B1E;
        }

        #discordAvatarBadge {
            background-color: #5865F2;
            color: #FFFFFF;
            font-weight: bold;
            font-size: 13px;
            border-radius: 17px;
        }

        #discordUserName {
            font-size: 13.5px;
            font-weight: 700;
            color: #F2F3F5;
            background: transparent;
        }

        #discordUserSubtext {
            font-size: 11px;
            color: #949BA4;
            background: transparent;
        }

        /* 3. Khung Chat Area (#313338) */
        #discordChatArea {
            background-color: #313338;
            border-radius: 12px;
            border: 1px solid #1F2023;
        }

        #discordChatHeader {
            background-color: #313338;
            border-top-left-radius: 12px;
            border-top-right-radius: 12px;
            border-bottom: 1px solid #232428;
        }

        #discordChannelTitle {
            font-size: 15px;
            font-weight: 700;
            color: #F2F3F5;
        }

        #discordChannelSubtext {
            font-size: 12px;
            color: #949BA4;
        }

        #chatScrollArea {
            background-color: transparent;
            border: none;
        }

        #chatScrollContent {
            background-color: transparent;
        }

        /* ====================================================================
         * BONG BÓNG CHAT MESSENGER (CHUẨN 100% THEO HÌNH ẢNH MẪU)
         * ==================================================================== */
        
        /* Tin nhắn của mình gửi đi (Bên phải): Màu đỏ san hô Messenger, bo tròn 18px */
        #myBubble {
            background-color: #D33A2C;
            color: #FFFFFF;
            border-radius: 18px;
            padding: 8px 14px;
            font-size: 13.5px;
            font-weight: 500;
        }

        /* Tin nhắn của bạn bè nhận về (Bên trái): Màu xám tối Messenger, bo tròn 18px */
        #peerBubble {
            background-color: #383A40;
            color: #F2F3F5;
            border-radius: 18px;
            padding: 8px 14px;
            font-size: 13.5px;
        }

        /* Avatar tròn nhỏ bên cạnh tin nhắn đối phương */
        #chatAvatarBadge {
            background-color: #5865F2;
            color: #FFFFFF;
            font-size: 11px;
            font-weight: bold;
            border-radius: 14px;
        }

        /* Thanh cuộn tối màu bo góc 4px */
        QScrollBar:vertical {
            background-color: #2B2D31;
            width: 8px;
            margin: 0;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical {
            background-color: #1A1B1E;
            min-height: 20px;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #111214;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        /* Khung tiến độ truyền file */
        #discordFileEmbed {
            background-color: #2B2D31;
            border-left: 4px solid #5865F2;
            border-radius: 10px;
            margin: 0 16px 10px 16px;
        }

        #discordEmbedTitle {
            font-size: 13px;
            font-weight: 600;
            color: #F2F3F5;
            background: transparent;
        }

        #discordProgressBar {
            background-color: #1E1F22;
            border: none;
            border-radius: 7px;
            height: 14px;
            text-align: center;
            font-size: 10px;
            font-weight: bold;
            color: #FFFFFF;
        }

        #discordProgressBar::chunk {
            background-color: #5865F2;
            border-radius: 7px;
        }

        /* Thanh nhập liệu nổi bo tròn 12px */
        #discordInputContainer {
            background-color: transparent;
            border-bottom-left-radius: 12px;
            border-bottom-right-radius: 12px;
        }

        #discordInputPill {
            background-color: #383A40;
            border-radius: 12px;
            padding: 2px 4px;
        }

        #discordAttachBtn {
            background-color: #4E5058;
            color: #DBDEE1;
            border: none;
            border-radius: 8px;
            padding: 8px 14px;
            font-size: 12px;
            font-weight: bold;
        }

        #discordAttachBtn:hover {
            background-color: #6D6F78;
            color: #FFFFFF;
        }

        #discordMessageInput {
            background-color: transparent;
            border: none;
            color: #DBDEE1;
            font-size: 14px;
            padding: 4px;
        }

        #discordMessageInput:focus {
            outline: none;
        }

        #discordSendBtn {
            background-color: #D33A2C; /* Màu đỏ ton-sur-ton với bong bóng chat gửi đi */
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            padding: 8px 18px;
            font-size: 13px;
            font-weight: bold;
        }

        #discordSendBtn:hover {
            background-color: #B52F23;
        }
    )";
    setStyleSheet(qss);
}

void MainWindow::initMockData() {
    // Nạp sẵn danh sách bạn bè trực tuyến
    addOrUpdatePeer("Alice", "192.168.1.15", 5001, "online", 2);
    addOrUpdatePeer("Bob", "192.168.1.20", 5002, "online", 0);
    addOrUpdatePeer("Charlie", "192.168.1.33", 5003, "away", 0);

    m_currentPeerKey = "192.168.1.15:5001";
    m_currentPeerName = "Alice";

    // Tin nhắn kiểm thử mẫu tiêu chuẩn
    appendChatMessage("Alice", "Xin chào! Ứng dụng chat LAN P2P đã kết nối thành công.", "15:28", false);
    appendChatMessage(m_myName, "Chào bạn! Mình đang kiểm thử giao diện bong bóng chat.", "15:29", true);
    appendChatMessage("Alice", "Bong bóng tin nhắn bo tròn dạng viên thuốc nhìn rất mượt.", "15:30", false);
    appendChatMessage(m_myName, "Chính xác, bo góc 18px mềm mại ôm sát nội dung.", "15:31", true);

    if (m_peerListWidget->count() > 0) {
        m_peerListWidget->setCurrentRow(0);
        onPeerSelected(m_peerListWidget->item(0));
    }
}

void MainWindow::renderMessageBubble(const QString& sender, const QString& text, const QString& timeStr, bool isMine) {
    QWidget* rowWidget = new QWidget(m_scrollContent);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(0, 1, 0, 1);
    rowLayout->setSpacing(8);

    QLabel* bubble = new QLabel(text, rowWidget);
    bubble->setWordWrap(true);
    bubble->setMaximumWidth(480);
    bubble->setTextInteractionFlags(Qt::TextSelectableByMouse);
    bubble->setAttribute(Qt::WA_StyledBackground, true); // Đảm bảo Qt render chuẩn bo góc tròn

    if (isMine) {
        // Tin nhắn gửi đi (Bên phải): Căn phải, bong bóng đỏ san hô bo tròn 18px
        rowLayout->addStretch(1);
        bubble->setObjectName("myBubble");
        rowLayout->addWidget(bubble);
    } else {
        // Tin nhắn nhận về (Bên trái): Căn trái kèm avatar tròn 28px
        QLabel* avatar = new QLabel(sender.left(1).toUpper(), rowWidget);
        avatar->setObjectName("chatAvatarBadge");
        avatar->setFixedSize(28, 28);
        avatar->setAlignment(Qt::AlignCenter);

        bubble->setObjectName("peerBubble");
        rowLayout->addWidget(avatar, 0, Qt::AlignBottom);
        rowLayout->addWidget(bubble);
        rowLayout->addStretch(1);
    }

    // Chèn vào vị trí trước stretch cuối cùng
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
    if (m_myProfileLabel) m_myProfileLabel->setText(name);
    if (m_myEndpointLabel) m_myEndpointLabel->setText("Port: " + QString::number(port));

    // Cập nhật chữ viết tắt trên avatar
    if (m_myAvatarLabel) {
        QString initials = "US";
        QStringList parts = name.split(" ", Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            initials = parts[0].left(1).toUpper() + parts[parts.size() - 1].left(1).toUpper();
        } else if (!parts.isEmpty()) {
            initials = parts[0].left(2).toUpper();
        }
        m_myAvatarLabel->setText(initials);
    }
}

void MainWindow::addOrUpdatePeer(const QString& name, const QString& ip, int port, const QString& status, int unreadCount) {
    QString key = ip + ":" + QString::number(port);
    QString displayText = QString("@ %1  (%2:%3)")
                              .arg(name)
                              .arg(ip)
                              .arg(port);

    if (unreadCount > 0) {
        displayText += QString("  [%1 NEW]").arg(unreadCount);
    }

    // Cập nhật nếu đã có trong danh sách
    for (int i = 0; i < m_peerListWidget->count(); ++i) {
        QListWidgetItem* item = m_peerListWidget->item(i);
        if (item->data(Qt::UserRole).toString() == key) {
            item->setText(displayText);
            return;
        }
    }

    // Thêm mục bạn bè mới
    QListWidgetItem* newItem = new QListWidgetItem(displayText, m_peerListWidget);
    newItem->setData(Qt::UserRole, key);
    newItem->setData(Qt::UserRole + 1, name);
    newItem->setData(Qt::UserRole + 2, status);
}

void MainWindow::removePeer(const QString& peerKey) {
    for (int i = 0; i < m_peerListWidget->count(); ++i) {
        QListWidgetItem* item = m_peerListWidget->item(i);
        if (item->data(Qt::UserRole).toString() == peerKey) {
            delete m_peerListWidget->takeItem(i);
            break;
        }
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
    if (!item) return;

    m_currentPeerKey = item->data(Qt::UserRole).toString();
    m_currentPeerName = item->data(Qt::UserRole + 1).toString();
    QString status = item->data(Qt::UserRole + 2).toString();

    m_partnerHeaderLabel->setText("@ " + m_currentPeerName);
    m_partnerStatusLabel->setText(QString("IP: %1").arg(m_currentPeerKey));
    m_messageInput->setPlaceholderText(QString("Message @%1... (Press Enter to send)").arg(m_currentPeerName));

    // Xóa sạch các bong bóng tin nhắn cũ trên màn hình (chừa lại stretch ở cuối)
    while (m_messagesLayout->count() > 1) {
        QLayoutItem* layoutItem = m_messagesLayout->takeAt(0);
        if (layoutItem->widget()) {
            delete layoutItem->widget();
        }
        delete layoutItem;
    }

    // Nạp lại toàn bộ lịch sử tin nhắn của bạn này
    const auto& history = m_chatHistories[m_currentPeerKey];
    for (const auto& msg : history) {
        renderMessageBubble(msg.sender, msg.text, msg.timeStr, msg.isMine);
    }
    scrollToBottom();
}

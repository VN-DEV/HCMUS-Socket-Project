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
 * Hiện thực giao diện đồ họa chính với Qt6 Widgets:
 * - Bố cục chia 2 cột bằng QSplitter.
 * - Khung hiển thị tin nhắn định dạng bong bóng HTML/CSS.
 * - Tương tác thời gian thực: chọn bạn chat, gửi tin nhắn, chọn và gửi file.
 * ============================================================================
 */

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("HCMUS LAN Chat & File Sharing (CSC10008)");
    resize(1000, 680);
    setMinimumSize(800, 500);

    setupUi();
    applyStyleSheet();
    initMockData();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(1);

    // ========================================================================
    // 1. CỘT TRÁI (SIDEBAR)
    // ========================================================================
    QWidget* leftPanel = new QWidget(this);
    leftPanel->setObjectName("leftPanel");
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(16, 16, 16, 16);
    leftLayout->setSpacing(12);

    // Khung hồ sơ người dùng hiện tại
    QWidget* profileBox = new QWidget(this);
    profileBox->setObjectName("profileBox");
    QVBoxLayout* profileLayout = new QVBoxLayout(profileBox);
    profileLayout->setContentsMargins(12, 12, 12, 12);
    profileLayout->setSpacing(4);

    m_myProfileLabel = new QLabel(m_myName, profileBox);
    m_myProfileLabel->setObjectName("myProfileName");

    m_myEndpointLabel = new QLabel("IP: 127.0.0.1 | Port: " + QString::number(m_myTcpPort), profileBox);
    m_myEndpointLabel->setObjectName("myProfileEndpoint");

    QHBoxLayout* statusRow = new QHBoxLayout();
    QLabel* statusPrompt = new QLabel("Status:", profileBox);
    m_statusCombo = new QComboBox(profileBox);
    m_statusCombo->addItem("Online");
    m_statusCombo->addItem("Away");
    statusRow->addWidget(statusPrompt);
    statusRow->addWidget(m_statusCombo);

    profileLayout->addWidget(m_myProfileLabel);
    profileLayout->addWidget(m_myEndpointLabel);
    profileLayout->addLayout(statusRow);
    leftLayout->addWidget(profileBox);

    // Tiêu đề danh sách bạn bè
    QLabel* peersHeader = new QLabel("ONLINE PEERS", leftPanel);
    peersHeader->setObjectName("peersHeader");
    leftLayout->addWidget(peersHeader);

    // Danh sách các peer đang hoạt động
    m_peerListWidget = new QListWidget(leftPanel);
    m_peerListWidget->setObjectName("peerListWidget");
    leftLayout->addWidget(m_peerListWidget);

    mainSplitter->addWidget(leftPanel);

    // ========================================================================
    // 2. CỘT PHẢI (CHAT & FILE TRANSFER AREA)
    // ========================================================================
    QWidget* rightPanel = new QWidget(this);
    rightPanel->setObjectName("rightPanel");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(20, 16, 20, 16);
    rightLayout->setSpacing(10);

    // Thanh Header hiển thị bạn chat hiện tại
    QWidget* chatHeaderWidget = new QWidget(rightPanel);
    chatHeaderWidget->setObjectName("chatHeaderWidget");
    QHBoxLayout* headerLayout = new QHBoxLayout(chatHeaderWidget);
    headerLayout->setContentsMargins(12, 8, 12, 8);

    m_partnerHeaderLabel = new QLabel("Select a peer to start chatting", chatHeaderWidget);
    m_partnerHeaderLabel->setObjectName("partnerHeaderLabel");

    m_partnerStatusLabel = new QLabel("", chatHeaderWidget);
    m_partnerStatusLabel->setObjectName("partnerStatusLabel");

    headerLayout->addWidget(m_partnerHeaderLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_partnerStatusLabel);
    rightLayout->addWidget(chatHeaderWidget);

    // Khung hiển thị lịch sử trò chuyện (Chat Bubbles)
    m_chatHistoryView = new QTextBrowser(rightPanel);
    m_chatHistoryView->setObjectName("chatHistoryView");
    m_chatHistoryView->setOpenExternalLinks(true);
    rightLayout->addWidget(m_chatHistoryView, 1);

    // Khung tiến độ truyền nhận tệp tin (mặc định ẩn)
    m_fileProgressContainer = new QWidget(rightPanel);
    m_fileProgressContainer->setObjectName("fileProgressContainer");
    QVBoxLayout* progressLayout = new QVBoxLayout(m_fileProgressContainer);
    progressLayout->setContentsMargins(12, 8, 12, 8);
    progressLayout->setSpacing(4);

    m_fileProgressLabel = new QLabel("Transferring file...", m_fileProgressContainer);
    m_fileProgressBar = new QProgressBar(m_fileProgressContainer);
    m_fileProgressBar->setRange(0, 100);
    m_fileProgressBar->setValue(0);
    m_fileProgressBar->setTextVisible(true);

    progressLayout->addWidget(m_fileProgressLabel);
    progressLayout->addWidget(m_fileProgressBar);
    m_fileProgressContainer->setVisible(false); // Ẩn khi chưa truyền file
    rightLayout->addWidget(m_fileProgressContainer);

    // Thanh nhập liệu phía dưới
    QHBoxLayout* inputBarLayout = new QHBoxLayout();
    inputBarLayout->setSpacing(8);

    m_sendFileBtn = new QPushButton("Send File", rightPanel);
    m_sendFileBtn->setObjectName("sendFileBtn");

    m_messageInput = new QLineEdit(rightPanel);
    m_messageInput->setObjectName("messageInput");
    m_messageInput->setPlaceholderText("Type a message... (Press Enter to send)");

    m_sendBtn = new QPushButton("Send", rightPanel);
    m_sendBtn->setObjectName("sendBtn");

    inputBarLayout->addWidget(m_sendFileBtn);
    inputBarLayout->addWidget(m_messageInput, 1);
    inputBarLayout->addWidget(m_sendBtn);
    rightLayout->addLayout(inputBarLayout);

    mainSplitter->addWidget(rightPanel);
    mainSplitter->setSizes({280, 720});

    rootLayout->addWidget(mainSplitter);

    // Kết nối các sự kiện tương tác (Signals & Slots)
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendMessageClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendMessageClicked);
    connect(m_sendFileBtn, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);
    connect(m_peerListWidget, &QListWidget::itemClicked, this, &MainWindow::onPeerSelected);
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStatusChanged);
}

void MainWindow::applyStyleSheet() {
    QString qss = R"(
        QWidget {
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13px;
            color: #24292F;
            background-color: #FFFFFF;
        }

        /* Sidebar bên trái */
        #leftPanel {
            background-color: #F6F8FA;
            border-right: 1px solid #D0D7DE;
        }

        #profileBox {
            background-color: #FFFFFF;
            border: 1px solid #D0D7DE;
            border-radius: 6px;
        }

        #myProfileName {
            font-size: 15px;
            font-weight: bold;
            color: #0969DA;
        }

        #myProfileEndpoint {
            font-size: 11px;
            color: #57606A;
        }

        #peersHeader {
            font-size: 11px;
            font-weight: bold;
            color: #57606A;
            letter-spacing: 0.5px;
            margin-top: 6px;
        }

        #peerListWidget {
            background-color: #FFFFFF;
            border: 1px solid #D0D7DE;
            border-radius: 6px;
            outline: none;
        }

        #peerListWidget::item {
            padding: 10px;
            border-bottom: 1px solid #F0F2F5;
            border-radius: 4px;
        }

        #peerListWidget::item:hover {
            background-color: #EBF5FF;
        }

        #peerListWidget::item:selected {
            background-color: #DDF4FF;
            color: #0969DA;
            font-weight: bold;
        }

        /* Cột chat bên phải */
        #rightPanel {
            background-color: #FFFFFF;
        }

        #chatHeaderWidget {
            background-color: #F6F8FA;
            border: 1px solid #D0D7DE;
            border-radius: 6px;
        }

        #partnerHeaderLabel {
            font-size: 14px;
            font-weight: bold;
            color: #1F2328;
        }

        #partnerStatusLabel {
            font-size: 12px;
            color: #1A7F37;
            font-weight: 500;
        }

        #chatHistoryView {
            background-color: #FAFBFC;
            border: 1px solid #D0D7DE;
            border-radius: 6px;
            padding: 12px;
        }

        #fileProgressContainer {
            background-color: #F6F8FA;
            border: 1px solid #D0D7DE;
            border-radius: 6px;
        }

        QProgressBar {
            border: 1px solid #D0D7DE;
            border-radius: 4px;
            text-align: center;
            background-color: #FFFFFF;
            height: 18px;
        }

        QProgressBar::chunk {
            background-color: #2DA44E;
            border-radius: 3px;
        }

        /* Thanh nhập liệu */
        #messageInput {
            border: 1px solid #D0D7DE;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 13px;
            background-color: #FFFFFF;
        }

        #messageInput:focus {
            border: 1px solid #0969DA;
        }

        #sendBtn {
            background-color: #0969DA;
            color: #FFFFFF;
            border: none;
            border-radius: 6px;
            padding: 8px 18px;
            font-weight: bold;
        }

        #sendBtn:hover {
            background-color: #0550AE;
        }

        #sendFileBtn {
            background-color: #F6F8FA;
            color: #24292F;
            border: 1px solid #D0D7DE;
            border-radius: 6px;
            padding: 8px 14px;
            font-weight: 500;
        }

        #sendFileBtn:hover {
            background-color: #EAEFF5;
        }

        QComboBox {
            border: 1px solid #D0D7DE;
            border-radius: 4px;
            padding: 3px 8px;
            background-color: #FFFFFF;
        }
    )";
    setStyleSheet(qss);
}

void MainWindow::initMockData() {
    // Nạp sẵn 3 bạn chat mẫu để giao diện trực quan ngay khi khởi chạy
    addOrUpdatePeer("Alice", "192.168.1.15", 5001, "online", 2);
    addOrUpdatePeer("Bob", "192.168.1.20", 5002, "online", 0);
    addOrUpdatePeer("Charlie", "192.168.1.33", 5003, "away", 0);

    // Mặc định chọn bạn chat đầu tiên
    if (m_peerListWidget->count() > 0) {
        m_peerListWidget->setCurrentRow(0);
        onPeerSelected(m_peerListWidget->item(0));

        // Nạp tin nhắn chào mừng mẫu
        appendChatMessage("Alice", "Chào Trí, ứng dụng Socket chat LAN sẵn sàng rồi nhé!", "15:30", false);
        appendChatMessage(m_myName, "Chào Alice! Mình đang kiểm thử giao diện Qt6.", "15:31", true);
    }
}

void MainWindow::setMyProfile(const QString& name, const QString& ip, int port) {
    m_myName = name;
    m_myTcpPort = port;
    if (m_myProfileLabel) m_myProfileLabel->setText(name);
    if (m_myEndpointLabel) m_myEndpointLabel->setText("IP: " + ip + " | Port: " + QString::number(port));
}

void MainWindow::addOrUpdatePeer(const QString& name, const QString& ip, int port, const QString& status, int unreadCount) {
    QString key = ip + ":" + QString::number(port);
    QString displayText = QString("%1 (%2:%3) [%4]")
                              .arg(name)
                              .arg(ip)
                              .arg(port)
                              .arg(status.toUpper());

    if (unreadCount > 0) {
        displayText += QString(" (%1 new)").arg(unreadCount);
    }

    // Kiểm tra xem peer đã tồn tại trong danh sách chưa
    for (int i = 0; i < m_peerListWidget->count(); ++i) {
        QListWidgetItem* item = m_peerListWidget->item(i);
        if (item->data(Qt::UserRole).toString() == key) {
            item->setText(displayText);
            return;
        }
    }

    // Nếu chưa có, thêm mới vào danh sách
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
    QString bubbleHtml;
    if (isMine) {
        // Bong bóng tin nhắn bên phải (của mình) - Màu xanh
        bubbleHtml = QString(
            "<div style='margin-bottom: 12px; text-align: right;'>"
            "  <div style='display: inline-block; max-width: 70%; text-align: left; background-color: #0969DA; color: #FFFFFF; padding: 8px 14px; border-radius: 12px 12px 2px 12px; font-size: 13px;'>"
            "    <div>%1</div>"
            "  </div>"
            "  <div style='font-size: 10px; color: #8C959F; margin-top: 3px;'>%2</div>"
            "</div>"
        ).arg(text.toHtmlEscaped()).arg(timeStr);
    } else {
        // Bong bóng tin nhắn bên trái (của đối phương) - Màu xám sáng
        bubbleHtml = QString(
            "<div style='margin-bottom: 12px; text-align: left;'>"
            "  <div style='font-size: 11px; font-weight: bold; color: #57606A; margin-bottom: 2px;'>%1</div>"
            "  <div style='display: inline-block; max-width: 70%; text-align: left; background-color: #EAEFF5; color: #1F2328; padding: 8px 14px; border-radius: 12px 12px 12px 2px; font-size: 13px;'>"
            "    <div>%2</div>"
            "  </div>"
            "  <div style='font-size: 10px; color: #8C959F; margin-top: 3px;'>%3</div>"
            "</div>"
        ).arg(sender.toHtmlEscaped()).arg(text.toHtmlEscaped()).arg(timeStr);
    }

    m_chatHtmlHistories[m_currentPeerKey] += bubbleHtml;
    m_chatHistoryView->setHtml(m_chatHtmlHistories[m_currentPeerKey]);

    // Tự động cuộn xuống dòng tin nhắn mới nhất
    QScrollBar* bar = m_chatHistoryView->verticalScrollBar();
    if (bar) {
        bar->setValue(bar->maximum());
    }
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

    // Giả lập tiến trình gửi file trực quan trên giao diện
    setFileProgress(0, QString("Sending %1 (%2 KB)...").arg(fileName).arg(fileSize / 1024));

    QTimer* timer = new QTimer(this);
    int* currentProgress = new int(0);

    connect(timer, &QTimer::timeout, this, [this, timer, currentProgress, fileName]() {
        *currentProgress += 15;
        if (*currentProgress >= 100) {
            setFileProgress(100, QString("Sent %1 successfully!").arg(fileName));
            timer->stop();
            timer->deleteLater();
            delete currentProgress;

            // Tự ẩn sau 2 giây
            QTimer::singleShot(2000, this, &MainWindow::hideFileProgress);
        } else {
            setFileProgress(*currentProgress, QString("Sending %1 (%2%)...").arg(fileName).arg(*currentProgress));
        }
    });

    timer->start(150);
}

void MainWindow::onPeerSelected(QListWidgetItem* item) {
    if (!item) return;

    m_currentPeerKey = item->data(Qt::UserRole).toString();
    QString peerName = item->data(Qt::UserRole + 1).toString();
    QString status = item->data(Qt::UserRole + 2).toString();

    m_partnerHeaderLabel->setText(QString("Chatting with: %1 (%2)").arg(peerName).arg(m_currentPeerKey));
    m_partnerStatusLabel->setText(status.toUpper());

    // Nạp lại lịch sử chat của peer này
    m_chatHistoryView->setHtml(m_chatHtmlHistories[m_currentPeerKey]);
}

void MainWindow::onStatusChanged(int index) {
    QString newStatus = (index == 0) ? "online" : "away";
    if (m_partnerStatusLabel && m_currentPeerKey.isEmpty()) {
        m_partnerStatusLabel->setText(newStatus.toUpper());
    }
}

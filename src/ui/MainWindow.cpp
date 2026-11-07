#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>

/*
 * ============================================================================
 * FILE: src/ui/MainWindow.cpp
 * NGUOI PHU TRACH: Mai Huu Tri & Nguyen Manh Tri
 * 
 * HUONG DAN HIEN THUC:
 * 1. setupUi():
 *    - Tao bo cuc 2 cot bang QSplitter.
 *    - Cot trai: Profile widget, m_statusCombo ("Online", "Away"), m_peerListWidget.
 *    - Cot phai: m_partnerHeaderLabel, m_chatHistoryView, m_fileProgressBar (an mac dinh),
 *      hang duoi cung chua m_sendFileBtn, m_messageInput, m_sendBtn.
 * 2. onSendMessageClicked():
 *    - Lay text tu m_messageInput, goi ChatManager::sendMessage().
 *    - Hien thi tin nhan len m_chatHistoryView va xoa m_messageInput.
 * 3. onSendFileClicked():
 *    - Mo QFileDialog::getOpenFileName chon file.
 *    - Goi ChatManager::sendFile(), hien thi m_fileProgressBar va cap nhat %.
 * 4. onPeerSelected(item):
 *    - Lay peerKey tu item, goi ChatManager::selectPartner(peerKey).
 *    - Load lai lich su chat len m_chatHistoryView.
 * ============================================================================
 */

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("HCMUS LAN Chat & File Sharing (CSC10008)");
    resize(960, 640);
    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // Tao khung co ban
    m_chatHistoryView = new QTextBrowser(this);
    m_messageInput = new QLineEdit(this);
    m_sendBtn = new QPushButton("Send", this);
    m_sendFileBtn = new QPushButton("Send File", this);
    m_peerListWidget = new QListWidget(this);

    // Ket noi su kien co ban
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendMessageClicked);
    connect(m_sendFileBtn, &QPushButton::clicked, this, &MainWindow::onSendFileClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendMessageClicked);
    connect(m_peerListWidget, &QListWidget::itemClicked, this, &MainWindow::onPeerSelected);

    // Layout don gian ban dau
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_peerListWidget);

    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->addWidget(m_chatHistoryView);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(m_sendFileBtn);
    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_sendBtn);
    rightLayout->addLayout(inputLayout);

    splitter->addWidget(rightPanel);
    splitter->setSizes({260, 700});

    mainLayout->addWidget(splitter);
}

void MainWindow::onSendMessageClicked() {
    // TODO: Mai Huu Tri & Nguyen Manh Tri se hien thuc gui tin nhan tu GUI tai day
}

void MainWindow::onSendFileClicked() {
    // TODO: Mai Huu Tri se hien thuc mo hop thoai chon file va goi truyen file tai day
}

void MainWindow::onPeerSelected(QListWidgetItem* item) {
    // TODO: Mai Huu Tri & Nguyen Manh Tri se hien thuc chuyen doi ban chat tai day
}

void MainWindow::onStatusChanged(int index) {
    // TODO: Nguyen Manh Tri se hien thuc doi trang thai online/away tai day
}


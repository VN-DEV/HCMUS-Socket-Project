#include "SetupDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <winsock2.h>
#include <ws2tcpip.h>

SetupDialog::SetupDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Thiết lập Người dùng - HCMUS P2P Chat");
    setFixedSize(500, 520);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Ẩn nút dấu hỏi ?

    setupUi();
    applyStyleSheet();
}

void SetupDialog::setupUi() {
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(36, 28, 36, 28);
    rootLayout->setSpacing(14);

    // 1. Tiêu đề chính và tiêu đề phụ
    QLabel* titleLabel = new QLabel("P2P LAN Chat & File Sharing", this);
    titleLabel->setObjectName("dialogTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel* subtitleLabel = new QLabel("Đồ án Mạng máy tính (CSC10008) — HCMUS", this);
    subtitleLabel->setObjectName("dialogSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(subtitleLabel);
    rootLayout->addSpacing(8);

    // 2. Nhập tên hiển thị
    QLabel* nameHeaderLabel = new QLabel("Tên hiển thị của bạn:", this);
    nameHeaderLabel->setObjectName("fieldHeader");
    rootLayout->addWidget(nameHeaderLabel);

    m_nameInput = new QLineEdit(this);
    m_nameInput->setObjectName("nameInput");
    
    // Tự động lấy tên User Windows (ví dụ LEGION) làm mặc định
    QString defaultUser = qgetenv("USERNAME");
    if (defaultUser.isEmpty()) defaultUser = "LEGION";
    m_nameInput->setText(defaultUser);
    rootLayout->addWidget(m_nameInput);

    rootLayout->addSpacing(4);

    // 3. Chọn Cổng TCP
    QLabel* portHeaderLabel = new QLabel("Cổng TCP (dùng để nhận chat và file):", this);
    portHeaderLabel->setObjectName("fieldHeader");
    rootLayout->addWidget(portHeaderLabel);

    QHBoxLayout* portLayout = new QHBoxLayout();
    portLayout->setSpacing(12);

    m_portSpinBox = new QSpinBox(this);
    m_portSpinBox->setObjectName("portSpinBox");
    m_portSpinBox->setRange(1024, 65535);
    
    // Tự động tìm cổng còn trống bắt đầu từ 5001
    int suggestedPort = findAvailablePort(5001);
    m_portSpinBox->setValue(suggestedPort);
    m_portSpinBox->setFixedWidth(130);
    m_portSpinBox->setFixedHeight(38);

    m_portStatusLabel = new QLabel("Cổng khả dụng (Sẵn sàng)", this);
    m_portStatusLabel->setObjectName("portStatusLabel");

    portLayout->addWidget(m_portSpinBox);
    portLayout->addWidget(m_portStatusLabel);
    portLayout->addStretch(1);
    rootLayout->addLayout(portLayout);

    connect(m_portSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SetupDialog::onPortChanged);

    rootLayout->addSpacing(6);

    // 4. Hộp mẹo (Tip Callout Card) bo tròn nền xanh nhạt
    QFrame* tipFrame = new QFrame(this);
    tipFrame->setObjectName("tipFrame");
    QVBoxLayout* tipLayout = new QVBoxLayout(tipFrame);
    tipLayout->setContentsMargins(16, 14, 16, 14);

    QLabel* tipText = new QLabel(
        "<span style='color: #0084FF; font-weight: bold;'>Mẹo mở nhiều người trên 1 máy:</span> "
        "<span style='color: #0077CC;'>Mỗi cửa sổ chọn một cổng khác nhau (5001, 5002, 5003...). "
        "Ứng dụng đã tự động kiểm tra và gợi ý sẵn cổng đang trống cho bạn!</span>", 
        tipFrame);
    tipText->setWordWrap(true);
    tipText->setFont(QFont("Segoe UI", 10));
    tipLayout->addWidget(tipText);

    rootLayout->addWidget(tipFrame);
    rootLayout->addStretch(1);

    // 5. Hai nút đáy (Thoát và Vào phòng Chat)
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(16);

    m_cancelBtn = new QPushButton("Thoát", this);
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setFixedSize(110, 42);

    m_joinBtn = new QPushButton("Vào phòng Chat", this);
    m_joinBtn->setObjectName("joinBtn");
    m_joinBtn->setFixedSize(150, 42);

    btnLayout->addStretch(1);
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_joinBtn);
    btnLayout->addStretch(1);

    rootLayout->addLayout(btnLayout);

    // Kết nối sự kiện nút bấm
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_joinBtn, &QPushButton::clicked, this, &SetupDialog::onJoinClicked);
}

void SetupDialog::applyStyleSheet() {
    QString qss = R"(
        QDialog {
            background-color: #FFFFFF;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        #dialogTitle {
            font-size: 22px;
            font-weight: 800;
            color: #0084FF;
        }

        #dialogSubtitle {
            font-size: 13px;
            color: #65676B;
            font-weight: 500;
        }

        #fieldHeader {
            font-size: 13.5px;
            font-weight: 600;
            color: #050505;
        }

        #nameInput {
            background-color: #FFFFFF;
            border: 2px solid #0084FF;
            border-radius: 10px;
            padding: 8px 12px;
            font-size: 14px;
            color: #050505;
            font-weight: 600;
        }

        #portSpinBox {
            background-color: #FFFFFF;
            border: 1.5px solid #CCD0D5;
            border-radius: 8px;
            padding: 4px 8px;
            font-size: 14px;
            font-weight: 700;
            color: #050505;
        }

        #portSpinBox:focus {
            border: 2px solid #0084FF;
        }

        #portStatusLabel {
            font-size: 13px;
            font-weight: 700;
            color: #0084FF;
        }

        #tipFrame {
            background-color: #EBF8FF;
            border: 1.5px solid #BAE3FF;
            border-radius: 14px;
        }

        #cancelBtn {
            background-color: #F0F2F5;
            color: #050505;
            border: none;
            border-radius: 10px;
            font-size: 13.5px;
            font-weight: 600;
        }

        #cancelBtn:hover {
            background-color: #E4E6EB;
        }

        #joinBtn {
            background-color: #0084FF;
            color: #FFFFFF;
            border: none;
            border-radius: 10px;
            font-size: 13.5px;
            font-weight: 700;
        }

        #joinBtn:hover {
            background-color: #0073E6;
        }
    )";
    setStyleSheet(qss);
}

bool SetupDialog::isPortAvailable(int port) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return false;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int res = bind(s, (sockaddr*)&addr, sizeof(addr));
    closesocket(s);
    return (res != SOCKET_ERROR);
}

int SetupDialog::findAvailablePort(int startPort) {
    for (int p = startPort; p <= 65535; ++p) {
        if (isPortAvailable(p)) return p;
    }
    return startPort;
}

void SetupDialog::onPortChanged(int port) {
    if (isPortAvailable(port)) {
        m_portStatusLabel->setText("Cổng khả dụng (Sẵn sàng)");
        m_portStatusLabel->setStyleSheet("color: #0084FF; font-weight: bold;");
        m_joinBtn->setEnabled(true);
    } else {
        m_portStatusLabel->setText("Cổng đã bị chiếm dụng!");
        m_portStatusLabel->setStyleSheet("color: #E02424; font-weight: bold;");
        m_joinBtn->setEnabled(false);
    }
}

void SetupDialog::onJoinClicked() {
    QString name = m_nameInput->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Thông báo", "Vui lòng nhập tên hiển thị của bạn!");
        return;
    }

    int port = m_portSpinBox->value();
    if (!isPortAvailable(port)) {
        QMessageBox::critical(this, "Lỗi cổng TCP", "Cổng " + QString::number(port) + " hiện đang được ứng dụng khác sử dụng!");
        return;
    }

    accept(); // Đóng hộp thoại với kết quả QDialog::Accepted
}

QString SetupDialog::getUserName() const {
    return m_nameInput->text().trimmed();
}

int SetupDialog::getTcpPort() const {
    return m_portSpinBox->value();
}
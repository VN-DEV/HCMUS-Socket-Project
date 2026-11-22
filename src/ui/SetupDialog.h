#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

class SetupDialog : public QDialog {
    Q_OBJECT

public:
    explicit SetupDialog(QWidget* parent = nullptr);

    // Lấy thông tin người dùng đã nhập
    QString getUserName() const;
    int getTcpPort() const;

private slots:
    void onPortChanged(int port);
    void onJoinClicked();

private:
    void setupUi();
    void applyStyleSheet();
    bool isPortAvailable(int port);
    int findAvailablePort(int startPort);

    QLineEdit* m_nameInput = nullptr;
    QSpinBox* m_portSpinBox = nullptr;
    QLabel* m_portStatusLabel = nullptr;
    QPushButton* m_cancelBtn = nullptr;
    QPushButton* m_joinBtn = nullptr;
};
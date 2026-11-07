#include <QApplication>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ui/MainWindow.h"

/*
 * ============================================================================
 * FILE: src/main.cpp
 * NGUOI PHU TRACH: Nguyen Manh Tri
 * 
 * MO TA:
 * Diem khoi dau (Entry Point) cua toan bo ung dung:
 * 1. Khoi dong Windows Sockets 2 (WSAStartup) truoc khi su dung socket.
 * 2. Khoi dong vong lap su kien Qt (QApplication).
 * 3. Hien thi cua so MainWindow.
 * 4. Don dep Winsock (WSACleanup) khi dong chuong trinh.
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    // 1. Khoi dong Winsock tren Windows
    WSADATA wsaData;
    int wsaRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaRes != 0) {
        return 1;
    }

    // 2. Khoi dong ung dung do hoa Qt
    QApplication app(argc, argv);
    app.setApplicationName("HCMUS LAN Chat");

    // 3. Hien thi cua so giao dien chinh
    MainWindow window;
    window.show();

    int result = app.exec();

    // 4. Giai phong tai nguyen mang Winsock khi thoat
    WSACleanup();

    return result;
}


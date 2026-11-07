#include "Protocol.h"
#include <sstream>
#include <iostream>

/*
 * ============================================================================
 * TẬP TIN: src/protocol/Protocol.cpp
 * NGƯỜI PHỤ TRÁCH: Nguyễn Mạnh Trí
 * 
 * HƯỚNG DẪN HIỆN THỰC CHI TIẾT:
 * 
 * 1. Hàm split(str, delimiter):
 *    - Cắt chuỗi theo ký tự delimiter '|' (dùng std::stringstream hoặc std::strtok).
 * 
 * 2. Hàm buildUdpHello(name, tcpPort, status):
 *    - Ghép thành chuỗi "HELLO|<name>|<tcpPort>|<status>".
 * 
 * 3. Hàm parseUdpHello(packet, outName, outPort, outStatus):
 *    - Dùng hàm split để tách theo '|'. Kiểm tra hợp lệ: tối thiểu 4 phần tử và
 *      phần tử đầu tiên phải là "HELLO".
 *    - Chuyển đổi parts[2] thành số nguyên cổng bằng std::stoi.
 * 
 * 4. Hàm buildTcpMsg(text):
 *    - Ghép thành "MSG|<length>|<text>" với length là text.length().
 * 
 * 5. Hàm buildTcpFileHeader(fileName, fileSize):
 *    - Ghép thành "FILE|<fileName>|<fileSize>|". Lưu ý có dấu '|' ở cuối để phân
 *      tách với các byte dữ liệu nhị phân tiếp theo.
 * 
 * 6. Hàm buildTcpAuth(name, tcpPort):
 *    - Ghép thành "AUTH|<name>|<tcpPort>|".
 * 
 * 7. Hàm buildTcpBye():
 *    - Trả về "BYE|".
 * ============================================================================
 */

namespace Protocol {

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }
    return tokens;
}

std::string buildUdpHello(const std::string& name, int tcpPort, const std::string& status) {
    return "HELLO|" + name + "|" + std::to_string(tcpPort) + "|" + status;
}

bool parseUdpHello(const std::string& packet, std::string& outName, int& outPort, std::string& outStatus) {
    auto parts = split(packet, '|');
    if (parts.size() < 4 || parts[0] != "HELLO") {
        return false;
    }
    outName = parts[1];
    try {
        outPort = std::stoi(parts[2]);
    } catch (...) {
        return false;
    }
    outStatus = parts[3];
    return true;
}

std::string buildTcpMsg(const std::string& text) {
    return "MSG|" + std::to_string(text.length()) + "|" + text;
}

std::string buildTcpFileHeader(const std::string& fileName, uint64_t fileSize) {
    return "FILE|" + fileName + "|" + std::to_string(fileSize) + "|";
}

std::string buildTcpAuth(const std::string& name, int tcpPort) {
    return "AUTH|" + name + "|" + std::to_string(tcpPort) + "|";
}

std::string buildTcpBye() {
    return "BYE|";
}

} // namespace Protocol

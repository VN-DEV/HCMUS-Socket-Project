#include "Protocol.h"
#include <sstream>
#include <iostream>

/*
 * ============================================================================
 * FILE: src/protocol/Protocol.cpp
 * NGUOI PHU TRACH: Nguyen Manh Tri
 * 
 * HUONG DAN HIEN THUC:
 * - Ham split: Cat chuoi dua vao ky tu delimiter '|' (dung stringstream hoac strtok).
 * - Ham buildUdpHello: Ghep chuoi theo format "HELLO|<name>|<tcpPort>|<status>".
 * - Ham parseUdpHello: Dung ham split kiem tra 4 phan, kiem tra "HELLO" va chuyen port thanh int.
 * - Ham buildTcpMsg: Ghep chuoi theo format "MSG|<length>|<text>".
 * - Ham buildTcpFileHeader: Ghep chuoi theo format "FILE|<fileName>|<fileSize>|".
 * - Ham buildTcpAuth: Ghep chuoi theo format "AUTH|<name>|<tcpPort>|".
 * - Ham buildTcpBye: Tra ve chuoi "BYE|".
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
    // TODO: Ghichu cho thanh vien implement: ghep thanh "HELLO|<name>|<tcpPort>|<status>"
    return "HELLO|" + name + "|" + std::to_string(tcpPort) + "|" + status;
}

bool parseUdpHello(const std::string& packet, std::string& outName, int& outPort, std::string& outStatus) {
    // TODO: Ghichu cho thanh vien implement: dung split tach theo '|', kiem tra dung 4 phan va phan dau la "HELLO"
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
    // TODO: Ghichu cho thanh vien implement: tao tien to do dai "MSG|<length>|<text>"
    return "MSG|" + std::to_string(text.length()) + "|" + text;
}

std::string buildTcpFileHeader(const std::string& fileName, uint64_t fileSize) {
    // TODO: Ghichu cho thanh vien implement: tao tien to file "FILE|<fileName>|<fileSize>|"
    return "FILE|" + fileName + "|" + std::to_string(fileSize) + "|";
}

std::string buildTcpAuth(const std::string& name, int tcpPort) {
    // TODO: Ghichu cho thanh vien implement: tao goi bat tay "AUTH|<name>|<tcpPort>|"
    return "AUTH|" + name + "|" + std::to_string(tcpPort) + "|";
}

std::string buildTcpBye() {
    // TODO: Ghichu cho thanh vien implement: tra ve "BYE|"
    return "BYE|";
}

} // namespace Protocol

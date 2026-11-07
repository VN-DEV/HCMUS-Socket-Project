#pragma once

#include <string>
#include <vector>
#include <cstdint>

/*
 * ============================================================================
 * FILE: src/protocol/Protocol.h
 * NGUOI PHU TRACH: Nguyen Manh Tri
 * 
 * MO TA:
 * File nay khai bao cac ham dong goi (serialize) va giai ma (parse/deserialize)
 * du lieu theo chuan giao thuc tang ung dung quy dinh trong de bai.
 * 
 * DINH DANG GIAO THUC (THEO DE BAI CSC10008):
 * 1. UDP Presence Broadcast:
 *    HELLO|<name>|<tcp_port>|<status>
 *    Vi du: HELLO|Alice|5001|online
 * 
 * 2. TCP Handshake / Authentication:
 *    AUTH|<name>|<tcp_port>|
 *    Vi du: AUTH|Alice|5001|
 * 
 * 3. TCP Message Framing:
 *    MSG|<length>|<utf-8 text>
 *    Vi du: MSG|5|Hello
 * 
 * 4. TCP File Header Framing:
 *    FILE|<filename>|<size_in_bytes>|
 *    Vi du: FILE|slides.pdf|2400000|
 * 
 * 5. TCP Disconnect:
 *    BYE|
 * ============================================================================
 */

namespace Protocol {

    // Ham tien ich: Cat chuoi str theo ky tu phan cach delimiter (vi du '|')
    std::vector<std::string> split(const std::string& str, char delimiter);

    // Giao thuc UDP: Tao chuoi HELLO|<name>|<port>|<status>
    std::string buildUdpHello(const std::string& name, int tcpPort, const std::string& status);

    // Giao thuc UDP: Boc tach chuoi packet nhan duoc thanh cac truong du lieu
    bool parseUdpHello(const std::string& packet, std::string& outName, int& outPort, std::string& outStatus);

    // Giao thuc TCP: Tao goi tin nhan dang MSG|<len>|<text>
    std::string buildTcpMsg(const std::string& text);

    // Giao thuc TCP: Tao header gui file dang FILE|<name>|<size>|
    std::string buildTcpFileHeader(const std::string& fileName, uint64_t fileSize);

    // Giao thuc TCP: Tao goi tin bat tay dinh danh AUTH|<name>|<port>|
    std::string buildTcpAuth(const std::string& name, int tcpPort);

    // Giao thuc TCP: Tao goi tin bao ngat ket noi BYE|
    std::string buildTcpBye();

} // namespace Protocol

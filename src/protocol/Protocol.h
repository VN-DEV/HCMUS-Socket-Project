#pragma once

#include <string>
#include <vector>
#include <cstdint>

/*
 * ============================================================================
 * TẬP TIN: src/protocol/Protocol.h
 * NGƯỜI PHỤ TRÁCH: Nguyễn Mạnh Trí
 * 
 * MÔ TẢ:
 * Khai báo các hàm đóng gói (serialize) và giải mã (parse/deserialize)
 * các gói tin tầng ứng dụng theo quy chuẩn đề bài CSC10008.
 * 
 * QUY CHUẨN GIAO THỨC (THEO MỤC 5.2 ĐỀ BÀI):
 * 1. UDP Presence Broadcast:
 *    HELLO|<name>|<tcp_port>|<status>
 *    Ví dụ: HELLO|Alice|5001|online
 * 
 * 2. TCP Authentication Handshake:
 *    AUTH|<name>|<tcp_port>|
 *    Ví dụ: AUTH|Alice|5001|
 * 
 * 3. TCP Message Framing:
 *    MSG|<length>|<utf-8 text>
 *    Ví dụ: MSG|5|Hello
 * 
 * 4. TCP File Header Framing:
 *    FILE|<filename>|<size_in_bytes>|
 *    Ví dụ: FILE|slides.pdf|2048000|
 * 
 * 5. TCP Disconnect:
 *    BYE|
 * ============================================================================
 */

namespace Protocol {

    // Hàm tiện ích: Cắt chuỗi str thành danh sách các chuỗi con theo ký tự phân cách delimiter (ví dụ '|')
    std::vector<std::string> split(const std::string& str, char delimiter);

    // Giao thức UDP: Ghép thông tin thành chuỗi HELLO|<name>|<port>|<status>
    std::string buildUdpHello(const std::string& name, int tcpPort, const std::string& status);

    // Giao thức UDP: Bóc tách chuỗi nhận được từ mạng thành các trường dữ liệu
    bool parseUdpHello(const std::string& packet, std::string& outName, int& outPort, std::string& outStatus);

    // Giao thức TCP: Tạo tiền tố độ dài MSG|<len>|<text>
    std::string buildTcpMsg(const std::string& text);

    // Giao thức TCP: Tạo header gửi file FILE|<name>|<size>|
    std::string buildTcpFileHeader(const std::string& fileName, uint64_t fileSize);

    // Giao thức TCP: Tạo gói bắt tay định danh khi vừa kết nối AUTH|<name>|<port>|
    std::string buildTcpAuth(const std::string& name, int tcpPort);

    // Giao thức TCP: Tạo gói báo ngắt kết nối an toàn BYE|
    std::string buildTcpBye();

} // namespace Protocol

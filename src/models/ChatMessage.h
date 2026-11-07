#pragma once

#include <string>
#include <chrono>

/*
 * ============================================================================
 * TẬP TIN: src/models/ChatMessage.h
 * NGƯỜI PHỤ TRÁCH: Mai Hữu Trí & Nguyễn Mạnh Trí
 * 
 * MÔ TẢ:
 * Cấu trúc dữ liệu đại diện cho một tin nhắn trao đổi giữa các peer.
 * 
 * CÁC TRƯỜNG DỮ LIỆU:
 * 1. sender: Tên người gửi tin nhắn.
 * 2. text: Nội dung tin nhắn văn bản.
 * 3. timestamp: Thời điểm gửi tin nhắn.
 * 4. isMine: Đánh dấu tin nhắn do chính mình gửi (true) hay nhận từ bạn chat (false).
 * ============================================================================
 */

struct ChatMessage {
    std::string sender;
    std::string text;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
    bool isMine = false;

    ChatMessage() = default;
    ChatMessage(const std::string& s, const std::string& t, bool mine = false)
        : sender(s), text(t), timestamp(std::chrono::system_clock::now()), isMine(mine) {}
};

#pragma once

#include <string>
#include <chrono>

/*
 * ============================================================================
 * FILE: src/models/ChatMessage.h
 * NGUOI PHU TRACH: Mai Huu Tri / Nguyen Manh Tri
 * 
 * MO TA:
 * File nay dinh nghia cau truc mot dong tin nhan chat duoc truyen nhan qua mang.
 * 
 * CAC TRUONG DU LIEU CAN QUAN LY:
 * 1. sender: Ten nguoi gui tin nhan.
 * 2. text: Noi dung tin nhan van ban.
 * 3. timestamp: Thoi diem gui tin nhan.
 * 4. isMine: true neu tin nhan do chinh minh gui, false neu nhan tu ban chat.
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

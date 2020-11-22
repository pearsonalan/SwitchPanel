#include <cstring>
#include <iostream>
#include <memory>

#include "Protocol.h"

constexpr char kHelloMessage[] = "HELO";
constexpr char kSwitchMessage[] = "SW";
constexpr char kLedMessage[] = "LD";

std::string ProtocolMessage::toString() const {
    switch (message_type_) {
    case MessageType::Hello:
        return kHelloMessage;    
    case MessageType::SwitchOn:
        return std::string(kSwitchMessage) + std::string(1, '0' + component_) + "1";
    case MessageType::SwitchOff:
        return std::string(kSwitchMessage) + std::string(1, '0' + component_) + "0";
    case MessageType::LedOn:
        return std::string(kLedMessage) + std::string(1, '0' + component_) + "1";
    case MessageType::LedOff:
        return std::string(kLedMessage) + std::string(1, '0' + component_) + "0";
    default:
        return std::string();
    }
}

ProtocolResult Protocol::addBytes(const char* data, int len) {
    if (input_buffer_len_ + len > kProtocolInputBufferLen) {
        return ProtocolResult::BufferFull;
    }
    memcpy(input_buffer_ + input_buffer_len_, data, len);
    input_buffer_len_ += len;
    return ProtocolResult::Ok;
}

bool Protocol::hasMessage() {
    return memchr(input_buffer_, '\n', input_buffer_len_) != nullptr;
}

ProtocolMessage Protocol::getMessage() {
    ProtocolMessage message;
    // Look for a newline which marks the end of the message
    char* message_end = static_cast<char*>(memchr(input_buffer_, '\n', input_buffer_len_));
    if (message_end != nullptr) {
        // Calculate the length of the message. This will not include the '\n' character.
        int message_len = message_end - input_buffer_;
        message = parseMessage(message_len);
        // Discard what was parsed out of the buffer (including the '\n')
        discardMessage(message_len + 1);
    }
    return message;
}

ProtocolMessage Protocol::parseMessage(int len) {
    ProtocolMessage message;
    
    // std::cout << "Parsing message of length " << len << std::endl;
    if (len == 4 && memcmp(input_buffer_, kHelloMessage, 4) == 0) {
        message = ProtocolMessage(MessageType::Hello);
    }
    else if (len == 4 &&
        memcmp(input_buffer_, kSwitchMessage, 2) == 0 &&
        isdigit(input_buffer_[2]) &&
        (input_buffer_[3] == '0' || input_buffer_[3] == '1')) {
        message = ProtocolMessage(
                      (input_buffer_[3] == '0') ? MessageType::SwitchOff : MessageType::SwitchOn,
                      input_buffer_[2] - '0');
    }
    else if (len == 4 &&
        memcmp(input_buffer_, kLedMessage, 2) == 0 &&
        isdigit(input_buffer_[2]) &&
        (input_buffer_[3] == '0' || input_buffer_[3] == '1')) {
        message = ProtocolMessage(
                      (input_buffer_[3] == '0') ? MessageType::LedOff : MessageType::LedOn,
                      input_buffer_[2] - '0');
    }


    return message;
}

void Protocol::discardMessage(int len) {
    if (input_buffer_len_ > len) {
        memmove(input_buffer_, input_buffer_ + len, input_buffer_len_ - len);
    }
    input_buffer_len_ -= len;
}
#include <iostream>

#include "Protocol.h"

#define BEGIN_TEST() bool test_ok__ = true

#define EXPECT(x) if (!(x)) { \
    std::cerr << "Failed expectation !(" << #x << ") in " << __FUNCTION__ << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
    test_ok__ = false; \
}

#define ASSERT(x) if (!(x)) { \
    std::cerr << "Failed assertion !(" << #x << ") in " << __FUNCTION__ << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
    exit(-1); \
}

#define END_TEST() { \
   std::cerr << __FUNCTION__  << ": " << (test_ok__ ? "OK" : "FAILED") << std::endl; \
}

void TestMessageToString() {
    BEGIN_TEST();
    EXPECT(ProtocolMessage().toString() == "");
    EXPECT(ProtocolMessage(MessageType::Hello).toString() == "HELO");
    EXPECT(ProtocolMessage(MessageType::SwitchOn, 1).toString() == "SW11");
    EXPECT(ProtocolMessage(MessageType::SwitchOff, 4).toString() == "SW40");
    EXPECT(ProtocolMessage(MessageType::LedOn, 0).toString() == "LD01");
    EXPECT(ProtocolMessage(MessageType::LedOff, 0).toString() == "LD00");
    END_TEST();
}

void TestNoMessage() {
    BEGIN_TEST();
    Protocol protocol;

    EXPECT(!protocol.hasMessage());
    ProtocolMessage message = protocol.getMessage();

    EXPECT(message.message_type() == MessageType::Invalid);
    END_TEST();
}

void TestHasMessage() {
    BEGIN_TEST();
    Protocol protocol;

    EXPECT(!protocol.hasMessage());
    protocol.addBytes("SW", 2);
    EXPECT(!protocol.hasMessage());
    protocol.addBytes("01", 2);
    EXPECT(!protocol.hasMessage());
    protocol.addBytes("\n", 1);
    EXPECT(protocol.hasMessage());

    END_TEST();
}

void TestParseMessage() {
    BEGIN_TEST();
    Protocol protocol;

    ProtocolResult result = protocol.addBytes("SW00\n", 5);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());
    ProtocolMessage message = protocol.getMessage();

    EXPECT(message.message_type() == MessageType::SwitchOff);
    EXPECT(message.component() == 0);

    EXPECT(!protocol.hasMessage());

    END_TEST();
}

void TestPartialMessages() {
    BEGIN_TEST();
    Protocol protocol;
    ProtocolResult result;
    ProtocolMessage message;

    result = protocol.addBytes("HEL", 3);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("O\n", 2);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::Hello);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("SW2", 3);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("1\nSW20\n", 7);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::SwitchOn);
    EXPECT(message.component() == 2);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::SwitchOff);
    EXPECT(message.component() == 2);
    EXPECT(!protocol.hasMessage());

    END_TEST();
}

void TestMultipleMessages() {
    BEGIN_TEST();
    Protocol protocol;
    ProtocolResult result;
    ProtocolMessage message;

    result = protocol.addBytes("HELO\nSW11\nLD91\n", 15);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::Hello);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::SwitchOn);
    EXPECT(message.component() == 1);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::LedOn);
    EXPECT(message.component() == 9);
    EXPECT(!protocol.hasMessage());

    END_TEST();
}

int main() {
    TestMessageToString();
    TestNoMessage();
    TestHasMessage(); 
    TestParseMessage();
    TestPartialMessages();
    TestMultipleMessages();
}
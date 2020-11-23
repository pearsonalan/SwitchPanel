#ifndef _PanelClient_h_
#define _PanelClient_h_

#include "Protocol.h"

class PanelClient;

class Switch {
public:
    Switch(int id, int pin, PanelClient* panel_client);
    void poll();

    int id() const { return id_; }
    int state() const { return state_; }

private:
    int id_;
    int pin_;
    int state_ = 0;
    PanelClient* panel_client_ = nullptr;
};

class LED {
public:
    LED(int id, int pin, PanelClient* panel_client);
    void setState(int state);

    int id() const { return id_; }
    int state() const { return state_; }
    
private:
    int id_;
    int pin_;
    int state_ = 0;
};

constexpr int kLEDCount = 8;
constexpr int kSwitchCount = 8;

class PanelClient {
public:
    void addSwitch(Switch* s);
    void addLED(LED* led);

    void switchUpdated(int id, int state);

    void processMessage(const ProtocolMessage& message);
    bool getMessage(ProtocolMessage* message);

    bool isConnected() const { return connected_; }

private:
    bool connected_ = false;
    long last_helo_timestamp_ = 0;
    long last_status_update_ = 0;

    Switch* switches_[kSwitchCount] = { 0 };
    LED* leds_[kLEDCount] = { 0 };
};

#endif
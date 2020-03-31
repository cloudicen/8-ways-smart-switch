#include "esphome.h"

#define QUERY_COMMAND 0x18
#define SET_COMMAND 0x29
#define ACK_COMMAND 0X33

class MyCustomSwitch : public Component, public Switch
{
public:
    void setup() override
    {
        // This will be called by App.setup()
    }
    void write_state(bool state) override
    {
        // This will be called every time the user requests a state change.
        publish_state(state); //do nothing, just change the state.
    }
};

class MyCustomComponent : public PollingComponent, public UARTDevice
{
public:
    uint8_t state = 0;         // store the state of all the switches
    uint8_t num_of_switch = 0; // number of switches

    MyCustomSwitch *switches = nullptr; // array of instances of switches

    uint8_t buf[2] = {0}; // buffer for reading bytes from uart

    MyCustomComponent(uint8_t _num_of_switch, UARTComponent *parent) : PollingComponent(300), UARTDevice(parent)
    {
        num_of_switch = _num_of_switch;
        switches = new MyCustomSwitch[num_of_switch];
    }

    void setup() override
    {
        // nothing to do here
    }

    void update() override
    {
        check_actual_state(); // check if switches being toggled through physical switches
        check_web_state();    // check if switches being toggled through frontend
    }

    void check_actual_state() // check if switches being toggled through physical switches
    {
        write(QUERY_COMMAND); // send command to query the actual state of switches
        delay(15);            // waiting for data transmitting complete
        while (available())   // read data
        {
            buf[1] = read();
            delay(2);
            buf[2] = read();
            delay(2);
        }
        if (buf[1] == ACK_COMMAND) // is the first byte equals to 'ACK_COMMAND'?
        {
            if (state != buf[2]) //if current state is different from the state reported by lower machine, meaning that switches' state is change by physical switch
            {
                ESP_LOGD("custom", "actual state chaned！ pre state: %d, new state: %d", state, buf[2]); //logging

                state = buf[2]; // set new state

                uint8_t stateToSet = state; // synchronize state to frontend
                for (int i = num_of_switch - 1; i >= 0; i--)
                {
                    switches[i].publish_state((bool)(stateToSet % 2));
                    delay(15);
                    stateToSet = stateToSet >> 1;
                }
            }
        }
    }

    void check_web_state() // check if switches being toggled through frontend
    {
        // read states of switches in frontend
        uint8_t nowState = 0;
        for (int i = 0; i < num_of_switch; i++)
        {
            nowState += (int)switches[i].state;
            if (i != num_of_switch - 1)
            {
                nowState = nowState << 1;
            }
        }
        if (nowState != state) // if not equal, means user requests a state change through frontend
        {
            ESP_LOGD("custom", "web state changed！ pre state: %d, new state: %d", state, nowState);

            state = nowState; // set new state

            write(SET_COMMAND); // send 'SET COMMAND' to lower machine for a state changing
            write(state);       // send state that need to be set
        }
    }
};
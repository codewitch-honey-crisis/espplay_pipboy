#include <Arduino.h>
#include <Wire.h>
#include "input.hpp"

using namespace arduino;
static uint8_t gamepad_value=0;
static bool gamepad_initialized = false;
static void gamepad_init() {
    if(!gamepad_initialized) {
        Wire.begin(21,22,100*1000);
        gamepad_initialized = true;
    }
}
static void read_gamepad() {
    static uint32_t ts = 0;
    if(millis()>ts+13) {
        gamepad_init();
        uint8_t data;
        Wire.requestFrom(0x20,1);
        if(Wire.available()) {
            gamepad_value = Wire.read();
            ts = millis();
        }
    }
}
gamepad_button::gamepad_button(int which) : m_pressed(false),m_which(which),m_callback(nullptr)  {

}
bool gamepad_button::initialized() const {
    return gamepad_initialized;
}
void gamepad_button::initialize() {
    gamepad_init();
}
void gamepad_button::deinitialize() {

}
bool gamepad_button::pressed() {
    read_gamepad();
    uint8_t val = gamepad_value ;
    bool pressed = ((1<<m_which)&val)==0;
    return pressed;
}
void gamepad_button::update() {
    bool pressed = this->pressed();
    if(m_callback!=nullptr) {
        if(pressed!=m_pressed) {
            m_pressed = pressed;
            m_callback(m_pressed,m_state);
            return;
        }
    } 
    m_pressed = pressed;
}
void gamepad_button::on_pressed_changed(button_on_pressed_changed callback, void* state) {
    m_callback = callback;
    m_state = state;
}

button_l_raw_t button_l_raw;
button_r_raw_t button_r_raw;
gp_button_t button_left_raw(GAMEPAD_LEFT);
gp_button_t button_right_raw(GAMEPAD_RIGHT);

button_t button_l(button_l_raw);
button_t button_r(button_r_raw);
button_t button_left(button_left_raw);
button_t button_right(button_right_raw);

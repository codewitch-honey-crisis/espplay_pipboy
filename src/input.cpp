#include <Arduino.h>
#include <Wire.h>
#include "input.hpp"
#define CHECK_BIT(bit,val) ((1<<bit)&val)
using namespace arduino;

using button_l_t = int_button<36,10,true>;
using button_r_t = int_button<34,10,true>;
using gp_button_t = gamepad_button;

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
gamepad_button::gamepad_button(int which) : m_initialized(false), m_pressed(false),m_which(which),m_callback(nullptr)  {

}
bool gamepad_button::initialized() const {
    return m_initialized;
}
void gamepad_button::initialize() {
    if(!initialized()) {
        gamepad_init();
        m_initialized=true;
    }
}
void gamepad_button::deinitialize() {
    m_initialized = false;
}
bool gamepad_button::pressed() {
    read_gamepad();
    uint8_t val = gamepad_value ;
    bool pressed = CHECK_BIT(m_which,val)==0;
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
static button_l_t button_l_raw;
static button_r_t button_r_raw;
static gp_button_t button_left_raw(GAMEPAD_LEFT);
static gp_button_t button_right_raw(GAMEPAD_RIGHT);
static gp_button_t button_up_raw(GAMEPAD_UP);
static gp_button_t button_down_raw(GAMEPAD_DOWN);

button_t button_l(button_l_raw);
button_t button_r(button_r_raw);
button_t button_left(button_left_raw);
button_t button_right(button_right_raw);
button_t button_up(button_up_raw);
button_t button_down(button_down_raw);

void input_initialize() {
    button_l.initialize();
    button_r.initialize();
    button_left.initialize();
    button_right.initialize();
    button_up.initialize();
    button_down.initialize();
}
void input_update() {
    button_l.update();
    button_r.update();
    button_left.update();
    button_right.update();
    button_up.update();
    button_down.update();
}
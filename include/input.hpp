#pragma once
#include <button.hpp>
enum
{
    GAMEPAD_START = 0,
    GAMEPAD_SELECT,
    GAMEPAD_UP,
    GAMEPAD_DOWN,
    GAMEPAD_LEFT,
    GAMEPAD_RIGHT,
    GAMEPAD_A,
    GAMEPAD_B,
    GAMEPAD_MAX
};
class gamepad_button : public arduino::button {
    bool m_pressed;
    int m_which;
    arduino::button_on_pressed_changed m_callback;
    void* m_state;
public:
    gamepad_button(int which);
    virtual bool initialized() const override;
    virtual void initialize() override;
    virtual void deinitialize() override;
    virtual bool pressed() override;
    virtual void update() override;
    virtual void on_pressed_changed(arduino::button_on_pressed_changed callback, void* state = nullptr) override;
};
using button_l_raw_t = arduino::int_button<36,10,true>;
using button_r_raw_t = arduino::int_button<34,10,true>;
using button_t = arduino::multi_button;
using gp_button_t = gamepad_button;

extern button_l_raw_t button_l_raw;
extern button_r_raw_t button_r_raw;
extern gp_button_t button_left_raw;
extern gp_button_t button_right_raw;
extern button_t button_l;
extern button_t button_r;
extern button_t button_left;
extern button_t button_right;

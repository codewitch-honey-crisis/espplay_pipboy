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
    bool m_initialized;
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
using button_t = arduino::multi_button;

extern button_t button_l;
extern button_t button_r;
extern button_t button_left;
extern button_t button_right;
extern button_t button_up;
extern button_t button_down;

extern void input_initialize();
extern void input_update();
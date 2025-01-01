#pragma once
#include <gfx.hpp>
#include <uix.hpp>

using color_t = gfx::color<gfx::rgb_pixel<16>>;
using uix_color_t = gfx::color<gfx::rgba_pixel<32>>;

extern void ui_title_screen(uix::display& disp);
extern void ui_pip_screen(uix::display& disp);
extern void ui_pip_update();
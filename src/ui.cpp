#include "ui.hpp"
#include "lcd_config.h"
#include "input.hpp"
#define FALLOUT_BOY_JPG_IMPLEMENTATION
#include "assets/fallout_boy_jpg.h"
#define SHARETECH_REGULAR_TTF_IMPLEMENTATION
#include "assets/ShareTech_Regular_ttf.h"
#define PIPBOY_IMPLEMENTATION
#include "assets/pipboy.h"
#define SIZEOFARR(x) (sizeof(x)/sizeof(*x))
using namespace gfx;
using namespace uix;

using screen_t = uix::screen<gfx::rgb_pixel<16>>;
using image_t = uix::image_box<screen_t::control_surface_type>;
using label_t = uix::label<screen_t::control_surface_type>;
using painter_t = uix::painter<screen_t::control_surface_type>;

static screen_t::pixel_type vdark_green(0,23,0);
static screen_t::pixel_type vgreen(0,63,0);
static rgba_pixel<32> uix_vgreen(0,255,0,255);
static const_buffer_stream title_stream(fallout_boy_jpg,sizeof(fallout_boy_jpg));
static jpg_image title_jpg(title_stream);

static const_buffer_stream main_font_stream(ShareTech_Regular_ttf,sizeof(ShareTech_Regular_ttf));
static tt_font title_font(main_font_stream,40,font_size_units::px);

static const_buffer_stream pip_boy_stream(pipboy,sizeof(pipboy));

template<typename ControlSurfaceType>
class pip_boy_stat: public uix::canvas_control<ControlSurfaceType> {
    using base_type = canvas_control<ControlSurfaceType>;
public:
    using type = pip_boy_stat;
    using control_surface_type = ControlSurfaceType;
private:
    matrix m_xfrm;
public:
    pip_boy_stat() : base_type() {
        m_xfrm = matrix::create_identity();
    }
protected:
    virtual void on_after_resize() override {
        base_type::on_after_resize();
        sizef sdim;
        pip_boy_stream.seek(0);
        canvas::svg_dimensions(pip_boy_stream,&sdim);
        rectf bounds(0,0,this->dimensions().width-1,this->dimensions().height-1);    
        float ar =sdim.aspect_ratio();
        if(ar>1.f) {
            bounds.y2 = bounds.y2/ar;
        } else {
            bounds.x2 = bounds.x2*ar;
        }
        bounds.center_inplace(rectf(0,0,this->dimensions().width-1,this->dimensions().height-1));
        pip_boy_stream.seek(0);
        m_xfrm = matrix::create_fit_to(sdim,bounds);
        m_xfrm.translate_inplace(0,-50);
    }
    virtual void on_paint(canvas& destination, const srect16& clip) override {
        pip_boy_stream.seek(0);
        gfx_result res = destination.render_svg(pip_boy_stream,m_xfrm);
        if(res!=gfx_result::success) {
            printf("render error: %d\n",(int)res);
        }
    }
};
using pip_boy_stat_t = pip_boy_stat<screen_t::control_surface_type>;

static pip_boy_stat_t pip_stat;

static screen_t title_screen;
static image_t title_image;
static label_t title_loading_label;



static screen_t pip_screen;
static tt_font pip_top_font(main_font_stream,20,font_size_units::px);
static constexpr const size_t pip_boy_top_labels_size = 5;
static label_t pip_top_labels[pip_boy_top_labels_size];
static int16_t pip_top_label_starts[pip_boy_top_labels_size];
static int16_t pip_top_label_ends[pip_boy_top_labels_size];
static size_t pip_sub_menu_indices[pip_boy_top_labels_size];
static const char* pip_top_captions[] {
    "STAT",
    "INV",
    "DATA",
    "MAP",
    "RADIO"
};

static const char* pip_stat_sub_menu[] {
    "STATUS",
    "SPECIAL",
    "PERKS"
};
static constexpr const size_t pip_stat_sub_menu_size = SIZEOFARR(pip_stat_sub_menu);

static const char* pip_inv_sub_menu[] {
    "ALL",
    "AMMO",
    "WEAPONS",
    "APPAREL",
    "AID",
    "MISC",
    "JUNK",
    "MODS"
};
static constexpr const size_t pip_inv_sub_menu_size = SIZEOFARR(pip_inv_sub_menu);

static const char* pip_data_sub_menu[] {
    "GENERAL",
    "QUEST",
    "COMBAT",
    "CRAFTING",
    "CRIME"
};
static constexpr const size_t pip_data_sub_menu_size = SIZEOFARR(pip_data_sub_menu);

static const char* pip_map_sub_menu[] {
    "WORLD",
    "LOCAL"
};

static constexpr const size_t pip_map_sub_menu_size = SIZEOFARR(pip_map_sub_menu);

static const char* pip_radio_sub_menu[0];

static constexpr const size_t pip_radio_sub_menu_size = 0;

static const char** pip_sub_menus[] = {
    pip_stat_sub_menu,
    pip_inv_sub_menu,
    pip_data_sub_menu,
    pip_map_sub_menu,
    pip_radio_sub_menu
};
static const size_t pip_sub_menu_sizes[] = {
    pip_stat_sub_menu_size,
    pip_inv_sub_menu_size,
    pip_data_sub_menu_size,
    pip_map_sub_menu_size,
    pip_radio_sub_menu_size
};

static constexpr const size_t pip_sub_menus_size = SIZEOFARR(pip_sub_menus);

static painter_t top_menu_painter;
static painter_t sub_menu_painter;

size_t top_menu_index = (size_t)-1;
size_t sub_menu_index = (size_t)-1;

static void sub_menu_painter_on_paint(painter_t::control_surface_type& destination, const srect16& clip, void* state) {
    static char temp[256] = {0};
    size16 tsz;
    text_info ti("",pip_top_font);
    if(sub_menu_index==((size_t)-1)) return;
    if(sub_menu_index>=pip_sub_menu_sizes[top_menu_index]) return;
    int16_t x = pip_top_label_starts[top_menu_index];
    if(sub_menu_index>0) {
        const char* first_txt = pip_sub_menus[top_menu_index][sub_menu_index-1];
        strcpy(temp,first_txt);
        strcat(temp,"M");
        ti.text_sz(temp);
        ti.text_font->measure(-1,ti,&tsz);
        ti.text_sz(first_txt);
        draw::text(destination,((srect16)tsz.bounds()).offset(x-tsz.width,0),ti,vdark_green);
    }
    temp[0]=0;
    const char* txt = pip_sub_menus[top_menu_index][sub_menu_index];
    strcpy(temp,txt);
    strcat(temp,"M");
    ti.text_sz(temp);
    ti.text_font->measure(-1,ti,&tsz);
    ti.text_sz(txt);
    draw::text(destination,srect16(x,0,x+tsz.width,tsz.height-1),ti,vgreen);
    //if(sub_menu_index<pip_sub_menu_sizes[top_menu_index]-1) {
    int c = 0;
    for(int i = sub_menu_index+1;i<pip_sub_menu_sizes[top_menu_index];++i) {
        x+=tsz.width;
        if(x>=LCD_WIDTH) {
            break;
        }
        if(c>1) {
            break;
        }
        temp[0]=0;
        const char* last_txt = pip_sub_menus[top_menu_index][i];
        strcpy(temp,last_txt);
        strcat(temp,"M");
        ti.text_sz(temp);
        ti.text_font->measure(-1,ti,&tsz);
        if(x+tsz.width>LCD_WIDTH) {
            break;
        }
        ti.text_sz(last_txt);
        draw::text(destination,srect16(x,0,x+tsz.width,tsz.height-1),ti,vdark_green);
        ++c;
    }
}

static void top_menu_painter_on_paint(painter_t::control_surface_type& destination, const srect16& clip, void* state) {
    const size_t index = top_menu_index;
    srect16 sr=(srect16)destination.bounds();
    int16_t y = sr.y1+sr.height()*.6667f;
    draw::line(destination,srect16(0,y,sr.x2,y),vgreen);
    int16_t xs = pip_top_label_starts[index]-3;
    int16_t xe = pip_top_label_ends[index]+3;
    draw::line(destination,srect16(xs,y,xe,y),color_t::black);
    
    draw::line(destination,srect16(xs,0,xs,y),vgreen);
    draw::line(destination,srect16(xs,0,xs+3,0),vgreen);
    draw::line(destination,srect16(xe,0,xe,y),vgreen);
    draw::line(destination,srect16(xe,0,xe-3,0),vgreen);
}

void ui_title_screen(uix::display& disp) {
    title_jpg.initialize();
    title_font.initialize();
    title_screen.dimensions({LCD_WIDTH,LCD_HEIGHT});
    title_screen.background_color(color_t::white);
    disp.active_screen(title_screen);
    title_image.bounds(((srect16)title_jpg.bounds()).center(title_screen.bounds()));
    title_image.image(title_jpg);
    title_image.visible(false);
    title_screen.register_control(title_image);
    title_loading_label.bounds(srect16(0,0,LCD_WIDTH-1,title_font.line_height()-1).center_vertical(title_screen.bounds()));
    title_loading_label.color(uix_color_t::black);
    title_loading_label.font(title_font);
    title_loading_label.padding({0,0});
    title_loading_label.text("loading...");
    title_screen.register_control(title_loading_label);
    disp.update();
    title_image.visible(true);
    disp.update();
    pip_top_font.initialize();
    title_loading_label.visible(false);
    title_font.deinitialize();
    title_jpg.deinitialize();
    title_screen.unregister_controls();
    delay(1500);
}
static void pip_sub_screen_clear() {
    pip_stat.visible(false);
}
static void pip_sub_screen_stat() {
    pip_stat.visible(true);
}
static void pip_sub_screen_set() {
    pip_sub_screen_clear();
    switch(top_menu_index) {
        case 0:
            switch(sub_menu_index) {
                case 0:
                    pip_sub_screen_stat();
                    break;
            }
            break;
    }
}
static void pip_set_sub_menu_index(size_t index) {
    if(index!=sub_menu_index) {
        sub_menu_index = index;
        pip_sub_menu_indices[top_menu_index]=index;
        sub_menu_painter.invalidate();
    } else {
        return;
    }
    pip_sub_screen_set();
}
static void pip_set_top_menu_index(size_t index) {
    if(index!=top_menu_index) {
        srect16 sr;
        if(top_menu_index>=0&&top_menu_index<pip_boy_top_labels_size) {
            sr=srect16(pip_top_label_starts[top_menu_index]-3,0,pip_top_label_ends[top_menu_index]+3,top_menu_painter.dimensions().height-1);
            top_menu_painter.invalidate(sr);
        }
        top_menu_index=index;
        sr=srect16(pip_top_label_starts[top_menu_index]-3,0,pip_top_label_ends[top_menu_index]+3,top_menu_painter.dimensions().height-1);
        top_menu_painter.invalidate(sr);
    }
    sub_menu_index = (size_t)-1;
    pip_set_sub_menu_index(pip_sub_menu_indices[top_menu_index]);
    
}

void top_menu_left_on_pressed_changed(bool pressed, void* state) {
    if(!pressed) {
        size_t i = top_menu_index>0?top_menu_index-1:pip_boy_top_labels_size-1;
        pip_set_top_menu_index(i);
    }
}
void top_menu_right_on_pressed_changed(bool pressed, void* state) {
    if(!pressed) {
        size_t i = top_menu_index<pip_boy_top_labels_size-1?top_menu_index+1:0;
        pip_set_top_menu_index(i);
    }
}
void sub_menu_left_on_pressed_changed(bool pressed, void* state) {
    if(!pressed) {
        size_t i = sub_menu_index>0?sub_menu_index-1:pip_sub_menu_sizes[top_menu_index]-1;
        pip_set_sub_menu_index(i);
    }
}
void sub_menu_right_on_pressed_changed(bool pressed, void* state) {
    if(!pressed) {
        size_t i = sub_menu_index<pip_sub_menu_sizes[top_menu_index]-1?sub_menu_index+1:0;
        pip_set_sub_menu_index(i);
    }
}
void ui_pip_screen(uix::display& disp) {
    button_l.on_pressed_changed(top_menu_left_on_pressed_changed);
    button_r.on_pressed_changed(top_menu_right_on_pressed_changed);
    button_left.on_pressed_changed(sub_menu_left_on_pressed_changed);
    button_right.on_pressed_changed(sub_menu_right_on_pressed_changed);
    for(int i = 0;i<pip_boy_top_labels_size;++i) {
        pip_sub_menu_indices[i]=0;
    }
    pip_screen.dimensions({LCD_WIDTH,LCD_HEIGHT});
    pip_screen.background_color(color_t::black);
    disp.active_screen(pip_screen);
    text_info ti("RADIO",pip_top_font);
    size16 tsz;
    ti.text_font->measure(-1,ti,&tsz);
    
    srect16 top_rect(0,0,tsz.width*5-1,pip_top_font.line_height()-1);
    top_rect.center_horizontal_inplace(pip_screen.bounds());
    top_rect.offset_inplace(0,5);
    srect16 top_border_rect = top_rect;
    top_border_rect.x1 = 0;
    top_border_rect.x2 = LCD_WIDTH-1;
    top_border_rect.y1=top_border_rect.y2-8;
    top_border_rect.y2+=10;
    top_menu_painter.bounds(top_border_rect);
    top_menu_painter.on_paint_callback(top_menu_painter_on_paint);
    pip_screen.register_control(top_menu_painter);
    for(int i = 0;i<pip_boy_top_labels_size;++i) {
        label_t& lab = pip_top_labels[i];
        lab.font(pip_top_font);
        ti.text_font = &pip_top_font;
        ti.text_sz(pip_top_captions[i]);
        size16 csz;
        pip_top_font.measure(-1,ti,&csz);
        int16_t x = top_rect.x1+tsz.width*i;
        srect16 b(x,top_rect.y1,x+tsz.width-1,top_rect.y2);
        pip_top_label_starts[i]=b.x1;
        pip_top_label_ends[i]=b.x1+csz.width;
        lab.bounds(b);
        lab.padding({0,0});
        lab.text_justify(uix::uix_justify::top_left);
        lab.text(pip_top_captions[i]);
        lab.color(uix_vgreen);
        pip_screen.register_control(lab);
    }
    srect16 subrect(0,0,LCD_WIDTH-1,pip_top_font.line_height()-1);
    subrect.offset_inplace(0,top_menu_painter.bounds().y2);
    sub_menu_painter.bounds(subrect);
    sub_menu_painter.on_paint_callback(sub_menu_painter_on_paint);
    pip_screen.register_control(sub_menu_painter);

    pip_stat.bounds(srect16(0,subrect.y2+1,200,subrect.y2+200).center_horizontal(pip_screen.bounds()));
    
    pip_screen.register_control(pip_stat);

    pip_set_top_menu_index(0);
    disp.active_screen(pip_screen);
}
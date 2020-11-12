/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>
#include <stdio.h>
#include <math.h>
#include <dfs_posix.h>

#include "drv_heap.h"
#include "drv_display.h"
#include "hal_base.h"
#include "image_info.h"
#include "display.h"

#include "app_main.h"
#include "lib_imageprocess.h"
/*
 **************************************************************************************************
 *
 * Music design callback
 *
 **************************************************************************************************
 */
static player_state_t play_state;
static rt_int8_t play_mode;
static int first_load = 1;

rt_err_t app_music_init(void)
{
    return RT_EOK;
}

rt_err_t app_music_design(void *param)
{
    main_page_design_param_t *par = (main_page_design_param_t *)param;
    rt_uint8_t  *fb    = app_main_page_data->top_fb;
    img_load_info_t img_load_info;
    struct image_st bg;
    rt_uint16_t startx = (rt_uint16_t)(par->buf_id) * CLOCK_WIN_XRES;
    rt_uint16_t starty = 0;
    rt_uint8_t  *buf;
    rt_uint16_t x, y;

    if (first_load)
    {
        if (g_app_info->cold_boot)
        {
            bg.width = CLOCK_WIN_XRES;
            bg.height = CLOCK_WIN_YRES;
            bg.stride = CLOCK_WIN_FB_W * (CLOCK_WIN_COLOR_DEPTH >> 3);
            bg.pdata = fb + startx * (CLOCK_WIN_COLOR_DEPTH >> 3);
            rk_image_reset(&bg, CLOCK_WIN_COLOR_DEPTH >> 3);
        }

        x = MUSIC_VOL_X;
        y = MUSIC_VOL_Y;
        buf = fb + (startx + x + (starty + y) * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
        img_load_info.w = MUSIC_VOL_W;
        img_load_info.h = MUSIC_VOL_H;
        img_load_info.name = MUSIC_ICONS_PATH"/icon_music_vol.dta";
        app_load_img(&img_load_info, buf, CLOCK_WIN_FB_W, MUSIC_VOL_H, 0, (CLOCK_WIN_COLOR_DEPTH >> 3));

        x = MUSIC_MORE_X;
        y = MUSIC_MORE_Y;
        buf = fb + (startx + x + (starty + y) * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
        img_load_info.w = MUSIC_MORE_W;
        img_load_info.h = MUSIC_MORE_H;
        img_load_info.name = MUSIC_ICONS_PATH"/icon_nusic_more.dta";
        app_load_img(&img_load_info, buf, CLOCK_WIN_FB_W, MUSIC_MORE_H, 0, (CLOCK_WIN_COLOR_DEPTH >> 3));

        x = MUSIC_PREV_X;
        y = MUSIC_PREV_Y;
        buf = fb + (startx + x + (starty + y) * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
        img_load_info.w = MUSIC_PREV_W;
        img_load_info.h = MUSIC_PREV_H;
        img_load_info.name = MUSIC_ICONS_PATH"/icon_nusic_prev.dta";
        app_load_img(&img_load_info, buf, CLOCK_WIN_FB_W, MUSIC_PREV_H, 0, (CLOCK_WIN_COLOR_DEPTH >> 3));

        x = MUSIC_NEXT_X;
        y = MUSIC_NEXT_Y;
        buf = fb + (startx + x + (starty + y) * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
        img_load_info.w = MUSIC_NEXT_W;
        img_load_info.h = MUSIC_NEXT_H;
        img_load_info.name = MUSIC_ICONS_PATH"/icon_nusic_next.dta";
        app_load_img(&img_load_info, buf, CLOCK_WIN_FB_W, MUSIC_NEXT_H, 0, (CLOCK_WIN_COLOR_DEPTH >> 3));
    }

    if ((play_state != app_main_data->play_state) || first_load)
    {
        play_state = app_main_data->play_state;

        switch (play_state)
        {
        case PLAYER_STATE_RUNNING:
            img_load_info.name = MUSIC_ICONS_PATH"/icon_music_suspend.dta";
            break;
        default:
            img_load_info.name = MUSIC_ICONS_PATH"/icon_nusic_play.dta";
            break;
        }
        x = MUSIC_PLAY_X;
        y = MUSIC_PLAY_Y;
        buf = fb + (startx + x + (starty + y) * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
        img_load_info.w = MUSIC_PLAY_W;
        img_load_info.h = MUSIC_PLAY_H;
        app_load_img(&img_load_info, buf, CLOCK_WIN_FB_W, MUSIC_PLAY_H, 0, (CLOCK_WIN_COLOR_DEPTH >> 3));
    }

    if ((play_mode != app_main_data->play_mode) || first_load)
    {
        char txt[128];

        if (app_main_data->play_mode < APP_PLAY_LIST)
            app_main_data->play_mode = APP_PLAY_LIST;
        if (app_main_data->play_mode > APP_PLAY_RANDOM)
            app_main_data->play_mode = APP_PLAY_RANDOM;

        play_mode = app_main_data->play_mode;

        memset(txt, 0, sizeof(txt));
        snprintf(txt, sizeof(txt), MUSIC_ICONS_PATH"/icon_music_%02d.dta", play_mode);

        x = MUSIC_MODE_X;
        y = MUSIC_MODE_Y;
        buf = fb + (startx + x + (starty + y) * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
        img_load_info.w = MUSIC_MODE_W;
        img_load_info.h = MUSIC_MODE_H;
        img_load_info.name = txt;
        app_load_img(&img_load_info, buf, CLOCK_WIN_FB_W, MUSIC_MODE_H, 0, (CLOCK_WIN_COLOR_DEPTH >> 3));
    }

    first_load = 0;

    return RT_EOK;
}

static int rotate_angle = 0;
void app_music_name_design(char *name)
{
    struct app_main_page_data_t *pdata = app_main_page_data;
    struct app_lvgl_label_design label;
    int start_x, start_y;
    img_load_info_t img_load_info;
    rt_uint8_t  *buf;
    rt_uint16_t x, y;
    char txt[128];

    start_x = 3 * CLOCK_WIN_XRES;
    start_y = MUSIC_NAME_Y;
    if (name)
    {
        label.txt = name;
    }
    else
    {
        label.txt = "No File";
    }

    label.ping_pong = 0;
    label.font = &lv_font_montserrat_30;
    label.align = LV_LABEL_ALIGN_CENTER;
    label.fmt = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    label.img[0].width = MUSIC_NAME_W;
    label.img[0].height = lv_font_montserrat_30.line_height;
    label.img[0].stride = CLOCK_WIN_FB_W * (CLOCK_WIN_COLOR_DEPTH >> 3);
    label.img[0].pdata = pdata->top_fb + (start_x + start_y * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
    app_lv_label_design(&label);

    memset(txt, 0, sizeof(txt));
    snprintf(txt, sizeof(txt), MUSIC_DIR_PATH"/%s.dta", label.txt);
    if (access(txt, F_OK) < 0)
        img_load_info.name = MUSIC_ICONS_PATH"/music_picture.dta";
    else
        img_load_info.name = txt;
    x = MUSIC_PIC_X;
    y = MUSIC_PIC_Y;
    buf = pdata->top_fb + (start_x + x + y * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
    img_load_info.w = MUSIC_PIC_W;
    img_load_info.h = MUSIC_PIC_H;
    app_load_img(&img_load_info, buf, CLOCK_WIN_FB_W, MUSIC_PIC_H, 0, (CLOCK_WIN_COLOR_DEPTH >> 3));
    rotate_angle = 0;
}

static refrsh_request_param_t app_music_refr_param;
void app_music_design_update(void)
{
    main_page_design_param_t design_param;

    design_param.buf_id = 3;
    app_music_design(&design_param);

    if (app_main_page_data->cur_page == 3 &&
            app_main_data->ver_page == VER_PAGE_NULL)
    {
        app_music_refr_param.wflag = 0x01 << main_page_refrsh_param.win_id;
        app_music_refr_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&app_music_refr_param);
    }
}

struct rotateimage_st r_ps = {0, 0, 0, 0, 0, NULL};
struct rotateimage_st r_pd = {0, 0, 0, 0, 0, NULL};
rt_err_t app_music_picture_rotate(void *param)
{
    main_page_design_param_t *par = (main_page_design_param_t *)param;
    rt_uint8_t  *fb    = app_main_page_data->top_fb;
    uint16_t start_x = par->buf_id * CLOCK_WIN_XRES + MUSIC_PIC_X;
    uint16_t start_y = MUSIC_PIC_Y;
    img_load_info_t img_load_info;

    if (app_main_page_data->cur_page != 3 ||
            app_main_data->ver_page != VER_PAGE_NULL)
    {
        return RT_EOK;
    }

    if (r_ps.pdata == NULL)
    {
        r_ps.width = MUSIC_PIC_W;
        r_ps.height = MUSIC_PIC_H;
        r_ps.cx = r_ps.width / 2 - 0.5;
        r_ps.cy = r_ps.height / 2 - 0.5;
        r_ps.stride = r_ps.width * 4;
        r_ps.pdata = rt_malloc_large(r_ps.height * r_ps.stride);
        if (r_ps.pdata == NULL)
        {
            rt_kprintf("Malloc %d failed\n", r_ps.height * r_ps.stride);
            return RT_ERROR;
        }
        img_load_info.w = MUSIC_PIC_W;
        img_load_info.h = MUSIC_PIC_H;
        img_load_info.name = MUSIC_ICONS_PATH"/music_picture_argb.dta";
        app_load_img(&img_load_info, r_ps.pdata, r_ps.width, r_ps.height, 0, 4);
    }

    if (r_pd.pdata == NULL)
    {
        r_pd.width = MUSIC_PIC_W;
        r_pd.height = MUSIC_PIC_H;
        r_pd.cx = r_pd.width / 2 - 0.5;
        r_pd.cy = r_pd.height / 2 - 0.5;
        r_pd.stride = CLOCK_WIN_FB_W * (CLOCK_WIN_COLOR_DEPTH >> 3);
        r_pd.pdata = fb + (start_x + start_y * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
    }

    rotate_angle += 10;
    if (rotate_angle >= 360)
        rotate_angle = 0;
    rk_rotate_process_16bit(&r_ps, &r_pd, 360 - (rotate_angle % 360));

    app_music_refr_param.wflag = 0x01 << main_page_refrsh_param.win_id;
    app_music_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&app_music_refr_param);

    return RT_EOK;
}

void app_music_page_leave(void)
{
    rt_free_large(r_ps.pdata);
    r_ps.pdata = NULL;
}

struct obj_area
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    void (*cb)(void);
};

struct obj_area btn[6] =
{
    {MUSIC_MODE_X, MUSIC_MODE_Y, MUSIC_MODE_W, MUSIC_MODE_H, app_music_mode_switch},
    {MUSIC_VOL_X,  MUSIC_VOL_Y,  MUSIC_VOL_W,  MUSIC_VOL_H, NULL},
    {MUSIC_MORE_X, MUSIC_MORE_Y, MUSIC_MORE_W, MUSIC_MORE_H, NULL},

    {MUSIC_PREV_TOUCH_X, MUSIC_PREV_TOUCH_Y, MUSIC_PREV_TOUCH_W, MUSIC_PREV_TOUCH_H, app_play_prev},
    {MUSIC_PLAY_X, MUSIC_PLAY_Y, MUSIC_PLAY_W, MUSIC_PLAY_H, app_play_pause},
    {MUSIC_NEXT_TOUCH_X, MUSIC_NEXT_TOUCH_Y, MUSIC_NEXT_TOUCH_W, MUSIC_NEXT_TOUCH_H, app_play_next},
};

void app_music_touch(struct rt_touch_data *point)
{
    uint16_t x = point->x_coordinate;
    uint16_t y = point->y_coordinate;

    for (int i = 0; i < sizeof(btn) / sizeof(struct obj_area); i++)
    {
        if ((x >= btn[i].x) && (x <= (btn[i].x + btn[i].w)) &&
                (y >= btn[i].y) && (y <= (btn[i].y + btn[i].h)))
        {
            if (btn[i].cb != NULL)
                btn[i].cb();
            break;
        }
    }
}

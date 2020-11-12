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
#include "image_info.h"
#include "display.h"

#include "app_main.h"

/*
 **************************************************************************************************
 *
 * initial
 *
 **************************************************************************************************
 */
static const char *icons_path[2][3] =
{
    {
        "L:"ICONS_PATH"/alarm_clock.bin",
        "L:"ICONS_PATH"/sleep.bin",
        "L:"ICONS_PATH"/no_disturb.bin",
    },
    // {
    //     "L:"ICONS_PATH"/screen_display.bin",
    //     "L:"ICONS_PATH"/flashlight.bin",
    //     "L:"ICONS_PATH"/find_phone.bin",
    // },
    // {
    //     "L:"ICONS_PATH"/activity_record.bin",
    //     "L:"ICONS_PATH"/exercise_record.bin",
    //     "L:"ICONS_PATH"/training_status.bin",
    // },
    {
        "L:"ICONS_PATH"/pressure.bin",
        "L:"ICONS_PATH"/user_guide.bin",
        "L:"ICONS_PATH"/about_watche.bin"
    }
};
static const char *icons_txt[2][3] =
{
    {
        " Alarm",
        " Sleep",
        " Undisturb",
    },
    // {
    //     " Screen",
    //     " FlashLight",
    //     " Find phone",
    // },
    // {
    //     " Activity",
    //     " Exercise",
    //     " Training",
    // },
    {
        " Pressure",
        " User Guide",
        " Aboult"
    }
};

void app_setting_init(void *param)
{
    struct app_func_data_t *pdata = g_func_data;
    struct app_lvgl_label_design title;
    struct app_lvgl_iconlist_design iconlist;
    uint32_t padding = 0;

    /* framebuffer malloc */
    pdata->fblen = MENU_WIN_FB_W * MENU_WIN_FB_H * (MENU_WIN_COLOR_DEPTH >> 3);
    pdata->fb   = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    RT_ASSERT(pdata->fb != RT_NULL);
    rt_memset((void *)pdata->fb, 0x0, pdata->fblen);

    pdata->max_w = MENU_WIN_FB_W;
    pdata->max_h = MENU_WIN_FB_H;
    pdata->alpha_win = 1;

    title.txt = "Setting";
    title.ping_pong = 0;
    title.font = &lv_font_montserrat_44;
    title.align = LV_LABEL_ALIGN_CENTER;
    title.fmt = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    title.img[0].width = MENU_WIN_XRES;
    title.img[0].height = lv_font_montserrat_44.line_height;
    title.img[0].stride = MENU_WIN_FB_W * (MENU_WIN_COLOR_DEPTH >> 3);
    title.img[0].pdata = pdata->fb + (MENU_WIN_YRES / 3 - 44) / 2 * MENU_WIN_XRES * (MENU_WIN_COLOR_DEPTH >> 3);
    app_lv_label_design(&title);

    app_func_refrsh_param.win_id    = APP_CLOCK_WIN_2;
    app_func_refrsh_param.win_layer = WIN_MIDDLE_LAYER;

    padding = MENU_WIN_XRES * (MENU_WIN_YRES / 3) * (MENU_WIN_COLOR_DEPTH >> 3);
    for (int i = 0; i < 2; i++)
    {
        iconlist.icons_num = 3;
        for (int j = 0; j < iconlist.icons_num; j++)
        {
            iconlist.path[j] = (char *)icons_path[i][j];
            iconlist.txt[j] = (char *)icons_txt[i][j];
        }
        iconlist.img[0].width = MENU_WIN_XRES;
        iconlist.img[0].height = MENU_WIN_YRES;
        iconlist.img[0].stride = MENU_WIN_FB_W * (MENU_WIN_COLOR_DEPTH >> 3);
        iconlist.img[0].pdata = pdata->fb + padding + i * MENU_WIN_XRES * MENU_WIN_YRES * (MENU_WIN_COLOR_DEPTH >> 3);
        iconlist.ping_pong = 0;
        app_lv_iconlist_design(&iconlist);
    }

    app_func_set_preview(APP_FUNC_SETTING, pdata->fb);
}

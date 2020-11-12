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

#if defined(RT_USING_TOUCH_DRIVERS)
#include "touch.h"
#include "touchpanel.h"
#endif

#include "app_main.h"

/*
 **************************************************************************************************
 *
 * Global static struct & data define
 *
 **************************************************************************************************
 */
struct app_func_data_t *g_func_data = RT_NULL;
app_func_refrsh_param_t app_func_refrsh_param;
static void *old_callback;
static void *old_param;

/*
 **************************************************************************************************
 *
 * Display refresh
 *
 **************************************************************************************************
 */
static rt_err_t app_func_refresh(struct rt_display_config *wincfg, void *param)
{
    struct rt_device_graphic_info *info = &app_main_data->disp->info;
    struct app_func_data_t *fdata = g_func_data;
    app_func_refrsh_param_t *par = (app_func_refrsh_param_t *)param;
    int16_t start_x, start_y;

    start_y = fdata->ver_offset;
    start_x = fdata->hor_offset;
    if (start_y < 0)
    {
        start_y = 0;
    }
    if (start_y > (fdata->max_h - MENU_WIN_YRES))
    {
        start_y = fdata->max_h - MENU_WIN_YRES;
    }

    if (start_x <= 0)
    {
        start_x = 1;
    }
    if (start_x >= (fdata->max_w))
    {
        start_x = fdata->max_w - 1;
    }

    wincfg->winId   = par->win_id;
    wincfg->zpos    = par->win_layer;
    wincfg->colorkey = 0;
    wincfg->alphaEn = 0;
    wincfg->lut     = RT_NULL;
    wincfg->lutsize = 0;
    wincfg->fb     = fdata->fb + start_x * (MENU_WIN_COLOR_DEPTH >> 3) + start_y * MENU_WIN_FB_W * (MENU_WIN_COLOR_DEPTH >> 3);
    wincfg->format = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    wincfg->fblen = fdata->fblen;
    wincfg->w     = MENU_WIN_XRES - start_x;
    wincfg->h     = MENU_WIN_YRES;
    wincfg->xVir  = MENU_WIN_FB_W;
    wincfg->x     = MENU_REGION_X + ((info->width - MENU_WIN_XRES) / 2);
    wincfg->y     = MENU_REGION_Y + ((info->height - wincfg->h) / 2);
    wincfg->ylast = wincfg->y;

    RT_ASSERT((wincfg->x + wincfg->w) <= info->width);
    RT_ASSERT((wincfg->y + wincfg->h) <= info->height);

    return RT_EOK;
}

/*
 **************************************************************************************************
 *
 * touch process
 *
 **************************************************************************************************
 */
//---------------------------------------------------------------------------------
// touch touch up
//---------------------------------------------------------------------------------
static rt_err_t app_func_touch_up(void *param)
{
    return RT_EOK;
}

//---------------------------------------------------------------------------------
// touch move updn
//---------------------------------------------------------------------------------
static rt_err_t               app_func_move_updn_design(void *param);
static design_cb_t            setting_move_updn_design = { .cb = app_func_move_updn_design, };
static refrsh_request_param_t setting_move_updn_refr_param;
static refrsh_request_param_t setting_refrsh_request_param;
static rt_err_t app_func_move_updn_design(void *param)
{
    struct app_func_data_t *fdata = g_func_data;
    mov_design_param *tar = (mov_design_param *)param;

    fdata->ver_offset += TOUCH_MOVE_STEP * tar->dir;
    if ((tar->dir > 0 && (fdata->ver_offset) >= tar->offset) ||
            (tar->dir < 0 && (fdata->ver_offset) <= tar->offset))
    {
        fdata->ver_start = fdata->ver_offset = tar->offset;

        app_main_touch_register(&app_func_main_touch_cb);
    }
    else
    {
        app_design_request(0, &setting_move_updn_design, param);
    }

    setting_move_updn_refr_param.wflag = (0x01 << app_func_refrsh_param.win_id) |
                                         ((fdata->alpha_win) << alpha_win_refr_param.win_id);
    setting_move_updn_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&setting_move_updn_refr_param);

    return RT_EOK;
}

static rt_err_t app_func_touch_move_updn(void *param)
{
    struct app_main_data_t  *pdata = (struct app_main_data_t *)param;
    struct app_func_data_t *fdata = g_func_data;

    fdata->ver_offset = fdata->ver_start - pdata->yoffset;

    if (pdata->smooth_design == 0)
    {
        pdata->smooth_design = 1;
        setting_refrsh_request_param.wflag = (0x01 << app_func_refrsh_param.win_id) |
                                             ((fdata->alpha_win) << alpha_win_refr_param.win_id);
        setting_refrsh_request_param.wait = RT_WAITING_FOREVER;
        setting_refrsh_request_param.id = ID_FUNC_UPDN;
        app_registe_refresh_done_cb(app_main_touch_smooth_design,
                                    &setting_refrsh_request_param);
        app_refresh_request(&setting_refrsh_request_param);
    }

    return RT_EOK;
}

//---------------------------------------------------------------------------------
// touch move lr
//---------------------------------------------------------------------------------
static rt_err_t               app_func_move_lr_design(void *param);
static design_cb_t            setting_move_lr_design = { .cb = app_func_move_lr_design, };
static refrsh_request_param_t setting_move_lr_refr_param;
static rt_err_t app_func_move_lr_design(void *param)
{
    struct app_func_data_t *fdata = g_func_data;
    mov_design_param *tar = (mov_design_param *)param;

    fdata->hor_offset += TOUCH_MOVE_STEP * tar->dir;
    if ((tar->dir > 0 && (fdata->hor_offset) >= tar->offset) ||
            (tar->dir < 0 && (fdata->hor_offset) <= tar->offset))
    {
        fdata->hor_offset = tar->offset;
        if (fdata->hor_offset)
        {
            app_message_main_refrsh_param.win_layer = WIN_MIDDLE_LAYER;
            if (fdata->alpha_win)
            {
                app_alpha_win_hide();
            }
            app_refresh_register(app_func_refrsh_param.win_id, old_callback, old_param);
            app_main_touch_unregister();
            app_main_touch_register(&app_funclist_main_touch_cb);
            setting_move_lr_refr_param.wflag = 0x01 << app_message_main_refrsh_param.win_id;
        }
        else
        {
            app_main_touch_register(&app_func_main_touch_cb);
            setting_move_lr_refr_param.wflag = (0x01 << app_func_refrsh_param.win_id) |
                                               ((fdata->alpha_win) << alpha_win_refr_param.win_id);
        }
    }
    else
    {
        setting_move_lr_refr_param.wflag = (0x01 << app_func_refrsh_param.win_id) |
                                           (0x01 << app_message_main_refrsh_param.win_id) |
                                           ((fdata->alpha_win) << alpha_win_refr_param.win_id);
        app_design_request(0, &setting_move_lr_design, param);
    }

    setting_move_lr_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&setting_move_lr_refr_param);

    return RT_EOK;
}

static rt_err_t app_func_touch_move_lr(void *param)
{
    struct app_main_data_t  *pdata = (struct app_main_data_t *)param;
    struct app_func_data_t *fdata = g_func_data;

    if (pdata->xoffset >= 0)
    {
        return RT_EOK;
    }
    if (pdata->xoffset <= -MENU_WIN_XRES)
    {
        pdata->xoffset = -MENU_WIN_XRES;
    }
    fdata->hor_offset = -pdata->xoffset;

    if (pdata->smooth_design == 0)
    {
        pdata->smooth_design = 1;
        setting_refrsh_request_param.wflag = (0x01 << app_func_refrsh_param.win_id) |
                                             (0x01 << app_message_main_refrsh_param.win_id) |
                                             ((fdata->alpha_win) << alpha_win_refr_param.win_id);
        setting_refrsh_request_param.wait = RT_WAITING_FOREVER;
        setting_refrsh_request_param.id = ID_FUNC_LR;
        app_registe_refresh_done_cb(app_main_touch_smooth_design,
                                    &setting_refrsh_request_param);
        app_refresh_request(&setting_refrsh_request_param);
    }

    return RT_EOK;
}

//---------------------------------------------------------------------------------
// touch move up
//---------------------------------------------------------------------------------
static mov_design_param touch_moveup_design_param;
rt_err_t app_func_touch_move_up(void *param)
{
    struct app_main_data_t *pdata = (struct app_main_data_t *)param;
    struct app_func_data_t *fdata = g_func_data;
    struct rt_touch_data *cur_p   = &pdata->cur_point[0];
    struct rt_touch_data *down_p   = &pdata->down_point[0];
    int16_t floor_ofs, ceil_ofs;

    if (pdata->dir_mode == TOUCH_DIR_MODE_UPDN)
    {
        app_registe_refresh_done_cb(NULL, NULL);
        app_main_touch_unregister();

        if (fdata->ver_offset < 0)
        {
            fdata->ver_offset = 0;
        }
        if (fdata->ver_offset > (fdata->max_h - MENU_WIN_YRES))
        {
            fdata->ver_offset = fdata->max_h - MENU_WIN_YRES;
        }

        floor_ofs = (fdata->ver_offset / (MENU_WIN_YRES / 3) + 1) * (MENU_WIN_YRES / 3);
        ceil_ofs  =  fdata->ver_offset / (MENU_WIN_YRES / 3) * (MENU_WIN_YRES / 3);

        if ((floor_ofs - fdata->ver_offset) < (fdata->ver_offset - ceil_ofs))
        {
            touch_moveup_design_param.dir = 1;
            touch_moveup_design_param.offset = floor_ofs;
        }
        else
        {
            touch_moveup_design_param.dir = -1;
            touch_moveup_design_param.offset = ceil_ofs;
        }
        app_design_request(0, &setting_move_updn_design, &touch_moveup_design_param);
    }
    else if (pdata->dir_mode == TOUCH_DIR_MODE_LR)
    {
        app_registe_refresh_done_cb(NULL, NULL);
        app_main_touch_unregister();

        if ((down_p->x_coordinate) > (cur_p->x_coordinate))
        {
            touch_moveup_design_param.dir = 1;
            touch_moveup_design_param.offset = MENU_WIN_XRES;
            app_func_exit(fdata->func_id);
        }
        else
        {
            touch_moveup_design_param.dir = -1;
            touch_moveup_design_param.offset = 0;
        }

        app_design_request(0, &setting_move_lr_design, &touch_moveup_design_param);
    }

    return RT_EOK;
}

struct app_touch_cb_t app_func_main_touch_cb =
{
    .tp_move_updn   = app_func_touch_move_updn,
    .tp_move_lr     = app_func_touch_move_lr,
    .tp_move_up     = app_func_touch_move_up,
    .tp_touch_up    = app_func_touch_up,
};

/*
 **************************************************************************************************
 *
 * initial
 *
 **************************************************************************************************
 */
void app_func_show(void *param)
{
    struct app_func_data_t *fdata = g_func_data;

    app_message_main_refrsh_param.win_layer = WIN_BOTTOM_LAYER;

    fdata->ver_start = 0;
    fdata->ver_offset = 0;
    fdata->hor_offset = 0;

    app_main_timer_cb_unregister();
    app_main_touch_unregister();
    app_main_touch_register(&app_func_main_touch_cb);
    app_get_refresh_callback(app_func_refrsh_param.win_id, &old_callback, &old_param);
    app_refresh_register(app_func_refrsh_param.win_id, app_func_refresh, &app_func_refrsh_param);

    if (fdata->alpha_win)
    {
        app_alpha_win_show();
    }

    setting_refrsh_request_param.wflag = (0x1 << app_func_refrsh_param.win_id) |
                                         ((fdata->alpha_win) << alpha_win_refr_param.win_id);
    setting_refrsh_request_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&setting_refrsh_request_param);
}

void app_func_common_exit(void)
{
    struct app_func_data_t *pdata = g_func_data;

    rt_free_psram(pdata->fb);
}

void app_func_common_init(void *param)
{
    struct app_func_data_t *pdata = g_func_data;
    struct app_lvgl_label_design title;

    /* framebuffer malloc */
    pdata->fblen = MENU_WIN_XRES * MENU_WIN_YRES * (MENU_WIN_COLOR_DEPTH >> 3);
    pdata->fb   = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    RT_ASSERT(pdata->fb != NULL);
    rt_memset((void *)pdata->fb, 0x0, pdata->fblen);

    app_func_refrsh_param.win_id    = APP_CLOCK_WIN_2;
    app_func_refrsh_param.win_layer = WIN_TOP_LAYER;

    pdata->max_w = MENU_WIN_XRES;
    pdata->max_h = MENU_WIN_YRES;
    pdata->alpha_win = 0;

    title.txt = "Work in process";
    title.ping_pong = 0;
    title.font = &lv_font_montserrat_30;
    title.align = LV_LABEL_ALIGN_CENTER;
    title.fmt = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    title.img[0].width = MENU_WIN_XRES;
    title.img[0].height = lv_font_montserrat_30.line_height;
    title.img[0].stride = MENU_WIN_FB_W * (MENU_WIN_COLOR_DEPTH >> 3);
    title.img[0].pdata = pdata->fb + (MENU_WIN_YRES - title.img[0].height) / 2 * MENU_WIN_XRES * (MENU_WIN_COLOR_DEPTH >> 3);
    app_lv_label_design(&title);

    app_func_set_preview(APP_FUNC_COMMON, pdata->fb);
}

static rt_err_t app_func_init_design(void *param)
{
    struct app_func_data_t *pdata;

    g_func_data = pdata = (struct app_func_data_t *)rt_malloc(sizeof(struct app_func_data_t));
    RT_ASSERT(pdata != RT_NULL);
    rt_memset((void *)pdata, 0, sizeof(struct app_func_data_t));

    pdata->hor_offset = MENU_WIN_XRES;

    app_func_set(APP_FUNC_EXERCISE, func_motion_init, NULL, func_motion_enter, NULL, func_motion_exit, NULL);
    app_func_set(APP_FUNC_SETTING, app_setting_init, NULL, app_func_show, NULL, NULL, NULL);
    app_func_set(APP_FUNC_COMMON, app_func_common_init, NULL, app_func_show, NULL, app_func_common_exit, NULL);
    app_func_set(APP_FUNC_HEARTRATE, func_heartrate_init, NULL, func_heartrate_enter, NULL, func_heartrate_exit, NULL);

    return RT_EOK;
}
static design_cb_t  app_func_init_design_t = {.cb = app_func_init_design,};

void app_func_memory_init(void)
{
    app_design_request(0, &app_func_init_design_t, RT_NULL);
}

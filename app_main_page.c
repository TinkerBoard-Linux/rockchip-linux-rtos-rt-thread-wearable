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

#if defined(RT_USING_TOUCH_DRIVERS)
#include "touch.h"
#include "touchpanel.h"
#endif

#include "app_main.h"
#include "lib_imageprocess.h"

#ifdef RT_USING_FWANALYSIS
#include "rkpart.h"
#endif

/*
 **************************************************************************************************
 *
 * Macro define
 *
 **************************************************************************************************
 */

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */

void app_main_page_clock_update(void);
void app_main_page_weather_update(void);
void app_main_page_music_update(void);

/*
 **************************************************************************************************
 *
 * Global static struct & data define
 *
 **************************************************************************************************
 */
struct app_main_page_data_t *app_main_page_data = RT_NULL;
main_page_refrsh_param_t main_page_refrsh_param;

app_main_page_timer_cb_t main_page_timer_cb[CLOCK_HOR_PAGE_MAX] =
{
    {RT_NULL,},
    {app_main_page_clock_update,},
    {app_main_page_weather_update,},
    {app_main_page_music_update,},
};

app_main_page_timer_cb_t main_page_leave[CLOCK_HOR_PAGE_MAX] =
{
    {RT_NULL,},
    {RT_NULL,},
    {RT_NULL,},
    {app_music_page_leave,},
};

static img_load_info_t img_page_bkg[CLOCK_HOR_PAGE_MAX] =
{
    { DISP_XRES, DISP_YRES, USERDATA_PATH"page_qrcode.dta"},
    { DISP_XRES, DISP_YRES, USERDATA_PATH"page_clock.dta"},
    { DISP_XRES, DISP_YRES, USERDATA_PATH"page_weather.dta"},
    { DISP_XRES, DISP_YRES, USERDATA_PATH"page_music.dta"},
};

/*
 **************************************************************************************************
 *
 * Main page1(clock) design
 *
 **************************************************************************************************
 */
rt_err_t app_main_page_clock_init(void)
{
    return app_clock_init();
}

rt_err_t app_main_page_clock_design(void *param)
{
    return app_clock_design(param);
}

static main_page_design_param_t clock_design_param;
static design_cb_t              clock_design = {.cb = app_main_page_clock_design,};
static refrsh_request_param_t   clock_refresh_request_param;
static rt_uint8_t  clock_second_bk = 60;
void app_main_page_clock_update(void)
{
    if (clock_second_bk == app_main_data->tmr_data.second)
    {
        return;
    }
    if (app_main_data->tmr_data.tick >= 2)
    {
        return;
    }
    clock_second_bk = app_main_data->tmr_data.second;

    clock_design_param.buf_id = 1;
    app_design_request(0, &clock_design, &clock_design_param);

    clock_refresh_request_param.wflag = 0x01 << main_page_refrsh_param.win_id;
    clock_refresh_request_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&clock_refresh_request_param);
}

rt_err_t app_main_page_weather_init(void)
{
    return app_weather_init();
}

static refrsh_request_param_t   weather_refresh_request_param;
rt_err_t app_main_page_weather_design(void *param)
{
    rt_err_t ret;

    ret = app_weather_design(param);

    if (ret == RT_EOK)
    {
        weather_refresh_request_param.wflag = 0x01 << main_page_refrsh_param.win_id;
        weather_refresh_request_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&weather_refresh_request_param);
    }

    return ret;
}

rt_err_t app_main_page_music_init(void)
{
    return app_music_init();
}

rt_err_t app_main_page_music_design(void *param)
{
    return app_music_design(param);
}

static main_page_design_param_t weather_design_param;
static design_cb_t              weather_design = {.cb = app_main_page_weather_design,};
void app_main_page_weather_update(void)
{
    weather_design_param.buf_id = 2;
    app_design_request(0, &weather_design, &weather_design_param);
}

static main_page_design_param_t music_design_param;
static design_cb_t              music_design = {.cb = app_music_picture_rotate,};
void app_main_page_music_update(void)
{
    static uint32_t last_tick = 0;
    if (app_main_data->play_state != PLAYER_STATE_RUNNING ||
            ((HAL_GetTick() - last_tick) < 300))
    {
        return;
    }
    last_tick = HAL_GetTick();

    music_design_param.buf_id = 3;
    app_design_request(0, &music_design, &music_design_param);
}

/*
 **************************************************************************************************
 *
 * Display refresh
 *
 **************************************************************************************************
 */
/**
 * top layer refresh.
 */
rt_err_t app_main_page_refresh(struct rt_display_config *wincfg, void *param)
{
    struct rt_device_graphic_info *info = &app_main_data->disp->info;
    struct app_main_page_data_t *pdata = app_main_page_data;
    struct app_message_main_data_t *msgdata = g_message_main_data;
    main_page_refrsh_param_t *par = (main_page_refrsh_param_t *)param;
    uint32_t start_x, start_y;

    /* Make sure the flip dose not exceed one page */
    if (((pdata->cur_page - 1) * CLOCK_WIN_XRES) > pdata->mov_offset)
        pdata->mov_offset = (pdata->cur_page - 1) * CLOCK_WIN_XRES;
    if (((pdata->cur_page + 1) * CLOCK_WIN_XRES) < pdata->mov_offset)
        pdata->mov_offset = (pdata->cur_page + 1) * CLOCK_WIN_XRES;

    if (pdata->mov_offset > 0)
        start_x = pdata->mov_offset;
    else
        start_x = (CLOCK_WIN_XRES * CLOCK_MAX_BUF_ID) + pdata->mov_offset;
    start_y = (msgdata->mov_offset <= 0 ? CLOCK_REGION_Y : ABS(msgdata->mov_offset));

    wincfg->winId   = par->win_id;
    wincfg->zpos    = par->win_layer;
    wincfg->colorkey = 0;//COLOR_KEY_EN | 0;
    wincfg->alphaEn = 0;
    wincfg->format  = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    wincfg->lut     = RT_NULL;
    wincfg->lutsize = 0;
    wincfg->fb    = pdata->top_fb    + (start_x + start_y * CLOCK_WIN_FB_W) * (CLOCK_WIN_COLOR_DEPTH >> 3);
    wincfg->fblen = pdata->top_fblen - (start_x) * (CLOCK_WIN_COLOR_DEPTH >> 3);
    wincfg->xVir  = CLOCK_WIN_FB_W;
    wincfg->w     = CLOCK_WIN_XRES;
    wincfg->h     = CLOCK_WIN_YRES - ABS(msgdata->mov_offset);
    wincfg->x     = CLOCK_REGION_X + ((info->width  - CLOCK_WIN_XRES) / 2);
    wincfg->y     = start_y + ((info->height - CLOCK_WIN_YRES) / 2);;
    wincfg->ylast = wincfg->y;

    // RT_ASSERT(((wincfg->w * CLOCK_WIN_COLOR_DEPTH) % 32) == 0);
    // RT_ASSERT(((wincfg->y % 2) == 0) && ((wincfg->h % 2) == 0));

    RT_ASSERT((wincfg->x + wincfg->w) <= info->width);
    RT_ASSERT((wincfg->y + wincfg->h) <= info->height);
    if (wincfg->h == 0)
        return RT_ERROR;

    return RT_EOK;
}

/*
 **************************************************************************************************
 *
 * Touch process
 *
 **************************************************************************************************
 */
//---------------------------------------------------------------------------------
// touch down & up callback
//---------------------------------------------------------------------------------
rt_err_t app_show_charging_anim(void *param)
{
    struct app_charging_data_t *cdata = g_charging_data;

    cdata->skip_touchup = 1;
    app_charging_enable(NULL);

    return RT_EOK;
}

static void page_qrcode_touch_down(void *param)
{
    app_main_register_timeout_cb(app_show_charging_anim, NULL, 300);
}

static void page_qrcode_touch_up(void *param)
{
    app_main_unregister_timeout_cb_if_is(app_show_charging_anim);
    // app_main_timer_cb_register(app_main_page_clock_update);
    if (main_page_timer_cb[app_main_page_data->cur_page].cb)
    {
        app_main_timer_cb_register(main_page_timer_cb[app_main_page_data->cur_page].cb);
    }
}

static void page_clock_touch_down(void *param)
{
    app_main_register_timeout_cb(app_preview_enter, NULL, 500);
}

static void page_clock_touch_up(void *param)
{
    app_main_unregister_timeout_cb_if_is(app_preview_enter);
    // app_main_timer_cb_register(app_main_page_clock_update);
    if (main_page_timer_cb[app_main_page_data->cur_page].cb)
    {
        app_main_timer_cb_register(main_page_timer_cb[app_main_page_data->cur_page].cb);
    }
}

static void page_music_touch_up(void *param)
{
    struct app_main_data_t *pdata = (struct app_main_data_t *)param;
    struct rt_touch_data *cur_p = &pdata->cur_point[0];

    app_music_touch(cur_p);

    // app_main_timer_cb_register(app_main_page_clock_update);
    if (main_page_timer_cb[app_main_page_data->cur_page].cb)
    {
        app_main_timer_cb_register(main_page_timer_cb[app_main_page_data->cur_page].cb);
    }
}

struct app_page_touch_ops page_touch_ops[CLOCK_HOR_PAGE_MAX] =
{
    {page_qrcode_touch_down, page_qrcode_touch_up},
    {page_clock_touch_down, page_clock_touch_up},
    {NULL, NULL},
    {NULL, page_music_touch_up},
};

static rt_err_t app_main_page_touch_down(void *param)
{
    struct app_main_page_data_t *appdata  = app_main_page_data;

    app_main_timer_cb_unregister();

    if (page_touch_ops[appdata->cur_page].touch_down)
    {
        page_touch_ops[appdata->cur_page].touch_down(param);
    }

    return RT_EOK;
}

static rt_err_t app_main_page_touch_up(void *param)
{
    struct app_main_page_data_t *appdata  = app_main_page_data;

    if (page_touch_ops[appdata->cur_page].touch_up)
    {
        page_touch_ops[appdata->cur_page].touch_up(param);
    }
    else
    {
        // app_main_timer_cb_register(app_main_page_clock_update);
        if (main_page_timer_cb[appdata->cur_page].cb)
        {
            app_main_timer_cb_register(main_page_timer_cb[appdata->cur_page].cb);
        }
    }

    return RT_EOK;
}

//---------------------------------------------------------------------------------
// touch move lr
//---------------------------------------------------------------------------------
static rt_err_t               app_main_page_move_lr_design(void *param);
static design_cb_t            move_lr_touch_design = { .cb = app_main_page_move_lr_design, };
static refrsh_request_param_t move_lr_design_refr_param;
static refrsh_request_param_t move_lr_refr_param;

static rt_err_t app_main_page_move_lr_design(void *param)
{
    struct app_main_page_data_t *pdata = app_main_page_data;
    mov_design_param *tar = (mov_design_param *)param;
    int turn_page;

    pdata->mov_offset += TOUCH_MOVE_STEP * tar->dir;
    if ((tar->dir > 0 && (pdata->mov_offset) >= tar->offset) ||
            (tar->dir < 0 && (pdata->mov_offset) <= tar->offset))
    {
        turn_page = (tar->offset != pdata->cur_page * CLOCK_WIN_XRES);
        if (turn_page)
        {
            if (main_page_leave[pdata->cur_page].cb)
            {
                main_page_leave[pdata->cur_page].cb();
            }
            pdata->cur_page += tar->dir;
            if (pdata->cur_page < 0)
            {
                pdata->cur_page += CLOCK_HOR_PAGE_MAX;
            }
            if (pdata->cur_page >= CLOCK_HOR_PAGE_MAX)
            {
                pdata->cur_page -= CLOCK_HOR_PAGE_MAX;
            }
            g_app_info->page_index = pdata->cur_page;
        }
        pdata->mov_dir      = 0;
        pdata->mov_offset   = pdata->cur_page * CLOCK_WIN_XRES;

        pdata->switch_done = 0;

        //app_main_timer_cb_register(app_main_page_clock_update);
        if (main_page_timer_cb[pdata->cur_page].cb)
        {
            app_main_timer_cb_register(main_page_timer_cb[pdata->cur_page].cb);
        }
        app_main_touch_register(&main_page_touch_cb);
    }
    else
    {
        app_design_request(0, &move_lr_touch_design, param);
    }

    move_lr_design_refr_param.wflag = 0x01 << main_page_refrsh_param.win_id;
    move_lr_design_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&move_lr_design_refr_param);

    return RT_EOK;
}

static rt_err_t app_main_page_touch_move_lr(void *param)
{
    struct app_main_data_t  *pdata = (struct app_main_data_t *)param;
    struct app_main_page_data_t *appdata  = app_main_page_data;

    appdata->mov_dir    = -pdata->xdir;
    appdata->mov_offset = -(pdata->xoffset + pdata->mov_fix);
    appdata->mov_offset += appdata->cur_page * CLOCK_WIN_XRES;

    app_main_unregister_timeout_cb_if_is(app_show_charging_anim);
    app_main_unregister_timeout_cb_if_is(app_preview_enter);

    move_lr_refr_param.wflag = 0x01 << main_page_refrsh_param.win_id;
    move_lr_refr_param.wait = RT_WAITING_FOREVER;
    move_lr_refr_param.id = ID_MAIN;
    app_slide_refresh(&move_lr_refr_param);

    return RT_EOK;
}

//---------------------------------------------------------------------------------
// touch move updn
//---------------------------------------------------------------------------------
static rt_err_t               app_main_page_move_updn_design(void *param);
static design_cb_t            move_updn_touch_design = { .cb = app_main_page_move_updn_design, };
static refrsh_request_param_t move_updn_design_refr_param;
static refrsh_request_param_t move_updn_refr_param;
static rt_err_t app_main_page_move_updn_design(void *param)
{
    struct app_main_data_t *appdata = app_main_data;
    struct app_message_main_data_t *pdata = g_message_main_data;
    struct app_funclist_data_t *fdata = g_funclist_data;
    mov_design_param *tar = (mov_design_param *)param;

    pdata->mov_offset += TOUCH_MOVE_STEP * tar->dir;
    if (((tar->dir > 0) && (pdata->mov_offset >= tar->offset)) ||
            ((tar->dir < 0) && (pdata->mov_offset <= tar->offset)))
    {

        pdata->mov_dir     = 0;
        pdata->mov_offset  = tar->offset;

        if (tar->offset == 0)
        {
            // app_main_timer_cb_register(app_main_page_clock_update);
            if (main_page_timer_cb[app_main_page_data->cur_page].cb)
            {
                app_main_timer_cb_register(main_page_timer_cb[app_main_page_data->cur_page].cb);
            }
            app_main_touch_register(&main_page_touch_cb);

            appdata->ver_page = VER_PAGE_NULL;
            // move_updn_design_refr_param.wflag = 0x01 << main_page_refrsh_param.win_id;
        }
        else
        {
            if (main_page_leave[app_main_page_data->cur_page].cb)
            {
                main_page_leave[app_main_page_data->cur_page].cb();
            }
            if (tar->offset > 0)
            {
                app_main_touch_register(&app_funclist_main_touch_cb);
                fdata->hor_start = 0;
                fdata->hor_offset = 0;
                appdata->ver_page = VER_PAGE_TOP;
            }
            else
            {
                app_main_touch_register(&app_message_main_touch_cb);
                appdata->ver_page = VER_PAGE_BOTTOM;
                app_message_page_new_message(NULL);
            }

            move_updn_design_refr_param.wflag = 0x01 << app_message_main_refrsh_param.win_id;
        }

        move_updn_design_refr_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&move_updn_design_refr_param);
    }
    else
    {
        app_design_request(0, &move_updn_touch_design, param);

        move_updn_design_refr_param.wflag = (0x01 << main_page_refrsh_param.win_id) | (0x01 << app_message_main_refrsh_param.win_id);
        move_updn_design_refr_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&move_updn_design_refr_param);
    }

    return RT_EOK;
}

rt_err_t app_main_page_touch_move_updn(void *param)
{
    struct app_main_data_t        *pdata = (struct app_main_data_t *)param;
    struct app_message_main_data_t *msgdata = g_message_main_data;
    struct rt_touch_data *down_p   = &pdata->down_point[0];

    msgdata->mov_dir = pdata->ydir;
    msgdata->mov_offset = pdata->yoffset + MSG_WIN_YRES * pdata->ver_page;

    if (pdata->ver_page == VER_PAGE_TOP && msgdata->mov_offset <= 0)
        msgdata->mov_offset = 1;
    if (pdata->ver_page == VER_PAGE_BOTTOM && msgdata->mov_offset >= 0)
        msgdata->mov_offset = -1;
    if (pdata->ver_page == VER_PAGE_NULL)
    {
        if (down_p->y_coordinate < (DISP_YRES / 2) && msgdata->mov_offset <= 0)
            msgdata->mov_offset = 1;
        if (down_p->y_coordinate > (DISP_YRES / 2) && msgdata->mov_offset >= 0)
            msgdata->mov_offset = -1;
    }
    app_main_unregister_timeout_cb_if_is(app_show_charging_anim);
    app_main_unregister_timeout_cb_if_is(app_preview_enter);

    move_updn_refr_param.wflag = 0x01 << main_page_refrsh_param.win_id | 0x01 << app_message_main_refrsh_param.win_id;
    move_updn_refr_param.wait = RT_WAITING_FOREVER;
    move_updn_refr_param.id = ID_MSG;
    app_slide_refresh(&move_updn_refr_param);

    return RT_EOK;
}

//---------------------------------------------------------------------------------
// touch move(touch) up
//---------------------------------------------------------------------------------
static mov_design_param   move_up_design_param;

static rt_err_t app_main_page_touch_move_up(void *param)
{
    struct app_main_data_t *pdata = (struct app_main_data_t *)param;
    struct rt_touch_data *cur_p   = &pdata->cur_point[0];
    struct rt_touch_data *down_p   = &pdata->down_point[0];
    int tar_page;

    app_main_unregister_timeout_cb_if_is(app_show_charging_anim);
    app_main_unregister_timeout_cb_if_is(app_preview_enter);
    app_slide_refresh_undo();
    app_main_touch_unregister();
    if (pdata->dir_mode == TOUCH_DIR_MODE_LR)
    {
        if (ABS((int32_t)down_p->x_coordinate - (int32_t)cur_p->x_coordinate) >= CLOCK_WIN_XRES / 3)
        {
            move_up_design_param.dir = down_p->x_coordinate > cur_p->x_coordinate ? 1 : -1;
            tar_page = app_main_page_data->cur_page + move_up_design_param.dir;
        }
        else
        {
            move_up_design_param.dir = down_p->x_coordinate > cur_p->x_coordinate ? -1 : 1;
            tar_page = app_main_page_data->cur_page;
        }
        move_up_design_param.offset = CLOCK_WIN_XRES * tar_page;

        app_design_request(1, &move_lr_touch_design, &move_up_design_param);
    }
    else if (pdata->dir_mode == TOUCH_DIR_MODE_UPDN)
    {
        app_main_timer_cb_unregister();
        if (ABS((int32_t)down_p->y_coordinate - (int32_t)cur_p->y_coordinate) >= CLOCK_WIN_YRES / 6)
        {
            move_up_design_param.dir = down_p->y_coordinate > cur_p->y_coordinate ? -1 : 1;
            move_up_design_param.offset = CLOCK_WIN_YRES * move_up_design_param.dir;
        }
        else
        {
            move_up_design_param.dir = down_p->y_coordinate > cur_p->y_coordinate ? 1 : -1;
            move_up_design_param.offset = 0;
        }
        app_design_request(1, &move_updn_touch_design, &move_up_design_param);
    }
    else
    {
        app_main_touch_register(&main_page_touch_cb);
    }

    return RT_EOK;
}

struct app_touch_cb_t main_page_touch_cb =
{
    .tp_touch_down  = app_main_page_touch_down,
    .tp_move_lr     = app_main_page_touch_move_lr,
    .tp_move_updn   = app_main_page_touch_move_updn,
    .tp_move_up     = app_main_page_touch_move_up,
    .tp_touch_up    = app_main_page_touch_up,
};

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */
static void app_main_page_load_default_source(void)
{
    struct app_main_page_data_t *pdata = (struct app_main_page_data_t *)app_main_page_data;

    app_load_img(&img_page_bkg[0], (rt_uint8_t *)pdata->top_fb, CLOCK_WIN_FB_W, CLOCK_WIN_FB_H, 0 * CLOCK_WIN_XRES, CLOCK_WIN_COLOR_DEPTH >> 3);
    // app_load_img(&img_page_bkg[1], (rt_uint8_t *)pdata->top_fb, CLOCK_WIN_FB_W, CLOCK_WIN_FB_H, 1 * CLOCK_WIN_XRES, CLOCK_WIN_COLOR_DEPTH >> 3);
    // app_load_img(&img_page_bkg[2], (rt_uint8_t *)pdata->top_fb, CLOCK_WIN_FB_W, CLOCK_WIN_FB_H, 2 * CLOCK_WIN_XRES, CLOCK_WIN_COLOR_DEPTH >> 3);
    // app_load_img(&img_page_bkg[3], (rt_uint8_t *)pdata->top_fb, CLOCK_WIN_FB_W, CLOCK_WIN_FB_H, 3 * CLOCK_WIN_XRES, CLOCK_WIN_COLOR_DEPTH >> 3);
}

static void app_main_page_mem_init(void)
{
    struct app_main_page_data_t *pdata;

    app_main_page_data = pdata = (struct app_main_page_data_t *)rt_malloc(sizeof(struct app_main_page_data_t));
    RT_ASSERT(pdata != RT_NULL);
    rt_memset((void *)pdata, 0, sizeof(struct app_main_page_data_t));

    /* top framebuffer malloc, one more line for loop */
    pdata->top_fblen = CLOCK_WIN_FB_W * (CLOCK_WIN_FB_H + 1) * (CLOCK_WIN_COLOR_DEPTH >> 3);
    pdata->top_fb   = (rt_uint8_t *)rt_malloc_psram(pdata->top_fblen);
    RT_ASSERT(pdata->top_fb != RT_NULL);
    rt_memset((void *)(pdata->top_fb + CLOCK_WIN_FB_W * CLOCK_WIN_FB_H * (CLOCK_WIN_COLOR_DEPTH >> 3)),
              0, CLOCK_WIN_FB_W * 1 * (CLOCK_WIN_COLOR_DEPTH >> 3));

    pdata->bottom_fblen = CLOCK_WIN_XRES * CLOCK_WIN_YRES * (CLOCK_WIN_COLOR_DEPTH >> 3);
    pdata->bottom_fb   = (rt_uint8_t *)rt_malloc_psram(pdata->bottom_fblen);
    RT_ASSERT(pdata->bottom_fb != RT_NULL);
    rt_memset((void *)pdata->bottom_fb, 0, pdata->bottom_fblen);
}

void app_main_page_init(void)
{
    app_main_page_mem_init();

    // refresh win layers register
    main_page_refrsh_param.win_id    = APP_CLOCK_WIN_2;
    main_page_refrsh_param.win_layer = WIN_BOTTOM_LAYER;
    app_refresh_register(main_page_refrsh_param.win_id, app_main_page_refresh, &main_page_refrsh_param);

    app_message_main_refrsh_param.win_id    = APP_CLOCK_WIN_1;
    app_message_main_refrsh_param.win_layer = WIN_MIDDLE_LAYER;
    app_refresh_register(app_message_main_refrsh_param.win_id, app_message_main_refresh, &app_message_main_refrsh_param);

    app_charging_refrsh_param.win_id    = APP_CLOCK_WIN_1;
    app_charging_refrsh_param.win_layer = WIN_TOP_LAYER;

    if (g_app_info->cold_boot)
    {
        app_main_page_data->cur_page = CLOCK_PAGE_FOC_ID;
    }
    else
    {
        app_main_page_data->cur_page = g_app_info->page_index;
    }

    if (app_main_page_data->cur_page < 0 ||
            app_main_page_data->cur_page >= CLOCK_HOR_PAGE_MAX)
    {
        app_main_page_data->cur_page = CLOCK_PAGE_FOC_ID;
    }
    app_main_page_data->mov_offset = CLOCK_WIN_XRES * app_main_page_data->cur_page;
    g_app_info->page_index = app_main_page_data->cur_page;

    if (g_app_info->cold_boot)
    {
        rt_kprintf("Cold boot %lu %d\n", HAL_GetTick(), g_app_info->page_index);
    }
    else
    {
        refrsh_request_param_t request_param;
        request_param.wflag = 0x01 << main_page_refrsh_param.win_id;
        request_param.wait = RT_WAITING_FOREVER;
        app_refresh_now(&request_param);

        rt_kprintf("Warm boot %lu %d\n", HAL_GetTick(), g_app_info->page_index);
    }

#ifdef RT_USING_SDIO0
    rt_device_t sd_dev;
    do
    {
        sd_dev = rt_device_find("sd0");
        rt_thread_mdelay(1);
    }
    while (!sd_dev);
    while (dfs_filesystem_get_mounted_path(sd_dev) == NULL)
    {
        rt_thread_mdelay(1);
    }
    /* Make sure the first dta source is loaded after here */
#endif
    get_app_info(app_main_data);
    rt_display_backlight_set(app_main_data->bl);

    rk_imagelib_init();
    app_main_page_load_default_source();

    main_page_design_param_t design_param;
    rt_memset(&design_param, 0, sizeof(main_page_design_param_t));

    design_param.buf_id = 1;
    app_main_page_clock_init();
    app_main_page_clock_design(&design_param);

    design_param.buf_id = 2;
    app_main_page_weather_init();
    app_main_page_weather_design(&design_param);

    design_param.buf_id = 3;
    app_main_page_music_init();
    app_main_page_music_design(&design_param);

    refrsh_request_param_t request_param;
    request_param.wflag = 0x01 << main_page_refrsh_param.win_id;
    request_param.wait = RT_WAITING_FOREVER;
    app_refresh_now(&request_param);

    //register callback
    // app_main_timer_cb_register(app_main_page_clock_update);
    if (main_page_timer_cb[app_main_page_data->cur_page].cb)
    {
        app_main_timer_cb_register(main_page_timer_cb[app_main_page_data->cur_page].cb);
    }
    app_main_touch_register(&main_page_touch_cb);

#ifdef APP_WEARABLE_ANIM_AUTO_PLAY
    app_main_register_timeout_cb(auto_play_init, NULL, 1000);
#endif
}

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
 * Macro define
 *
 **************************************************************************************************
 */

#define FUNC_WIN_XRES2              DISP_XRES
#define FUNC_WIN_YRES2              DISP_XRES

#define SHOW_TICK                   0

/*
 **************************************************************************************************
 *
 * Global static struct & data define
 *
 **************************************************************************************************
 */
struct app_funclist_data_t *g_funclist_data = RT_NULL;
app_funclist_refrsh_param_t app_funclist_refrsh_param;

extern rt_err_t app_main_page_touch_move_updn(void *param);
extern rt_err_t app_message_touch_move_up(void *param);

/*
 **************************************************************************************************
 *
 * Display refresh
 *
 **************************************************************************************************
 */
static refrsh_request_param_t funclist_refrsh_request_param;
static void *old_callback;
static void *old_param;
static int scale_index;

rt_err_t app_funclist_refresh(struct rt_display_config *wincfg, void *param)
{
    struct rt_device_graphic_info *info = &app_main_data->disp->info;
    struct app_funclist_data_t *pdata = g_funclist_data;
    app_funclist_refrsh_param_t *par = (app_funclist_refrsh_param_t *)param;

    wincfg->winId   = par->win_id;
    wincfg->zpos    = par->win_layer;
    wincfg->colorkey = 0;
    wincfg->alphaEn = 0;
    wincfg->lut     = RT_NULL;
    wincfg->lutsize = 0;
    wincfg->fb     = pdata->fb[pdata->buf_id];
    wincfg->format = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    wincfg->fblen = pdata->fblen;
    wincfg->w     = FUNC_WIN_XRES;
    wincfg->h     = FUNC_WIN_YRES;
    wincfg->xVir  = wincfg->w;
    wincfg->x     = FUNC_REGION_X + ((info->width  - FUNC_WIN_XRES) / 2);
    wincfg->y     = FUNC_REGION_Y + ((info->height - FUNC_WIN_YRES) / 2);
    wincfg->ylast = wincfg->y;

    RT_ASSERT((wincfg->x + wincfg->w) <= info->width);
    RT_ASSERT((wincfg->y + wincfg->h) <= info->height);

    return RT_EOK;
}

static void app_func_anim_ending(int func_index)
{
    struct app_funclist_data_t *pdata = g_funclist_data;

    app_refresh_register(app_funclist_refrsh_param.win_id, old_callback, old_param);

    if (pdata->img_buflen)
        rt_free_large(pdata->img_buf);

    rt_free_psram(pdata->fb[0]);
    rt_free_psram(pdata->fb[1]);

    app_asr_start();
    app_func_enter(func_index);
}

static rt_err_t app_do_funclist_design(void *param);
design_cb_t do_funclist_design_t = {.cb = app_do_funclist_design,};
static rt_err_t app_do_funclist_design(void *param)
{
    struct app_funclist_data_t *pdata = g_funclist_data;
    struct image_st ips, ipd;
    int32_t start_x, start_y;
    rt_uint8_t buf_id;
    rt_uint8_t *fb;
    int func_index = (int)param;

    buf_id = (pdata->buf_id == 1) ? 0 : 1;
    fb = pdata->fb[buf_id];

#if SHOW_TICK
    uint32_t st, et;
    st = HAL_GetTick();
#endif

    if (scale_index < FUNC_ANIM_STEP)
    {
        ips.width = FUNC_WIN_XRES2;
        ips.height = FUNC_WIN_YRES - ((FUNC_ANIM_STEP - scale_index) * (FUNC_WIN_YRES - FUNC_WIN_YRES2) / FUNC_ANIM_STEP);
        ips.stride = ips.width * (16 >> 3);
        ips.pdata = pdata->img_buf;

        ipd.width = ips.width - ((FUNC_ANIM_STEP - scale_index) * (FUNC_WIN_XRES - FUNC_ANIM_START_W) / FUNC_ANIM_STEP);
        ipd.width = RT_ALIGN(ipd.width, 8);
        ipd.height = (uint32_t)(ips.height * ((float)ipd.width / ips.width));
        ipd.stride = FUNC_WIN_XRES * (FUNC_WIN_COLOR_DEPTH >> 3);
        start_x = RT_ALIGN(pdata->pos_x - (pdata->pos_x * scale_index / FUNC_ANIM_STEP), 8);
        start_y = RT_ALIGN(pdata->pos_y - (pdata->pos_y * scale_index / FUNC_ANIM_STEP), 8);

        if (start_x < 0)
        {
            start_x = 0;
        }
        if ((start_x + ipd.width) > FUNC_WIN_XRES)
        {
            start_x = FUNC_WIN_XRES - ipd.width;
        }
        if (start_y < 0)
        {
            start_y = 0;
        }
        if ((start_y + ipd.height) > FUNC_WIN_YRES)
        {
            start_y = FUNC_WIN_YRES - ipd.height;
        }

        ipd.pdata = fb + (start_y * ipd.stride) + start_x * (FUNC_WIN_COLOR_DEPTH >> 3);
        rk_scale_process(&ips, &ipd, TYPE_RGB565_565);

        scale_index++;
        funclist_refrsh_request_param.wflag = 0x01 << app_funclist_refrsh_param.win_id;
        funclist_refrsh_request_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&funclist_refrsh_request_param);
        app_design_request(0, &do_funclist_design_t, param);
    }
    else
    {
        app_func_anim_ending(func_index);
    }

#if SHOW_TICK
    et = HAL_GetTick();
    rt_kprintf("design %p %d [%lu ms] %d FPS\n", fb, scale_index - 1, et - st, 1000 / (et - st));
#endif

    pdata->buf_id = buf_id;

    return RT_EOK;
}

static rt_err_t app_start_funclist_design(void *param)
{
    struct app_funclist_data_t *pdata = (struct app_funclist_data_t *)g_funclist_data;
    void *preview;
    struct image_st ps, pd;
    uint32_t touch_padding = 0;
    uint32_t ofs = 0;
    int func_index;

    scale_index = 0;
    func_index  = app_lv_icon_index_check(pdata->point[0].x_coordinate, pdata->point[0].y_coordinate);
    if (func_index == -RT_ERROR)
    {
        app_asr_start();
        app_main_touch_register(&app_funclist_main_touch_cb);
        return RT_EOK;
    }
    func_index += (pdata->hor_offset / (FUNC_WIN_XRES / 2)) * FUNC_ICON_VER_NUM;

    pdata->fb[0] = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    pdata->fb[1] = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    RT_ASSERT(pdata->fb[0] != RT_NULL);
    RT_ASSERT(pdata->fb[1] != RT_NULL);
    pdata->buf_id = 0;

    ps.width  = FUNC_WIN_XRES;
    ps.height = FUNC_WIN_YRES;
    ps.stride = FUNC_WIN_FB_W * (FUNC_WIN_COLOR_DEPTH >> 3);
    ps.pdata = pdata->funclist_fb + ofs + pdata->hor_start * (FUNC_WIN_COLOR_DEPTH >> 3);

    pd.width  = ps.width;
    pd.height = ps.height;
    pd.stride = FUNC_WIN_XRES * (FUNC_WIN_COLOR_DEPTH >> 3);
    pd.pdata = pdata->fb[0];

    rk_image_copy(&ps, &pd, FUNC_WIN_COLOR_DEPTH >> 3);
    memcpy(pdata->fb[1], pdata->fb[0], pdata->fblen);

    touch_padding = ((FUNC_WIN_YRES - FUNC_WIN_YRES2) / 2);

    pdata->pos_x = pdata->point[0].x_coordinate - FUNC_ANIM_START_W / 2;
    pdata->pos_y = pdata->point[0].y_coordinate - FUNC_ANIM_START_H / 2;

    if (pdata->pos_x < 0)
    {
        pdata->pos_x = 0;
    }
    if ((pdata->pos_x + FUNC_ANIM_START_W) > FUNC_WIN_XRES2)
    {
        pdata->pos_x = FUNC_WIN_XRES2 - FUNC_ANIM_START_W;
    }
    if (pdata->pos_y < touch_padding)
    {
        pdata->pos_y = touch_padding;
    }
    if ((pdata->pos_y + FUNC_ANIM_START_H) > (FUNC_WIN_YRES2 + touch_padding))
    {
        pdata->pos_y = (FUNC_WIN_YRES2 + touch_padding) - FUNC_ANIM_START_H;
    }

    app_func_init(func_index);

    preview = app_func_get_preview(func_index);
    if (preview)
    {
        if ((((uint32_t)preview & 0xFF000000) == XIP_MAP0_BASE0) ||
                (((uint32_t)preview & 0xFF000000) == XIP_MAP1_BASE0) ||
                (((uint32_t)preview & 0xFF000000) == XIP_MAP1_BASE1))
        {
            // rt_kprintf("Preview in PSRAM or XIP\n");
            pdata->img_buflen = FUNC_WIN_XRES * FUNC_WIN_YRES * (FUNC_WIN_COLOR_DEPTH >> 3);
            pdata->img_buf = (rt_uint8_t *)rt_malloc_large(pdata->img_buflen);
            RT_ASSERT(pdata->img_buf != RT_NULL);
            memcpy(pdata->img_buf, app_func_get_preview(func_index), pdata->img_buflen);
        }
        else
        {
            // rt_kprintf("Preview in SRAM\n");
            pdata->img_buflen = 0;
            pdata->img_buf = preview;
        }

        app_get_refresh_callback(app_funclist_refrsh_param.win_id, &old_callback, &old_param);
        app_refresh_register(app_funclist_refrsh_param.win_id, app_funclist_refresh, &app_funclist_refrsh_param);

        app_design_request(0, &do_funclist_design_t, (void *)func_index);
    }
    else
    {
        pdata->img_buflen = 0;
        app_func_anim_ending(func_index);
    }

    return RT_EOK;
}
design_cb_t start_funclist_design_t = {.cb = app_start_funclist_design,};

/*
 **************************************************************************************************
 *
 * Touch press
 *
 **************************************************************************************************
 */
//---------------------------------------------------------------------------------
// touch touch up
//---------------------------------------------------------------------------------
rt_err_t app_funclist_page_show_funclist(void *param)
{
    struct app_funclist_data_t *pdata = (struct app_funclist_data_t *)g_funclist_data;

    app_asr_stop();
    app_main_touch_unregister();
    memcpy(&pdata->point[0], param, sizeof(struct rt_touch_data));

    app_design_request(0, &start_funclist_design_t, RT_NULL);

    return RT_EOK;
}

static rt_err_t app_funclist_touch_up(void *param)
{
    struct app_main_data_t *maindata = (struct app_main_data_t *)param;

    app_funclist_page_show_funclist(&maindata->cur_point[0]);

    return RT_EOK;
}

//---------------------------------------------------------------------------------
// touch move lr
//---------------------------------------------------------------------------------
static rt_err_t app_funclist_move_lr_design(void *param);
design_cb_t funclist_move_lr_design = { .cb = app_funclist_move_lr_design, };
static refrsh_request_param_t funclist_move_lr_refr_param;
static refrsh_request_param_t clkmov_refrsh_request_param;
static rt_err_t app_funclist_move_lr_design(void *param)
{
    struct app_funclist_data_t *fdata = g_funclist_data;
    mov_design_param *tar = (mov_design_param *)param;

    fdata->hor_offset += TOUCH_MOVE_STEP * tar->dir;
    if ((tar->dir > 0 && (fdata->hor_offset) >= tar->offset) ||
            (tar->dir < 0 && (fdata->hor_offset) <= tar->offset))
    {
        fdata->hor_start = fdata->hor_offset = tar->offset;

        app_main_touch_register(&app_funclist_main_touch_cb);
    }
    else
    {
        app_design_request(0, &funclist_move_lr_design, param);
    }

    funclist_move_lr_refr_param.wflag = 0x01 << app_funclist_refrsh_param.win_id;
    funclist_move_lr_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&funclist_move_lr_refr_param);

    return RT_EOK;
}

static rt_err_t app_funclist_touch_move_lr(void *param)
{
    struct app_main_data_t  *pdata = (struct app_main_data_t *)param;
    struct app_funclist_data_t *fdata = g_funclist_data;

    fdata->hor_offset = fdata->hor_start - pdata->xoffset;

    if (pdata->smooth_design == 0)
    {
        pdata->smooth_design = 1;
        clkmov_refrsh_request_param.wflag = 0x01 << app_funclist_refrsh_param.win_id;
        clkmov_refrsh_request_param.wait = RT_WAITING_FOREVER;
        clkmov_refrsh_request_param.id = ID_FUNCLIST;
        app_registe_refresh_done_cb(app_main_touch_smooth_design,
                                    (void *)&clkmov_refrsh_request_param);
        app_refresh_request(&clkmov_refrsh_request_param);
    }

    return RT_EOK;
}

//---------------------------------------------------------------------------------
// touch move up
//---------------------------------------------------------------------------------
static mov_design_param touch_moveup_design_param;
rt_err_t app_funclist_touch_move_up(void *param)
{
    struct app_main_data_t *pdata = (struct app_main_data_t *)param;
    struct app_funclist_data_t *fdata = g_funclist_data;
    int16_t floor_ofs, ceil_ofs;

    if (pdata->dir_mode == TOUCH_DIR_MODE_UPDN)
    {
        return app_message_touch_move_up(param);
    }
    else if (pdata->dir_mode == TOUCH_DIR_MODE_LR)
    {
        app_registe_refresh_done_cb(NULL, NULL);
        app_main_touch_unregister();

        if (fdata->hor_offset < 0)
        {
            fdata->hor_offset = 0;
        }
        if (fdata->hor_offset > (FUNC_WIN_FB_W - FUNC_WIN_XRES))
        {
            fdata->hor_offset = FUNC_WIN_FB_W - FUNC_WIN_XRES;
        }

        floor_ofs = (fdata->hor_offset / (FUNC_WIN_XRES / 2) + 1) * (FUNC_WIN_XRES / 2);
        ceil_ofs  =  fdata->hor_offset / (FUNC_WIN_XRES / 2) * (FUNC_WIN_XRES / 2);

        if ((floor_ofs - fdata->hor_offset) < (fdata->hor_offset - ceil_ofs))
        {
            touch_moveup_design_param.dir = 1;
            touch_moveup_design_param.offset = floor_ofs;
        }
        else
        {
            touch_moveup_design_param.dir = -1;
            touch_moveup_design_param.offset = ceil_ofs;
        }

        app_design_request(0, &funclist_move_lr_design, &touch_moveup_design_param);
    }

    return RT_EOK;
}

struct app_touch_cb_t app_funclist_main_touch_cb =
{
    .tp_move_updn   = app_main_page_touch_move_updn,
    .tp_move_lr     = app_funclist_touch_move_lr,
    .tp_move_up     = app_funclist_touch_move_up,
    .tp_touch_up    = app_funclist_touch_up,
};

/*
 **************************************************************************************************
 *
 * initial
 *
 **************************************************************************************************
 */
static const char *icons_path[FUNC_HOR_PAGE_MAX][FUNC_ICON_VER_NUM * FUNC_ICON_HOR_NUM] =
{
    // {
    //     "L:"ICONS_PATH"/message.bin", "L:"ICONS_PATH"/weather.bin", "L:"ICONS_PATH"/bus_card.bin",
    //     "L:"ICONS_PATH"/alipay.bin", "L:"ICONS_PATH"/stopwatch.bin", "L:"ICONS_PATH"/clock_timer.bin"
    // },
    {
        "L:"ICONS_PATH"/heart_rate.bin", "L:"ICONS_PATH"/physical_exercise.bin", "L:"ICONS_PATH"/system_menu.bin",
        "L:"ICONS_PATH"/altitude_barometer.bin", "L:"ICONS_PATH"/compass.bin", "L:"ICONS_PATH"/pressure.bin"
    },
    // {
    //     "L:"ICONS_PATH"/alarm_clock.bin", "L:"ICONS_PATH"/sleep.bin", "L:"ICONS_PATH"/no_disturb.bin",
    //     "L:"ICONS_PATH"/screen_display.bin", "L:"ICONS_PATH"/flashlight.bin", "L:"ICONS_PATH"/find_phone.bin"
    // },
    {
        "L:"ICONS_PATH"/activity_record.bin", "L:"ICONS_PATH"/exercise_record.bin", "L:"ICONS_PATH"/training_status.bin",
        "L:"ICONS_PATH"/user_guide.bin", "L:"ICONS_PATH"/about_watche.bin", "L:"ICONS_PATH"/breathing_training.bin"
    }
};
static rt_err_t app_funclist_init_design(void *param)
{
    struct app_funclist_data_t *pdata;
    struct app_lvgl_iconarray_design iconarray;

    g_funclist_data = pdata = (struct app_funclist_data_t *)rt_malloc(sizeof(struct app_funclist_data_t));
    RT_ASSERT(pdata != RT_NULL);
    rt_memset((void *)pdata, 0, sizeof(struct app_funclist_data_t));

    /* framebuffer malloc */
    pdata->fblen = FUNC_WIN_XRES * FUNC_WIN_YRES * (FUNC_WIN_COLOR_DEPTH >> 3);

    app_funclist_refrsh_param.win_id    = APP_CLOCK_WIN_1;
    app_funclist_refrsh_param.win_layer = WIN_BOTTOM_LAYER;

    pdata->funclist_fblen = FUNC_WIN_FB_W * FUNC_WIN_FB_H * (FUNC_WIN_COLOR_DEPTH >> 3);
    pdata->funclist_fb   = (rt_uint8_t *)rt_malloc_psram(pdata->funclist_fblen);
    RT_ASSERT(pdata->funclist_fb != RT_NULL);
    rt_memset((void *)pdata->funclist_fb, 0x0, pdata->funclist_fblen);

    for (int i = 0; i < FUNC_HOR_PAGE_MAX; i++)
    {
        iconarray.icons_num = 6;
        for (int j = 0; j < iconarray.icons_num; j++)
        {
            iconarray.path[j] = (char *)icons_path[i][j];
        }
        iconarray.img[0].width = FUNC_WIN_XRES;
        iconarray.img[0].height = FUNC_WIN_YRES;
        iconarray.img[0].stride = FUNC_WIN_FB_W * (FUNC_WIN_COLOR_DEPTH >> 3);
        iconarray.img[0].pdata = pdata->funclist_fb + i * FUNC_WIN_XRES * (FUNC_WIN_COLOR_DEPTH >> 3);
        iconarray.ping_pong = 0;
        app_lv_iconarray_design(&iconarray);
    }

    return RT_EOK;
}
static design_cb_t  app_funclist_init_design_t = {.cb = app_funclist_init_design,};

void app_funclist_init(void)
{
    app_design_request(0, &app_funclist_init_design_t, RT_NULL);
}
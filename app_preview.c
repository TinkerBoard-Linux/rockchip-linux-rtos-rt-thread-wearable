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

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */

/*
 **************************************************************************************************
 *
 * Global static struct & data define
 *
 **************************************************************************************************
 */
struct app_preview_data_t *g_preview_data = RT_NULL;
app_disp_refrsh_param_t app_preview_refrsh_param;

/*
 **************************************************************************************************
 *
 * Declaration
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
static refrsh_request_param_t preview_refrsh_request_param;
static void *old_callback;
static void *old_param;

rt_err_t app_preview_refresh(struct rt_display_config *wincfg, void *param)
{
    struct rt_device_graphic_info *info = &app_main_data->disp->info;
    struct app_preview_data_t *pdata = g_preview_data;
    app_disp_refrsh_param_t *par = (app_disp_refrsh_param_t *)param;

    if (pdata->hor_offset < 0)
        pdata->hor_offset = 0;
    if (pdata->hor_offset > (PREVIEW_WIN_FB_W - PREVIEW_WIN_XRES))
        pdata->hor_offset = PREVIEW_WIN_FB_W - PREVIEW_WIN_XRES;

    wincfg->winId   = par->win_id;
    wincfg->zpos    = par->win_layer;
    wincfg->colorkey = 0;
    wincfg->alphaEn = 0;
    wincfg->lut     = RT_NULL;
    wincfg->lutsize = 0;
    wincfg->fb     = pdata->fb + pdata->hor_offset * (PREVIEW_WIN_COLOR_DEPTH >> 3);
    wincfg->format = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    wincfg->fblen = pdata->fblen;
    wincfg->w     = PREVIEW_WIN_XRES;
    wincfg->h     = PREVIEW_WIN_YRES;
    wincfg->xVir  = PREVIEW_WIN_FB_W;
    wincfg->x     = PREVIEW_REGION_X + ((info->width  - PREVIEW_WIN_XRES) / 2);
    wincfg->y     = PREVIEW_REGION_Y + ((info->height - PREVIEW_WIN_YRES) / 2);
    wincfg->ylast = wincfg->y;

    RT_ASSERT((wincfg->x + wincfg->w) <= info->width);
    RT_ASSERT((wincfg->y + wincfg->h) <= info->height);

    return RT_EOK;
}

static rt_err_t app_preview_touch_up(void *param)
{
    struct app_preview_data_t *fdata = g_preview_data;

    if (fdata->skip_touchup)
    {
        fdata->skip_touchup--;
        return RT_EOK;
    }

    app_main_data->clock_style = fdata->hor_offset / (PREVIEW_WIN_SHOW_X + PREVIEW_GAP);
    if (app_main_data->clock_style >= CLOCK_STYLE_MAX_NUM)
    {
        app_main_data->clock_style = CLOCK_STYLE_MAX_NUM;
    }
    save_app_info(app_main_data);

#if 0
    /* TEST ONLY */
    struct image_st ips, ipd;
    ipd.width = CLOCK_WIN_XRES;
    ipd.height = CLOCK_WIN_YRES;
    ipd.stride = CLOCK_WIN_FB_W * (CLOCK_WIN_COLOR_DEPTH >> 3);
    ipd.pdata = app_main_page_data->top_fb + CLOCK_WIN_XRES * (CLOCK_WIN_COLOR_DEPTH >> 3);

    rk_image_reset(&ipd, CLOCK_WIN_COLOR_DEPTH >> 3);

    ips.width = PREVIEW_WIN_SHOW_X;
    ips.height = PREVIEW_WIN_SHOW_Y;
    ips.stride = PREVIEW_WIN_FB_W * (PREVIEW_WIN_COLOR_DEPTH >> 3);
    ips.pdata = fdata->fb + (fdata->hor_offset + PREVIEW_GAP) * (PREVIEW_WIN_COLOR_DEPTH >> 3);

    ipd.width = CLOCK_WIN_XRES;
    ipd.height = (ipd.width * ips.height) / ips.width;
    ipd.stride = CLOCK_WIN_FB_W * (CLOCK_WIN_COLOR_DEPTH >> 3);
    ipd.pdata = app_main_page_data->top_fb + (CLOCK_WIN_FB_W * (CLOCK_WIN_YRES - ipd.height) / 2 + CLOCK_WIN_XRES) * (CLOCK_WIN_COLOR_DEPTH >> 3);

    rk_scale_process(&ips, &ipd, TYPE_RGB565_565);

    /* TEST ONLY END */
#else
    main_page_design_param_t clock_param = { .buf_id = 1};
    app_clock_init();
    app_clock_design(&clock_param);
#endif

    rt_free_psram(fdata->fb);

    app_refresh_register(app_preview_refrsh_param.win_id, old_callback, old_param);

    app_main_touch_unregister();
    app_main_touch_register(&main_page_touch_cb);

    app_main_timer_cb_register(app_main_page_clock_update);

    preview_refrsh_request_param.wflag = 0x01 << main_page_refrsh_param.win_id;
    preview_refrsh_request_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&preview_refrsh_request_param);

    return RT_EOK;
}

static refrsh_request_param_t clkmov_refrsh_request_param;
static rt_err_t app_preview_touch_move_lr(void *param)
{
    struct app_main_data_t  *pdata = (struct app_main_data_t *)param;
    struct app_preview_data_t *fdata = g_preview_data;

    fdata->hor_offset = fdata->hor_start - pdata->xoffset;

    clkmov_refrsh_request_param.wflag = 0x01 << app_preview_refrsh_param.win_id;
    clkmov_refrsh_request_param.wait = RT_WAITING_FOREVER;
    clkmov_refrsh_request_param.id = ID_FUNC_LR;
    app_slide_refresh(&clkmov_refrsh_request_param);

    return RT_EOK;
}
static rt_err_t app_preview_move_lr_design(void *param);
design_cb_t preview_move_lr_design = { .cb = app_preview_move_lr_design, };
static refrsh_request_param_t preview_move_lr_refr_param;

static rt_err_t app_preview_move_lr_design(void *param)
{
    struct app_preview_data_t *fdata = g_preview_data;
    mov_design_param *tar = (mov_design_param *)param;

    fdata->hor_offset += TOUCH_MOVE_STEP * tar->dir;
    if ((tar->dir > 0 && (fdata->hor_offset) >= tar->offset) ||
            (tar->dir < 0 && (fdata->hor_offset) <= tar->offset))
    {
        fdata->hor_start = fdata->hor_offset = tar->offset;

        app_main_touch_register(&app_preview_main_touch_cb);
    }
    else
    {
        app_design_request(0, &preview_move_lr_design, param);
    }

    preview_move_lr_refr_param.wflag = 0x01 << app_preview_refrsh_param.win_id;
    preview_move_lr_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&preview_move_lr_refr_param);

    return RT_EOK;
}

static mov_design_param touch_moveup_design_param;
rt_err_t app_preview_touch_move_up(void *param)
{
    struct app_main_data_t *pdata = (struct app_main_data_t *)param;
    struct app_preview_data_t *fdata = g_preview_data;
    int16_t floor_ofs, ceil_ofs;

    if (pdata->dir_mode == TOUCH_DIR_MODE_LR)
    {
        app_slide_refresh_undo();
        app_main_touch_unregister();

        if (fdata->hor_offset < 0)
            fdata->hor_offset = 0;
        if (fdata->hor_offset > (PREVIEW_WIN_FB_W - PREVIEW_WIN_XRES))
            fdata->hor_offset = PREVIEW_WIN_FB_W - PREVIEW_WIN_XRES;

        floor_ofs = (fdata->hor_offset / (PREVIEW_WIN_SHOW_X + PREVIEW_GAP) + 1) * (PREVIEW_WIN_SHOW_X + PREVIEW_GAP);
        ceil_ofs  =  fdata->hor_offset / (PREVIEW_WIN_SHOW_X + PREVIEW_GAP) * (PREVIEW_WIN_SHOW_X + PREVIEW_GAP);

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

        // rt_kprintf("floor %d(%d) ceil %d(%d)\n", floor_ofs, floor_ofs - fdata->hor_offset, ceil_ofs, fdata->hor_offset - ceil_ofs);
        // rt_kprintf("from %d to %d cur %d\n", fdata->hor_start, touch_moveup_design_param.offset, fdata->hor_offset);
        app_design_request(0, &preview_move_lr_design, &touch_moveup_design_param);
    }

    return RT_EOK;
}

struct app_touch_cb_t app_preview_main_touch_cb =
{
    .tp_move_lr     = app_preview_touch_move_lr,
    .tp_move_up     = app_preview_touch_move_up,
    .tp_touch_up    = app_preview_touch_up,
};

static rt_err_t app_preview_enter_do(void *param)
{
    struct app_preview_data_t *pdata = g_preview_data;

    /* framebuffer malloc */
    pdata->fblen = PREVIEW_WIN_FB_W * PREVIEW_WIN_FB_H * (PREVIEW_WIN_COLOR_DEPTH >> 3);

    app_preview_refrsh_param.win_id    = APP_CLOCK_WIN_2;
    app_preview_refrsh_param.win_layer = WIN_BOTTOM_LAYER;

    pdata->fb   = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    RT_ASSERT(pdata->fb != RT_NULL);
    rt_memset((void *)pdata->fb, 0x0, pdata->fblen);

    img_load_info_t img_page_bkg = { PREVIEW_WIN_FB_W, PREVIEW_WIN_FB_H, USERDATA_PATH"preview.dta"};
    app_load_img(&img_page_bkg, (rt_uint8_t *)pdata->fb, PREVIEW_WIN_FB_W, PREVIEW_WIN_FB_H, 0, PREVIEW_WIN_COLOR_DEPTH >> 3);

    pdata->hor_start = pdata->hor_offset = app_main_data->clock_style * (PREVIEW_WIN_SHOW_X + PREVIEW_GAP);
    pdata->skip_touchup = 1;

    app_main_touch_register(&app_preview_main_touch_cb);

    app_get_refresh_callback(app_preview_refrsh_param.win_id, &old_callback, &old_param);
    app_refresh_register(app_preview_refrsh_param.win_id, app_preview_refresh, &app_preview_refrsh_param);

    preview_refrsh_request_param.wflag = 0x01 << app_preview_refrsh_param.win_id;
    preview_refrsh_request_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&preview_refrsh_request_param);

    return RT_EOK;
}
static design_cb_t  app_preview_enter_design_t = {.cb = app_preview_enter_do,};

rt_err_t app_preview_enter(void *param)
{
    app_design_request(0, &app_preview_enter_design_t, RT_NULL);

    return RT_EOK;
}

static rt_err_t app_preview_init_design(void *param)
{
    struct app_preview_data_t *pdata;

    g_preview_data = pdata = (struct app_preview_data_t *)rt_malloc(sizeof(struct app_preview_data_t));
    RT_ASSERT(pdata != RT_NULL);
    rt_memset((void *)pdata, 0, sizeof(struct app_preview_data_t));

    return RT_EOK;
}
static design_cb_t  app_preview_init_design_t = {.cb = app_preview_init_design,};

void app_preview_init(void)
{
    app_design_request(0, &app_preview_init_design_t, RT_NULL);
}

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
#include "lib_imageprocess.h"

#define SHOW_TICK           0

uint32_t m_bpp_lut[256] = {0};
static int g_first_dta;

struct app_charging_data_t *g_charging_data = RT_NULL;
app_disp_refrsh_param_t app_charging_refrsh_param;
app_disp_refrsh_param_t app_charging_refrsh_lut_param;
static void app_charing_anim_unload(void);

#if SHOW_TICK
uint32_t anim_st, anim_et;
uint32_t algo_t = 0;
#endif

static int anim_index = 1;
static refrsh_request_param_t clock_anim_refr_param;
static rt_err_t app_charging_anim_design(void *param);
design_cb_t charging_anim_design_t = {.cb = app_charging_anim_design,};
static rt_err_t app_charging_anim_design(void *param)
{
    struct app_charging_data_t *pdata = g_charging_data;
    rt_uint8_t buf_id;
    rt_uint8_t *fb;

    rt_uint8_t *src_buf;
    rt_uint32_t src_size;
    rt_uint32_t dec_size;
    int lut_index;

    if (pdata->enable == 0 && (app_charging_refrsh_param.win_id == APP_CLOCK_WIN_0))
    {
        app_refresh_unregister(app_charging_refrsh_param.win_id);
        app_refresh_unregister(app_charging_refrsh_lut_param.win_id);
        app_refresh_register(main_page_refrsh_param.win_id, app_main_page_refresh, &main_page_refrsh_param);
        app_refresh_register(app_message_main_refrsh_param.win_id, app_message_main_refresh, &app_message_main_refrsh_param);

        clock_anim_refr_param.wflag = 0x01 << main_page_refrsh_param.win_id | 0x01 << app_message_main_refrsh_param.win_id;
        clock_anim_refr_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&clock_anim_refr_param);

        app_main_touch_unregister();
        app_main_touch_register(&main_page_touch_cb);
        // app_main_timer_cb_register(app_main_page_clock_update);
        if (main_page_timer_cb[app_main_page_data->cur_page].cb)
        {
            app_main_timer_cb_register(main_page_timer_cb[app_main_page_data->cur_page].cb);
        }

        rt_free_psram(pdata->fb[0]);
        rt_free_psram(pdata->fb[1]);
        app_charing_anim_unload();

        app_asr_start();

        return RT_EOK;
    }

    buf_id = (pdata->buf_id == 1) ? 0 : 1;
    fb = pdata->fb[buf_id];

    lut_index = anim_index;/* + 1;
    if (lut_index > CHARGING_ANIM_STEP)
        lut_index -= CHARGING_ANIM_STEP;*/

#if SHOW_TICK
    uint32_t st, et;
    st = HAL_GetTick();
#endif
    for (int i = 0; i < 256; i++)
    {
        m_bpp_lut[i] = pdata->anim_lut[lut_index][i];
    }

    dec_size = CHARGING_WIN_FB_W * CHARGING_WIN_FB_H;
    if (!g_first_dta)
    {
        src_size = pdata->anim_buflen[anim_index];
        src_buf = (rt_uint8_t *)pdata->anim_buf[anim_index];
        rk_lzw_decompress_dsp(src_buf, src_size, fb, dec_size);
    }
    else
    {
        memset(fb, 0x0, dec_size);
        g_first_dta = 0;
    }

#if SHOW_TICK
    et = HAL_GetTick();
    algo_t += et - st;
#endif

    app_charging_refrsh_lut_param.win_id = app_charging_refrsh_param.win_id;
    app_charging_refrsh_param.win_id = app_charging_refrsh_param.win_id == APP_CLOCK_WIN_1 ? APP_CLOCK_WIN_0 : APP_CLOCK_WIN_1;

    app_refresh_register(app_charging_refrsh_param.win_id, app_charging_win_refresh, &app_charging_refrsh_param);
    app_refresh_register(app_charging_refrsh_lut_param.win_id, app_charging_win_refresh_lut, &app_charging_refrsh_lut_param);

    clock_anim_refr_param.wflag = 0x01 << APP_CLOCK_WIN_1 | 0x01 << APP_CLOCK_WIN_0;
    clock_anim_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&clock_anim_refr_param);

#if SHOW_TICK
    if (anim_index == CHARGING_ANIM_LOOP_START)
        anim_st = HAL_GetTick();
#endif
    anim_index++;
    if (anim_index >= CHARGING_ANIM_STEP)
    {
        anim_index = CHARGING_ANIM_LOOP_START;
#if SHOW_TICK
        anim_et = HAL_GetTick();
        rt_kprintf("Design %p [%lu ms] FPS %d:algo %lu ms\n", fb, anim_et - anim_st,
                   1000 * (CHARGING_ANIM_STEP - CHARGING_ANIM_LOOP_START) / (anim_et - anim_st), algo_t);
        algo_t = 0;
#endif
    }

    rt_thread_mdelay(20);
    app_design_request(0, &charging_anim_design_t, RT_NULL);

    pdata->buf_id = buf_id;

    return RT_EOK;
}

rt_err_t app_charging_win_refresh(struct rt_display_config *wincfg, void *param)
{
    struct rt_device_graphic_info *info = &app_main_data->disp->info;
    struct app_charging_data_t *pdata = g_charging_data;
    app_disp_refrsh_param_t *par = (app_disp_refrsh_param_t *)param;

    // rt_kprintf("%s %d\n", __func__, par->win_id);
    wincfg->winId   = par->win_id;
    wincfg->zpos    = WIN_MIDDLE_LAYER;
    wincfg->colorkey = 0;//COLOR_KEY_EN | 0;
    wincfg->alphaEn = 0;
    wincfg->format  = RTGRAPHIC_PIXEL_FORMAT_RGB332;
    wincfg->lut     = m_bpp_lut;
    wincfg->lutsize = sizeof(m_bpp_lut) / sizeof(m_bpp_lut[0]);
    wincfg->new_lut = 1;
    wincfg->fb    = pdata->fb[pdata->buf_id];
    wincfg->fblen = pdata->fblen;
    wincfg->xVir  = CHARGING_WIN_FB_W;
    wincfg->w     = CHARGING_WIN_XRES;
    wincfg->h     = CHARGING_WIN_YRES;
    wincfg->x     = CHARGING_REGION_X + ((info->width  - CHARGING_WIN_XRES) / 2);
    wincfg->y     = CHARGING_REGION_Y + ((info->height - CHARGING_WIN_YRES) / 2);
    wincfg->ylast = wincfg->y;

    return RT_EOK;
}

rt_err_t app_charging_win_refresh_lut(struct rt_display_config *wincfg, void *param)
{
    struct rt_device_graphic_info *info = &app_main_data->disp->info;
    struct app_charging_data_t *pdata = g_charging_data;
    app_disp_refrsh_param_t *par = (app_disp_refrsh_param_t *)param;

    // rt_kprintf("%s %d\n", __func__, par->win_id);
    wincfg->winId   = par->win_id;
    wincfg->zpos    = WIN_BOTTOM_LAYER;
    wincfg->colorkey = 0;//COLOR_KEY_EN | 0;
    wincfg->alphaEn = 0;
    wincfg->format  = RTGRAPHIC_PIXEL_FORMAT_RGB332;
    wincfg->lut     = m_bpp_lut;
    wincfg->lutsize = sizeof(m_bpp_lut) / sizeof(m_bpp_lut[0]);
    wincfg->hide_win = 1;
    wincfg->fb    = pdata->fb[pdata->buf_id];
    wincfg->fblen = pdata->fblen;
    wincfg->xVir  = CHARGING_WIN_FB_W;
    wincfg->w     = CHARGING_WIN_XRES;
    wincfg->h     = 2;
    wincfg->x     = CHARGING_REGION_X + ((info->width  - CHARGING_WIN_XRES) / 2);
    wincfg->x     = CHARGING_REGION_Y + ((info->height - CHARGING_WIN_YRES) / 2);
    wincfg->ylast = wincfg->y;

    return RT_EOK;
}

rt_err_t app_charging_touch_up(void *param)
{
    struct app_charging_data_t *pdata = g_charging_data;

    if (pdata->skip_touchup == 0)
        pdata->enable = 0;
    else
        pdata->skip_touchup--;

    return RT_EOK;
}

struct app_touch_cb_t app_charging_touch_cb =
{
    .tp_touch_up = app_charging_touch_up,
};

static void app_charing_anim_unload(void)
{
    struct app_charging_data_t *pdata = g_charging_data;

    rt_free_psram(pdata->anim_dta);
    pdata->anim_loaded = 0;
}

static void app_charing_anim_load_source(void)
{
    struct app_charging_data_t *pdata = g_charging_data;
    char dta_path[DFS_PATH_MAX * 2];
    struct stat dta_buf;
    uint32_t ofs;
    FILE *fd;

    if (pdata->anim_loaded)
        return;

    // uint32_t st, et;
    // st = HAL_GetTick();

    snprintf(dta_path, sizeof(dta_path), ANIM_SRCPATH"/charging.dta%c", '\0');
    if (stat(dta_path, &dta_buf) == 0)
    {
        pdata->anim_dta = rt_malloc_psram(dta_buf.st_size);
        RT_ASSERT(pdata->anim_dta != NULL);
        fd = fopen(dta_path, "r");
        fread(pdata->anim_dta, 1, dta_buf.st_size, fd);
        fclose(fd);
    }

    ofs = 0;
    for (int i = 0; i < CHARGING_ANIM_STEP; i++)
    {
        pdata->anim_lut[i] = (uint32_t *)(pdata->anim_dta + ofs);
        ofs += sizeof(m_bpp_lut);
        pdata->anim_buflen[i] = *(uint32_t *)(pdata->anim_dta + ofs);
        ofs += sizeof(uint32_t);
        pdata->anim_buf[i] = pdata->anim_dta + ofs;
        ofs += pdata->anim_buflen[i];
    }

    // et = HAL_GetTick();
    // rt_kprintf("%s %d cast %lu ms\n", dta_path, dta_buf.st_size, et - st);

    pdata->anim_loaded = 1;
}

static rt_err_t app_start_charging_anim(void *param)
{
    struct app_charging_data_t *pdata = g_charging_data;

    app_asr_stop();
    app_charing_anim_load_source();

    pdata->fb[0] = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    pdata->fb[1] = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    RT_ASSERT(pdata->fb[0] != RT_NULL);
    RT_ASSERT(pdata->fb[1] != RT_NULL);
    pdata->buf_id = 0;

    g_first_dta = 1;
    app_message_page_exit();
    app_main_timer_cb_unregister();
    app_main_touch_unregister();
    app_main_touch_register(&app_charging_touch_cb);

    app_refresh_register(APP_CLOCK_WIN_0, app_charging_win_refresh, &app_charging_refrsh_param);
    app_refresh_register(APP_CLOCK_WIN_1, app_charging_win_refresh, &app_charging_refrsh_param);

    app_design_request(0, &charging_anim_design_t, RT_NULL);

    return RT_EOK;
}
design_cb_t start_charging_anim_t = {.cb = app_start_charging_anim,};

rt_err_t app_charging_enable(void *param)
{
    struct app_charging_data_t *pdata = g_charging_data;

    if (pdata->enable == 0)
    {
        pdata->enable = 1;
        app_design_request(0, &start_charging_anim_t, RT_NULL);
    }

    return RT_EOK;
}

MSH_CMD_EXPORT(app_charging_enable, charging);

static rt_err_t app_charging_init_design(void *param)
{
    struct app_charging_data_t *pdata;

    g_charging_data = pdata = (struct app_charging_data_t *)rt_malloc(sizeof(struct app_charging_data_t));
    RT_ASSERT(pdata != RT_NULL);
    rt_memset((void *)pdata, 0, sizeof(struct app_charging_data_t));

    /* framebuffer malloc */
    pdata->fblen = CHARGING_WIN_FB_W * CHARGING_WIN_FB_H * (CHARGING_WIN_COLOR_DEPTH >> 3);
    pdata->enable = 0;
    pdata->anim_loaded = 0;
    pdata->skip_touchup = 0;

    return RT_EOK;
}
static design_cb_t  app_charging_init_design_t = {.cb = app_charging_init_design,};

/**
 * App clock fast init.
 */
void app_charging_init(void)
{
    app_design_request(0, &app_charging_init_design_t, RT_NULL);
}

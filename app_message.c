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

#include <littlevgl2rtt.h>
#include <lvgl/lvgl.h>

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

#define SHOW_TICK   0

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
struct app_message_main_data_t *g_message_main_data = RT_NULL;
app_message_main_refrsh_param_t app_message_main_refrsh_param;

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
static int scale_index = 0;
static int mov_index = MSG_ANIM_STEP;
struct app_lvgl_label_design g_name;
struct app_lvgl_label_design g_no_message;
struct image_st g_pre_tips = {0, 0, 0, NULL};
struct image_st g_pre_area[2] = {{0, 0, 0, NULL}, {0, 0, 0, NULL}};
struct image_st g_pre_txt[2] = {{0, 0, 0, NULL}, {0, 0, 0, NULL}};
static refrsh_request_param_t clock_move_updn_refr_param;

static rt_err_t app_lv_new_message_design(void *param);
design_cb_t lv_new_message_design_t = {.cb = app_lv_new_message_design,};
static rt_err_t app_lv_new_message_design(void *param)
{
    struct app_message_main_data_t *pdata = g_message_main_data;
    struct app_main_data_t *maindata = app_main_data;
    struct image_st ips, ipd;
    uint32_t start_x, start_y;
    rt_uint8_t buf_id;
    rt_uint8_t *fb;

    if (maindata->touch_event != RT_TOUCH_EVENT_UP)
        return RT_EOK;

    buf_id = (pdata->buf_id == 1) ? 0 : 1;
    fb = pdata->fb[buf_id];

#if SHOW_TICK
    uint32_t st, et;
    st = HAL_GetTick();
#endif

    ips.width = MSG_LOGO_BIG_W;
    ips.height = MSG_LOGO_BIG_H;
    ips.stride = ips.width * 2;
    ips.pdata = pdata->logo_buf;

    ipd.width = ips.width - (MSG_ANIM_STEP - scale_index) * 6;
    ipd.height = (uint32_t)(ips.height * ((float)ipd.width / ips.width));
    ipd.stride = MSG_WIN_FB_W * (MSG_WIN_COLOR_DEPTH >> 3);
    start_y = MSG_LOGO_Y_START - (int)((float)scale_index * (MSG_LOGO_Y_START - MSG_LOGO_Y_MID) / MSG_ANIM_STEP);
    start_x = (MSG_WIN_XRES - ipd.width) / 2;
    ipd.pdata = fb + start_y * ipd.stride + start_x * (MSG_WIN_COLOR_DEPTH >> 3);
    // if (g_pre_area[buf_id].pdata != NULL)
    //     rk_image_reset(&g_pre_area[buf_id], MSG_WIN_COLOR_DEPTH >> 3);
    if (g_pre_txt[buf_id].pdata != NULL)
        rk_image_reset(&g_pre_txt[buf_id], MSG_WIN_COLOR_DEPTH >> 3);
    memcpy(&g_pre_area[buf_id], &ipd, sizeof(struct image_st));

    rk_scale_process(&ips, &ipd, TYPE_RGB565_565);

    ipd.width = g_name.img[0].width + scale_index * 4;//(uint32_t)(g_name.img[0].width * ((float)ipd.width / 2 / ips.width));
    ipd.height = (uint32_t)(g_name.img[0].height * ((float)ipd.width / g_name.img[0].width));
    ipd.stride = MSG_WIN_FB_W * (MSG_WIN_COLOR_DEPTH >> 3);
    start_y = MSG_NAME_Y_START - (int)((float)scale_index * (MSG_NAME_Y_START - MSG_NAME_Y_MID) / MSG_ANIM_STEP);
    start_x = (MSG_WIN_XRES - ipd.width) / 2;
    ipd.pdata = fb + start_y * ipd.stride + start_x * (MSG_WIN_COLOR_DEPTH >> 3);
    memcpy(&g_pre_txt[buf_id], &ipd, sizeof(struct image_st));

    rk_scale_process(&g_name.img[0], &ipd, TYPE_RGB565_565);

    clock_move_updn_refr_param.wflag = 0x01 << app_message_main_refrsh_param.win_id;
    clock_move_updn_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&clock_move_updn_refr_param);
    scale_index++;
    if (scale_index > MSG_ANIM_STEP)
    {
        scale_index = 0;
        app_main_register_timeout_cb(app_message_page_show_message, NULL, 1500);
    }
    else if (maindata->ver_page == VER_PAGE_BOTTOM)
    {
        app_design_request(0, &lv_new_message_design_t, RT_NULL);
    }
#if SHOW_TICK
    et = HAL_GetTick();
    rt_kprintf("design %p [%lu ms]\n", fb, et - st);
#endif

    pdata->buf_id = buf_id;

    return RT_EOK;
}

static rt_err_t app_lv_show_message_design(void *param);
design_cb_t lv_show_message_design_t = {.cb = app_lv_show_message_design,};
static rt_err_t app_lv_show_message_design(void *param)
{
    struct app_message_main_data_t *pdata = g_message_main_data;
    struct app_main_data_t *maindata = app_main_data;
    struct image_st ips, ipd;
    uint32_t start_x, start_y;
    rt_uint8_t buf_id;
    rt_uint8_t *fb;

    if (maindata->touch_event != RT_TOUCH_EVENT_UP)
    {
        if (mov_index == MSG_ANIM_STEP)
            mov_index--;
        return RT_EOK;
    }

    buf_id = (pdata->buf_id == 1) ? 0 : 1;
    fb = pdata->fb[buf_id];

#if SHOW_TICK
    uint32_t st, et;
    st = HAL_GetTick();
#endif

    if (mov_index > 0)
    {
        ips.width = MSG_LOGO_BIG_W;
        ips.height = MSG_LOGO_BIG_H;
        ips.stride = ips.width * 2;
        ips.pdata = pdata->logo_buf;

        ipd.width = ips.width - (MSG_ANIM_STEP - mov_index) * 6;
        ipd.height = (uint32_t)(ips.height * ((float)ipd.width / ips.width));
        ipd.stride = MSG_WIN_FB_W * (MSG_WIN_COLOR_DEPTH >> 3);
        start_y = MSG_LOGO_Y_MID + (int)((float)(MSG_ANIM_STEP - mov_index) * ABS(MSG_LOGO_Y_FINAL - MSG_LOGO_Y_MID) / MSG_ANIM_STEP);
        start_x = (MSG_WIN_XRES - ipd.width) / 2;
    }
    else
    {
        ips.width = MSG_LOGO_SMALL_W;
        ips.height = MSG_LOGO_SMALL_H;
        ips.stride = ips.width * 2;
        ips.pdata = pdata->minilogo_buf;

        ipd.width = ips.width;
        ipd.height = ips.height;
        ipd.stride = MSG_WIN_FB_W * (MSG_WIN_COLOR_DEPTH >> 3);
        start_y = MSG_LOGO_Y_FINAL;
        start_x = (MSG_WIN_XRES - ipd.width) / 2;
    }
    ipd.pdata = fb + start_y * ipd.stride + start_x * (MSG_WIN_COLOR_DEPTH >> 3);
    if (g_pre_area[buf_id].pdata != NULL)
        rk_image_reset(&g_pre_area[buf_id], MSG_WIN_COLOR_DEPTH >> 3);
    if (g_pre_txt[buf_id].pdata != NULL)
        rk_image_reset(&g_pre_txt[buf_id], MSG_WIN_COLOR_DEPTH >> 3);
    memcpy(&g_pre_area[buf_id], &ipd, sizeof(struct image_st));

    rk_scale_process(&ips, &ipd, TYPE_RGB565_565);

    ipd.width = g_name.img[0].width + mov_index * 4;//(uint32_t)(g_name.img[0].width * ((float)ipd.width / 2 / ips.width));
    ipd.height = (uint32_t)(g_name.img[0].height * ((float)ipd.width / g_name.img[0].width));
    ipd.stride = MSG_WIN_FB_W * (MSG_WIN_COLOR_DEPTH >> 3);
    start_y = MSG_NAME_Y_MID + (int)((float)(MSG_ANIM_STEP - mov_index) * ABS(MSG_NAME_Y_FINAL - MSG_NAME_Y_MID) / MSG_ANIM_STEP);
    start_x = (MSG_WIN_XRES - ipd.width) / 2;
    ipd.pdata = fb + start_y * ipd.stride + start_x * (MSG_WIN_COLOR_DEPTH >> 3);
    memcpy(&g_pre_txt[buf_id], &ipd, sizeof(struct image_st));

    rk_scale_process(&g_name.img[0], &ipd, TYPE_RGB565_565);

    pdata->offset = (MSG_ANIM_STEP - mov_index) * (MSG_BUF_OFFSET / MSG_ANIM_STEP);
    clock_move_updn_refr_param.wflag = 0x01 << app_message_main_refrsh_param.win_id;
    clock_move_updn_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&clock_move_updn_refr_param);
    mov_index--;
    if (mov_index < 0)
    {
        mov_index = MSG_ANIM_STEP;
        pdata->msg_cnt--;
        app_asr_start();
        // app_main_touch_register(&app_message_main_touch_cb);
    }
    else if (maindata->ver_page == VER_PAGE_BOTTOM)
    {
        app_design_request(0, &lv_show_message_design_t, RT_NULL);
    }
#if SHOW_TICK
    et = HAL_GetTick();
    rt_kprintf("design %p [%lu ms]\n", fb, et - st);
#endif

    pdata->buf_id = buf_id;

    return RT_EOK;
}

rt_err_t app_message_anim_continue(void)
{
    if (scale_index != 0)
    {
        app_design_request(0, &lv_new_message_design_t, RT_NULL);
        return RT_EOK;
    }
    if (mov_index != MSG_ANIM_STEP)
    {
        app_design_request(0, &lv_show_message_design_t, RT_NULL);
        return RT_EOK;
    }

    return RT_ERROR;
}

void app_message_page_exit(void)
{
    struct app_main_data_t *maindata = app_main_data;
    struct app_message_main_data_t *pdata = g_message_main_data;

    if (maindata->ver_page == VER_PAGE_NULL)
        return;
    maindata->ver_page = VER_PAGE_NULL;

    app_asr_start();

    scale_index = 0;
    mov_index = MSG_ANIM_STEP;

    if (g_pre_tips.pdata != NULL)
    {
        rk_image_reset(&g_pre_tips, MSG_WIN_COLOR_DEPTH >> 3);
        memset(&g_pre_tips, 0x0, sizeof(struct image_st));
    }

    if (g_pre_area[0].pdata != NULL)
    {
        rk_image_reset(&g_pre_area[0], MSG_WIN_COLOR_DEPTH >> 3);
        g_pre_area[0].pdata = NULL;
    }
    if (g_pre_area[1].pdata != NULL)
    {
        rk_image_reset(&g_pre_area[1], MSG_WIN_COLOR_DEPTH >> 3);
        g_pre_area[1].pdata = NULL;
    }
    if (g_pre_txt[0].pdata != NULL)
    {
        rk_image_reset(&g_pre_txt[0], MSG_WIN_COLOR_DEPTH >> 3);
        g_pre_txt[0].pdata = NULL;
    }
    if (g_pre_txt[1].pdata != NULL)
    {
        rk_image_reset(&g_pre_txt[1], MSG_WIN_COLOR_DEPTH >> 3);
        g_pre_txt[1].pdata = NULL;
    }
    if (pdata->logo_buf)
    {
        rt_free_large(pdata->logo_buf);
        pdata->logo_buf = NULL;
    }
    if (pdata->minilogo_buf)
    {
        rt_free_large(pdata->minilogo_buf);
        pdata->minilogo_buf = NULL;
    }

    pdata->offset = 0;
}

rt_err_t app_message_page_new_message(void *param)
{
    struct app_message_main_data_t *pdata = g_message_main_data;
    pdata->buf_id = 0;
    scale_index = 0;
    app_asr_stop();

    // pdata->msg_cnt++;
    pdata->msg_cnt = 1;

    if (g_pre_tips.pdata != NULL)
    {
        rk_image_reset(&g_pre_tips, MSG_WIN_COLOR_DEPTH >> 3);
        memset(&g_pre_tips, 0x0, sizeof(struct image_st));
    }

    pdata->logo_buf = (rt_uint8_t *)rt_malloc_large(MSG_LOGO_BIG_W * MSG_LOGO_BIG_H * 2);
    RT_ASSERT(pdata->logo_buf != NULL);
    pdata->minilogo_buf = (rt_uint8_t *)rt_malloc_large(MSG_LOGO_SMALL_W * MSG_LOGO_SMALL_H * 2);
    RT_ASSERT(pdata->minilogo_buf != NULL);

    img_load_info_t img_load_info;
    img_load_info.w = MSG_LOGO_BIG_W;
    img_load_info.h = MSG_LOGO_BIG_H;
    img_load_info.name = ICONS_PATH"/WhatsApp_160x160.dta";
    app_load_img(&img_load_info, pdata->logo_buf, MSG_LOGO_BIG_W, MSG_LOGO_BIG_H, 0, 2);

    img_load_info.w = MSG_LOGO_SMALL_W;
    img_load_info.h = MSG_LOGO_SMALL_H;
    img_load_info.name = ICONS_PATH"/WhatsApp_64x64.dta";
    app_load_img(&img_load_info, pdata->minilogo_buf, MSG_LOGO_SMALL_W, MSG_LOGO_SMALL_H, 0, 2);
    app_design_request(0, &lv_message_main_design_t, RT_NULL);
    app_design_request(0, &lv_new_message_design_t, RT_NULL);

    return RT_EOK;
}

rt_err_t app_message_page_show_message(void *param)
{
    mov_index = MSG_ANIM_STEP;
    // app_main_touch_unregister();
    app_design_request(0, &lv_show_message_design_t, RT_NULL);

    return RT_EOK;
}

static rt_err_t app_message_page_show_tips(void *param)
{
    struct app_lvgl_label_design *design = (struct app_lvgl_label_design *)param;
    struct app_message_main_data_t *msgdata = g_message_main_data;
    uint32_t start_x, start_y;

    msgdata->offset = 0;
    if (g_pre_area[0].pdata != NULL)
    {
        rk_image_reset(&g_pre_area[0], MSG_WIN_COLOR_DEPTH >> 3);
        g_pre_area[0].pdata = NULL;
    }
    if (g_pre_area[1].pdata != NULL)
    {
        rk_image_reset(&g_pre_area[1], MSG_WIN_COLOR_DEPTH >> 3);
        g_pre_area[1].pdata = NULL;
    }
    if (g_pre_txt[0].pdata != NULL)
    {
        rk_image_reset(&g_pre_txt[0], MSG_WIN_COLOR_DEPTH >> 3);
        g_pre_txt[0].pdata = NULL;
    }
    if (g_pre_txt[1].pdata != NULL)
    {
        rk_image_reset(&g_pre_txt[1], MSG_WIN_COLOR_DEPTH >> 3);
        g_pre_txt[1].pdata = NULL;
    }

    g_pre_tips.width = design->img[0].width;
    g_pre_tips.height = design->img[0].height;
    g_pre_tips.stride = MSG_WIN_FB_W * (MSG_WIN_COLOR_DEPTH >> 3);
    start_x = (MSG_WIN_XRES - g_pre_tips.width) / 2;
    start_y = (MSG_WIN_YRES - g_pre_tips.height) / 2;
    g_pre_tips.pdata = msgdata->fb[msgdata->buf_id] + start_y * g_pre_tips.stride + start_x * (MSG_WIN_COLOR_DEPTH >> 3);
    rk_image_copy(&design->img[0], &g_pre_tips, MSG_WIN_COLOR_DEPTH >> 3);

    clock_move_updn_refr_param.wflag = 0x01 << app_message_main_refrsh_param.win_id;
    clock_move_updn_refr_param.wait = RT_WAITING_FOREVER;
    app_refresh_request(&clock_move_updn_refr_param);

    return RT_EOK;
}
design_cb_t message_page_show_tips_t = {.cb = app_message_page_show_tips,};

rt_err_t app_message_page_check_message(void *param)
{
    struct app_message_main_data_t *msgdata = g_message_main_data;

    if (msgdata->msg_cnt == 0)
    {
        if (g_no_message.img[0].pdata == NULL)
        {
            g_no_message.txt = "No Message";
            g_no_message.ping_pong = 0;
            g_no_message.font = &lv_font_montserrat_44;
            g_no_message.align = LV_LABEL_ALIGN_CENTER;
            g_no_message.fmt = RTGRAPHIC_PIXEL_FORMAT_RGB565;
            g_no_message.img[0].width = MSG_WIN_XRES;
            g_no_message.img[0].height = lv_font_montserrat_44.line_height;
            g_no_message.img[0].stride = MSG_WIN_XRES * (MSG_WIN_COLOR_DEPTH >> 3);
            g_no_message.img[0].pdata = rt_malloc_psram(g_no_message.img[0].stride * g_no_message.img[0].height);
            RT_ASSERT(g_no_message.img[0].pdata != RT_NULL);
            memset(g_no_message.img[0].pdata, 0x0, g_no_message.img[0].stride * g_no_message.img[0].height);
            app_design_request(0, &lv_lvgl_label_design_t, &g_no_message);
        }
        app_design_request(0, &message_page_show_tips_t, &g_no_message);
    }

    return RT_EOK;
}

static rt_err_t app_message_move_updn_design(void *param);
design_cb_t message_move_updn_design = { .cb = app_message_move_updn_design, };
static refrsh_request_param_t message_move_updn_refr_param;

static rt_err_t app_message_move_updn_design(void *param)
{
    struct app_main_data_t *maindata = app_main_data;
    struct app_message_main_data_t *pdata = g_message_main_data;
    struct app_funclist_data_t *fdata = g_funclist_data;
    mov_design_param *tar = (mov_design_param *)param;

    pdata->mov_offset += TOUCH_MOVE_STEP * tar->dir;
    if (((tar->dir > 0) && (pdata->mov_offset >= tar->offset)) ||
            ((tar->dir < 0) && (pdata->mov_offset <= tar->offset)))
    {
        pdata->mov_dir = 0;

        if (tar->offset == 0)
        {
            app_main_touch_unregister();
            app_main_unregister_timeout_cb_if_is(app_message_page_show_message);
            app_main_touch_register(&main_page_touch_cb);
            // app_main_timer_cb_register(app_main_page_clock_update);
            if (main_page_timer_cb[app_main_page_data->cur_page].cb)
            {
                app_main_timer_cb_register(main_page_timer_cb[app_main_page_data->cur_page].cb);
            }
            app_message_page_exit();
            fdata->hor_start = 0;
            fdata->hor_offset = 0;
            message_move_updn_refr_param.wflag = 0x01 << main_page_refrsh_param.win_id;
        }
        else
        {
            message_move_updn_refr_param.wflag = (0x01 << main_page_refrsh_param.win_id) | (0x01 << app_message_main_refrsh_param.win_id);
            if (maindata->ver_page == VER_PAGE_BOTTOM)
                app_message_anim_continue();
        }

        pdata->mov_offset  = maindata->ver_page * MSG_WIN_YRES;

        message_move_updn_refr_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&message_move_updn_refr_param);
    }
    else // continue move
    {
        app_design_request(0, &message_move_updn_design, param);

        message_move_updn_refr_param.wflag = (0x01 << main_page_refrsh_param.win_id) | (0x01 << app_message_main_refrsh_param.win_id);
        message_move_updn_refr_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&message_move_updn_refr_param);
    }

    return RT_EOK;
}

static mov_design_param touch_moveup_design_param;
rt_err_t app_message_touch_move_up(void *param)
{
    struct app_main_data_t *maindata = app_main_data;
    struct app_main_data_t *pdata = (struct app_main_data_t *)param;
    struct rt_touch_data *cur_p   = &pdata->cur_point[0];
    struct rt_touch_data *down_p   = &pdata->down_point[0];

    if (pdata->dir_mode == TOUCH_DIR_MODE_UPDN)
    {
        if ((maindata->ver_page == VER_PAGE_TOP && (down_p->y_coordinate <= cur_p->y_coordinate)) ||
                (maindata->ver_page == VER_PAGE_BOTTOM && (down_p->y_coordinate >= cur_p->y_coordinate)))
        {
            if (maindata->ver_page == VER_PAGE_BOTTOM)
                app_message_anim_continue();
            return RT_EOK;
        }

        if ((maindata->ver_page == VER_PAGE_TOP && (down_p->y_coordinate - cur_p->y_coordinate) >= MSG_WIN_YRES / 6) ||
                (maindata->ver_page == VER_PAGE_BOTTOM && (cur_p->y_coordinate - down_p->y_coordinate) >= MSG_WIN_YRES / 6))
        {
            touch_moveup_design_param.dir = -maindata->ver_page;
            touch_moveup_design_param.offset = 0;
        }
        else
        {
            touch_moveup_design_param.dir = maindata->ver_page;
            touch_moveup_design_param.offset = MSG_WIN_YRES * touch_moveup_design_param.dir;
        }
        app_design_request(0, &message_move_updn_design, &touch_moveup_design_param);
    }

    return RT_EOK;
}

static rt_err_t app_message_touch_up(void *param)
{
    if (app_message_anim_continue() == RT_ERROR)
    {
        app_message_page_check_message(param);
    }

    return RT_EOK;
}

static rt_err_t app_message_touch_down(void *param)
{
    // struct app_message_main_data_t *appdata = g_message_main_data;

    return RT_EOK;
};

extern rt_err_t app_main_page_touch_move_updn(void *param);
struct app_touch_cb_t app_message_main_touch_cb =
{
    .tp_touch_down  = app_message_touch_down,
    .tp_move_updn   = app_main_page_touch_move_updn,
    .tp_move_up     = app_message_touch_move_up,
    .tp_touch_up    = app_message_touch_up,
};

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */
/**
 * top layer refresh.
 */
rt_err_t app_message_main_refresh(struct rt_display_config *wincfg, void *param)
{
    struct rt_device_graphic_info *info = &app_main_data->disp->info;
    struct app_message_main_data_t *pdata = g_message_main_data;
    struct app_funclist_data_t *fdata = g_funclist_data;
    struct app_func_data_t *fcdata = g_func_data;
    app_message_main_refrsh_param_t *par = (app_message_main_refrsh_param_t *)param;
    rt_int16_t offset;

    //RT_ASSERT(pdata->mov_offset > 0);
    if (pdata->mov_offset == 0)
        return RT_ERROR;
    if (fcdata->hor_offset > MSG_WIN_XRES || fcdata->hor_offset <= 0)
        return RT_ERROR;

    if (pdata->mov_offset < -MSG_WIN_YRES)
        pdata->mov_offset = -MSG_WIN_YRES;
    if (pdata->mov_offset > MSG_WIN_YRES)
        pdata->mov_offset = MSG_WIN_YRES;

    offset = pdata->mov_offset;
    if (offset < 0)
    {
        offset = -offset;
        wincfg->fb    = pdata->fb[pdata->buf_id] + pdata->offset * MSG_WIN_FB_W * (MSG_WIN_COLOR_DEPTH >> 3);
        wincfg->fblen = offset * (MSG_WIN_COLOR_DEPTH >> 3) * MSG_WIN_FB_W;
        wincfg->y     = DISP_YRES - offset;
        wincfg->format  = RTGRAPHIC_PIXEL_FORMAT_RGB565;
        wincfg->xVir  = MSG_WIN_FB_W;
        wincfg->w     = MSG_WIN_XRES;
        wincfg->x     = MSG_REGION_X + ((info->width  - wincfg->w) / 2);
    }
    else
    {
        if (fdata->hor_offset < 0)
            fdata->hor_offset = 0;
        if (fdata->hor_offset > (FUNC_WIN_FB_W - FUNC_WIN_XRES))
            fdata->hor_offset = FUNC_WIN_FB_W - FUNC_WIN_XRES;
        wincfg->fb    = fdata->funclist_fb + ((MSG_WIN_XRES - fcdata->hor_offset) + fdata->hor_offset) * (FUNC_WIN_COLOR_DEPTH >> 3) + (MSG_WIN_YRES - offset) * FUNC_WIN_FB_W * (FUNC_WIN_COLOR_DEPTH >> 3);
        wincfg->fblen = offset * (FUNC_WIN_COLOR_DEPTH >> 3) * FUNC_WIN_FB_W;
        wincfg->y     = MSG_REGION_Y + (info->height - MSG_WIN_YRES) / 2;;
        wincfg->format  = RTGRAPHIC_PIXEL_FORMAT_RGB565;
        // wincfg->format  = RTGRAPHIC_PIXEL_FORMAT_RGB888;
        wincfg->xVir  = FUNC_WIN_FB_W;
        wincfg->w     = fcdata->hor_offset;
        wincfg->x     = MSG_WIN_XRES - wincfg->w + (info->width - MSG_WIN_XRES) / 2;
    }
    wincfg->winId   = par->win_id;
    wincfg->zpos    = par->win_layer;
    wincfg->colorkey = 0;//COLOR_KEY_EN | 0x5555;
    wincfg->alphaEn = 0;
    wincfg->lut     = RT_NULL;
    wincfg->lutsize = 0;
    wincfg->h     = offset; /* MSG_WIN_YRES */
    wincfg->ylast = wincfg->y;

    // RT_ASSERT(((wincfg->w * MSG_WIN_COLOR_DEPTH) % 32) == 0);

    RT_ASSERT((wincfg->x + wincfg->w) <= info->width);
    RT_ASSERT((wincfg->y + wincfg->h) <= info->height);

    return RT_EOK;
}

static rt_err_t app_message_main_init_design(void *param)
{
    struct app_message_main_data_t *pdata;

    g_message_main_data = pdata = (struct app_message_main_data_t *)rt_malloc(sizeof(struct app_message_main_data_t));
    RT_ASSERT(pdata != RT_NULL);
    rt_memset((void *)pdata, 0, sizeof(struct app_message_main_data_t));

    /* framebuffer malloc */
    // pdata->fblen = MSG_WIN_FB_W * MSG_WIN_FB_H * MSG_WIN_COLOR_DEPTH / 8;
    pdata->fblen = MSG_WIN_XRES * MSG_WIN_YRES * 2 * MSG_WIN_COLOR_DEPTH / 8;
    pdata->fb[0] = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    pdata->fb[1] = (rt_uint8_t *)rt_malloc_psram(pdata->fblen);
    RT_ASSERT(pdata->fb[0] != RT_NULL);
    RT_ASSERT(pdata->fb[1] != RT_NULL);
    memset(pdata->fb[0], 0, pdata->fblen);
    memset(pdata->fb[1], 0, pdata->fblen);
    pdata->buf_id = 0;
    pdata->msg_cnt = 0;
    memset(&g_name, 0x0, sizeof(struct app_lvgl_label_design));
    memset(&g_no_message, 0x0, sizeof(struct app_lvgl_label_design));

    return RT_EOK;
}
static design_cb_t  app_message_main_init_design_t = {.cb = app_message_main_init_design,};

/**
 * App clock fast init.
 */
void app_message_main_init(void)
{
    app_design_request(0, &app_message_main_init_design_t, RT_NULL);
}

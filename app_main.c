/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>
#include <rtdef.h>
#include <stdio.h>
#include <math.h>
#include <dfs_posix.h>

#include "drv_heap.h"
#include "drv_display.h"
#include "drv_wdt.h"
#include "image_info.h"
#include "display.h"
#include "auto_version.h"

#if defined(RT_USING_TOUCH_DRIVERS)
#include "touch.h"
#include "touchpanel.h"
#endif

#include "app_main.h"

#include "board.h"
#include "hal_bsp.h"

/*
 **************************************************************************************************
 *
 * Macro define
 *
 **************************************************************************************************
 */
#define APP_TIMER_UPDATE_TICKS      (RT_TICK_PER_SECOND / 10)
#define APP_MAGIC_SUM               0x21082108
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
struct app_main_data_t *app_main_data = RT_NULL;

static const rt_uint8_t month_days[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

extern uint8_t xPsramDataBase[];
extern uint8_t xPsramBase[];
extern uint8_t xPsramSize[];
struct app_info *g_app_info;

/*
 **************************************************************************************************
 *
 * Clock tick funcitons
 *
 **************************************************************************************************
 */
rt_err_t app_load_img(img_load_info_t *info, rt_uint8_t *pbuf, rt_uint16_t w, rt_uint16_t h, rt_uint16_t offset, rt_uint8_t bitsize)
{
    FILE *fd;
    rt_uint32_t flen;

    fd = fopen(info->name, "r");
    if (fd <= 0)
    {
        rt_kprintf("open %s failed\n", info->name);
        return RT_ERROR;
    }
    fseek(fd, 0, SEEK_SET);

    rt_uint32_t xoffset = offset;
    rt_uint32_t yoffset = (h - info->h) / 2;

    for (rt_uint16_t j = yoffset; j < yoffset + info->h; j++)
    {
        flen = fread(pbuf + (j * w + xoffset) * bitsize, 1, info->w * bitsize, fd);
        if (!flen)
        {
            break;
        }
    }

    fclose(fd);

    return RT_EOK;
}

rt_uint32_t app_str2num(const char *str, uint8_t len)
{
    rt_uint32_t num = 0;

    for (rt_uint8_t i = 0; i < len; i++)
    {
        num += (str[i] - '0') * pow(10, (len - i - 1));
    }

    return num;
}

static rt_uint8_t get_day_of_week(uint16_t year, uint8_t month, uint8_t day)
{
    rt_uint32_t a = month < 3 ? 1 : 0;
    rt_uint32_t b = year - a;

    rt_uint32_t day_of_week = (day + (31 * (month - 2 + 12 * a) / 12) + b + (b / 4) - (b / 100) + (b / 400)) % 7;

    return day_of_week;
}

static void app_main_init_variable(clock_time_t *time)
{
    const char *str = FIRMWARE_AUTO_VERSION;

    /*Init the date of clock*/
    time->year     = app_str2num(&str[0], 4);
    time->month    = app_str2num(&str[5], 2);
    time->day      = app_str2num(&str[8], 2);
    time->weekdays = get_day_of_week(time->year, time->month, time->day);

    /*Init the date of time*/
    time->hour    = app_str2num(&str[11], 2);
    time->minute  = app_str2num(&str[14], 2);
    time->second  = app_str2num(&str[17], 2);
}

static app_clock_cb_t g_timer_cb = {NULL, NULL, 0};
static app_clock_cb_t g_timer_cb_bak = {NULL, NULL, 0};
static app_clock_cb_t g_refr_cb = {NULL, NULL, 0};

void app_main_register_timeout_cb(rt_err_t (*cb)(void *param), void *param, uint32_t ms)
{
    if (g_timer_cb.cb)
    {
        g_timer_cb_bak.cb = g_timer_cb.cb;
        g_timer_cb_bak.param = g_timer_cb.param;
        g_timer_cb_bak.timeout = g_timer_cb.timeout;
    }
    g_timer_cb.cb = cb;
    g_timer_cb.param = param;
    g_timer_cb.timeout = ms * (RT_TICK_PER_SECOND / APP_TIMER_UPDATE_TICKS) / 1000;
}

void app_main_unregister_timeout_cb(void)
{
    g_timer_cb.cb = NULL;
    g_timer_cb.param = NULL;
    g_timer_cb.timeout = 0;
}

void app_main_unregister_timeout_cb_if_is(rt_err_t (*cb)(void *param))
{
    if (g_timer_cb.cb == cb)
    {
        g_timer_cb.cb = NULL;
        g_timer_cb.param = NULL;
        g_timer_cb.timeout = 0;
    }
}

static void app_main_timer_tick(void *parameter)
{
    struct app_main_data_t *pdata = (struct app_main_data_t *)parameter;
    clock_time_t *time = &pdata->tmr_data;

    dw_wdt_set_top(APP_WDT_TIMEOUT);
    if (g_timer_cb.timeout)
    {
        g_timer_cb.timeout--;
        if (g_timer_cb.timeout == 0 && g_timer_cb.cb)
        {
            g_timer_cb.cb(g_timer_cb.param);
            if (g_timer_cb_bak.cb)
            {
                g_timer_cb.cb = g_timer_cb_bak.cb;
                g_timer_cb.param = g_timer_cb_bak.param;
                g_timer_cb.timeout = g_timer_cb_bak.timeout;

                g_timer_cb_bak.cb = NULL;
                g_timer_cb_bak.param = NULL;
                g_timer_cb_bak.timeout = 0;
            }
            else
            {
                app_main_register_timeout_cb(NULL, NULL, 0);
            }
        }
    }
    if (++time->tick >= (RT_TICK_PER_SECOND / APP_TIMER_UPDATE_TICKS))
    {
        time->tick = 0;
        if (++time->second >= 60)
        {
            time->second = 0;
            if (++time->minute >= 60)
            {
                time->minute = 0;
                if (++time->hour >= 24)
                {
                    time->hour = 0;
                    if (++time->weekdays >= 7)
                    {
                        time->weekdays = 0;
                    }

                    if (++time->day > month_days[time->month - 1])
                    {
                        time->day = 1;
                        if (++time->month > 12)
                        {
                            time->month = 1;
                            if (++time->year > 3000)
                            {
                                time->year = 1900;
                            }
                        }
                    }
                }
            }
        }
    }

    if (pdata->timer_cb)
    {
        pdata->timer_cb();
    }
}

void app_main_timer_cb_register(void (*cb)(void))
{
    app_main_data->timer_cb = cb;
}

void app_main_timer_cb_unregister(void)
{
    app_main_data->timer_cb = RT_NULL;
}

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */
static refrsh_request_param_t clksmooth_refrsh_request_param;
rt_err_t app_main_touch_smooth_design(void *param)
{
    struct app_main_page_data_t *cdata = app_main_page_data;
    struct app_message_main_data_t *msgdata = g_message_main_data;
    struct app_funclist_data_t *fdata = g_funclist_data;
    struct app_main_data_t *pdata = app_main_data;
    refrsh_request_param_t *par = (refrsh_request_param_t *)param;
    struct rt_touch_data *down_p   = &pdata->down_point[0];
    uint32_t tick;

    if (pdata->dir_mode && pdata->mov_speed)
    {
        tick = HAL_GetTick() - pdata->down_timestamp;
        pdata->down_timestamp = HAL_GetTick();
        pdata->mov_fix = ((int32_t)tick * pdata->mov_speed) / 1000;

        switch (par->id)
        {
        case ID_MAIN:
            cdata->mov_offset -= pdata->mov_fix;
            break;

        case ID_MSG:
            msgdata->mov_offset += pdata->mov_fix;
            if (pdata->ver_page == VER_PAGE_TOP && msgdata->mov_offset <= 0)
            {
                msgdata->mov_offset = 1;
            }
            if (pdata->ver_page == VER_PAGE_BOTTOM && msgdata->mov_offset >= 0)
            {
                msgdata->mov_offset = -1;
            }

            if (pdata->ver_page == VER_PAGE_NULL)
            {
                if (down_p->y_coordinate < (DISP_YRES / 2) && msgdata->mov_offset <= 0)
                {
                    msgdata->mov_offset = 1;
                }
                if (down_p->y_coordinate > (DISP_YRES / 2) && msgdata->mov_offset >= 0)
                {
                    msgdata->mov_offset = -1;
                }
            }
            break;

        case ID_FUNCLIST:
            fdata->hor_offset -= pdata->mov_fix;
            if (fdata->hor_offset < 0)
            {
                fdata->hor_offset = 0;
            }
            if (fdata->hor_offset > (FUNC_WIN_FB_W - FUNC_WIN_XRES))
            {
                fdata->hor_offset = FUNC_WIN_FB_W - FUNC_WIN_XRES;
            }
            break;

        default:
            break;
        }

        clksmooth_refrsh_request_param.wflag = (rt_uint32_t)par->wflag;
        clksmooth_refrsh_request_param.wait = RT_WAITING_FOREVER;
        app_refresh_request(&clksmooth_refrsh_request_param);
    }

    pdata->smooth_design = 0;

    return RT_EOK;
}

void app_main_keep_screen_on(void)
{
#if APP_TIMING_LIGHTOFF
    if (app_main_data->bl_time)
        rt_timer_start(app_main_data->bl_timer);
#endif
}

rt_err_t app_main_touch_process(struct rt_touch_data *point, rt_uint8_t num)
{
    struct app_main_data_t *pdata = app_main_data;
    struct rt_touch_data *p       = &point[0];
    struct rt_touch_data *pre_p   = &pdata->pre_point[0];
    struct rt_touch_data *cur_p   = &pdata->cur_point[0];
    struct rt_touch_data *down_p   = &pdata->down_point[0];
    struct rt_touchpanel_block *b = &pdata->touch_block;
    static rt_uint8_t wait_event  = RT_TOUCH_EVENT_NONE;
    rt_err_t ret;

    if (RT_EOK != rt_touchpoint_is_valid(p, b))
    {
        return RT_ERROR;
    }

    struct rt_touch_data cur_point;
    rt_memcpy(&cur_point, p, sizeof(struct rt_touch_data));
    p = &cur_point;
    p->x_coordinate -= b->x;
    p->y_coordinate -= b->y;

    if (app_main_data->bl_en == 0)
    {
        clock_app_mq_t mq = {MQ_BACKLIGHT_SWITCH, NULL};
        rt_err_t ret = rt_mq_send(app_main_data->mq, &mq, sizeof(clock_app_mq_t));
        RT_ASSERT(ret != RT_ERROR);
        wait_event = RT_TOUCH_EVENT_UP;
        return RT_EOK;
    }
    app_main_keep_screen_on();
    if (wait_event && b->event != wait_event)
        return RT_EOK;
    wait_event = RT_TOUCH_EVENT_NONE;

    switch (b->event)
    {
    case RT_TOUCH_EVENT_DOWN:
        rt_memcpy(pre_p, p, sizeof(struct rt_touch_data));
        rt_memcpy(cur_p, p, sizeof(struct rt_touch_data));
        rt_memcpy(down_p, p, sizeof(struct rt_touch_data));

        pdata->tb_flag  = 0;
        pdata->dir_mode = 0;
        pdata->xdir     = 0;
        pdata->ydir     = 0;
        pdata->xoffset  = 0;
        pdata->yoffset  = 0;
        pdata->mov_fix = 0;
        pdata->touch_event = RT_TOUCH_EVENT_DOWN;

        if (pdata->tp_touch_down)
        {
            ret = pdata->tp_touch_down(pdata);
            RT_ASSERT(ret == RT_EOK);
        }
        break;

    case RT_TOUCH_EVENT_MOVE:
        pdata->touch_event = RT_TOUCH_EVENT_MOVE;
        if (pdata->dir_mode == TOUCH_DIR_MODE_NULL)
        {
            if ((ABS(p->x_coordinate - pre_p->x_coordinate) >= TOUCH_START_THRESHOLD) ||
                    (ABS(p->y_coordinate - pre_p->y_coordinate) >= TOUCH_START_THRESHOLD))
            {
                if (ABS(p->x_coordinate - down_p->x_coordinate) > ABS(p->y_coordinate - down_p->y_coordinate))
                {
                    pdata->dir_mode = TOUCH_DIR_MODE_LR;
                    if (pdata->tp_move_lr_start)
                    {
                        ret = pdata->tp_move_lr_start(pdata);
                        RT_ASSERT(ret == RT_EOK);
                    }
                }
                else
                {
                    if (pdata->ver_page)
                    {
                        pdata->dir_mode = TOUCH_DIR_MODE_UPDN;
                        if (pdata->tp_move_updn_start)
                        {
                            ret = pdata->tp_move_updn_start(pdata);
                            RT_ASSERT(ret == RT_EOK);
                        }
                    }
                    else if (((int32_t)down_p->y_coordinate <= CLOCK_WIN_YRES / 3) ||
                             ((int32_t)down_p->y_coordinate >= CLOCK_WIN_YRES * 2 / 3))
                    {
                        pdata->dir_mode = TOUCH_DIR_MODE_UPDN;
                        if (pdata->tp_move_updn_start)
                        {
                            ret = pdata->tp_move_updn_start(pdata);
                            RT_ASSERT(ret == RT_EOK);
                        }
                    }
                }
            }
        }
        if (pdata->dir_mode == TOUCH_DIR_MODE_LR)
        {
            if ((pdata->tb_flag) || (ABS(p->x_coordinate - pre_p->x_coordinate) >= TOUCH_MOVE_THRESHOLD))
            {
                if (pdata->tb_flag == 0)
                {
                    pdata->xdir = -1;
                    if (p->x_coordinate > pre_p->x_coordinate)
                    {
                        pdata->xdir = 1;
                    }
                }
                pdata->xoffset += (rt_int16_t)(p->x_coordinate - pre_p->x_coordinate);
                pdata->mov_speed = ((int32_t)p->x_coordinate - (int32_t)pre_p->x_coordinate) * 1000 / (int32_t)(p->timestamp - pre_p->timestamp);
                pdata->mov_fix = 0;
                pdata->down_timestamp = HAL_GetTick();

                rt_memcpy(cur_p, p, sizeof(struct rt_touch_data));
                if (pdata->tp_move_lr)
                {
                    pdata->tb_flag = 0;
                    ret = pdata->tp_move_lr(pdata);
                    if (RT_EOK != ret)
                    {
                        pdata->tb_flag = 1;
                    }
                }
                rt_memcpy(pre_p, p, sizeof(struct rt_touch_data));
            }
        }
        else if (pdata->dir_mode == TOUCH_DIR_MODE_UPDN)
        {
            if ((pdata->tb_flag) || (ABS(p->y_coordinate - pre_p->y_coordinate) >= TOUCH_MOVE_THRESHOLD))
            {
                if (pdata->tb_flag == 0)
                {
                    pdata->ydir = -1;
                    if (p->y_coordinate > pre_p->y_coordinate)
                    {
                        pdata->ydir = 1;
                    }
                }
                pdata->yoffset += (rt_int16_t)(p->y_coordinate - pre_p->y_coordinate);
                pdata->mov_speed = ((int32_t)p->y_coordinate - (int32_t)pre_p->y_coordinate) * 1000 / (int32_t)(p->timestamp - pre_p->timestamp);
                pdata->mov_fix = 0;
                pdata->down_timestamp = HAL_GetTick();

                rt_memcpy(cur_p, p, sizeof(struct rt_touch_data));
                if (pdata->tp_move_updn)
                {
                    pdata->tb_flag = 0;
                    ret = pdata->tp_move_updn(pdata);
                    if (RT_EOK != ret)
                    {
                        pdata->tb_flag = 1;
                    }
                }
                rt_memcpy(pre_p, p, sizeof(struct rt_touch_data));
            }
        }
        break;

    case RT_TOUCH_EVENT_UP:
        if (pdata->dir_mode != TOUCH_DIR_MODE_NULL)
        {
            if (pdata->tp_move_up)
            {
                ret = pdata->tp_move_up(pdata);
                RT_ASSERT(ret == RT_EOK);
            }
        }
        else
        {
            if (pdata->tp_touch_up)
            {
                ret = pdata->tp_touch_up(pdata);
                RT_ASSERT(ret == RT_EOK);
            }
        }
        pdata->touch_event = RT_TOUCH_EVENT_UP;
        pdata->smooth_design = 0;
        pdata->mov_fix = 0;
        app_main_touch_value_reset();
        break;

    default:
        break;
    }

    return RT_EOK;
}

static void app_main_touch_init(struct rt_touchpanel_block *block)
{
    struct rt_device_graphic_info *info = &app_main_data->disp->info;

    rt_memset(block, 0, sizeof(struct rt_touchpanel_block));

    block->x = TOUCH_REGION_X + ((info->width  - DISP_XRES) / 2);
    block->y = TOUCH_REGION_Y + ((info->height - DISP_YRES) / 2);
    block->w = TOUCH_REGION_W;
    block->h = TOUCH_REGION_H;
    block->name = "wearable";

#ifdef APP_WEARABLE_ANIM_AUTO_PLAY
    block->callback = NULL;
#else
    block->callback = app_main_touch_process;
#endif
}

void app_main_touch_value_reset(void)
{
    struct app_main_data_t *pdata = app_main_data;

    pdata->tb_flag  = 0;
    pdata->dir_mode = 0;
    pdata->xdir     = 0;
    pdata->ydir     = 0;
    pdata->xoffset  = 0;
    pdata->yoffset  = 0;
}

void app_main_touch_unregister(void)
{
    struct app_main_data_t *pdata = app_main_data;

    pdata->tp_touch_down      = RT_NULL;

    pdata->tp_move_lr_start   = RT_NULL;
    pdata->tp_move_updn_start = RT_NULL;

    pdata->tp_move_lr         = RT_NULL;
    pdata->tp_move_updn       = RT_NULL;

    pdata->tp_move_up         = RT_NULL;
    pdata->tp_touch_up        = RT_NULL;
}

void app_main_touch_register(struct app_touch_cb_t *tcb)
{
    struct app_main_data_t *pdata = app_main_data;

    pdata->tp_touch_down      = tcb->tp_touch_down;

    pdata->tp_move_lr_start   = tcb->tp_move_lr_start;
    pdata->tp_move_updn_start = tcb->tp_move_updn_start;

    pdata->tp_move_lr         = tcb->tp_move_lr;
    pdata->tp_move_updn       = tcb->tp_move_updn;

    pdata->tp_move_up         = tcb->tp_move_up;
    pdata->tp_touch_up        = tcb->tp_touch_up;
}

/*
 **************************************************************************************************
 *
 * Design subroutines
 *
 **************************************************************************************************
 */
/**
 * Design request.
 */
void app_design_request(rt_uint8_t urgent, design_cb_t *design, void *param)
{
    register rt_base_t level;
    rt_err_t ret;

    struct app_main_data_t *pdata = app_main_data;
    rt_slist_t *head = &pdata->design_list;

    level = rt_hw_interrupt_disable();
    if (urgent)
    {
        rt_slist_insert(head, &design->list);
    }
    else
    {
        rt_slist_append(head, &design->list);
    }
    rt_hw_interrupt_enable(level);

    clock_app_mq_t mq = {MQ_DESIGN_UPDATE, param};
    ret = rt_mq_send(pdata->mq, &mq, sizeof(clock_app_mq_t));
    RT_ASSERT(ret != RT_ERROR);
}

/**
 * Design task.
 */
static void app_main_design(struct app_main_data_t *pdata, void *param)
{
    rt_err_t ret;
    rt_slist_t *head = &pdata->design_list;
    rt_slist_t *list = rt_slist_first(head);
    register rt_base_t level;

    if (list != RT_NULL)
    {
        level = rt_hw_interrupt_disable();
        design_cb_t *design = rt_slist_entry(list, design_cb_t, list);
        rt_slist_remove(head, list);
        rt_hw_interrupt_enable(level);

        ret = design->cb(param);
        if (ret != RT_EOK)
        {
            rt_kprintf("Design failed %p %d\n", design->cb, ret);
        }
    }
}

/*
 **************************************************************************************************
 *
 * Display refresh subroutines
 *
 **************************************************************************************************
 */

/**
 * Refresh callback register.
 */
void app_refresh_register(rt_uint8_t winid, void *cb, void *param)
{
    app_main_data->refr[winid].cb    = cb;
    app_main_data->refr[winid].param = param;
}

/**
 * Refresh callback unregister.
 */
void app_refresh_unregister(rt_uint8_t winid)
{
    app_main_data->refr[winid].cb    = RT_NULL;
    app_main_data->refr[winid].param = RT_NULL;
}

/**
 * Get refresh callback.
 */
void app_get_refresh_callback(rt_uint8_t winid, void **cb, void **param)
{
    *cb = app_main_data->refr[winid].cb;
    *param = app_main_data->refr[winid].param;
}

/**
 * Refresh callback request.
 */
void app_refresh_request(void *param)
{
    clock_app_mq_t mq = {MQ_REFR_UPDATE, param};
    rt_err_t ret = rt_mq_send(app_main_data->mq, &mq, sizeof(clock_app_mq_t));
    RT_ASSERT(ret != RT_ERROR);
}

/**
 * app clock display refresh request: request send data to LCD pannel.
 */
void app_registe_refresh_done_cb(rt_err_t (*cb)(void *param), void *param)
{
    g_refr_cb.cb = cb;
    g_refr_cb.param = param;
    g_refr_cb.timeout = 0;
}

static rt_err_t app_main_refresh_done(void)
{
    if (g_refr_cb.cb)
    {
        g_refr_cb.cb(g_refr_cb.param);
        app_registe_refresh_done_cb(NULL, NULL);
    }
    return (rt_event_send(app_main_data->event, EVENT_REFR_DONE));
}

static rt_err_t app_main_refresh_request(struct rt_display_mq_t *disp_mq, rt_int32_t wait)
{
    rt_err_t ret;

    //wait refresh done
    rt_uint32_t event;

    ret = rt_event_recv(app_main_data->event, EVENT_REFR_DONE,
                        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                        RT_WAITING_FOREVER, &event);
    if (ret != RT_EOK)
    {
        return ret;
    }

    //request refresh display data to Pannel
    disp_mq->disp_finish = app_main_refresh_done;
    ret = rt_mq_send(app_main_data->disp->disp_mq, disp_mq, sizeof(struct rt_display_mq_t));
    RT_ASSERT(ret != RT_ERROR);

    if (ret != RT_EOK)
    {
        rt_kprintf("mq send failed %d\n", ret);
        return RT_EOK;
    }

    return RT_EOK;
}

/**
 * app clock display task.
 */
static rt_err_t app_main_refresh(struct app_main_data_t *pdata, void *param)
{
    rt_err_t ret = RT_EOK;
    struct rt_display_mq_t disp_mq;
    refrsh_request_param_t *par = (refrsh_request_param_t *)param;

    rt_memset(&disp_mq, 0, sizeof(struct rt_display_mq_t));
    for (rt_uint8_t winid = 0; winid < 3; winid++)
    {
        if ((pdata->refr[winid].cb) && (par->wflag & (0x01 << winid)))
        {
            ret = pdata->refr[winid].cb(&disp_mq.win[winid], pdata->refr[winid].param);
            if (ret == RT_EOK)
            {
                disp_mq.cfgsta |= (0x01 << winid);
            }
        }
    }

    if (disp_mq.cfgsta)
    {
        ret = app_main_refresh_request(&disp_mq, par->wait);
    }

    return ret;
}

/**
 * Direct refrest to LCD.
 */
rt_err_t app_refresh_now(void *param)
{
    struct app_main_data_t *pdata = app_main_data;

    return (app_main_refresh(pdata, param));
}

void app_asr_cb(void)
{
    rt_kprintf("Xiao Rui Wakeup\n");

    app_asr_set_callback(NULL);
    app_play_notice(NOTICE_IAMHERE);
}

void app_play_cb(void)
{
    app_asr_set_callback(app_asr_cb);
}

/*
 **************************************************************************************************
 *
 * clock demo init & thread
 *
 **************************************************************************************************
 */
/**
 * clock demo thread
 */

#ifdef RT_USING_BMI270
extern rt_err_t bmi270_isr_sethook(void (*hook)(void));
void app_bmi_isr(void)
{
    if (app_main_data->bl_en == 0)
    {
        clock_app_mq_t mq = {MQ_BACKLIGHT_SWITCH, NULL};
        rt_err_t ret = rt_mq_send(app_main_data->mq, &mq, sizeof(clock_app_mq_t));
        RT_ASSERT(ret != RT_ERROR);
    }
}
#endif

static void app_main_switch_panel(void)
{
    uint16_t bl;
    rt_err_t ret;

    if (app_main_data->bl_en == 0)
        bl = app_main_data->bl;
    else
        bl = 0;

    ret = rt_display_backlight_set(bl);
    if (ret == RT_EOK)
        app_main_data->bl_en = (bl > 0) ? 1 : 0;
#if APP_TIMING_LIGHTOFF
    if (app_main_data->bl_en == 1)
        rt_timer_start(app_main_data->bl_timer);
#endif
}
MSH_CMD_EXPORT_ALIAS(app_main_switch_panel, switch, switch panel brightness);

rt_err_t app_reboot_loader(void *param)
{
    BSP_SetLoaderFlag();
    rt_hw_cpu_reset();

    return RT_EOK;
}

#ifdef POWER_KEY_PIN
static void app_gpio_isr(void *arg)
{
    if (rt_pin_read(POWER_KEY_BANK_PIN) == PIN_HIGH)
    {
        clock_app_mq_t mq = {MQ_BACKLIGHT_SWITCH, NULL};
        rt_err_t ret = rt_mq_send(app_main_data->mq, &mq, sizeof(clock_app_mq_t));
        RT_ASSERT(ret != RT_ERROR);
        app_main_register_timeout_cb(app_reboot_loader, NULL, 5000);
    }
    else
    {
        app_main_unregister_timeout_cb_if_is(app_reboot_loader);
    }
}
#endif

#if APP_TIMING_LIGHTOFF
static void app_main_backlight_timer(void *arg)
{
    if (app_main_data->bl_en != 0)
    {
        clock_app_mq_t mq = {MQ_BACKLIGHT_SWITCH, NULL};
        rt_err_t ret = rt_mq_send(app_main_data->mq, &mq, sizeof(clock_app_mq_t));
        RT_ASSERT(ret != RT_ERROR);
    }
}
#endif

void app_main_set_bl_timeout(uint32_t set_time)
{
    app_main_data->bl_time = set_time;
    save_app_info(app_main_data);
    if (app_main_data->bl_timer)
    {
        if (set_time)
        {
            set_time *= RT_TICK_PER_SECOND;
            rt_timer_control(app_main_data->bl_timer, RT_TIMER_CTRL_SET_TIME, &set_time);
            rt_timer_control(app_main_data->bl_timer, RT_TIMER_CTRL_GET_TIME, &set_time);
            rt_timer_start(app_main_data->bl_timer);
            rt_kprintf("Set bl timeout %dms", set_time);
        }
        else
        {
            rt_timer_stop(app_main_data->bl_timer);
        }
    }
}

static void app_main_thread(void *p)
{
    rt_err_t ret;
    clock_app_mq_t mq;
    struct app_main_data_t *pdata;

    //test ...
    // rt_thread_mdelay(3000);

    g_app_info = (struct app_info *)((uint32_t)xPsramBase + (uint32_t)xPsramSize - RT_PSRAM_END_RESERVE);
    if (g_app_info->magic == APP_MAGIC_SUM)
    {
        g_app_info->cold_boot = 0;
    }
    else
    {
        g_app_info->cold_boot = 1;
    }
    g_app_info->magic = APP_MAGIC_SUM;

    app_main_data = pdata = (struct app_main_data_t *)rt_malloc(sizeof(struct app_main_data_t));
    RT_ASSERT(pdata != RT_NULL);
    rt_memset((void *)pdata, 0, sizeof(struct app_main_data_t));

    pdata->disp = rt_display_get_disp();
    RT_ASSERT(pdata->disp != RT_NULL);

    app_main_data->bl = 50;
    app_main_data->bl_en = 1;
    app_main_data->bl_time = 5;

    struct rt_device_graphic_info *info = &pdata->disp->info;
    if ((info->width < DISP_XRES) || (info->height < DISP_YRES))
    {
        rt_kprintf("Error: the pannel size(%dx%d) is less than required size(%dx%d)!\n",
                   info->width, info->height, DISP_XRES, DISP_YRES);
        RT_ASSERT(!(info->width < DISP_XRES));
        RT_ASSERT(!(info->height < DISP_YRES));
    }

    pdata->mq = rt_mq_create("clock_mq", sizeof(clock_app_mq_t), CLOCK_APP_MQ_NUM, RT_IPC_FLAG_FIFO);
    RT_ASSERT(pdata->mq != RT_NULL);

    pdata->event = rt_event_create("display_event", RT_IPC_FLAG_FIFO);
    RT_ASSERT(pdata->event != RT_NULL);
    rt_event_send(pdata->event, EVENT_REFR_DONE);

    app_main_init_variable(&pdata->tmr_data);

    pdata->clk_timer = rt_timer_create("appclk_timer",
                                       app_main_timer_tick, (void *)pdata,
                                       APP_TIMER_UPDATE_TICKS, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
    RT_ASSERT(pdata->clk_timer != RT_NULL);

    // app init...
    app_lvgl_init(); // last app init
    app_main_page_init(); // First app init
    app_message_main_init();
    app_funclist_init();
    app_charging_init();
    app_func_memory_init();
    app_preview_init();
    // app_asr_init();
    // app_asr_set_callback(app_asr_cb);

    scan_audio();
    app_play_init();
    app_play_set_callback(app_play_cb);
    app_alpha_win_init();

#if APP_TIMING_LIGHTOFF
    uint32_t bl_time = pdata->bl_time > 0 ? pdata->bl_time : 1;
    pdata->bl_timer = rt_timer_create("appbl_timer",
                                      app_main_backlight_timer, (void *)pdata,
                                      RT_TICK_PER_SECOND * bl_time, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_HARD_TIMER);
    RT_ASSERT(pdata->bl_timer != RT_NULL);
#endif

    int type = 1;
    pdata->wdt_dev = rt_device_find("dw_wdt");
    rt_device_init(pdata->wdt_dev);
    dw_wdt_set_top(APP_WDT_TIMEOUT);
    rt_device_control(pdata->wdt_dev, RT_DEVICE_CTRL_WDT_START, &type);
    rt_timer_start(pdata->clk_timer);
#if APP_TIMING_LIGHTOFF
    if (pdata->bl_time)
        rt_timer_start(pdata->bl_timer);
#endif

    // touch init...
    app_main_touch_init(&pdata->touch_block);
    rt_touchpanel_block_register(&pdata->touch_block);
    pdata->touch_event = RT_TOUCH_EVENT_UP;

#ifdef POWER_KEY_PIN
    uint32_t pin_info = POWER_KEY_BANK_PIN;
    HAL_GPIO_SetPinsDirection(POWER_KEY_GPIO, POWER_KEY_PIN, GPIO_IN);
    HAL_PINCTRL_SetParam(POWER_KEY_BANK, POWER_KEY_PIN, PIN_CONFIG_PUL_NORMAL);
    rt_pin_attach_irq(pin_info, PIN_IRQ_MODE_RISING_FALLING, app_gpio_isr, NULL);
    rt_pin_irq_enable(pin_info, PIN_IRQ_ENABLE);
#endif

#ifdef RT_USING_BMI270
    bmi270_isr_sethook(app_bmi_isr);
#endif
    while (1)
    {
        ret = rt_mq_recv(pdata->mq, &mq, sizeof(clock_app_mq_t), RT_WAITING_FOREVER);
        RT_ASSERT(ret == RT_EOK);

        switch (mq.cmd)
        {
        case MQ_DESIGN_UPDATE:
            app_main_design(pdata, mq.param);
            break;
        case MQ_REFR_UPDATE:
            if (app_main_data->bl_en != 0)
                app_main_refresh(pdata, mq.param);
            break;
        case MQ_BACKLIGHT_SWITCH:
            app_main_switch_panel();
            break;
        default:
            break;
        }
    }

    rt_timer_stop(pdata->clk_timer);
    ret = rt_timer_delete(pdata->clk_timer);
    RT_ASSERT(ret == RT_EOK);

    rt_event_delete(pdata->event);
    rt_mq_delete(pdata->mq);

    rt_free(pdata);
}

/**
 * clock demo init
 */
static int app_main_init(void)
{
    rt_thread_t thread;

    thread = rt_thread_create("app", app_main_thread, RT_NULL, 8192, 5, 10);
    RT_ASSERT(thread != RT_NULL);

    rt_thread_startup(thread);

    return RT_EOK;
}

#ifndef RT_USB_DEVICE_MSTORAGE
INIT_APP_EXPORT(app_main_init);
#else
static void app_gpio_isr2(void *arg)
{
    app_reboot_loader(arg);
}

static int power_key_init(void)
{
#ifdef POWER_KEY_PIN
    uint32_t pin_info = POWER_KEY_BANK_PIN;
    HAL_GPIO_SetPinsDirection(POWER_KEY_GPIO, POWER_KEY_PIN, GPIO_IN);
    HAL_PINCTRL_SetParam(POWER_KEY_BANK, POWER_KEY_PIN, PIN_CONFIG_PUL_NORMAL);
    rt_pin_attach_irq(pin_info, PIN_IRQ_MODE_RISING_FALLING, app_gpio_isr2, NULL);
    rt_pin_irq_enable(pin_info, PIN_IRQ_ENABLE);
#endif

    return 0;
}
INIT_APP_EXPORT(power_key_init);
#endif

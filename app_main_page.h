/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_MAIN_PAGE__
#define __APP_MAIN_PAGE__
#include <rtthread.h>

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
 * Struct & data define
 *
 **************************************************************************************************
 */

struct app_main_page_data_t
{
    rt_uint8_t *bottom_fb;
    rt_uint32_t bottom_fblen;
    rt_uint8_t *top_fb;
    rt_uint32_t top_fblen;
    rt_uint8_t *bak_fb;
    rt_uint32_t bak_fblen;

    rt_int8_t   cur_page;
    rt_int8_t   switch_done;
    rt_int16_t  mov_dir;
    rt_int16_t  mov_offset;
};
extern struct app_main_page_data_t *app_main_page_data;
extern struct app_touch_cb_t main_page_touch_cb;

typedef struct
{
    void (*cb)(void);
} app_main_page_timer_cb_t;
extern app_main_page_timer_cb_t main_page_timer_cb[CLOCK_HOR_PAGE_MAX];

struct app_page_touch_ops
{
    void (*touch_down)(void *param);
    void (*touch_up)(void *param);
};

// Params
typedef struct
{
    rt_int8_t buf_id;
} main_page_design_param_t;

typedef struct
{
    rt_uint8_t win_id;
    rt_uint8_t win_layer;
} main_page_refrsh_param_t;
extern main_page_refrsh_param_t main_page_refrsh_param;

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */

/**
 * App clock fast init.
 */
rt_err_t app_main_page_refresh(struct rt_display_config *wincfg, void *param);
rt_err_t app_main_page_clock_design(void *param);
void app_main_page_clock_update(void);
void app_main_page_init(void);

#endif

/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_FUNCLIST__
#define __APP_FUNCLIST__
#include <rtthread.h>

#if defined(RT_USING_TOUCH_DRIVERS)
#include "touch.h"
#include "touchpanel.h"
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
 * Struct & data define
 *
 **************************************************************************************************
 */

// Params
typedef struct
{
    rt_uint8_t win_id;
    rt_uint8_t win_layer;
} app_funclist_refrsh_param_t;
extern app_funclist_refrsh_param_t app_funclist_refrsh_param;

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */

struct app_funclist_data_t
{
    rt_uint8_t *funclist_fb;
    rt_uint32_t funclist_fblen;
    rt_int16_t  hor_offset;
    rt_int16_t  hor_start;

    rt_uint8_t  buf_id;
    rt_uint8_t *fb[2];
    rt_uint32_t fblen;

    rt_uint32_t img_buflen;
    rt_uint8_t *img_buf;

    struct rt_touch_data point[1];
    int pos_x;
    int pos_y;
};
extern struct app_funclist_data_t *g_funclist_data;
extern struct app_touch_cb_t app_funclist_main_touch_cb;

void app_funclist_init(void);
rt_err_t app_funclist_page_show_funclist(void *param);
rt_err_t app_funclist_refresh(struct rt_display_config *wincfg, void *param);

#endif

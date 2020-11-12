/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_FUNC__
#define __APP_FUNC__
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
} app_func_refrsh_param_t;
extern app_func_refrsh_param_t app_func_refrsh_param;

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */

struct app_func_data_t
{
    rt_uint8_t *fb;
    rt_uint32_t fblen;

    rt_int8_t   alpha_win;

    rt_int16_t  hor_offset;
    rt_int16_t  ver_offset;
    rt_int16_t  ver_start;

    rt_int16_t  max_w;
    rt_int16_t  max_h;

    rt_int8_t   func_id;
};
extern struct app_func_data_t *g_func_data;
extern struct app_touch_cb_t app_func_main_touch_cb;

void app_func_memory_init(void);
void app_func_show(void *param);

#endif

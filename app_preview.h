/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_PREVIEW__
#define __APP_PREVIEW__
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

extern app_disp_refrsh_param_t app_preview_refrsh_param;

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */

struct app_preview_data_t
{
    rt_int16_t hor_offset;
    rt_int16_t hor_start;

    rt_uint8_t *fb;
    rt_uint32_t fblen;

    rt_int8_t  skip_touchup;
};
extern struct app_preview_data_t *g_preview_data;
extern struct app_touch_cb_t app_preview_main_touch_cb;

void app_preview_init(void);
rt_err_t app_preview_enter(void *arg);

#endif

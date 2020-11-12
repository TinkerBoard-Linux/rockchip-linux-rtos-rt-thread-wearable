/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_MESSAGE_MAIN__
#define __APP_MESSAGE_MAIN__
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

enum ver_page
{
    VER_PAGE_BOTTOM = -1,
    VER_PAGE_NULL   = 0,
    VER_PAGE_TOP    = 1,
};

// Params
typedef struct
{
    rt_uint8_t win_id;
    rt_uint8_t win_layer;
} app_message_main_refrsh_param_t;
extern app_message_main_refrsh_param_t app_message_main_refrsh_param;

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */

struct app_message_main_data_t
{
    rt_uint8_t  buf_id;
    rt_uint8_t *fb[2];
    rt_uint32_t fblen;
    rt_uint32_t offset;
    rt_uint8_t *logo_buf;
    rt_uint8_t *minilogo_buf;

    rt_int16_t  mov_dir;
    rt_int16_t  mov_offset;

    rt_int8_t   msg_cnt;
};
extern struct app_message_main_data_t *g_message_main_data;
extern struct app_touch_cb_t app_message_main_touch_cb;

rt_err_t app_message_main_refresh(struct rt_display_config *wincfg, void *param);

void app_message_main_init(void);
rt_err_t app_message_page_new_message(void *param);
rt_err_t app_message_page_show_message(void *param);
void app_message_page_exit(void);
rt_err_t app_message_anim_continue(void);

#endif

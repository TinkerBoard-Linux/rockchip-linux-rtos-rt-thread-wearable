/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_FUNC_BACKLIGHT__
#define __APP_FUNC_BACKLIGHT__
#include <rtthread.h>

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */
void func_backlight_init(void *param);
void func_backlight_enter(void *param);
void func_backlight_exit(void);

#endif

/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_FUNC_TIME__
#define __APP_FUNC_TIME__
#include <rtthread.h>

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */
void func_time_set_init(void *param);
void func_time_set_enter(void *param);
void func_time_set_exit(void);

#endif

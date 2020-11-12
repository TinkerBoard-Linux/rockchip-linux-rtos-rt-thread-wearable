/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_CLOCK__
#define __APP_CLOCK__
#include <rtthread.h>

/*
 **************************************************************************************************
 *
 * Declaration
 *
 **************************************************************************************************
 */

rt_err_t app_clock_init(void);
rt_err_t app_clock_design(void *param);

#endif

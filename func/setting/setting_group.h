/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __SETTING_GROUP_H__
#define __SETTING_GROUP_H__

enum app_setting_e
{
    APP_SETTING_BACKLIGHT,  // Backlight
    APP_SETTING_TIME,       // Time
    APP_SETTING_SLEEP,      // Sleep
    APP_SETTING_UNDISTURB,  // Undisturb
    APP_SETTING_PRESSURE,   // Pressure
    APP_SETTING_USERGUIDE,  // User Guide
    APP_SETTING_ABOUT,      // Aboult
    APP_SETTING_COMMON,     // for all setting
};

extern struct app_func app_setting_group[];

void app_setting_init(enum app_setting_e index);
void app_setting_enter(enum app_setting_e index);
void app_setting_exit(enum app_setting_e index);
void app_setting_set(enum app_setting_e index, void (*init)(void *param), void *init_param,
                     void (*enter)(void *param), void *param, void (*exit)(void));

#endif

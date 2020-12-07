/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include "app_main.h"

struct app_func app_setting_group[APP_SETTING_COMMON + 1] = {NULL};

void app_setting_set(enum app_setting_e index,
                     void (*init)(void *param),
                     void *init_param,
                     void (*enter)(void *param),
                     void *param,
                     void (*exit)(void))
{
    app_setting_group[index].init = init;
    app_setting_group[index].init_param = init_param;
    app_setting_group[index].enter = enter;
    app_setting_group[index].param = param;
    app_setting_group[index].exit = exit;
}

void app_setting_exit(enum app_setting_e index)
{
    struct app_page_data_t *page = g_setting_page;
    struct app_setting_private *pdata = page->private;

    if (app_setting_group[index].exit)
    {
        app_setting_group[index].exit();
    }
    else if (app_setting_group[APP_SETTING_COMMON].exit)
    {
        app_setting_group[APP_SETTING_COMMON].exit();
    }
    pdata->setting_id = APP_SETTING_COMMON;
}

void app_setting_enter(enum app_setting_e index)
{
    struct app_page_data_t *page = g_setting_page;
    struct app_setting_private *pdata = page->private;

    if (app_setting_group[index].enter)
    {
        app_setting_group[index].enter(app_setting_group[index].param);
        pdata->setting_id = index;
    }
    else if (app_setting_group[APP_SETTING_COMMON].enter)
    {
        app_setting_group[APP_SETTING_COMMON].enter(app_setting_group[APP_SETTING_COMMON].param);
        pdata->setting_id = APP_SETTING_COMMON;
    }
}

void app_setting_init(enum app_setting_e index)
{
    if (app_setting_group[index].init)
    {
        app_setting_group[index].init(app_setting_group[index].init_param);
    }
    else if (app_setting_group[APP_SETTING_COMMON].init)
    {
        app_setting_group[APP_SETTING_COMMON].init(app_setting_group[APP_SETTING_COMMON].init_param);
        app_setting_group[index].pre_view = app_setting_group[APP_SETTING_COMMON].pre_view;
    }
}


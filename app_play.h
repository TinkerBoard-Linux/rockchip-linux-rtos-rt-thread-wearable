/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __APP_PLAY_H__
#define __APP_PLAY_H__

#include "app_notice.h"

void app_play_set_callback(void (*cb)(void));
void app_play_init(void);
void app_play_notice(int index);
void app_play_music(char *target);
void app_play_pause(void);
void app_play_random(void);
void app_play_next(void);
void app_play_prev(void);
void app_music_mode_switch(void);

#endif

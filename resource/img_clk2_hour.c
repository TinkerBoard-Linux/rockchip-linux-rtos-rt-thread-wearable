/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>
#include <rtdef.h>
#include "image_info.h"

#if 1//def NEW_ROTATE_ALGORITHM

ALIGN(128) SECTION(".sram.data") static unsigned char img_clk2_hour_map[] =
{
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x4D, 0xE6, 0xD2, 0x3E, 0xBE, 0xE6, 0xD2, 0x3E, 0xF3, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xF3, 0xE6, 0xD2, 0x3E, 0xBE,
    0xE6, 0xD2, 0x3E, 0x4C, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x84, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0x84, 0xE6, 0xD2, 0x3E, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0x4C, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0x4D, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0xBE,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xB7, 0xE6, 0xD2, 0x3E, 0x19, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x19,
    0xE6, 0xD2, 0x3E, 0xB7, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xBE, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0xF2, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0x19,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x19, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xF2, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0xF2, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0x19, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x18, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xF3, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xE6, 0xD2, 0x3E, 0xBE, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xB5, 0xE6, 0xD2, 0x3E, 0x18, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00,
    0xE6, 0xD2, 0x3E, 0x1C, 0xE6, 0xD2, 0x3E, 0xB5, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xBE, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0x4D, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0x4D, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x84, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0x84, 0xE6, 0xD2, 0x3E, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x4E, 0xE6, 0xD2, 0x3E, 0xBE, 0xE6, 0xD2, 0x3E, 0xF4, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xFF,
    0xE6, 0xD2, 0x3E, 0xFF, 0xE6, 0xD2, 0x3E, 0xF4, 0xE6, 0xD2, 0x3E, 0xBE, 0xE6, 0xD2, 0x3E, 0x4D, 0xE6, 0xD2, 0x3E, 0x00, 0xE6, 0xD2, 0x3E, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00
};

image_info_t img_clk2_hour_info =
{
    .type  = IMG_TYPE_RAW,
    .pixel = RTGRAPHIC_PIXEL_FORMAT_ARGB888,
    .x = 0,
    .y = 0,
    .w = 77,
    .h = 12,
    .size = sizeof(img_clk2_hour_map),
    .data = (uint8_t *)img_clk2_hour_map,
};

#endif

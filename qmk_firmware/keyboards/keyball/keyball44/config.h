/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// Key matrix parameters
#define MATRIX_MASKED

// Split parameters
#define SPLIT_HAND_MATRIX_GRID F7, D4

#define SPLIT_TRANSACTION_IDS_KB                                               \
    KEYBALL_GET_INFO, KEYBALL_GET_MOTION, KEYBALL_SET_CPI

#ifndef OLED_FONT_H
#define OLED_FONT_H "keyboards/keyball/lib/glcdfont.c"
#endif

#if !defined(LAYER_STATE_8BIT) && !defined(LAYER_STATE_16BIT) &&               \
    !defined(LAYER_STATE_32BIT)
#define LAYER_STATE_8BIT
#endif

// To squeeze firmware size
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE

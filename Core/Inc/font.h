//
// Created by 21032 on 2021/7/17.
//
#include "main.h"

#ifndef T12_FONT_H
#define T12_FONT_H

#define ADD_FONT(font_size)     extern const font_dsc_t font_##font_size;

typedef struct {
    uint8_t w;
    uint8_t h;
    uint32_t size;
    const uint8_t *data;
}font_dsc_t;

#define FONT_32x64           0
#define FONT_48x96           1

#define FONT_NUM_12x24       1
#define FONT_NUM_40x40       1

#define FONT_CHAR_32x32        1
#define FONT_CHAR_24x24        1


/* Ìí¼Ó×ÖÌå */
#if FONT_32x64
ADD_FONT(32x64)
#endif
#if FONT_48x96
ADD_FONT(48x96)
#endif
#if FONT_CHAR_32x32
extern const font_dsc_t font_char_32x32;
#endif
#if FONT_CHAR_24x24
extern const font_dsc_t font_char_24x24;
#endif

#if FONT_NUM_12x24
extern const font_dsc_t font_num_12x24;
#endif

#if FONT_NUM_40x40
extern const font_dsc_t font_num_40x40;
#endif


#endif //T12_FONT_H

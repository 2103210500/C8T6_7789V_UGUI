#ifndef __IMG_H
#define __IMG_H
#include "main.h"



#if 0

typedef struct {
    uint16_t w;
    uint16_t h;
    uint32_t size;
    const uint8_t *data;
}img_dsc_t;

#define ADD_IMG_NAME(name) extern const img_dsc_t img_##name;

extern const img_dsc_t img1;
extern const img_dsc_t img2;

ADD_IMG_NAME(no_connect)


ADD_IMG_NAME(num_0)
ADD_IMG_NAME(num_1)
ADD_IMG_NAME(num_2)
ADD_IMG_NAME(num_3)
ADD_IMG_NAME(num_4)
ADD_IMG_NAME(num_5)
ADD_IMG_NAME(num_6)
ADD_IMG_NAME(num_7)
ADD_IMG_NAME(num_8)
ADD_IMG_NAME(num_9)
#endif


#endif
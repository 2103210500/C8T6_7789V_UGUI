//
// Created by 21032 on 2022/2/2.
//

#ifndef IPS_C8T6_7789V_LCD_H
#define IPS_C8T6_7789V_LCD_H

#include "main.h"
#include "gpio.h"
#include "font.h"
#include "img.h"

/* ÆÁÄ»·Ö±æÂÊ */
#define LCD_W 240
#define LCD_H 240


//»­±ÊÑÕÉ«
#define WHITE           0xFFFF
#define BLACK           0x0000
#define BLUE            0x001F
#define BRED            0XF81F
#define GRED            0XFFE0
#define GBLUE           0X07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define GREEN           0x07E0
#define CYAN            0x7FFF
#define YELLOW          0xFFE0
#define BROWN           0XBC40 //×ØÉ«
#define BRRED           0XFC07 //×ØºìÉ«
#define GRAY            0X8430 //»ÒÉ«


#define RST_SET()      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET)            //PA11
#define RST_RESET()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET)
//
//#define RS_SET()      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET)             //PA12
//#define RS_RESET()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET)
//
//#define WR_SET()      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET)              //PA15
//#define WR_RESET()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET)


#define SET_LCD_DB(cmd)    GPIOA->BSRR = (((cmd) & 0xff) | ((~(cmd) & 0xff) << 16))
#define SET_LCD_PIN(GPIO_PIN)   GPIOA->BSRR = (GPIO_PIN)
#define RST_LCD_PIN(GPIO_PIN)   GPIOA->BSRR = (uint32_t)(GPIO_PIN) << 16u

#define LCD_WRITE_DATA(dat)     { \
                                SET_LCD_PIN(GPIO_PIN_12);\
                                SET_LCD_DB((dat) >> 8);\
                                RST_LCD_PIN(GPIO_PIN_15);\
                                SET_LCD_PIN(GPIO_PIN_15);\
                                SET_LCD_DB((dat));\
                                RST_LCD_PIN(GPIO_PIN_15);\
                                SET_LCD_PIN(GPIO_PIN_15);\
                                }

#define LCD_WRITE_DATA8(dat)        { \
                                    SET_LCD_PIN(GPIO_PIN_12);\
                                    SET_LCD_DB(dat);\
                                    RST_LCD_PIN(GPIO_PIN_15);\
                                    SET_LCD_PIN(GPIO_PIN_15);\
                                    }

void Lcd_Init(void);

void LCD_WR_DATA16(uint16_t dat);

void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

void LCD_Clear(uint16_t Color);

void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void LCD_DrawLineVH(uint16_t x, uint16_t y, uint8_t vh, uint16_t len, uint16_t color);


void LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t color, uint16_t Radius);

//void LCD_Draw_Image(uint16_t x, uint16_t y, img_dsc_t img);

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

void LCD_DrawString(uint16_t x, uint16_t y, uint16_t color,
                    uint16_t background_color, font_dsc_t font, uint8_t pos, uint8_t len);

void LCD_DrawFullRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

uint32_t RGB565ToRGB888(uint16_t n565Color);

uint16_t RGB888ToRGB565(uint32_t n888Color);

uint32_t GetAlphaColorRGB888(uint32_t BackColor, uint32_t ForeColor, uint8_t Alpha);

#endif //IPS_C8T6_7789V_LCD_H

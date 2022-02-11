//
// Created by 21032 on 2022/2/2.
//

#include "LCD.h"
#include "math.h"


#define RGB888_RED      0x00ff0000
#define RGB888_GREEN    0x0000ff00
#define RGB888_BLUE     0x000000ff

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

/**
 * @brief
 * @param BackColor 0xffff
 * @param ForeColor 0xffff
 * @param Alpha ͸���� 0-ff
 * @return
 */
uint32_t GetAlphaColorRGB888(uint32_t BackColor, uint32_t ForeColor, uint8_t Alpha) {
    uint8_t R1, G1, B1, R2, G2, B2,RO,BO,GO;
    uint32_t R, G, B,RGB;

    R1 = (BackColor >> 16U) & 0xff;
    G1 = (BackColor >> 8U) & 0xff;
    B1 = (BackColor >> 0) & 0xff;

    R2 = (ForeColor >> 16U) & 0xff;
    G2 = (ForeColor >> 8U) & 0xff;
    B2 = (ForeColor >> 0) & 0xff;

    R = ((uint32_t) R2 * Alpha) + ((uint32_t) R1 * (0xff - Alpha));
    G = ((uint32_t) G2 * Alpha) + ((uint32_t) G1 * (0xff - Alpha));
    B = ((uint32_t) B2 * Alpha) + ((uint32_t) B1 * (0xff - Alpha));

    RO  = (uint8_t)(R/0xff);
    GO  = (uint8_t)(G/0xff);
    BO  = (uint8_t)(B/0xff);

    RGB = ((uint32_t)RO<<16u)|((uint16_t)GO<<8u)|BO;
    return RGB;
}

/**
 * @brief RGB888תRGB565
 * @param n888Color
 * @return
 */
uint16_t RGB888ToRGB565(uint32_t n888Color) {
    uint16_t n565Color = 0;
    uint8_t cRed = (n888Color & RGB888_RED) >> 19;
    uint8_t cGreen = (n888Color & RGB888_GREEN) >> 10;
    uint8_t cBlue = (n888Color & RGB888_BLUE) >> 3;

    n565Color = (cRed << 11) + (cGreen << 5) + (cBlue << 0);
    return n565Color;

}

uint32_t RGB565ToRGB888(uint16_t n565Color) {
    uint32_t n888Color = 0;
    uint8_t cRed = (n565Color & RGB565_RED) >> 8;
    uint8_t cGreen = (n565Color & RGB565_GREEN) >> 3;
    uint8_t cBlue = (n565Color & RGB565_BLUE) << 3;

    n888Color = (cRed << 16) + (cGreen << 8) + (cBlue << 0);
    return n888Color;
}

/**
 * д�Ĵ���
 * @param cmd
 */
void LCD_WR_REG(uint8_t cmd) {
    RST_LCD_PIN(GPIO_PIN_12);//RS
    SET_LCD_DB(cmd);
    RST_LCD_PIN(GPIO_PIN_15);//RW
    SET_LCD_PIN(GPIO_PIN_15);
}


/**
 * @brief ����Ļд��8bit����
 * @param dat
 */
void LCD_WR_DATA8(uint8_t dat) {
    SET_LCD_PIN(GPIO_PIN_12);//RS
    SET_LCD_DB(dat);
    RST_LCD_PIN(GPIO_PIN_15);//RW
    SET_LCD_PIN(GPIO_PIN_15);
}

void LCD_WR_DATA16(uint16_t dat) {
    SET_LCD_PIN(GPIO_PIN_12);//RS
    SET_LCD_DB(dat >> 8);
    RST_LCD_PIN(GPIO_PIN_15);//RW
    SET_LCD_PIN(GPIO_PIN_15);
    SET_LCD_DB(dat);
    RST_LCD_PIN(GPIO_PIN_15);//RW
    SET_LCD_PIN(GPIO_PIN_15);
}


/**
 * @brief  ���õ�ַ
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    LCD_WR_REG(0x2a);//�е�ַ����
    LCD_WR_DATA16(x1);
    LCD_WR_DATA16(x2);
    LCD_WR_REG(0x2b);//�е�ַ����
    LCD_WR_DATA16(y1);
    LCD_WR_DATA16(y2);
    LCD_WR_REG(0x2c);//������д
}


/**
 * @brief ����
 * @param Color
 */
void LCD_Clear(uint16_t Color) {
    uint32_t k, j;
    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);
    for (j = 0; j < 240; j++)
        for (k = 0; k < 240; ++k) {
            LCD_WRITE_DATA(Color);
        }
}


/**
 * @brief ��Ļ��ʼ��
 */
void Lcd_Init(void) {
    RST_RESET();
    HAL_Delay(100);
    RST_SET();
    HAL_Delay(120);

    LCD_WR_REG(0x36);
    LCD_WR_DATA8(0x00);//����

    LCD_WR_REG(0x3A);        //65k mode
    LCD_WR_DATA8(0x05);
    LCD_WR_REG(0xC5);        //VCOM
    LCD_WR_DATA8(0x1A);
    //-------------ST7789V Frame rate setting-----------//
    LCD_WR_REG(0xb2);        //Porch Setting
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xb7);            //Gate Control
    LCD_WR_DATA8(0x05);            //12.2v   -10.43v
    //--------------ST7789V Power setting---------------//
    LCD_WR_REG(0xBB);//VCOM
    LCD_WR_DATA8(0x3F);

    LCD_WR_REG(0xC0); //Power control
    LCD_WR_DATA8(0x2c);

    LCD_WR_REG(0xC2);        //VDV and VRH Command Enable
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);            //VRH Set
    LCD_WR_DATA8(0x0F);        //4.3+( vcom+vcom offset+vdv)

    LCD_WR_REG(0xC4);            //VDV Set
    LCD_WR_DATA8(0x20);                //0v

    LCD_WR_REG(0xC6);                //Frame Rate Control in Normal Mode
    LCD_WR_DATA8(0X01);            //111Hz

    LCD_WR_REG(0xd0);                //Power Control 1
    LCD_WR_DATA8(0xa4);
    LCD_WR_DATA8(0xa1);

    LCD_WR_REG(0xE8);                //Power Control 1
    LCD_WR_DATA8(0x03);

    LCD_WR_REG(0xE9);                //Equalize time control
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x08);
    //---------------ST7789V gamma setting-------------//
    LCD_WR_REG(0xE0); //Set Gamma
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x07);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x30);

    LCD_WR_REG(0XE1); //Set Gamma
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x03);
    LCD_WR_DATA8(0x24);
    LCD_WR_DATA8(0x32);
    LCD_WR_DATA8(0x32);
    LCD_WR_DATA8(0x3B);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x2F);
    LCD_WR_REG(0x21);    //��ת��ʾ
    LCD_WR_REG(0x11);    //�˳�˯��ģʽ
    HAL_Delay(120);

    LCD_Clear(BLACK);

    LCD_WR_REG(0x29); //Display on//28 off
}

//����
//x,y:����
//color:��ɫ
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    LCD_WR_REG(0x2a);//�е�ַ����
    LCD_WR_DATA16(x);
    LCD_WR_DATA16(x + 1);
    LCD_WR_REG(0x2b);//�е�ַ����
    LCD_WR_DATA16(y);
    LCD_WR_DATA16(y + 1);
    LCD_WR_REG(0x2c);//������д
    LCD_WR_DATA16(color);        //д����
}

/**
 * @brief ������
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void LCD_DrawFullRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    if (x2 - x1 < 0) {
        x1 ^= x2;
        x2 ^= x1;
        x1 ^= x2;
    }
    if (y2 - y1 < 0) {
        y1 ^= y2;
        y2 ^= y1;
        y1 ^= y2;
    }
    LCD_Address_Set(x1, y1, x2, y2);
    for (int i = 0; i < (1 + x2 - x1) * (1 + y2 - y1); ++i) LCD_WRITE_DATA(color);
}

/**
 * @breif	����ɫ��Բ����
 * @param   x1,x2 ���� Բ������
 * @param	r ���� �뾶
 * @param	color ���� ��ɫ
 * @retval	none
 */
void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t color, uint16_t r) {
    /* Bresenham��Բ�㷨 */
    int32_t a = 0, b = r;
    int32_t d = 3 - (r << 1);        //�㷨���߲���

    /* ���Բ����Ļ�ɼ������⣬ֱ���˳� */
//    if (x - r < 0 || x + r > LCD_W || y - r < 0 || y + r > LCD_H)
//        return;

    /* ��ʼ��Բ */
    while (a <= b) {
        LCD_DrawPixel(x - b, y - a, color);
        LCD_DrawPixel(x + b, y - a, color);
        LCD_DrawPixel(x - a, y + b, color);
        LCD_DrawPixel(x - b, y - a, color);
        LCD_DrawPixel(x - a, y - b, color);
        LCD_DrawPixel(x + b, y + a, color);
        LCD_DrawPixel(x + a, y - b, color);
        LCD_DrawPixel(x + a, y + b, color);
        LCD_DrawPixel(x - b, y + a, color);
        a++;

        if (d < 0)
            d += 4 * a + 6;
        else {
            d += 10 + 4 * (a - b);
            b--;
        }

        LCD_DrawPixel(x + a, y + b, color);
    }
}

/**
  * @brief  ���һ��ʵ��Բ
  * @param  Xpos: X����ֵ
  * @param  Ypos: Y����ֵ
  * @param  Radius: Բ�İ뾶
  * @retval ��
  */
void LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t color, uint16_t Radius) {
    int32_t decision;    /* ���߱��� */
    uint32_t current_x;   /* ��ǰx����ֵ */
    uint32_t current_y;   /* ��ǰy����ֵ */

    decision = 3 - (Radius << 1);

    current_x = 0;
    current_y = Radius;


    while (current_x <= current_y) {
        if (current_y > 0) {
            LCD_DrawLineVH(Xpos - current_y, Ypos + current_x, 1, 2 * current_y, color);
            LCD_DrawLineVH(Xpos - current_y, Ypos - current_x, 1, 2 * current_y, color);
        }

        if (current_x > 0) {
            LCD_DrawLineVH(Xpos - current_x, Ypos - current_y, 1, 2 * current_x, color);
            LCD_DrawLineVH(Xpos - current_x, Ypos + current_y, 1, 2 * current_x, color);
        }
        if (decision < 0) {
            decision += (current_x << 2) + 6;
        } else {
            decision += ((current_x - current_y) << 2) + 10;
            current_y--;
        }
        current_x++;
    }

    LCD_DrawCircle(Xpos, Ypos, color, Radius);
}


/**
 * @brief ��ʾͼƬ
 * @param x
 * @param y
 * @param img
 */
//void LCD_Draw_Image(uint16_t x, uint16_t y, img_dsc_t img) {
//    LCD_Address_Set(x, y, (x + img.w - 1) > LCD_W ? LCD_W : (x + img.w - 1),
//                    (y + img.h - 1) > LCD_H ? LCD_H : (y + img.h - 1));
//    SET_LCD_PIN(GPIO_PIN_12);//RS
//    for (int i = 0; i < img.size; ++i) {
//        SET_LCD_DB((*img.data++));
//        RST_LCD_PIN(GPIO_PIN_15);//RW
//        SET_LCD_PIN(GPIO_PIN_15);
//    }
//}

/**
 * @brief ��ʾͼƬ
 * @param x
 * @param y
 * @param img
 */
//void LCD_Draw_Image_Replace_Color(uint16_t x, uint16_t y, img_dsc_t img, uint16_t color) {
//    uint16_t c0;
//    LCD_Address_Set(x, y, (x + img.w - 1) > LCD_W ? LCD_W : (x + img.w - 1),
//                    (y + img.h - 1) > LCD_H ? LCD_H : (y + img.h - 1));
//    SET_LCD_PIN(GPIO_PIN_12);//RS
//    for (int i = 0; i < img.size; ++i) {
//        SET_LCD_DB((*img.data++));
//        RST_LCD_PIN(GPIO_PIN_15);//RW
//        SET_LCD_PIN(GPIO_PIN_15);
//    }
//}


/**
 * @brief ˮƽ��ֱ����
 * @param x
 * @param y
 * @param vh ˮƽ���Ǵ�ֱ:0��ֱ��1ˮƽ
 * @param len
 * @param color
 */
void LCD_DrawLineVH(uint16_t x, uint16_t y, uint8_t vh, uint16_t len, uint16_t color) {
    /* ��ֱ���� */
    if (vh == 0) {
        LCD_Address_Set(x, y, x, y + len);
        while (len--) LCD_WRITE_DATA(color);
    } else {
        LCD_Address_Set(x, y, x + len, y);
        while (len--) LCD_WRITE_DATA(color);
    }
}

/**
 * @brief		����ɫ���ߺ���(ֱ�ߡ�б��)
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
 * @return  none
 */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int32_t delta_x = 0, delta_y = 0;
    int8_t incx = 0, incy = 0;
    uint16_t distance = 0;
    uint16_t t = 0;
    uint16_t x = 0, y = 0;
    uint16_t x_temp = 0, y_temp = 0;

    if (y1 == y2) {
        LCD_DrawLineVH(x1 > x2 ? x2 : x1, y1, 1, x2 - x1 < 0 ? x1 - x2 : x2 - x1, color);
        return;
    } else {
        /* ��б�ߣ�Bresenham�㷨�� */
        /* ��������֮����x��y����ļ�࣬�õ�������x��y����Ĳ���ֵ */
        delta_x = x2 - x1;
        delta_y = y2 - y1;
        if (delta_x > 0) {
            //б��(������)
            incx = 1;
        } else if (delta_x == 0) {
            //��ֱб��(����)
            incx = 0;
        } else {
            //б��(���ҵ���)
            incx = -1;
            delta_x = -delta_x;
        }
        if (delta_y > 0) {
            //б��(������)
            incy = 1;
        } else if (delta_y == 0) {
            //ˮƽб��(ˮƽ��)
            incy = 0;
        } else {
            //б��(���ҵ���)
            incy = -1;
            delta_y = -delta_y;
        }

        /* ���㻭�ʴ�����(ȡ��������е����ֵ) */
        if (delta_x > delta_y) {
            distance = delta_x;
        } else {
            distance = delta_y;
        }

        /* ��ʼ��� */
        x = x1;
        y = y1;
        //��һ������Ч������t�Ĵ�����һ
        for (t = 0; t <= distance + 1; t++) {
            LCD_DrawPixel(x, y, color);

            /* �ж���ʵ��ֵ��������ص� */
            x_temp += delta_x;
            if (x_temp > distance) {
                //x����Խ�磬��ȥ����ֵ��Ϊ��һ�μ����׼��
                x_temp -= distance;
                //��x����������
                x += incx;

            }
            y_temp += delta_y;
            if (y_temp > distance) {
                //y����Խ�磬��ȥ����ֵ��Ϊ��һ�μ����׼��
                y_temp -= distance;
                //��y����������
                y += incy;
            }
        }
    }
}


uint16_t lastX, lastY;
uint8_t firstPoint = 1;

void drawCurve(short int rawValue, uint16_t color) {
    uint16_t x, y;
    y = 120 - rawValue / 280;    //data processing code
    //����֮������120-rawValue/280������Ļ��ɨ�跽���йأ�����������µߵ�����������Ըĳ�120 +
    if (firstPoint)//����ǵ�һ�λ��㣬���������ߣ�ֱ����㼴��
    {
        LCD_DrawPixel(0, y, color);
        lastX = 0;
        lastY = y;
        firstPoint = 0;
    } else {
        x = lastX + 1;
        if (x < 320)  //��������Ļ���
        {
            LCD_DrawLine(lastX, lastY, x, y, color);
            lastX = x;
            lastY = y;
        } else  //������Ļ��ȣ��������ӵ�һ���㿪ʼ���ƣ�ʵ�ֶ�̬����Ч��
        {
            LCD_Clear(WHITE);//��������ɫ����
            LCD_DrawPixel(0, y, color);
            lastX = 0;
            lastY = y;
        }
    }
}

/**
 * @brief ��������
 * @param x
 * @param y
 * @param color
 * @param background_color
 * @param font
 * @param num
 */
void LCD_DrawChar(uint16_t x, uint16_t y, uint16_t color, uint16_t background_color, font_dsc_t font, uint8_t num) {
    uint8_t dat;
    uint16_t color_t;
    LCD_Address_Set(x, y, x + font.w - 1, y + font.h - 1);
    font.data += font.size * num;
    SET_LCD_PIN(GPIO_PIN_12);//RS
    for (int i = 0; i < font.size; ++i) {
        dat = *font.data;
        for (int j = 0; j < 8; ++j) {
            color_t = (dat & 0x80) != 0 ? color : background_color;
            SET_LCD_DB((dat) >> 8);
            RST_LCD_PIN(GPIO_PIN_15);
            SET_LCD_PIN(GPIO_PIN_15);
            SET_LCD_DB((dat));
            RST_LCD_PIN(GPIO_PIN_15);
            SET_LCD_PIN(GPIO_PIN_15);
            dat <<= 1;
        }
        font.data++;
    }
}

/**
 * @brief
 * @param x
 * @param y
 * @param color
 * @param background_color
 * @param font
 * @param num_str
 * @param Decimal
 */
void LCD_DrawString(uint16_t x, uint16_t y, uint16_t color,
                    uint16_t background_color, font_dsc_t font, uint8_t pos, uint8_t len) {
    for (int i = 0; i < len; ++i) {
        LCD_DrawChar(x, y, color, background_color, font, pos + i);
        x += font.w;
    }
}

void LCD_DrawStringMessy(uint16_t x, uint16_t y, uint16_t color,
                         uint16_t background_color, font_dsc_t font, uint8_t *pos, uint8_t len) {
    for (int i = 0; i < len; ++i) {
        LCD_DrawChar(x, y, color, background_color, font, *pos++);
        x += font.w;
    }
}

/**
 * @brief ��ʾ����
 * @param x
 * @param y
 * @param color
 * @param background_color
 * @param font
 * @param integer ����λ��
 * @param decimal С��λ��
 * @param num ����
 * @param unit ��λ��һ���ַ���
 */
void LCD_DrawVariable(uint16_t x, uint16_t y, uint16_t color,
                      uint16_t background_color, font_dsc_t font, uint8_t integer, uint8_t decimal, uint16_t num,
                      uint8_t unit) {
    uint16_t integer_x = x;
    if (integer == 2 && decimal == 1) {
        LCD_DrawChar(integer_x, y, color, background_color, font, num / 100 % 10);
        integer_x += 12;
        LCD_DrawChar(integer_x, y, color, background_color, font, num / 10 % 10);
        integer_x += 12;
        LCD_DrawChar(integer_x, y, color, background_color, font, 10);
        integer_x += 8;
        LCD_DrawChar(integer_x, y, color, background_color, font, num / 1 % 10);
    } else if (integer == 1 && decimal == 2) {
        LCD_DrawChar(integer_x, y, color, background_color, font, num / 100 % 10);
        integer_x += 12;
        LCD_DrawChar(integer_x, y, color, background_color, font, 10);
        integer_x += 8;
        LCD_DrawChar(integer_x, y, color, background_color, font, num / 10 % 10);
        integer_x += 12;
        LCD_DrawChar(integer_x, y, color, background_color, font, num / 1 % 10);
    }
    integer_x += 12;
    LCD_DrawChar(integer_x + 2, y, color, background_color, font, unit);
}

void InterBresenhamline(int x0, int y0, int x1, int y1, int width, int color) {
    int x, y, dx, dy, e;

    dx = x1 - x0;
    dy = y1 - y0;
    e = -dx;
    x = x0;
    y = y0;

    uint16_t yy = width / 2;

    for (int i = 0; i < dx; i++) {
        for (int j = 0; j < width; ++j) {
            LCD_DrawPixel(x, y - yy + j, color);
        }
        x++;
        e = e + 2 * dy;
        if (e >= 0) {
            y++;
            e = e - 2 * dx;
        }
    }
}



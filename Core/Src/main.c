/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LCD.h"
#include "ugui.h"
#include "IMG0.h"
#include "image.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t MX_TRUE = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
UG_GUI gui;

#define MAX_OBJECTS        10
UG_WINDOW window_1;
UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];
UG_BUTTON button1_1;

//由于像素值不能为浮点数，因此以下算法假设仅将整数坐标作为输入。因此去除了首尾点的特殊操作
#define ipart_(X) ((int)(X))
#define round_(X) ((int)(((float)(X))+0.5f))
#define fpart_(X) (((float)(X))-(float)ipart_(X))
#define rfpart_(X) (1.0-fpart_(X))
#define swap_(a,b)    (a=(a)+(b),b=(a)-(b),a=(a)-(b))

/**
 * @brief 绘制虚拟带透明度像素点
 * @param x
 * @param y
 * @param brightness 0-1.0
 */
void setPixelAlpha(int x, int y, float brightness)
{
    uint32_t alpha_color =  GetAlphaColorRGB888(0x00ffffff,0x00000000,(uint8_t)(brightness*0xff));
    LCD_DrawPixel(x,y,RGB888ToRGB565(alpha_color));
}

int abs(int x )
{
    if (x < 0) return -x;
    else return x;
}

/**
 * @brief 绘制1宽度抗锯齿直线
 * @param x0
 * @param y0
 * @param x1
 * @param y1
 */
void lineAnti_Wu(int x0, int y0, int x1, int y1)
{
    int steep = abs(y1 - y0) > abs(x1 - x0);

    // swap the co-ordinates if slope > 1 or we
    // draw backwards
    if (steep)
    {
        swap_(x0, y0);
        swap_(x1, y1);
    }
    if (x0 > x1)
    {
        swap_(x0, x1);
        swap_(y0, y1);
    }

    //compute the slope
    float dx = x1 - x0;
    float dy = y1 - y0;
    float gradient = dy / dx;
    if (dx == 0.0)
        gradient = 1;

    int xpxl1 = x0;
    int xpxl2 = x1;
    float intersectY = y0;
    int  x;

    // main loop
    if (steep)
    {
        for (x = xpxl1; x <= xpxl2; x++)
        {
            setPixelAlpha(ipart_(intersectY), x, rfpart_(intersectY));
            setPixelAlpha(ipart_(intersectY) + 1, x,fpart_(intersectY));
            intersectY += gradient;
        }
    }
    else
    {
        for (x = xpxl1; x <= xpxl2; x++)
        {
            setPixelAlpha(x, ipart_(intersectY),rfpart_(intersectY));
            setPixelAlpha(x, ipart_(intersectY)+1,fpart_(intersectY));
            intersectY += gradient;
        }
    }
}

/**
 * @brief 绘制任意宽度抗锯齿直线
 * @param x0
 * @param y0
 * @param x1
 * @param y1
 * @param r
 */
void lineAnti_WuMulti(int x0, int y0, int x1, int y1, int r)
{
    int steep = abs(y1 - y0) > abs(x1 - x0);

    // swap the co-ordinates if slope > 1 or we
    // draw backwards
    if (steep)
    {
        swap_(x0, y0);
        swap_(x1, y1);
    }
    if (x0 > x1)
    {
        swap_(x0, x1);
        swap_(y0, y1);
    }

    //compute the slope
    float dx = x1 - x0;
    float dy = y1 - y0;
    float gradient = dy / dx;
    if (dx == 0.0)
        gradient = 1;

    int xpxl1 = x0;
    int xpxl2 = x1;
    float intersectY = y0;

    int i, x;

    // main loop
    if (steep)
    {
        for (x = xpxl1; x <= xpxl2; x++)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            setPixelAlpha(ipart_(intersectY), x, rfpart_(intersectY));
            for (i = 1; i < r; i++)
            {
                LCD_DrawPixel(ipart_(intersectY) + i, x,0x0000);
            }
            setPixelAlpha(ipart_(intersectY) + r, x, fpart_(intersectY));
            intersectY += gradient;
        }
    }
    else
    {
        for (x = xpxl1; x <= xpxl2; x++)
        {
            setPixelAlpha(x, ipart_(intersectY), rfpart_(intersectY));
            for (i = 1; i < r; i++)
            {
                LCD_DrawPixel(x, ipart_(intersectY) + i,0x0000);
            }
            setPixelAlpha(x, ipart_(intersectY) + r, fpart_(intersectY));
            intersectY += gradient;
        }
    }
}

/**
 * @brief 添加画点驱动
 * @param x
 * @param y
 * @param c
 */
void Driver_DrawPixel(UG_S16 x, UG_S16 y, UG_COLOR c) {
    LCD_DrawPixel(x, y, c);
}
/**
 * @brief 添加画线驱动
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param c
 * @return
 */
UG_RESULT Driver_DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) {
    if (y1 == y2) {
        LCD_DrawLineVH(x1, y1, 1, x2 - x1 + 1, c);
    } else if (x1 == x2) {
        LCD_DrawLineVH(x1, y1, 0, y2 - y1 + 1, c);
    } else {
        return UG_RESULT_FAIL;
    }
    return UG_RESULT_OK;
}

/**
 * @brief 绘制矩形面积
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param c
 * @return
 */
UG_RESULT Driver_FillFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c) {
    LCD_Address_Set(x1, y1, x2, y2);
    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            LCD_WRITE_DATA(c);
        }
    }
    return UG_RESULT_OK;
}

/**
 * @brief 绘制图片驱动
 * @param x
 * @param y
 * @param width
 * @param height
 * @param bmp
 * @return
 */
UG_RESULT Driver_DrawImage(UG_S16 x, UG_S16 y, UG_U16 width, UG_U16 height, UG_BMP *bmp) {
    uint16_t *p;
    uint16_t tmp;
    LCD_Address_Set(x, y, x + (bmp->width) - 1, y + (bmp->height) - 1);

    p = (UG_U16 *) bmp->p;
    for (int i = 0; i < width * height; ++i) {
        tmp = *p++;
        if ((bmp->bpp) == BMP_BPP_16) {
            LCD_WR_DATA16(tmp);
        } else if ((bmp->bpp) == BMP_BPP_8) {
            LCD_WR_DATA16(((uint16_t) (tmp << 8)) | (tmp >> 8));
        }
    }
    return UG_RESULT_OK;
}

/*
font model toos : PCtoLCD2002完美版
model : 阴码, 顺向, 逐行式, 十六进制, C51格式. 点阵16x16
*/

void window_1_callback(UG_MESSAGE *msg) {
    if (msg->type == MSG_TYPE_OBJECT) {
        if (msg->id == OBJ_TYPE_BUTTON) {
            switch (msg->sub_id) {
                case BTN_ID_0:

                    break;
            }
        }
    }
}

UG_IMAGE image_1;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    /* USER CODE BEGIN 2 */

    Lcd_Init();

    /* 初始化UGUI */
    UG_Init(&gui, (void *) Driver_DrawPixel, 240, 240);

    /* 添加驱动 */
    UG_DriverRegister(DRIVER_FILL_FRAME, (void *) Driver_FillFrame);
    UG_DriverEnable(DRIVER_FILL_FRAME);

    UG_DriverRegister(DRIVER_DRAW_LINE, (void *) Driver_DrawLine);
    UG_DriverEnable(DRIVER_DRAW_LINE);

    UG_DriverRegister(DRIVER_DRAW_IMAGE, (void *) Driver_DrawImage);
    UG_DriverEnable(DRIVER_DRAW_IMAGE);

    UG_SelectGUI(&gui);

    UG_FontSelect(&FONT_12X20);

    UG_FillScreen(C_WHITE);/* 填充屏幕 */

    /* 创建窗口 */
    UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback);
    UG_WindowSetStyle(&window_1, WND_STYLE_HIDE_TITLE);

    /* 创建按钮 */
    UG_ButtonCreate(&window_1, &button1_1, BTN_ID_0, 10, 10, 110, 60);
    UG_ButtonSetStyle(&window_1, BTN_ID_0, BTN_STYLE_TOGGLE_COLORS);
    UG_ButtonSetFont(&window_1, BTN_ID_0, &FONT_12X20);
    UG_ButtonSetBackColor(&window_1, BTN_ID_0, C_WHITE);
    UG_ButtonSetText(&window_1, BTN_ID_0, "Button");

    /* 创建 */
    UG_TEXTBOX textbox2_1;
    UG_TextboxCreate(&window_1, &textbox2_1, TXB_ID_0, 10, 70, UG_WindowGetInnerWidth(&window_1) - 10, 100);
    UG_TextboxSetFont(&window_1, TXB_ID_0, &FONT_16X26);
    UG_TextboxSetText(&window_1, TXB_ID_0, "礕UI v0.3");
    UG_TextboxSetAlignment(&window_1, TXB_ID_0, ALIGN_TOP_CENTER);

    /* 创建图片控件 */
    UG_ImageCreate(&window_1, &image_1, IMG_ID_0, 50, 50, 150, 150);
    UG_ImageSetBMP(&window_1, IMG_ID_0, &img0);

    UG_WindowShow(&window_1);

    /* 放在任务里或者定时器里定时刷新 */
    UG_Update();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

//    for (int i = 0; i < 240; i+=10) {
//        lineAnti_Wu(0,239-i,239,i);
//    }

//    lineAnti_WuMulti(0,140,239,100,5);
//    lineAnti_WuMulti(140,0,100,239,8);
//    lineAnti_WuMulti(50,86,123,210,3);


    while (MX_TRUE) {
        HAL_Delay(1);
        //UG_Update();
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


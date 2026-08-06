#ifndef __PIN_CFG_H__
#define __PIN_CFG_H__

/* LED */
#define LED_BLUE_PIN        GPIO_NUM_4

/* SPI */       
#define SPI_SCK_PIN         GPIO_NUM_7
#define SPI_MOSI_PIN        GPIO_NUM_16
#define SPI_MISO_PIN        GPIO_NUM_15

/* SD Card */   
#define SPI_TF_CS_PIN       GPIO_NUM_17

/* I2C */       
#define MAX_I2C_DEV_NUM     3
#define XL9555_ADDR         0x20
#define E2PROM_ADDR         0x50
#define I2C_PORT            0
#define I2C_INT_PIN         GPIO_NUM_3
#define I2C_SDA_PIN         GPIO_NUM_48
#define I2C_SCL_PIN         GPIO_NUM_45

/* LCD */   
#define LCD_CS_PIN          GPIO_NUM_1
#define LCD_RS_PIN          GPIO_NUM_2
#define LCD_WR_PIN          GPIO_NUM_42
#define LCD_RD_PIN          GPIO_NUM_41

#define LCD_D0_PIN          GPIO_NUM_40
#define LCD_D1_PIN          GPIO_NUM_39
#define LCD_D2_PIN          GPIO_NUM_38
#define LCD_D3_PIN          GPIO_NUM_12
#define LCD_D4_PIN          GPIO_NUM_11
#define LCD_D5_PIN          GPIO_NUM_10
#define LCD_D6_PIN          GPIO_NUM_9
#define LCD_D7_PIN          GPIO_NUM_46

/* I2S */   
#define I2S_LRCK_PIN        GPIO_NUM_13
#define I2S_SDOUT_PIN       GPIO_NUM_14
#define I2S_BCK_PIN         GPIO_NUM_21
#define I2S_SDIN_PIN        GPIO_NUM_47

/* KEY */   
#define KEY_PIN             GPIO_NUM_0


#endif
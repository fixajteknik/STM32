/**
 * @file fixajLCD.h
 * @brief fixajLCD - I2C LCD ekran kütüphanesi (4x20 LCD ekranlar için) - Geliştirilmiş Versiyon
 * @version 2.2
 * @date 2025
 * @author fixaj
 */

#ifndef FIXAJLCD_H
#define FIXAJLCD_H


//#include "stm32f2xx_hal.h" // STM32H7 serisi HAL kütüphanesi
#include "main.h"
#include <stdarg.h>
#include <stdio.h>

// fixajLCD komutları
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// Bayraklar için display kontrol talimatları
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// Bayraklar için display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Bayraklar için display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// PCF8574 I2C fixajLCD arayüzü için bayraklar
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00
#define LCD_EN 0x04  // Enable bit
#define LCD_RW 0x02  // Read/Write bit
#define LCD_RS 0x01  // Register select bit

// Timeout ve delay değerleri
#define LCD_I2C_TIMEOUT 100
#define LCD_INIT_DELAY 100
#define LCD_CMD_DELAY 1
#define LCD_CLEAR_DELAY 2
#define LCD_4BIT_INIT_DELAY 5

// Maksimum string uzunluğu (printf için)
#define LCD_MAX_STRING_LENGTH 128

// fixajLCD yapısı
typedef struct {
    uint8_t I2C_Adres;
    uint8_t Satir;
    uint8_t Sutun;
    uint8_t DisplayControl;
    uint8_t DisplayFunction;
    uint8_t DisplayMode;
    uint8_t Arka_Isik;
    I2C_HandleTypeDef* I2C_Port;
} LCD_t;

// Hata kodları
typedef enum {
    LCD_OK = 0,
    LCD_ERROR = 1,
    LCD_TIMEOUT = 2,
    LCD_INVALID_PARAM = 3,
    LCD_I2C_ERROR = 4
} LCD_Status_t;

/**
 * @brief fixajLCD ekranı başlatır
 * @param I2C_Port: I2C portu (örn: &hi2c1)
 * @param I2C_Adres: LCD'nin I2C adresi (genellikle 0x27 veya 0x3F)
 * @param Sutun: LCD sütun sayısı (örn: 20)
 * @param Satir: LCD satır sayısı (örn: 4)
 * @param lcd: LCD yapısının pointer'ı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_kur(I2C_HandleTypeDef* I2C_Port, uint8_t I2C_Adres, uint8_t Sutun, uint8_t Satir, LCD_t* lcd);

/**
 * @brief I2C veri yolunda fixajLCD adresini tarar
 * @param I2C_Port: I2C portu
 * @return Bulunan LCD adresi (0 = bulunamadı)
 */
uint8_t LCD_adres_tara(I2C_HandleTypeDef* I2C_Port);

/**
 * @brief Yazıyı fixajLCD ekranda ortalar
 * @param lcd: LCD yapısı
 * @param satir: Yazının yazdırılacağı satır (0-3)
 * @param yazi: Yazdırılacak yazı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_ortala(LCD_t* lcd, uint8_t satir, const char* yazi);

/**
 * @brief fixajLCD ekranı temizler
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_temizle(LCD_t* lcd);

/**
 * @brief Belirli bir satırı temizler
 * @param lcd: LCD yapısı
 * @param satir: Temizlenecek satır (0-3)
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_satir_temizle(LCD_t* lcd, uint8_t satir);

/**
 * @brief İmleci belirtilen konuma yerleştirir
 * @param lcd: LCD yapısı
 * @param x: Sütun konumu (0'dan başlar)
 * @param y: Satır konumu (0'dan başlar)
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_imlec(LCD_t* lcd, uint8_t x, uint8_t y);

/**
 * @brief fixajLCD'ye string yazdırır
 * @param lcd: LCD yapısı
 * @param str: Yazdırılacak string
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_yaz(LCD_t* lcd, const char* str);

/**
 * @brief Belirli pozisyonda string yazdırır
 * @param lcd: LCD yapısı
 * @param x: Sütun konumu
 * @param y: Satır konumu
 * @param str: Yazdırılacak string
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_yaz_pozisyon(LCD_t* lcd, uint8_t x, uint8_t y, const char* str);

/**
 * @brief Printf benzeri fonksiyon - formatlanmış yazı yazdırır
 * @param lcd: LCD yapısı
 * @param format: Format string'i
 * @param ...: Değişken parametreler
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_printf(LCD_t* lcd, const char* format, ...);

/**
 * @brief Belirli pozisyonda printf benzeri fonksiyon
 * @param lcd: LCD yapısı
 * @param x: Sütun konumu
 * @param y: Satır konumu
 * @param format: Format string'i
 * @param ...: Değişken parametreler
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_printf_pozisyon(LCD_t* lcd, uint8_t x, uint8_t y, const char* format, ...);

/**
 * @brief fixajLCD'ye tek karakter yazdırır
 * @param lcd: LCD yapısı
 * @param data: Yazdırılacak karakter
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_karakter(LCD_t* lcd, char data);

/**
 * @brief Özel karakter tanımlar (CGRAM'a yazar)
 * @param lcd: LCD yapısı
 * @param konum: Özel karakter konumu (0-7)
 * @param karakter: 8 byte'lık karakter verisi
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_ozel_karakter(LCD_t* lcd, uint8_t konum, uint8_t karakter[8]);

/**
 * @brief fixajLCD arka ışığını açar
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_arkaIsik_ac(LCD_t* lcd);

/**
 * @brief fixajLCD arka ışığını kapatır
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_arkaIsik_kapat(LCD_t* lcd);

/**
 * @brief fixajLCD display'ini açar
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_display_ac(LCD_t* lcd);

/**
 * @brief fixajLCD display'ini kapatır
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_display_kapat(LCD_t* lcd);

/**
 * @brief İmleci görünür yapar
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_imlec_ac(LCD_t* lcd);

/**
 * @brief İmleci gizler
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_imlec_kapat(LCD_t* lcd);

/**
 * @brief İmleç yanıp sönmesini açar
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_imlec_yanip_son_ac(LCD_t* lcd);

/**
 * @brief İmleç yanıp sönmesini kapatır
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_imlec_yanip_son_kapat(LCD_t* lcd);

/**
 * @brief fixajLCD ana pozisyona döner ve imleci (0,0)'a yerleştirir
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_ana_ekran(LCD_t* lcd);

/**
 * @brief Ekranı sola kaydırır
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_ekran_sola_kaydir(LCD_t* lcd);

/**
 * @brief Ekranı sağa kaydırır
 * @param lcd: LCD yapısı
 * @return LCD_Status_t hata kodu
 */
LCD_Status_t LCD_ekran_saga_kaydir(LCD_t* lcd);

#endif /* FIXAJLCD_H */

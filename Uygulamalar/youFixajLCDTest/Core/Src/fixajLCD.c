/**
 * @file fixajLCD.c
 * @brief fixajLCD - I2C LCD ekran kütüphanesi (4x20 LCD ekranlar için) - Geliştirilmiş Versiyon
 * @version 2.2
 * @date 2025
 * @author fixaj
 */

#include "fixajLCD.h"

// Private fonksiyon prototipler
static void LCD_bekle(uint32_t ms);
static LCD_Status_t LCD_komut_gonder(LCD_t* lcd, uint8_t komut);
static LCD_Status_t LCD_veri_gonder(LCD_t* lcd, uint8_t veri);
static LCD_Status_t LCD_parametreler_kontrol(LCD_t* lcd);
static LCD_Status_t LCD_pozisyon_kontrol(LCD_t* lcd, uint8_t x, uint8_t y);
static LCD_Status_t LCD_init_4bit(LCD_t* lcd);

// Private fonksiyonlar
static void LCD_bekle(uint32_t ms) {
    HAL_Delay(ms);
}

static LCD_Status_t LCD_parametreler_kontrol(LCD_t* lcd) {
    if (lcd == NULL || lcd->I2C_Port == NULL) {
        return LCD_INVALID_PARAM;
    }
    return LCD_OK;
}

static LCD_Status_t LCD_pozisyon_kontrol(LCD_t* lcd, uint8_t x, uint8_t y) {
    if (x >= lcd->Sutun || y >= lcd->Satir) {
        return LCD_INVALID_PARAM;
    }
    return LCD_OK;
}

static LCD_Status_t LCD_komut_gonder(LCD_t* lcd, uint8_t komut) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) {
        return LCD_INVALID_PARAM;
    }

    uint8_t yuksek_nibble = (komut & 0xF0) | lcd->Arka_Isik;
    uint8_t dusuk_nibble = ((komut << 4) & 0xF0) | lcd->Arka_Isik;
    uint8_t veri[1];
    HAL_StatusTypeDef hal_status;
    
    // Yüksek nibble gönder
    veri[0] = yuksek_nibble | LCD_EN;  // EN yüksek
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(1);
    
    veri[0] = yuksek_nibble & ~LCD_EN;  // EN düşük
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(1);
    
    // Düşük nibble gönder
    veri[0] = dusuk_nibble | LCD_EN;  // EN yüksek
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(1);
    
    veri[0] = dusuk_nibble & ~LCD_EN;  // EN düşük
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(LCD_CMD_DELAY);
    
    return LCD_OK;
}

static LCD_Status_t LCD_veri_gonder(LCD_t* lcd, uint8_t veri) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) {
        return LCD_INVALID_PARAM;
    }

    uint8_t yuksek_nibble = (veri & 0xF0) | LCD_RS | lcd->Arka_Isik;
    uint8_t dusuk_nibble = ((veri << 4) & 0xF0) | LCD_RS | lcd->Arka_Isik;
    uint8_t i2c_veri[1];
    HAL_StatusTypeDef hal_status;
    
    // Yüksek nibble gönder
    i2c_veri[0] = yuksek_nibble | LCD_EN;  // EN yüksek
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, i2c_veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(1);
    
    i2c_veri[0] = yuksek_nibble & ~LCD_EN;  // EN düşük
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, i2c_veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(1);
    
    // Düşük nibble gönder
    i2c_veri[0] = dusuk_nibble | LCD_EN;  // EN yüksek
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, i2c_veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(1);
    
    i2c_veri[0] = dusuk_nibble & ~LCD_EN;  // EN düşük
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, i2c_veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(LCD_CMD_DELAY);
    
    return LCD_OK;
}

static LCD_Status_t LCD_init_4bit(LCD_t* lcd) {
    uint8_t veri[1];
    HAL_StatusTypeDef hal_status;
    
    // 4-bit mod için başlatma - 3 kez 0x30 gönder
    uint8_t init_cmd = 0x30 | lcd->Arka_Isik;
    for (int i = 0; i < 3; i++) {
        veri[0] = init_cmd | LCD_EN;  // EN yüksek
        hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT);
        if (hal_status != HAL_OK) return LCD_I2C_ERROR;
        LCD_bekle(LCD_4BIT_INIT_DELAY);
        veri[0] = init_cmd & ~LCD_EN;  // EN düşük
        hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT);
        if (hal_status != HAL_OK) return LCD_I2C_ERROR;
        // Not: Sabit gecikmeler (10ms) RTOS'da osDelay ile değiştirilebilir.
        LCD_bekle(10);
    }
    
    // 4 bit mod komutunu gönder (0x20)
    uint8_t mode_cmd = 0x20 | lcd->Arka_Isik;
    veri[0] = mode_cmd | LCD_EN;
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(LCD_4BIT_INIT_DELAY);
    veri[0] = mode_cmd & ~LCD_EN;
    hal_status = HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT);
    if (hal_status != HAL_OK) return LCD_I2C_ERROR;
    LCD_bekle(10);
    
    return LCD_OK;
}

// Public fonksiyonlar
LCD_Status_t LCD_kur(I2C_HandleTypeDef* I2C_Port, uint8_t I2C_Adres, uint8_t Sutun, uint8_t Satir, LCD_t* lcd) {
    if (lcd == NULL || I2C_Port == NULL) {
        return LCD_INVALID_PARAM;
    }
    
    // I2C bağlantısını test et
    if (HAL_I2C_IsDeviceReady(I2C_Port, I2C_Adres << 1, 3, LCD_I2C_TIMEOUT) != HAL_OK) {
        return LCD_I2C_ERROR;
    }
    
    lcd->I2C_Port = I2C_Port;
    lcd->I2C_Adres = I2C_Adres;
    lcd->Sutun = Sutun;
    lcd->Satir = Satir;
    lcd->DisplayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    lcd->DisplayMode = LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    lcd->Arka_Isik = LCD_BACKLIGHT;
    
    // Başlangıç için ekstra bekle - LCD güç kararlılığı için
    LCD_bekle(LCD_INIT_DELAY);

    // Arka ışığı açıp başlatma verisini gönder
    uint8_t veri[1];
    veri[0] = lcd->Arka_Isik;
    if (HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT) != HAL_OK) {
        return LCD_I2C_ERROR;
    }
    LCD_bekle(20);
    
    // HD44780 4-bit başlatma prosedürü
    LCD_Status_t status = LCD_init_4bit(lcd);
    if (status != LCD_OK) return status;
    
    // LCD konfigürasyon komutları
    status = LCD_komut_gonder(lcd, LCD_FUNCTIONSET | 0x08);
    if (status != LCD_OK) return status;
    
    status = LCD_komut_gonder(lcd, LCD_DISPLAYCONTROL | lcd->DisplayControl);
    if (status != LCD_OK) return status;
    
    status = LCD_komut_gonder(lcd, LCD_CLEARDISPLAY);
    if (status != LCD_OK) return status;
    
    status = LCD_komut_gonder(lcd, LCD_ENTRYMODESET | lcd->DisplayMode);
    if (status != LCD_OK) return status;
    
    status = LCD_komut_gonder(lcd, LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    if (status != LCD_OK) return status;

    // Ekranı bir kez daha temizle
    return LCD_temizle(lcd);
}

uint8_t LCD_adres_tara(I2C_HandleTypeDef* I2C_Port) {
    if (I2C_Port == NULL) return 0;
    
    for (uint8_t adres = 0x20; adres <= 0x3F; adres++) {
        if (HAL_I2C_IsDeviceReady(I2C_Port, adres << 1, 1, LCD_I2C_TIMEOUT) == HAL_OK) {
            return adres;
        }
    }
    return 0; // Bulunamadı
}

LCD_Status_t LCD_temizle(LCD_t* lcd) {
    LCD_Status_t status = LCD_komut_gonder(lcd, LCD_CLEARDISPLAY);
    if (status == LCD_OK) {
        LCD_bekle(LCD_CLEAR_DELAY); // Temizleme işlemi için ekstra bekleme
    }
    return status;
}

LCD_Status_t LCD_satir_temizle(LCD_t* lcd, uint8_t satir) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    if (satir >= lcd->Satir) return LCD_INVALID_PARAM;
    
    LCD_Status_t status = LCD_imlec(lcd, 0, satir);
    if (status != LCD_OK) return status;
    
    for (uint8_t i = 0; i < lcd->Sutun; i++) {
        status = LCD_karakter(lcd, ' ');
        if (status != LCD_OK) return status;
    }
    
    return LCD_imlec(lcd, 0, satir); // İmleci satır başına getir
}

LCD_Status_t LCD_imlec(LCD_t* lcd, uint8_t x, uint8_t y) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    if (LCD_pozisyon_kontrol(lcd, x, y) != LCD_OK) return LCD_INVALID_PARAM;
    
    uint8_t offset;
    
    switch (y) {
        case 0: offset = 0x00; break;
        case 1: offset = 0x40; break;
        case 2: offset = 0x14; break;
        case 3: offset = 0x54; break;
        default: offset = 0x00; break;
    }
    
    return LCD_komut_gonder(lcd, LCD_SETDDRAMADDR | (offset + x));
}

LCD_Status_t LCD_yaz(LCD_t* lcd, const char* str) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    if (str == NULL) return LCD_INVALID_PARAM;
    
    LCD_Status_t status;
    while (*str) {
        status = LCD_karakter(lcd, *str++);
        if (status != LCD_OK) return status;
    }
    return LCD_OK;
}

LCD_Status_t LCD_yaz_pozisyon(LCD_t* lcd, uint8_t x, uint8_t y, const char* str) {
    LCD_Status_t status = LCD_imlec(lcd, x, y);
    if (status != LCD_OK) return status;
    
    return LCD_yaz(lcd, str);
}

LCD_Status_t LCD_printf(LCD_t* lcd, const char* format, ...) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    if (format == NULL) return LCD_INVALID_PARAM;
    
    char buffer[LCD_MAX_STRING_LENGTH];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    return LCD_yaz(lcd, buffer);
}

LCD_Status_t LCD_printf_pozisyon(LCD_t* lcd, uint8_t x, uint8_t y, const char* format, ...) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    if (LCD_pozisyon_kontrol(lcd, x, y) != LCD_OK) return LCD_INVALID_PARAM;
    if (format == NULL) return LCD_INVALID_PARAM;
    
    char buffer[LCD_MAX_STRING_LENGTH];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    LCD_Status_t status = LCD_imlec(lcd, x, y);
    if (status != LCD_OK) return status;
    
    return LCD_yaz(lcd, buffer);
}

LCD_Status_t LCD_karakter(LCD_t* lcd, char data) {
    return LCD_veri_gonder(lcd, (uint8_t)data);
}

LCD_Status_t LCD_ozel_karakter(LCD_t* lcd, uint8_t konum, uint8_t karakter[8]) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    if (konum > 7 || karakter == NULL) return LCD_INVALID_PARAM;
    
    LCD_Status_t status = LCD_komut_gonder(lcd, LCD_SETCGRAMADDR | (konum << 3));
    if (status != LCD_OK) return status;
    
    for (uint8_t i = 0; i < 8; i++) {
        status = LCD_veri_gonder(lcd, karakter[i]);
        if (status != LCD_OK) return status;
    }
    
    return LCD_OK;
}

LCD_Status_t LCD_ortala(LCD_t* lcd, uint8_t satir, const char* yazi) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    if (satir >= lcd->Satir || yazi == NULL) return LCD_INVALID_PARAM;
    
    uint8_t uzunluk = strlen(yazi);
    uint8_t bosluk = 0;

    if (uzunluk < lcd->Sutun) {
        bosluk = (lcd->Sutun - uzunluk) / 2;
    }

    LCD_Status_t status = LCD_imlec(lcd, bosluk, satir);
    if (status != LCD_OK) return status;

    return LCD_yaz(lcd, yazi);
}

LCD_Status_t LCD_arkaIsik_ac(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    lcd->Arka_Isik = LCD_BACKLIGHT;
    
    uint8_t veri[1];
    veri[0] = lcd->Arka_Isik;
    if (HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT) != HAL_OK) {
        return LCD_I2C_ERROR;
    }
    return LCD_OK;
}

LCD_Status_t LCD_arkaIsik_kapat(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    lcd->Arka_Isik = LCD_NOBACKLIGHT;
    
    uint8_t veri[1];
    veri[0] = lcd->Arka_Isik;
    if (HAL_I2C_Master_Transmit(lcd->I2C_Port, lcd->I2C_Adres << 1, veri, 1, LCD_I2C_TIMEOUT) != HAL_OK) {
        return LCD_I2C_ERROR;
    }
    return LCD_OK;
}

LCD_Status_t LCD_display_ac(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    lcd->DisplayControl |= LCD_DISPLAYON;
    return LCD_komut_gonder(lcd, LCD_DISPLAYCONTROL | lcd->DisplayControl);
}

LCD_Status_t LCD_display_kapat(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    lcd->DisplayControl &= ~LCD_DISPLAYON;
    return LCD_komut_gonder(lcd, LCD_DISPLAYCONTROL | lcd->DisplayControl);
}

LCD_Status_t LCD_imlec_ac(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    lcd->DisplayControl |= LCD_CURSORON;
    return LCD_komut_gonder(lcd, LCD_DISPLAYCONTROL | lcd->DisplayControl);
}

LCD_Status_t LCD_imlec_kapat(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    lcd->DisplayControl &= ~LCD_CURSORON;
    return LCD_komut_gonder(lcd, LCD_DISPLAYCONTROL | lcd->DisplayControl);
}

LCD_Status_t LCD_imlec_yanip_son_ac(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    lcd->DisplayControl |= LCD_BLINKON;
    return LCD_komut_gonder(lcd, LCD_DISPLAYCONTROL | lcd->DisplayControl);
}

LCD_Status_t LCD_imlec_yanip_son_kapat(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    lcd->DisplayControl &= ~LCD_BLINKON;
    return LCD_komut_gonder(lcd, LCD_DISPLAYCONTROL | lcd->DisplayControl);
}

LCD_Status_t LCD_ana_ekran(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    LCD_Status_t status = LCD_komut_gonder(lcd, LCD_RETURNHOME);
    if (status == LCD_OK) {
        LCD_bekle(LCD_CLEAR_DELAY); // Home komutu için ekstra bekleme
    }
    return status;
}

LCD_Status_t LCD_ekran_sola_kaydir(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    return LCD_komut_gonder(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

LCD_Status_t LCD_ekran_saga_kaydir(LCD_t* lcd) {
    if (LCD_parametreler_kontrol(lcd) != LCD_OK) return LCD_INVALID_PARAM;
    
    return LCD_komut_gonder(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

//#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADRESS 0x3C

#define I2C_SDA 1
#define I2C_SCL 2

Adafruit_SSD1306 oled_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup_oled(uint8_t text_size, uint16_t color, int16_t cursor_x, int16_t cursor_y)
{
    oled_display.clearDisplay();
    oled_display.setTextSize(text_size);
    oled_display.setTextColor(color);
    oled_display.setCursor(cursor_x, cursor_y);
    oled_display.println("Dupa");
    oled_display.display();
}

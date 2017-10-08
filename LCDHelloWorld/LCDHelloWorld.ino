//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
int c = 0;
void setup()
{
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Another Test Here");
  lcd.setCursor(0,1);
  lcd.print("2nd LINE HERE");
  lcd.setCursor(0,2);
  lcd.print("More Lines Here");
  lcd.setCursor(0,3);
  lcd.print("Test Here");
}

void loop(){
    // do simple count
    //lcd.init();
    //lcd.backlight();
    lctad.setCursor(0,0);
    lcd.print(c++);
    delay(1000);
}

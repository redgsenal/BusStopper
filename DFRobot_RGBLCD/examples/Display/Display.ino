 /*!
  * file Display.ino
  * brief display.
  * @n [Get the module here](等上架后添加商品购买链接)
  * @n This example Set the volume size and play music snippet.
  * @n [Connection and Diagram](等上架后添加wiki链接)
  *
  * Copyright	[DFRobot](http://www.dfrobot.com), 2016
  * Copyright	GNU Lesser General Public License
  *
  * @author [yangyang](971326313@qq.com)
  * version  V1.0
  * date  2017-2-10
  */

#include <Wire.h>
#include "DFRobot_RGBLCD.h"

DFRobot_RGBLCD lcd(16,2);  //16 characters and 2 lines of show

void setup() {
    // initialize
    lcd.init();
    // Print a message to the LCD.
    lcd.print("hello, world!");
}

void loop() {
    // Turn off the display:
    lcd.noDisplay();
    delay(500);
    // Turn on the display:
    lcd.display();
    delay(500);
}


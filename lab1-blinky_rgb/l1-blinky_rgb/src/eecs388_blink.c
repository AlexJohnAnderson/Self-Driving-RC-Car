#include <stdint.h>

#include "eecs388_lib.h"

/*
  Task 2.1: Review eecs388_lib.h and eecs388_lib.c 

  Task 2.2: Implement RGB blinky. (50 points)

    - set red, green, and blue leds as output mode, using gpio_mode() function
    - in the main loop, turn on/off a different LED at each iteration. 
      for example, 
         at 1st iteration
            turn on RED led
	         delay 500 ms 
	         turn off RED led
	         delay 300 ms.
	      at 2nd iteration
	         do the same but for GREEN led
	      at 3rd iteration
	         do the same but for BLUE led
	      at 4th iteration
	         back to RED led. 
	      ...
    - You must use arrays and the modulo (%) operator to change the color. 

  Task 2.3. Implement RGBA (bonus: +5 points)
    - rotate red, green, blue, and white (all three LEDs are enabled) colors.
*/

int main()
{
    int gpio = GREEN_LED;

    gpio_mode(gpio, OUTPUT);
    
    while(1)
    {
        gpio_write(gpio, ON);
        delay(1000);
        gpio_write(gpio, OFF);
        delay(300);
    }
}

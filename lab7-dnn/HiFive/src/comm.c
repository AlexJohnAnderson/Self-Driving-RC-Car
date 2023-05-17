#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

volatile int g_angle;

void raspberrypi_int_handler(int devid)
{
    char str[20];
    ser_readline(1, 20 , str);
    ser_printline(0, str);
    
}

int main()
{
    // initialize UART channels
    ser_setup(0); // uart0 (debug)
    ser_setup(1); // uart1 (raspberry pi)

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {
        // Check the rxwm bit of the UART ip register
        // See manual section 18.8 for more information
        if (ser_isready(1) & 2) {
            raspberrypi_int_handler(1);
        }
    }
}

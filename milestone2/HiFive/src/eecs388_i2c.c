#include <stdio.h>
#include <stdint.h>
#include "eecs388_lib.h"
#include "metal/i2c.h"
#include <string.h>
#include <stdlib.h>

struct metal_i2c *i2c;

#define bufWriteSize 5
#define bufReadSize  1

uint8_t bufWrite[bufWriteSize];
uint8_t bufRead[bufReadSize];

volatile int g_angle;

int forwardCheck = PCA9685_LED0_ON_L;


// The entire setup sequence
void set_up_I2C(){
    uint8_t oldMode;
    uint8_t newMode;
    _Bool success;

    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = MODE1_RESTART;
    printf("%d\n", bufWrite[0]);
    
    i2c = metal_i2c_get_device(0);

    if(i2c == NULL)
    {
        printf("Connection Unsuccessful\n");
    }
    else
    {
        printf("Connection Successful\n");
    }
    
    // Setup Sequence
    metal_i2c_init(i2c, I2C_BAUDRATE, METAL_I2C_MASTER);
    success = metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 2, bufWrite, METAL_I2C_STOP_DISABLE);   // reset
    delay(100);
    printf("resetting PCA9685 control 1\n");

    // Initial Read of control 1
    bufWrite[0] = PCA9685_MODE1;    // Address
    success = metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 1, bufRead, 1);            // initial read
    printf("Read success: %d and control value is: %d\n", success, bufWrite[0]);
    
    // Configuring Control 1
    oldMode = bufRead[0];
    newMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP;
    printf("sleep setting is %d\n", newMode);
    bufWrite[0] = PCA9685_MODE1;    // address
    bufWrite[1] = newMode;          // writing to register
    success = metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 2, bufWrite, METAL_I2C_STOP_DISABLE);  // sleep
    bufWrite[0] = PCA9685_PRESCALE; // Setting PWM prescale
    bufWrite[1] = 0x79;
    success = metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 2, bufWrite, METAL_I2C_STOP_DISABLE);  // sets prescale
    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = 0x01 | MODE1_AI | MODE1_RESTART;
    printf("on setting is %d\n", bufWrite[1]);
    success = metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 2, bufWrite, METAL_I2C_STOP_DISABLE);  // awake
    delay(100);
    printf("Setting the control register\n");
    bufWrite[0] = PCA9685_MODE1;
    success = metal_i2c_transfer(i2c, PCA9685_I2C_ADDRESS, bufWrite, 1, bufRead, 1);           // initial read
    printf("Set register is %d\n", bufRead[0]);

} 

void breakup(int bigNum, uint8_t* low, uint8_t* high){

    *low = (bigNum & 0xff);
    *high = (bigNum>>8) & 0xff; 
}

void write(int LED, uint8_t ON_L, uint8_t ON_H, uint8_t OFF_L, uint8_t OFF_H){

    bufWrite[0] = LED;
    bufWrite[1] = ON_L;
    bufWrite[2] = ON_H;
    bufWrite[3] = OFF_L;
    bufWrite[4] = OFF_H;

    metal_i2c_write(i2c, PCA9685_I2C_ADDRESS, 5, bufWrite, METAL_I2C_STOP_ENABLE);
}

void steering(int angle){

    uint8_t var1;
    uint8_t var2;
    int valToBreak = getServoCycle(angle);
    breakup(valToBreak, &var1, &var2);
    write(PCA9685_LED1_ON_L, 0, 0, var1, var2);
}

/*
    Motor config/stop. 

    Task 1: using the breakup(..) and write(..) function, implement the 
    funcion defined below. This function configures the motor by 
    writing a value of 280 to the motor.

    Usage: when calling stopMotor for calibration, include
    a 2 second delay for proper configuration. 

    Note: the motor's speed controller is configured for 
    LED0 and the servo for LED1. 

    Example Use: stopMotor(); -> sets LED0_Off to 280 
*/
void stopMotor()
{
    uint8_t var1;
    uint8_t var2;
    breakup(280, &var1, &var2);
    write(PCA9685_LED0_ON_L, 0, 0, var1, var2);

}

/*
    Motor Forward

    Task 2: using breakup(..) and write(..), implement 
    the function defined below to Drive the motor 
    forward. The given speedFlag will alter the 
    motor speed as follows:
    
    speedFlag = 1 -> value to breakup = 303 
    speedFlag = 2 -> value to breakup = 305
    speedFlag = 3 -> value to breakup = 307

    Note: the motor's speed controller is configured for 
    LED0 and the servo for LED1. 

    Example Use: driveForward(3); -> sets LED0_Off to 307 
*/

void driveForward(int speedFlag)
{
    uint8_t var1;
    uint8_t var2;
    int speed = 301;
    speed = speed + (2*speedFlag);
    breakup(speed, &var1, &var2);
    write(PCA9685_LED0_ON_L, 0, 0, var1, var2);
}

/*void driveReverse(int speedFlag)
{
    uint8_t var1;
    uint8_t var2;  
    if (forwardCheck == 0)
    {
    int speed = 269;
    speed = speed - (2*speedFlag);
    breakup(speed, &var1, &var2);
    write(PCA9685_LED0_ON_L, 0, 0, var1, var2);
    }
    else
    {
    int speed = 269;
    speed = speed - (2*speedFlag);
    speed = 280;
    speed = speed - (2*speedFlag);
    speed = 269;
    speed = speed - (2*speedFlag);
    breakup(speed, &var1, &var2);
    write(PCA9685_LED0_ON_L, 0, 0, var1, var2);
    }
    
}
*/

void raspberrypi_int_handler(int devid)
{
  //  printf("rpi handler\n");
    char str[30];
    ser_readline(1, 30 , str);
    printf("\n");
    ser_printline(0, str);
    printf("\n");
    int val;
    char *  lhs = "angle";
    //  printf("TEST:\n");
    if(strncmp(lhs, str, 5) == 0)
    {
        sscanf(str+5, "%d", &val);
        g_angle = val;
    }
    else
    {
        printf("Failed\n");
    }

}

int main()
{   //*
    // Initialize I2C

    set_up_I2C();
    //initialize UART channels;
    ser_setup(1); // uart1 (raspberry pi)

    printf("Setup Completed.\n");
    printf("Begin the main loop.\n");
    //Initialize global angle
    
    g_angle = 0;
    //delay(2000);
    
    
    stopMotor();
    delay(3000);
    
    driveForward(3);
   // delay(2000);
   // driveReverse(2);
   // delay(2000);
  //  steering(0);
    

   // steering(-45);
   // delay(1000);
   // steering(0);
   // delay(1000);
    

    while (1) {
        
         if (ser_isready(1) & 2) {
            
            printf("Main Loop\n");
            raspberrypi_int_handler(1);
            //printf("Handler Working\n");
            //printf("Angle: %d \n", g_angle);
            printf("Angle: %d\n", g_angle);
            steering(g_angle);
        
         }
         
    }
}

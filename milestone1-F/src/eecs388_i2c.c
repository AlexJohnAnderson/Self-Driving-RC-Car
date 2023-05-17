#include <stdio.h>
#include <stdint.h>
#include "eecs388_lib.h"
#include "metal/i2c.h"

struct metal_i2c *i2c;

const uint8_t bufWriteSize = 5;
const uint8_t bufReadSize  = 1;

uint8_t bufWrite[bufWriteSize];
uint8_t bufRead[bufReadSize];

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
    metal_i2c_init(i2c,I2C_BAUDRATE,METAL_I2C_MASTER);
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);  // reset
    delay(100);
    printf("resetting PCA9685 control 1\n");

    // Initial Read of control 1
    bufWrite[0] = PCA9685_MODE1;    // Address
    success = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,1,bufRead,1);            // initial read
    printf("Read success: %d and control value is: %d\n", success, bufWrite[0]);
    
    // Configuring Control 1
    oldMode = bufRead[0];
    newMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP;
    printf("sleep setting is %d\n", newMode);
    bufWrite[0] = PCA9685_MODE1;    // address
    bufWrite[1] = newMode;          // writing to register
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);  // sleep
    bufWrite[0] = PCA9685_PRESCALE; // Setting PWM prescale
    bufWrite[1] = 0x79;
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);  // sets prescale
    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = 0x01 | MODE1_AI | MODE1_RESTART;
    printf("on setting is %d\n", bufWrite[1]);
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);  // awake
    delay(100);
    printf("Setting the control register\n");
    bufWrite[0] = PCA9685_MODE1;
    success = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,1,bufRead,1);            // initial read
    printf("Set register is %d\n",bufRead[0]);

} 

/*  
    Defining the breakup function

    Task 1: breaking 12 bit into two 8-bit
    Define the function created that recieves a 12 bit number,
    0 to 4096 and breaks it up into two 8 bit numbers.

    Assign these values to a referenced value handed into
    the function. 

    ex: 
    uint8_t variable1;
    uint8_t variable2;
    breakup(2000,&variable1,&variable2);
    variable1 -> low 8 bits of 2000
    variable2 -> high 8 bits of 2000 
*/

void breakup(int bigNum, uint8_t* low, uint8_t* high){
    int bigNum; 
    low = bigNum & 0xff;
    high = (bigNum>>8) & 0xff;
}

/*
    Changing Steering Heading

    Task 2: using getServoCycle(), bufWrite, bufRead, 
    breakup(), and write(...), implement 
    the function defined below to control the servo
    by sending it an angle ranging from -45 to 45.

    Use the getServoCycle function to get the value to 
    breakup.

    ex: 
    int valToBreak = getServoCycle(45);
    >>>sets valToBreak to 355

    note: the motor's speed controller is either 
    LED0 or LED1 depending on where its plugged into 
    the board. If its LED1, simply add 4 to the LED0
    address

    ex: steering(0); -> driving angle forward
*/


void write(int LED, uint8_t ON_L, uint8_t ON_H, uint8_t OFF_L, uint8_t OFF_H){

metal_i2c_write(i2c, bufWrite[0], 5, bufWrite[1], METAL_I2C_STOP_DISABLE);

}



void steering(int angle){
    uint8_t var1;
    uint8_t var2;
    int valToBreak = getServoCycle(angle);
    breakup(valToBreak, &var1, &var2);
    write(PCA9685_LED0_ON_L, 0, 0, var1, var2);


   //convert angles to cycles
   //write the on/off cycles to PCA
}

/*
    Writing with i2c

    -Task 3: using bufWrite, bufRead, breakup(), and
    and metal_i2c_transfer(), implement the funcion made
    below.

    ex:
    uint8_t variable1;
    uint8_t variable2;
    breakup(2000,&variable1,&variable2);
    write(PCA9685_LED0_ON_L, &variable1, &variable2);
*/



int main()
{
    set_up_I2C();

    /*
        Partially Controlling the PCA9685

        Add necessary function calls here to 
        demo the three functions in operation.
    */
    
}

/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.76
        Device            :  PIC16F18446
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"


volatile uint16_t addressCount;
volatile char dmxFrame[512];

void update_lightsPWM(void); // function directed to update light value once 
                             // full dmx frame received. 
void LEDinit(void);
void LED_setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t white);

void __interrupt() INTERRUPT_InterruptManager (void)
{
    if(PIR3bits.RC1IF == 1){ 
        
        if(RC1STAbits.FERR == 1){
            
            //framing error occurred
            addressCount = 0;
            dmxFrame[addressCount] = RC1REG; // reading/placing start bit in address 0 
            PIR3bits.RC1IF = 0;
        }else if(RC1STAbits.OERR == 1){
            //restart uart
            RC1REG; // read RC1 REG
            
            RC1STAbits.CREN = 0;
            RC1STAbits.CREN = 1;
            PIR3bits.RC1IF = 0;
            
        }else{
            addressCount++; // increasing address
            if(addressCount == 512){
                // dmxFrame Complete
                update_lightsPWM();
              
                addressCount = 0;
                PIR3bits.RC1IF = 0;
            }
            dmxFrame[addressCount] = RC1REG;
            PIR3bits.RC1IF = 0;
            
        
        }
        
    }
}   

void main(void)
{
    // initialize the device
    addressCount = 0;
    for(int i = 0; i < 512; i++){
        dmxFrame[i] = 0;
    }
//    PIR3bits.RC1IF = 0;
    SYSTEM_Initialize();
    LEDinit();
    

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    while (1)
    {
        // Add your application code
    }
}


void LEDinit(void){
    T2CLKCON = 0x01; // configuring clock source to Fosc/4
    T2CONbits.CKPS = 0b101;
    
    T2CONbits.ON = 1; //turning on timer2
    // default period is 255 which gives maximum PWM resolution
    CCP1CONbits.CCP1EN = 1;
    CCP1CONbits.MODE = 0b1100; //blue RC3
    CCP2CONbits.CCP2EN = 1;
    CCP2CONbits.MODE = 0b1100; //green RC6
    CCP3CONbits.CCP3EN = 1;
    CCP3CONbits.MODE = 0b1100; // red RC7
    CCP4CONbits.CCP4EN = 1;
    CCP4CONbits.MODE = 0b1100;// white RC4
    
    RC3PPS = 0x09; // CCP1
    RC6PPS = 0x0A; // CCP2
    RC7PPS = 0x0B; // CCP3
    RC4PPS = 0x0C; // CCP4
    TRISC3 = 0;
    TRISC6 = 0;
    TRISC7 = 0;
    TRISC4 = 0;
    
}

void LED_setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t white){
   
   uint16_t redBit = red / 2;
   uint16_t greenBit = green / 2;
   uint16_t blueBit = blue / 2;
   uint16_t whiteBit = white / 2;
   
   CCPR1H = blueBit >> 8;
   CCPR1L = blueBit;
   
   CCPR2H = greenBit >> 8;
   CCPR2L = greenBit;
   
   CCPR3H = redBit >> 8;
   CCPR3L = redBit;
   
   CCPR4H = whiteBit >> 8;
   CCPR4L = whiteBit;
}

void update_lightsPWM(void){
    // channel 2 of dmxFrame corresponds with the first slider on uDMX app
    // slider 1 is red; slider 2 is green, slider 3 is blue; slider 4 is white
    LED_setColor(dmxFrame[2], dmxFrame[3], dmxFrame[4], dmxFrame[5]);
}
/**
 End of File
*/
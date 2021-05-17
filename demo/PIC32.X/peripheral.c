/*
 * Peripheral access functions (digital/analog inputs/outputs)
 *
 * @file        peripheral.c
 * @author      --
 * @copyright   2021 --
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "peripheral.h"
#include "CANopen.h"
#include "OD.h"


/* CANopen LED diodes, as present on Explorer 16 and Max32 boards. */
#define CAN_INIT_LEDS() TRISAbits.TRISA2 = TRISCbits.TRISC1 = TRISAbits.TRISA3=0
#define CAN_RUN_LED LATAbits.LATA2 = LATCbits.LATC1
#define CAN_ERROR_LED LATAbits.LATA3

/* Variables used for triggering TPDO, app_programRt(). */
OD_extension_t OD_6000_DI_extension = {
    .object = NULL,
    .read = OD_readOriginal,
    .write = OD_writeOriginal
};
uint8_t *OD_6000_DI_flagsPDO = NULL;

/******************************************************************************/
CO_ReturnError_t peripheral_init(void) {
    /* CANopen led diodes */
    CAN_INIT_LEDS();
    CAN_RUN_LED = 0;
    CAN_ERROR_LED = 1;

    /* Setup extension and flags for triggering TPDO. */
    OD_extension_init(OD_ENTRY_H6000_readDigitalInput_8_bit,
                      &OD_6000_DI_extension);
    OD_6000_DI_flagsPDO = OD_getFlagsPDO(OD_ENTRY_H6000_readDigitalInput_8_bit);

    return CO_ERROR_NO;
}


/******************************************************************************/
void app_peripheralRead(CO_t *co, uint32_t timer1usDiff) {

    /* All analog inputs must be read or interrupt source for RT thread won't be
     * cleared. See analog input configuration in main_PIC32.c */
    OD_RAM.x6401_readAnalogInput_16_bit[0x0] = ADC1BUF0;
    OD_RAM.x6401_readAnalogInput_16_bit[0x1] = ADC1BUF1;
    OD_RAM.x6401_readAnalogInput_16_bit[0x2] = ADC1BUF2;
    OD_RAM.x6401_readAnalogInput_16_bit[0x3] = ADC1BUF3;
    OD_RAM.x6401_readAnalogInput_16_bit[0x4] = ADC1BUF4;
    OD_RAM.x6401_readAnalogInput_16_bit[0x5] = ADC1BUF5;
    OD_RAM.x6401_readAnalogInput_16_bit[0x6] = ADC1BUF6;
    OD_RAM.x6401_readAnalogInput_16_bit[0x7] = ADC1BUF7;
    OD_RAM.x6401_readAnalogInput_16_bit[0x8] = ADC1BUF8;
    OD_RAM.x6401_readAnalogInput_16_bit[0x9] = ADC1BUF9;
    OD_RAM.x6401_readAnalogInput_16_bit[0xA] = ADC1BUFA;
    OD_RAM.x6401_readAnalogInput_16_bit[0xB] = ADC1BUFB;
    OD_RAM.x6401_readAnalogInput_16_bit[0xC] = ADC1BUFC;
    OD_RAM.x6401_readAnalogInput_16_bit[0xD] = ADC1BUFD;
    OD_RAM.x6401_readAnalogInput_16_bit[0xE] = ADC1BUFE;
    OD_RAM.x6401_readAnalogInput_16_bit[0xF] = ADC1BUFF;

    /* Read digital inputs, chipKIT Pins 0..7 */
    uint8_t digIn = 0;
    if(PORTFbits.RF2  != 0) digIn |= 0x01;
    if(PORTFbits.RF8  != 0) digIn |= 0x02;
    if(PORTEbits.RE8  != 0) digIn |= 0x04;
    if(PORTDbits.RD0  != 0) digIn |= 0x08;
    if(PORTCbits.RC14 != 0) digIn |= 0x10;
    if(PORTDbits.RD1  != 0) digIn |= 0x20;
    if(PORTDbits.RD2  != 0) digIn |= 0x40;
    if(PORTEbits.RE9  != 0) digIn |= 0x80;

    if (digIn != OD_RAM.x6000_readDigitalInput_8_bit[0]) {
        OD_RAM.x6000_readDigitalInput_8_bit[0] = digIn;
        OD_requestTPDO(OD_6000_DI_flagsPDO, 1); /* subindex is 1 */
    }

    /* Read digital inputs, chipKIT Pins 8..15 */
//    digIn = 0;
//    if(PORTDbits.RD12 != 0) digIn |= 0x01;
//    if(PORTDbits.RD3  != 0) digIn |= 0x02;
//    if(PORTDbits.RD4  != 0) digIn |= 0x04;
//    if(PORTCbits.RC4  != 0) digIn |= 0x08;
//    if(PORTAbits.RA2  != 0) digIn |= 0x10;
//    if(PORTAbits.RA3  != 0) digIn |= 0x20;
//    if(PORTFbits.RF13 != 0) digIn |= 0x40;
//    if(PORTFbits.RF12 != 0) digIn |= 0x80;
//
//    if (digIn != OD_RAM.x6000_readDigitalInput_8_bit[1]) {
//        OD_RAM.x6000_readDigitalInput_8_bit[1] = digIn;
//        OD_requestTPDO(OD_6000_DI_flagsPDO, 2); /* subindex is 2 */
//    }
}


/******************************************************************************/
void app_peripheralWrite(CO_t *co, uint32_t timer1usDiff) {
    CAN_RUN_LED = CO_LED_GREEN(co->LEDs, CO_LED_CANopen);
    CAN_ERROR_LED = CO_LED_RED(co->LEDs, CO_LED_CANopen);

    /* Write to digital outputs */
    //uint8_t digOut = OD_RAM.x6200_writeDigitalOutput_8_bit[0];
    //LATAbits.LATA0 = (digOut & 0x01) ? 1 : 0;
    //LATAbits.LATA1 = (digOut & 0x02) ? 1 : 0;
    //LATAbits.LATA4 = (digOut & 0x10) ? 1 : 0;
    //LATAbits.LATA5 = (digOut & 0x20) ? 1 : 0;
    //LATAbits.LATA6 = (digOut & 0x40) ? 1 : 0;
    //LATAbits.LATA7 = (digOut & 0x80) ? 1 : 0;
}

#include <msp430.h>

#define ushort unsigned short
    #define USHORT  ushort

    #define uchar unsigned char
    #define uint8 uchar
    #define INT8U uint8
    #define u8  uchar

    #define uint16 unsigned int
    #define INT16U uint16
    #define u16 uint16

    #define uint32 unsigned long
    #define INT32U uint32
    #define u32 uint32
#define L_RSset P1OUT|=BIT6	//_RES  hardware reset
#define L_LDset P2OUT|=BIT5 //A0
#define L_CKset P2OUT|=BIT4	//SCLK
#define L_DAset P2OUT|=BIT3	//SI
#define L_CSset P1OUT|=BIT7 //CS
#define L_RSreset P1OUT&=~BIT6	//_RES  hardware reset
#define L_LDreset P2OUT&=~BIT5 //A0
#define L_CKreset P2OUT&=~BIT4	//SCLK
#define L_DAreset P2OUT&=~BIT3	//SI
#define L_CSreset P1OUT&=~BIT7 //CS
 void initLCDM(void);
 void ResetLCD(void);
 void ClearRAM(void);
 void Print8(uint8 x, uint8 y, uchar *pstr);
 void Print16(uint8 x, uint8 y, uchar *pstr,uchar n);
 void Printvalue(uint8 x, uint8 y, unsigned int value,uchar n);
 void reverse(uint8 x, uint8 y,uchar n);


#include "MSP430F1612.h"
#define CPU_CLOCK 8000000
#define delay_us(us) __delay_cycles(CPU_CLOCK/1000000*(us))
#define delay_ms(ms) __delay_cycles(CPU_CLOCK/1000*(ms))
#define RS_LOW  P2OUT &= ~BIT7
#define RS_HIGH P2OUT |= BIT7
#define RW_LOW  P2OUT &= ~BIT6
#define RW_HIGH P2OUT |= BIT6
#define E_LOW   P2OUT &= ~BIT5
#define E_HIGH  P2OUT |= BIT5
unsigned char a1[]={"The frequency is"}; 
unsigned char a2[]={"              50"};

void checkbusy (void)
{
   RS_LOW;
   RW_HIGH;
   E_HIGH;
   _NOP(); //一个机械周期
   _NOP();
   P3DIR &= ~BIT7;
   while(P3IN & BIT7);
   P3DIR |= BIT7;
   E_LOW;
}
void writecommand(unsigned char command)
{
  checkbusy();
  RS_LOW;
  RW_LOW;
  E_HIGH;
  _NOP();
  _NOP();
  P3OUT = command;
  _NOP();
  _NOP();
  E_LOW;
}
void writeword(unsigned char word)
{
   checkbusy();
   RS_HIGH;
   RW_LOW;
   E_HIGH;
   _NOP();
   _NOP();
   P3OUT = word;
   E_LOW;
}
void setxy(unsigned char x , unsigned char y) //写入指令
{
  unsigned char wei;
  if(y == 0X01)
    wei = 0X80 + x; //16 X 2
  else
    wei = 0XC0 + x;
  writecommand(wei);
}
void writedata(unsigned char x , unsigned char y,unsigned char data)
{
  setxy(x,y);
  writeword(data);
}
void writestring(unsigned char x , unsigned char y,unsigned char *p)
{
  setxy(x,y);
  while (*p)
  {
    P3OUT = *p;
    writeword(*p);
    p++;
  }  
}

void LEDint (void) //初始化
{
  writecommand(0X38);//设置16*2显示，5*7点阵，8位数据接口
  delay_ms(10);
  writecommand(0X0F);
  delay_ms(10);
  writecommand(0X06);
  delay_ms(10);
  writecommand(0X01);
}
void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  BCSCTL1 &= ~XT2OFF;
  do
  {
    IFG1 &= ~OFIFG;
    _NOP();
  }while((IFG1&OFIFG));
  BCSCTL2 |= SELS +SELM1;  //8MHZ
  P2SEL = 0X00;
  P3SEL = 0X00;
  P3DIR = 0XFF;
  P2DIR = 0XFF;
  LEDint();
  _DINT();
  writestring(0,1,a1);
  writestring(0,2,a2);
  while(1);
}


//光电传感器黑高白低

#include<msp430x16x.h>
#include "LCD1602.h"
#define CPU_F ((double)8000000)
#define delay_us(x)    __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x)    __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define left_reversal       P1OUT=0X02+0X10;      //3轮反转
#define right_reversal      P1OUT=0X01+0X20;      //4轮反转
#define back_both           P1OUT=0X02+0X20;      //3,4反转
#define straight_both       P1OUT=0X01+0x10;      //3,4正传

/**********************************************************
*P1.2控制OUT3、4 P1.3控制OUT1、2
*P1.0 P1.1控制3正反转 P1.4、P1.5控制4正反转
*光电模块：1左微 2右微 3左 4右
*分布：                *1     *2问




                *3                   *4
**********************************************************/
char diff[3];
char key=0;

void system(void)                       //I/O端口定义
{
     int j;
     P2DIR=0X00;                         //设置P2.0~P2.4为光电传感器的输入引脚

     BCSCTL1&=~XT2OFF;              //开启TX2
     do
     {
       IFG1&=~OFIFG;                //清除振荡器失效标志
       for(j=0xff;j>0;j--);         //延时，待稳定
     }
     while((IFG1&OFIFG)!=0);
     BCSCTL2|=SELS;                 //SMCLK=XT2
     BCSCTL2|=DIVS0+DIVS1;           //SMCLK 8分频

     P1DIR=0x3F;                   //P1.0~P1.5为输出模式     
     P1SEL=BIT2+BIT3;              //设置P1.2 P1.3为外围模块的功能 输出PWM

     CCR0=100;                    //频率10000
     CCTL1=OUTMOD_7;                //CCR1复位/置位
     CCR1=50;                     //CCR1PWN初始占空比50%
     CCTL2=OUTMOD_7;                //CCR2复位/置位
     CCR2=50;                     //CCR1PWN初始占空比50%
     TACTL=TASSEL_2+MC_1;           //定时器A时钟源为SMCLK，增计数模式

     straight_both;                //设置正反转
     //P2IE|=BIT3+BIT4;                 //开启P2.3 P2.4中断使能
     P2IES &=~(BIT3+BIT4);             //设置P2.3 P2.4都为上升沿触发
     P2IFG &=~(BIT3+BIT4);             //清除P2.3 P2.4中断标志
     _EINT();
}

void PWM1(char a)                      //设置左轮PWM占空比
{
  if(CCR1>95||CCR1<5)                  //如果占空比大于95或者小于5强行设置为50
  {
    CCR1=50;
  }
  else
  {
    CCR1=a;
  }
}

void PWM2(char b)                      //设置右轮PWM占空比
{
  if(CCR2>95||CCR2<5)                  //如果占空比大于95或者小于5强行设置为50
  {
    CCR2=50;
  }
  else
  {
    CCR2=b;
  }
}

void slow(void)                         //当1，2检测到未检测到黑线时向前慢走
{
  PWM1(60);
  PWM2(60);
}

void go_straight(void)                 //直走
{
  PWM1(75);
  PWM2(75);
}

void slight_right(void)                //右转微调
{
  PWM1(78);
  PWM2(55);
}

void slight_left(void)                //左转微调
{
  PWM1(55);
  PWM2(78);
}

void right(void)                      //右转
{
  right_reversal;
  PWM1(78);
  PWM2(62);
}

void left(void)                      //左转
{
  left_reversal;
  PWM1(62);
  PWM2(78);
}

void right_higher(void)                  //右转更高级
{
  right_reversal;
  PWM1(70);
  PWM2(76);
  delay_ms(10);
  //straight_both;
  right();
  delay_ms(5);                            //延时一段时间让传感器1 2离开黑线
  while((P2IN&0X06)==0x00);                //若传感器1 2一直读白则循环
  left_reversal;
  PWM1(65);
  PWM2(78);
  delay_ms(8);
  straight_both;
}

void left_higher(void)                  //左转更高级
{
  left_reversal;
  PWM1(76);
  PWM2(70);
  delay_ms(10);
  straight_both;
  left();
  delay_ms(5);                            //延时一段时间让传感器1 2离开黑线
  while((P2IN&0X06)==0x00);               //若传感器1 2一直读白则循环
  right_reversal;
  PWM1(78);
  PWM2(65);
  delay_ms(8);
  straight_both;
}

void right_middle(void)                  //右转中级
{
  right_reversal;
  PWM1(60);
  PWM2(65);              //延时一段时间让传感器1 2离开黑线
  while((P2IN&0X06)==0x04);                //若传感器1 2一直读白则循环
  straight_both;
  PWM1(75);
  PWM2(50);      
  
}

void left_middle(void)                  //左转中级
{
  left_reversal;
  PWM1(65);
  PWM2(60);                  //延时一段时间让传感器1 2离开黑线
  while((P2IN&0X06)==0x02);               //若传感器1 2一直读白则循环
  straight_both;
  PWM1(50);
  PWM2(75);
}

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  system();                            //初始I\O端口

  diff[0]=1;diff[1]=1;diff[2]=1;
  while(1)
  { 
    char j;
         j=P2IN&0X06;
         switch(j)                  //只检查微调控制1 2
         { 
         case 0x04:slight_right(); break;          //2高 右微调
         case 0x02:slight_left();  break;          //1高 左微调
         case 0x06:slow();         break;          //都高慢走
         case 0x00:go_straight();  break;          //都低直走
         }
  }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
  char j;
  delay_ms(1);                            //这里延时给小车向前跑一段距离再来判断状态
  back_both();
  pwm1(40);
  pwm2(40);
  delay_ms(1);
  go_straight();

  if((P2IN&0X18)==0x18)
  {
    switch(diff[0])
    {
      case 1:right_higher();break;
      case 2:left_higher(); break;
      case 3:right_higher(); break; 
      case 4:right_higher(); break;
    }
    diff[0]++;
  }
  if((P2IN&0X18)==0X08)                             //当3检测到黑线
  {
    if((P2IN&0X06)==0X00)                          //转直角弯（第三种情况）
    {
      left_higher();
    }
    else                                          //丁字路口（第二种情况）
    {
      switch(diff[1])
      {
        case 1:slight_right();break;
        case 2:slight_right();break;
        case 3:left_higher();break;
        case 4:
             right_middle();
             while((P2IN&0X18)==0X00)
             {
                j=P2IN&0X06;
                switch(j)                  //只检查微调控制1 2
               { 
                case 0x04:PWM1(75);PWM2(30);  break;          //2高 右微调
                case 0x02:PWM1(30);PWM2(75);  break;          //1高 左微调
                case 0x00:slow();             break;          //都低停止
                case 0x06:PWM1(75);PWM2(75);  break;          //都高直走
               }
             }
             break;
      }
        diff[1]++;
    } 

  }
  if((P2IN&0X18)==0X10)                             //当4检测到黑线      
  {
    if((P2IN&0X02)==0X00)                          //转直角弯（第三种情况）
    {
      right_higher();
    }
    else                                         //丁字路口（第二种情况）
    {
      switch(diff[2])
      {
        case 1:go_straight();break;
        case 2:right_higher();break;
        case 3:go_straight();break;
        case 4:go_straight();break;
        case 5:go_straight();break;
      }
       diff[2]++;
    }
  }
  P2IFG&=~(BIT3+BIT4);
}
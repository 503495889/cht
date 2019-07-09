
//��紫�����ڸ߰׵�

#include<msp430x16x.h>
#include "LCD1602.h"
#define CPU_F ((double)8000000)
#define delay_us(x)    __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x)    __delay_cycles((long)(CPU_F*(double)x/1000.0))
#define left_reversal       P1OUT=0X02+0X10;      //3�ַ�ת
#define right_reversal      P1OUT=0X01+0X20;      //4�ַ�ת
#define back_both           P1OUT=0X02+0X20;      //3,4��ת
#define straight_both       P1OUT=0X01+0x10;      //3,4����

/**********************************************************
*P1.2����OUT3��4 P1.3����OUT1��2
*P1.0 P1.1����3����ת P1.4��P1.5����4����ת
*���ģ�飺1��΢ 2��΢ 3�� 4��
*�ֲ���                *1     *2��




                *3                   *4
**********************************************************/
char diff[3];
char key=0;

void system(void)                       //I/O�˿ڶ���
{
     int j;
     P2DIR=0X00;                         //����P2.0~P2.4Ϊ��紫��������������

     BCSCTL1&=~XT2OFF;              //����TX2
     do
     {
       IFG1&=~OFIFG;                //�������ʧЧ��־
       for(j=0xff;j>0;j--);         //��ʱ�����ȶ�
     }
     while((IFG1&OFIFG)!=0);
     BCSCTL2|=SELS;                 //SMCLK=XT2
     BCSCTL2|=DIVS0+DIVS1;           //SMCLK 8��Ƶ

     P1DIR=0x3F;                   //P1.0~P1.5Ϊ���ģʽ     
     P1SEL=BIT2+BIT3;              //����P1.2 P1.3Ϊ��Χģ��Ĺ��� ���PWM

     CCR0=100;                    //Ƶ��10000
     CCTL1=OUTMOD_7;                //CCR1��λ/��λ
     CCR1=50;                     //CCR1PWN��ʼռ�ձ�50%
     CCTL2=OUTMOD_7;                //CCR2��λ/��λ
     CCR2=50;                     //CCR1PWN��ʼռ�ձ�50%
     TACTL=TASSEL_2+MC_1;           //��ʱ��Aʱ��ԴΪSMCLK��������ģʽ

     straight_both;                //��������ת
     //P2IE|=BIT3+BIT4;                 //����P2.3 P2.4�ж�ʹ��
     P2IES &=~(BIT3+BIT4);             //����P2.3 P2.4��Ϊ�����ش���
     P2IFG &=~(BIT3+BIT4);             //���P2.3 P2.4�жϱ�־
     _EINT();
}

void PWM1(char a)                      //��������PWMռ�ձ�
{
  if(CCR1>95||CCR1<5)                  //���ռ�ձȴ���95����С��5ǿ������Ϊ50
  {
    CCR1=50;
  }
  else
  {
    CCR1=a;
  }
}

void PWM2(char b)                      //��������PWMռ�ձ�
{
  if(CCR2>95||CCR2<5)                  //���ռ�ձȴ���95����С��5ǿ������Ϊ50
  {
    CCR2=50;
  }
  else
  {
    CCR2=b;
  }
}

void slow(void)                         //��1��2��⵽δ��⵽����ʱ��ǰ����
{
  PWM1(60);
  PWM2(60);
}

void go_straight(void)                 //ֱ��
{
  PWM1(75);
  PWM2(75);
}

void slight_right(void)                //��ת΢��
{
  PWM1(78);
  PWM2(55);
}

void slight_left(void)                //��ת΢��
{
  PWM1(55);
  PWM2(78);
}

void right(void)                      //��ת
{
  right_reversal;
  PWM1(78);
  PWM2(62);
}

void left(void)                      //��ת
{
  left_reversal;
  PWM1(62);
  PWM2(78);
}

void right_higher(void)                  //��ת���߼�
{
  right_reversal;
  PWM1(70);
  PWM2(76);
  delay_ms(10);
  //straight_both;
  right();
  delay_ms(5);                            //��ʱһ��ʱ���ô�����1 2�뿪����
  while((P2IN&0X06)==0x00);                //��������1 2һֱ������ѭ��
  left_reversal;
  PWM1(65);
  PWM2(78);
  delay_ms(8);
  straight_both;
}

void left_higher(void)                  //��ת���߼�
{
  left_reversal;
  PWM1(76);
  PWM2(70);
  delay_ms(10);
  straight_both;
  left();
  delay_ms(5);                            //��ʱһ��ʱ���ô�����1 2�뿪����
  while((P2IN&0X06)==0x00);               //��������1 2һֱ������ѭ��
  right_reversal;
  PWM1(78);
  PWM2(65);
  delay_ms(8);
  straight_both;
}

void right_middle(void)                  //��ת�м�
{
  right_reversal;
  PWM1(60);
  PWM2(65);              //��ʱһ��ʱ���ô�����1 2�뿪����
  while((P2IN&0X06)==0x04);                //��������1 2һֱ������ѭ��
  straight_both;
  PWM1(75);
  PWM2(50);      
  
}

void left_middle(void)                  //��ת�м�
{
  left_reversal;
  PWM1(65);
  PWM2(60);                  //��ʱһ��ʱ���ô�����1 2�뿪����
  while((P2IN&0X06)==0x02);               //��������1 2һֱ������ѭ��
  straight_both;
  PWM1(50);
  PWM2(75);
}

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  system();                            //��ʼI\O�˿�

  diff[0]=1;diff[1]=1;diff[2]=1;
  while(1)
  { 
    char j;
         j=P2IN&0X06;
         switch(j)                  //ֻ���΢������1 2
         { 
         case 0x04:slight_right(); break;          //2�� ��΢��
         case 0x02:slight_left();  break;          //1�� ��΢��
         case 0x06:slow();         break;          //��������
         case 0x00:go_straight();  break;          //����ֱ��
         }
  }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
  char j;
  delay_ms(1);                            //������ʱ��С����ǰ��һ�ξ��������ж�״̬
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
  if((P2IN&0X18)==0X08)                             //��3��⵽����
  {
    if((P2IN&0X06)==0X00)                          //תֱ���䣨�����������
    {
      left_higher();
    }
    else                                          //����·�ڣ��ڶ��������
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
                switch(j)                  //ֻ���΢������1 2
               { 
                case 0x04:PWM1(75);PWM2(30);  break;          //2�� ��΢��
                case 0x02:PWM1(30);PWM2(75);  break;          //1�� ��΢��
                case 0x00:slow();             break;          //����ֹͣ
                case 0x06:PWM1(75);PWM2(75);  break;          //����ֱ��
               }
             }
             break;
      }
        diff[1]++;
    } 

  }
  if((P2IN&0X18)==0X10)                             //��4��⵽����      
  {
    if((P2IN&0X02)==0X00)                          //תֱ���䣨�����������
    {
      right_higher();
    }
    else                                         //����·�ڣ��ڶ��������
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
#include "pid.h"

struct _pid
 {
 float Setdate;        //�����趨ֵ
 float Actualdate;     //����ʵ��ֵ
 float err;             //����ƫ��ֵ
 float err_last;
 float Kp,Ki,Kd;         
 float expect;         //��������ֵ
 float integral;        //�������ֵ
 }pid;
 

 
 void PID_init(float want,float Kp,float Ki,float Kd)
 {
  pid.Setdate = want; 
  pid.Actualdate = 0;
  pid.err = 0;
  pid.err_last = 0;
  pid.expect = 0; 
  pid.integral = 0; 
  pid.Kp = Kp;
  pid.Ki = Ki;
  pid.Kd = Kd;
 }
 
 
 
 
 float PID_realize(float date)
 {
    pid.Actualdate=date;
    pid.err=pid.Setdate-pid.Actualdate;
    pid.integral=pid.integral+pid.err;
    pid.expect=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
    pid.err_last=pid.err;
    return pid.expect;
 }
 
 
 
 
 
 
 
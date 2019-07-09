#include "stdio.h"
typedef struct _pid
{
    float set;
    float actual;
    float err;
    float last_err;
    float pre_err;
    float Kp,Ki,Kd;
    float increase;
    float expect;
}PID;
PID pid;
void PID_init(float want,float P,float I,float D)
{
    pid.set = want;
    pid.actual = 0;
    pid.err = 0;
    pid.last_err = 0;
    pid.pre_err = 0;
    pid.Kp = P;
    pid.Ki = I;
    pid.Kd = D;
    pid.increase = 0;
}
float PID_realize(float input)
{
    pid.actual = input;
    pid.err = pid.set-pid.actual;
    pid.increase = pid.Kp*(pid.err-pid.last_err) + pid.Ki*pid.err + pid.Kd*(pid.err-2*pid.last_err+pid.pre_err);
    pid.expect += pid.increase;
    pid.last_err = pid.err;
    pid.pre_err = pid.last_err;
    return pid.expect;
}
int main()
{
    PID_init(200,0.8,0.015,0);
    int count=0;
    float data =0;

    while(count<1000)
    {
       data = PID_realize(data);
       printf("%f\n",data);
       int i;
       for(i=0;i<30000000;i++);
       count++;
    }
}
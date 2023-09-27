// #include <Servo.h>

#include<Servo.h>  //调用舵机库函数
#include<Math.h>
Servo myservo;  //声明两个舵机
int myservopin = 3; // 定义舵机的引脚
#define Gyroscope_left_LimitAngle_X   -1.05  //读取到陀螺仪 X 轴向左偏的极限数值
#define Gyroscope_Right_LimitAngle_X   1.01  //读取到陀螺仪 X 轴向右偏的极限数值
#define Gyroscope_Middle_LimitAngle_X   0    //读取到陀螺仪 X 轴平放时的数值
#define Servo_One_Mix_Angle 30       //1号舵机最小角度
#define Servo_One_Max_Angle 150     //1号舵机最大角度
#define Servo_Speed 4              //舵机速度

unsigned char Re_buf[11],counter=0;
unsigned char sign=0;
float a[3],w[3],angle[3],T;
int servo_angle_current = 0;
float value_init = 90; 
float f = 8;                     //舵机的频率 


//放到主函数中
void setup()
{
   Serial.begin(115200);           //打开串口，并设置波特率为115200
   myservo.attach(myservopin);
} 

void loop()
{
   Get_gyroscope_And_Control();   //根据陀螺仪传感器的数据实现姿态跟随
}
//以上放入主函数


void Get_gyroscope_And_Control()
{
   int gyroscope_acc_data=0;
   int map_data=0;
   if(sign)
  {  
     sign=0;
     if(Re_buf[0]==0x55)      //检查帧头
     {  
	switch(Re_buf [1])
	{
	case 0x51:
               {
		a[0] = (short(Re_buf [3]<<8| Re_buf [2]))/32768.0*16;
		a[1] = (short(Re_buf [5]<<8| Re_buf [4]))/32768.0*16;
		a[2] = (short(Re_buf [7]<<8| Re_buf [6]))/32768.0*16;
		T = (short(Re_buf [9]<<8| Re_buf [8]))/340.0+36.25;

                //把陀螺仪的沿X轴的加速度值转为舵机的角度
                map_data = a[0] * 100;
                if(a[0]>=Gyroscope_Middle_LimitAngle_X && a[0]<=Gyroscope_Right_LimitAngle_X){                                      
                   gyroscope_acc_data = (int)map(map_data, 0, (int)Gyroscope_Right_LimitAngle_X*100, (Servo_One_Mix_Angle+Servo_One_Max_Angle)/2, Servo_One_Mix_Angle);
//                   Serial.print("X_left:"); Serial.print(gyroscope_acc_data[0]); Serial.print("  |  ");
                }
                if(a[0]<Gyroscope_Middle_LimitAngle_X && a[0]>=Gyroscope_left_LimitAngle_X){                   
                   gyroscope_acc_data = (int)map(map_data, 0, (int)Gyroscope_left_LimitAngle_X*100, (Servo_One_Mix_Angle+Servo_One_Max_Angle)/2, Servo_One_Max_Angle);
//                   Serial.print("X_right: "); Serial.print(gyroscope_acc_data[0]); Serial.print("  |  ");
                }
//                Serial.println();          
                servo_move(gyroscope_acc_data);                
               }break;
	}
    }
  } 
  // delay(100); //后面如果没有其他的程序最好加一个小小的延时  
}

void serialEvent() {
  while (Serial.available()) {    
    //char inChar = (char)Serial.read(); Serial.print(inChar); //Output Original Data, use this code   
    Re_buf[counter]=(unsigned char)Serial.read();
    if(counter==0&&Re_buf[0]!=0x55) return;      //第0号数据不是帧头              
    counter++;       
    if(counter==11)             //接收到11个数据
    {    
       counter=0;               //重新赋值，准备下一帧数据的接收 
       sign=1;
    }      
  }
}

void servo_move(float value) 
{
  float value_arguments = value;
  float value_delta;
  int value_date_int;

  value_delta = (value_arguments - value_init) / f;
  
  for(int i=0;i<f;i++)
  {
    
      value_init = value_delta == 0 ? value_arguments : value_init + value_delta;
      value_date_int = value_init;
      myservo.write(map(value_date_int,30,150,900,2300));//舵机映射的脉冲角度
      delay(Servo_Speed);
    
  }
}
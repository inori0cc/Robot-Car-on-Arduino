#include <Arduino.h>

//颜色部分头文件
#include <Wire.h>        //调用IIC库函数
#include "MH_TCS34725.h" //调用颜色识别传感器库函数
#ifdef __AVR__
  #include <avr/power.h>
#endif

//颜色传感器不同通道值设置
MH_TCS34725 tcs = MH_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); //设置颜色传感器采样周期50毫秒
//MH_TCS34725 tcs = MH_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);//设置颜色传感器采样周期700毫秒
//MH_TCS34725 tcs = MH_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);//设置颜色传感器采样周期50毫秒（不推荐）
//MH_TCS34725 tcs = MH_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);//设置颜色传感器采样周期50毫秒（不推荐）

//电机
#define left_wheel   5    //左轮电机引脚
#define right_wheel  9    //右轮电机引脚

//舵机

//灰度
#define left_hudu       A3//左侧灰度传感器引脚
#define right_hudu      A2//右侧灰度传感器引脚
int left_val  = 0;      //左灰度值
int right_val = 0;      //右灰度值
int turn_panduan  = 0;  //转向判断值 = 左灰度值-右灰度值


//颜色传感器


//通道标志
bool flag_tongdao=0;
int chuxu1=0,chuxu2=0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);//开启串口，并设置串口波特率位115200
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);//触碰传感器初始化
  
}

void loop() 
{
  // put your main code here, to run repeatedly:

  Serial.print(color());
  color();
      

}


int color()
{
    uint16_t clear, red, green, blue; //分别定义用于存储红、绿、蓝三色值变量
    
    //分别用于红、绿、蓝三色计数
    uint16_t r_num=0;
    uint16_t g_num=0;
    uint16_t b_num=0;

    //色卡颜色
    int yanse=0;

    for(int i=0;i<10;i++)
    {
        // 计算红绿、蓝三色数值，并计数
        tcs.getRGBC(&red, &green, &blue, &clear); //将原始R/G/B值转换为色温（以度为单位）
        uint32_t sum = clear;         
        float r, g, b;                 
        r = red;    
        r /= sum;              
        g = green;  
        g /= sum;            
        b = blue; 
        b /= sum;             
        r *= 256; g *= 256; b *= 256;  

        Serial.print("\t");////////////////////////////////////////////
        Serial.print((int)r); Serial.print("\t"); // 在串口中分别打印
        Serial.print((int)g); Serial.print("\t"); // 红、绿、蓝三色
        Serial.print((int)b);                     // 值
        Serial.println();//////////////////////////////////////////////

         if(r>128||g>128||b>128)
          {
              if((r>g)&&(r>b))
              {
                r_num++;
              }
              else if(g>=r&&g>b)
              {
                g_num++;
              }
              else 
              {
                b_num++;
              }
          } 
    }   


 
//根据计数结果，判断颜色

  if((r_num>=g_num)&&(r_num>b_num))
    {
      yanse=1;   //红色卡   
    }
    else if((g_num>r_num)&&(g_num>b_num))
    {
      yanse=2;  //绿色卡
    }
     else if((b_num>r_num)&&(b_num>g_num))
    {
      yanse=3;  //蓝色卡
    }  

  return yanse; 
}

   

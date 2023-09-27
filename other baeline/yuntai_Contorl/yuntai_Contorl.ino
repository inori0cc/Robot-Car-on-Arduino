#include <KalmanFilter.h>    //卡尔曼滤波
#include <MsTimer2.h>        //定时中断
#include "I2Cdev.h"        
#include "MPU6050_6Axis_MotionApps20.h"//MPU6050库文件
#include "Wire.h" 
#include <Servo.h> 

Servo sg90;
int servo_pin = 3;

MPU6050 Mpu6050; //实例化一个 MPU6050 对象，对象名称为 Mpu6050
KalmanFilter KalFilter;//实例化一个卡尔曼滤波器对象，对象名称为 KalFilter
int16_t ax, ay, az, gx, gy, gz;  //MPU6050的三轴加速度和三轴陀螺仪数据
float Angle, Gryo_x, Gryo_y, Gryo_z;  //用于显示角度和角速度的临时变量

//***************下面是卡尔曼滤波相关变量***************//
float K1 = 0.05; // 对加速度计取值的权重
float Q_angle = 0.001, Q_gryo = 0.005;
float R_angle = 0.5 , C_0 = 1;
float dt = 0.005; //注意：dt的取值为滤波器采样时间 5ms

/**************************************************************************
函数功能：5ms中断服务函数 
入口参数：无
返回  值：无
**************************************************************************/
void control()
{
  sei();//全局中断开启
  Mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  //获取MPU6050陀螺仪和加速度计的数据
  KalFilter.Angletest(ax, ay, az, gx, gy, gz, dt, Q_angle, Q_gryo, R_angle, C_0, K1);//通过卡尔曼滤波获取角度
  Angle = KalFilter.angle;//卡尔曼滤波后的X轴角度
  Gryo_x= KalFilter.Gyro_x;//卡尔曼滤波后的X轴角速度
  Gryo_y= KalFilter.Gyro_y;
  Gryo_z= KalFilter.Gyro_z;
}

/**************************************************************************
函数功能：初始化 相当于STM32里面的Main函数 
入口参数：无
返回  值：无
**************************************************************************/
void setup() {
  sg90.attach (servo_pin);
  Wire.begin();             //加入 IIC 总线
  Serial.begin(9600);       //开启串口，设置波特率为 9600
  delay(1500);              //延时等待初始化完成
  Mpu6050.initialize();     //初始化MPU6050
  Serial.println("检测MPU6050是否连接...");
  Serial.println(Mpu6050.testConnection() ? "MPU6050已成功连接" : "没有连接到MPU6050");
  Serial.println();
  delay(20); 
  MsTimer2::set(4, control);  //使用Timer2设置5ms定时中断
  MsTimer2::start();          //启动中断使能
}

/**************************************************************************
函数功能：主循环程序体
入口参数：无
返回  值：无
**************************************************************************/
void loop() {
  Serial.println(Angle);//卡尔曼滤波后的X轴倾角角度
  Angle = map(Angle,-65,65,15,165);
  sg90.write (Angle);
  // delay(20);
}

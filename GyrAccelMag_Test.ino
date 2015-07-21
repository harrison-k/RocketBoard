//GyrAccelMag_Test
//  Harrison King
//  7/14/2015

//I2C code based on:
//  Arduino user JohnChi for MPU6050: http://playground.arduino.cc/Main/MPU-6050#short
//  Nordevx.com: https://www.nordevx.com/content/lsm9ds1-9-dof-accelerometer-magnetometer-and-gyro

//Documentation for chip:
//  LSM9DS1 Datasheet: http://www.st.com/web/en/resource/technical/document/datasheet/DM00103319.pdf
//  Breakout board datasheet: http://www.st.com/web/en/resource/technical/document/data_brief/DM00164423.pdf

#include <Wire.h>


typedef union Sensor
{
  float num[3];
  byte byt[12];
}sensor;

//arrays for measurement storage
sensor gyr;
sensor mag;
sensor acc;

//conversion factors for sensors
float accfact = 0.000061;
float gyrfact = 0.00875;
float magfact = 0.00014;

//variables for regulating time increments
byte dt = 25;
unsigned long timer = 0;
unsigned long reporter = 0;

//device address where SDO is tied to Vdd
byte accAdd = 0B11010110;
byte gyrAdd = 0B11010110; //Not necessary, as acc and gyr share an address. Makes code more readable.
byte magAdd = 0B00111100;

//first register of measurements for each device (start register)
byte accSt = 0B00101000;
byte gyrSt = 0B00011000;
byte magSt = 0B00101000;

//other necessary registers
byte CTRL_REG4    = 0B00011110;
byte CTRL_REG5_XL = 0B00011111;
byte CTRL_REG1_G  = 0B00010000;
byte CTRL_REG3_M  = 0B00100010;

//not sure why these exist, but the code I "borrowed" uses them, so they're included.
byte Read    = 0B00000001;
byte Write   = 0B00000000;


void setup(){
  Wire.begin();
  
  WriteReg(CTRL_REG4,0B00111000,accAdd);    //enable gyro
  WriteReg(CTRL_REG5_XL,0B00111000,accAdd); //enable accelerometer
  WriteReg(CTRL_REG1_G,0B01000000,accAdd);  //gyro/accel odr and bw
  WriteReg(CTRL_REG3_M,0B00000000,magAdd);  //enable mag continuous
  
  Serial.begin(9600);
  delay(100);
  
  //Call and response with Matlab
  Serial.println('a'); //call
  char a = 'b';
  while (a!='a')   //listen for response
  {
    a=Serial.read();
  }
}

void loop(){
  if (millis()-timer>=dt){
    timer=millis();
    
    //get current measurements
    ReadData(acc.num, accAdd, accSt);
    ReadData(gyr.num, gyrAdd, gyrSt);
    ReadData(mag.num, magAdd, magSt);
    
    //convert measurements to useful units
    for (byte i=0;i<3;i++){
      acc.num[i] *= accfact;  //multiply by conversion factor for Gs
      gyr.num[i] *= gyrfact;  //multiply by conversion factor for deg/s
      mag.num[i] *= magfact;  //multiply by conversion factor for Gauss
    }
  }
  if (millis()-reporter>=50){    //Sent data to matlab at regular intervals
    reporter = millis();
    Serial.write(acc.byt,12);
    Serial.write(gyr.byt,12);
    Serial.write(mag.byt,12);
  }
}

//This function takes a value and writes it to a given register at a given address
void WriteReg(byte reg, byte value, byte device){
  Wire.beginTransmission((device | Write) >>1 );
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

//This function updates a data array with new data
void ReadData(float data[], byte devAdd, byte startReg){
  Wire.beginTransmission((devAdd | Write)>>1);
  Wire.write(startReg);
  Wire.endTransmission(false);
  Wire.requestFrom((devAdd | Read)>>1,6,true);
  while (Wire.available() == 0);
  for (byte i=0;i<3;i++){
    data[i] = (Wire.read())|(Wire.read()<<8);
  }
  Wire.endTransmission();
}




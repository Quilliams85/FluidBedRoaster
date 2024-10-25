#include "ModbusRtu.h"
#include "max6675.h"
#include <LiquidCrystal.h>



#define RPWM 5
#define LPWM 6
#define SSR 8
#define potFan A0
#define potHeat A1
#define switchPin 9




LiquidCrystal lcd(50, 2, 42, 44, 46, 48);


int motorPercentage;
int motorPinVal;
int lastMotorPinVal;
float heatLastTime;
float lastHeatingPercentage;
float lastTime;
bool heaterOn;
bool usingArtisan;
int maxTemp = 450;

//Modbus setup
// data array for modbus network sharing
// third place [2] is used for BTtemperature, [5] for fan speed, [6] for heat
uint16_t au16data[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1 };
Modbus slave;

//Thermocouple setup 
int thermoBtDO = 11;
int thermoBtCS = 12;
int thermoBtCLK = 13;
MAX6675 thermocoupleBT(thermoBtCLK, thermoBtCS, thermoBtDO);

unsigned long tempTime=0;
unsigned long lastUpdate=0;
unsigned long longTemp[8];
unsigned long avgTemp;




void setup() {
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(SSR, OUTPUT);
  //Modbus Start
  slave = Modbus(1,0,0);                                    // this is slave @1 and RS-232 or USB-FTDI
  slave.begin( 19200 );                                     // 19200 baud, 8-bits, non, 1-bit stop
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 
  delay(500);

  
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
  digitalWrite(SSR, LOW); // switch the relay off initially

  lastTime = millis();
}


void loop() {
  int reading = digitalRead(switchPin);
  usingArtisan = (reading == LOW) ? true : false; //switch between modes

  tempReading(); //update temperature

  if (usingArtisan){        //use artisan modbus values
    digitalWrite(10, HIGH); //indicator light
    slave.poll( au16data, 16 );                              // poll MODBUS
    updateHeater(au16data[4]);
    setFan(au16data[5]);
  }
  else{         //use dials not artisan
    digitalWrite(10, LOW); //indicator light
    slave.poll( au16data, 16 );                              // poll MODBUS
    int potValueF = analogRead(potFan);
    int potValueH = analogRead(potHeat);
    updateHeater(map(potValueH, 0, 1024, 0, 99));
    setFan(map(potValueF, 0, 1024, 0, 99));
    lcd.setCursor(9,1);
    lcd.print("HEAT:");
    lcd.setCursor(14,1);
    lcd.print((map(potValueH, 0, 1024, 0, 99)));
  }
  delay(50);
  updateLCD(float(au16data[2])/100);
}


void updateHeater(int heatingPercentage){
  if (usingArtisan && heatingPercentage>10 && au16data[5]<5){
    heatingPercentage = 0; //prevent heating element from running without fan on
  }
  if(au16data[2] > maxTemp*100){
    heatingPercentage = 0; //prevent heat going higher than set max temp
  }
  float heatingInterval = map(heatingPercentage, 0, 100, 0, 1000);
  float heatCurrentTime = millis();
  if((heatCurrentTime - heatLastTime)>=1000){
    //begin cycle
    heaterOn = true;
    digitalWrite(SSR, HIGH);
    heatLastTime = heatCurrentTime;
  }

  if ((heatCurrentTime - heatLastTime >= heatingInterval)) {
    if(heaterOn){
      digitalWrite(SSR, LOW);
      heaterOn = false;
    }
    //switch off for percentage of 1s    
  }
}

void setFan(int fanPercentage){
  motorPinVal = map(fanPercentage, 0, 100, 0, 256);
  updateLCD(fanPercentage);
  if (fanPercentage != lastMotorPinVal){
    lastMotorPinVal = fanPercentage;
    analogWrite(LPWM, motorPinVal);
  }

}


void tempReading(){
   if( millis()>tempTime+250 ){                             // do this every 250 ms Close to the fastest a max6675 can sample
     tempTime=millis();
     longTemp[0] = thermocoupleBT.readFahrenheit()*100;        //reading temp sensor
     if(longTemp[1]==0){                                    // After first run of temp read, the average temp array is populated to ensure imediate optimized average calculation 
        for(byte i=1;i<8;i++) {longTemp[i]=longTemp[0];}}
     avgTemp = ( longTemp[0] + longTemp[1] + longTemp[2] + longTemp[3] + longTemp[4] + longTemp[5] + longTemp[6] + longTemp[7] ) / 8;              // average of eight temp readings
     longTemp[7] = longTemp[6];
     longTemp[6] = longTemp[5];
     longTemp[5] = longTemp[4];
     longTemp[4] = longTemp[3];
     longTemp[3] = longTemp[2];
     longTemp[2] = longTemp[1];
     longTemp[1] = longTemp[0];
     au16data[2] = avgTemp;                                 // Save average temp to MODBUS Array
   }
}

void updateLCD(float val){
    if( millis()>tempTime+250 ){
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.print("BT:");
      lcd.setCursor(3,0);
      lcd.print(val);
    }
    if(val != float(au16data[2])/100){
      lcd.setCursor(0,1);
      lcd.print("FAN:");
      lcd.setCursor(4,1);
      lcd.print(val);
    }

}

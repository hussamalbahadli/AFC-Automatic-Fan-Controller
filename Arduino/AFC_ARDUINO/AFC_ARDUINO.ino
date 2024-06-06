/*
  File : main.cpp
  Author : Hussam
  Created on March 29, 2024, 5:14 AM
*/

// Library
#include <Arduino.h> // Arduino Platform
// #include <Wire.h>   // for I2C Communecation
#include <PWM.h> // Modify PWM frequency on AVR (arduino) platform

// ----[ Globals ]----
int32_t Frequency = 100, Temperature = 0, StoreTemp = 0, StoreData = 0, S_D = 0, TempMin = 45, TempMax = 100, FanSpeed = 0, FanTest = 0, R_D = 0;
float Voltage = 0, RT = 0, VR = 0, LN = 0, T0 = 0, VRT = 0, Max = 65535.0;
bool Success = 0;
// ---- [ Macros ]----
#define PWM 9
#define D   7
#define Indic 13
#define RT0 5684 // 25°C Degree In 5684 Ω.
#define B 3965   // Beta sabiti
#define VCC 5    // Supply voltage
#define R 5500   // R=5.4 KΩ

int GetTemp();
int GetData();
void Test();

void setup()
{

  Serial.begin(9600);
  InitTimersSafe();

  // sets the frequency for the specified pin
  Success = SetPinFrequencySafe(PWM, Frequency);

  // if the pin frequency was set successfully, turn pin 13 on
  pinMode(D, INPUT);
  if (Success)
  {
    pinMode(Indic, OUTPUT);
    digitalWrite(Indic, HIGH);
    delay(5000);
    digitalWrite(Indic, LOW);
  }

//  Test();
}

void loop()
{
  R_D = digitalRead(D);
//  Serial.println(R_D);
  //Test();
  //pwmWrite(PWM, 200);

  StoreData = GetData();
      StoreTemp = GetTemp();
  //
  if ((StoreData >= 20) && (StoreData <= 700)) {
    StoreData = GetData();
    StoreTemp = GetTemp();
    //    StoreData = GetData();

    if ((StoreTemp >= TempMin) && (StoreTemp <= TempMax))
    {
      FanSpeed = map(StoreTemp, TempMin, TempMax, 12000, 61535);
      pwmWriteHR(PWM, FanSpeed);
      Serial.print("Temperature:");
      Serial.print(" ");
      Serial.println(StoreTemp);
      Serial.print("==================");
      Serial.print("PWM:");
      Serial.print(" ");
      Serial.println(FanSpeed);
      digitalWrite(Indic, LOW);
    }

    else if (StoreTemp > TempMax)
    {
      pwmWriteHR(PWM, 61535);
      digitalWrite(Indic, LOW);
//      Serial.println(StoreTemp);
    }
    else if (StoreTemp < TempMax)
    {
      pwmWriteHR(PWM, 0);
      digitalWrite(Indic, LOW);
//      Serial.println(StoreTemp);
    }

  }

  while (StoreData >= 800) {
    StoreData = GetData();
    pwmWriteHR(PWM, 61535);
    digitalWrite(Indic, HIGH);

  }
  if (R_D == 1 && StoreTemp <= 50  ){
    R_D = digitalRead(D);
    StoreTemp = GetTemp();

    pwmWriteHR(PWM, 30767);
//    Serial.println(R_D);
    
  }
  /// 61535

  //pwmWriteHR(PWM, 12000);
  //pwmWrite(PWM, 242);
  //    if ((StoreTemp >= TempMin) && (StoreTemp <= TempMax))
  //    {
  //      FanSpeed = map(StoreTemp, TempMin, TempMax, 12000, 61535);
  //      pwmWriteHR(PWM, FanSpeed);
  //      Serial.print("Temperature:");
  //      Serial.print(" ");
  //      Serial.println(StoreTemp);
  //      Serial.print("==================");
  //      Serial.print("PWM:");
  //      Serial.print(" ");
  //      Serial.println(FanSpeed);
  //      delay(10);
  //    }
  //    else if (StoreTemp > TempMax)
  //   {
  //      pwmWriteHR(PWM, 61535);
  //   }
  //      else if (StoreTemp < TempMin)
  //   {
  //      pwmWriteHR(PWM, 0);
}

//    Serial.print("Temperature:");
//    Serial.print(" ");
//    Serial.println(StoreTemp);
//    Serial.print("==================");
//    Serial.print("Data:");
//    Serial.print(" ");
//    Serial.println(StoreData);
//    delay(100);


//}

int GetTemp()
{
  VRT = analogRead(A0);
  T0 = 25 + 273.15;             // Temperature T0 from datasheet, conversion from Celsius to kelvin
  VRT = (5.00 / 1023.00) * VRT; // Conversion to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R); // Resistance of RT
  LN = log(RT / RT0);
  Temperature = (1 / ((LN / B) + (1 / T0))); // Temperature from thermistor
  Temperature = Temperature - 273.15;        // Conversion to Celsius
  return Temperature;
  delay(20);
}

int GetData()
{
  S_D = analogRead(A0);
  return S_D;
}

void Test()
{
  for (FanTest = 12000; FanTest <= 61535; FanTest += 100)
  {

    pwmWriteHR(PWM, FanTest);
    delay(15);

  }

  for (FanTest = 61535; FanTest >= 12000; FanTest -= 100)
  {
    pwmWriteHR(PWM, FanTest);
    delay(15);
  }
}

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

BlynkTimer timer;

char auth[] = ""; //from blynk app
char ssid[] = ""; //your wifi ssid
char pass[] = ""; //your wifi password

const int sensorIn = A0;
unsigned int mVperAmp = 66;   
float ACSVoltage = 0;
float VRMS = 0;
float AmpsRMS = 0;
float inputV = 127.0;
float mcuVoltage = 5.0;        
float WH = 0;
unsigned int calibration = 62;  
unsigned int pF = 95;
float energyCostpermonth = 0;   
float energyTariff = 1.62;

void getACS712() {
  ACSVoltage = getVPP();
  VRMS = (ACSVoltage / 2.0) * 0.707;
  AmpsRMS = (VRMS * 1000) / mVperAmp;
  if ((AmpsRMS > -0.015) && (AmpsRMS < 0.008)) {
    AmpsRMS = 0.0;
  }
  Serial.print(String(AmpsRMS, 3));
  Serial.print(" Amps RMS");
  Serial.print("\tPower: ");
  WH = (inputV * AmpsRMS) * (pF / 100.0);
  if (WH > 50)
  {
    Blynk.notify("Over power absorbed");
  }
  Serial.print(String(WH, 3));
  Serial.print(" WH");
  Blynk.virtualWrite(V5, String(AmpsRMS, 3));
  Blynk.virtualWrite(V6, String(WH, 3));
  Blynk.virtualWrite(V7, int(WH + 0.5));
  energyCostpermonth = (24.0 * 30.0 * (WH / 1000.0) * energyTariff);   
  Serial.print("  Approx cost per month: Mex$ ");
  Serial.println(String(energyCostpermonth, 2));
  Blynk.virtualWrite(V8, String(energyCostpermonth, 2));
}

float getVPP()
{
  float result;
  int readValue;
  int maxValue = 0;
  int minValue = 1024;
  uint32_t start_time = millis();
  while ((millis() - start_time) < 950) 
  {
    readValue = analogRead(sensorIn);
    if (readValue > maxValue)
    {
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      minValue = readValue;
    }
  }
  result = ((maxValue - minValue) * mcuVoltage) / 1024.0;
  return result;
}

// V5 Pantalla de CA IRMS
// V6 Pantalla WH
// V7 Medidor WH
// V8 Medidor de costos


void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, getACS712); 
}

void loop() {
  Blynk.run();
  timer.run();
}

#define BLYNK_PRINT Serial // Blynk Print Serial. This "define" should place on top of sketch
#define BLYNK_DEBUG_ALL Serial
#define BLYNK_MAX_SENDBYTES 1024 // set Limit Blynk Symbol Number(include Subject + body)
#define BLYNK_MAX_READBYTES  4096
#define BLYNK_MSG_LIMIT 200
#include <DHT.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>



//EEPROM Settings
#define EEPROM_RESET_ADDR 0x58
struct config_reset_cnt_struct
{
  unsigned int reset_cnt;
} eeprom_reset_struct;


//SENSORS
#define DHT_IN 7 // Air Temperature
#define LDR_IN A0 // Light Denity 
#define DS18_IN 6 // Waterㄴ Temperature
#define CO2_IN A1 // Co2
#define EC_IN A2 // Elcectricity Conductivity
#define PH_IN A3 // PH 

//Actuators
#define PH_MINUS_PUMP 0
#define PH_PLUS_PUMP 1
#define WATER_PUMP 2
#define AIR_FAN 3
#define LED 4
#define AIR_PUMP 5
#define VENTIL_FAN 6
#define SOL_A_PUMP 7
#define SOL_B_PUMP 8
#define HUMIDIFIER_1 9
#define HUMIDIFERR_2 10

int ch16_relay[16] = {26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41};
// 1,3,5,7,9,11,13,15 | 26,28,30,32,34,36,38,40
// 2,4,6,8,10,12,14,16 | 27,29,31,33,35,37,39,41


//LCD KEYPAD
#define btnRIGHT
#define btnUP
#define btnDOWN
#define btnLEFT
#define btnSELECT
#define btnNONE 

LiquidCrystal lcd(12,13,8,9,10,11); 



#define ESP_SERIAL Serial2
#define ESP_BAUD 115200

//You should get Auth Token in the Blynk App.
//Go to the Project Settings (nut icon).
char auth[] = "b1b31fe06fe445d18bbc01284dcaedbd"; // PFC_0002
//Your WiFi credentials.
//Set password to "" for open networks.
char ssid[] = "FabLab_2.4G";
char pass[] = "innovationpark";

ESP8266 wifi(&ESP_SERIAL);

BlynkTimer bl_timer;
int count_blynk_fail = 0;
float discon_msec = 0.0;
int last_connect_start = 0;

//Software Reset 
unsigned long RESET_TIMEOUT; // Every 300 min.
unsigned long arduino_smsec;
WidgetLCD blynk_lcd(V21);

//SD card
File micro_sd_file;
int sd_card_pin = 53;
boolean is_sd_card_init = false;








BLYNK_CONNECTED() {
  Serial.println("[Function Call]Called BLYNK_CONNECTED");


//  IPAddress server(210,92,91,225);
//  WiFiClient client;
//   //if (client.connect(server,5000)){
//   if (client.connect("horticulture.ezfarm.co.kr",5000)){
//    Serial.println("210.92.91.225 connected");
//    client.println("GET /v1/all_data HTTP/1.0");
//    client.println();
//   }
//   else
//   {
//    Serial.println("210.92.91.225 not connected");
//   }
   
  // Request Blynk server to re-send latest values for all pins
  //Blynk.syncAll();

  // Manual Sync without "Blnk.syncAll()" => It invoke "buffer overflow!"
  int virtual_relay[] = {V26,V27,V28,V29,V30,V31,V32,V33,V34,V35,V36,V37,V38,V39,V40,V41};
  for(int i=0; i< sizeof(virtual_relay); i++)
  {
    Blynk.syncVirtual(virtual_relay[i]);
    delay(10);
  }

  // You can also update individual virtual pins like this:
  //Blynk.syncVirtual(V0, V2);

  // Let's write your hardware uptime to Virtual Pin 2
  //  int value = millis() / 1000;
  //  Blynk.virtualWrite(V2, value);

  count_blynk_fail = (unsigned long)0;

//  last_connect_start = millis(); 
//  bl_timer.setTimeout(1000L,LCD_display_connected);
}

BLYNK_APP_CONNECTED()
{
  Serial.println("[BLYNK] BLYNK APP CONNECTED");
}
BLYNK_APP_DISCONNECTED()
{
  Serial.println("[BLYNK] BLYNK APP DISCONNECTED");
}


void setup() {
  
//  wdt_enable(WDTO_15MS);

  
//  lcd.begin(16,2);
//  lcd.setCursor(0,0);
//  lcd.print("[PFC]");
//  lcd.setCursor(0,1);
//  lcd.print("SetUp Now.....");

  arduino_smsec = millis();

  Serial.begin(9600);
  delay(10);
  ESP_SERIAL.begin(ESP_BAUD);
  delay(10);
  RESET_TIMEOUT = (unsigned long)300 *  (unsigned long)60 * (unsigned long)1000; 
  
  Serial.println(">>>>>>>>>>>>");
  Serial.println("[Arduino Mega] Start");
  Serial.println(">>>>>>>>>>>>");


  
  EEPROM_readAnything(EEPROM_RESET_ADDR,eeprom_reset_struct);
  if (eeprom_reset_struct.reset_cnt > 60,000)
  {
    eeprom_reset_struct.reset_cnt = 0;
  }
  eeprom_reset_struct.reset_cnt += 1;
  EEPROM_writeAnything(EEPROM_RESET_ADDR, eeprom_reset_struct);
  Serial.print("[EEPROM RESET CNT] on EEPROM MEMORY : ");
  Serial.println(eeprom_reset_struct.reset_cnt);





 
  
//
//  if (!SD.begin(sd_card_pin))
//  {
//    Serial.println("[SD Card]Initialization Failed"); 
//    is_sd_card_init = true;
//  }
//  else
//  {
//    micro_sd_file = SD.open("pfc_log.txt",FILE_WRITE);
//  }


//  for(int i=0; i<16; i++)
//  {
//    pinMode(ch16_relay[i],OUTPUT);
//  }

//  int ri = 0;
//  while(true)
//  {
//    digitalWrite(ch16_relay[ri],HIGH);
//    if(ri==0)
//    {
//      digitalWrite(ch16_relay[15],LOW);
//    }
//    else
//    {
//      digitalWrite(ch16_relay[ri-1],LOW);
//    }
//    if(ri++ == 16)
//    {
//      ri=0;
//    }
//    delay(200);
//  }

//  for(int i=0; i<16;i++)
//  {
//    digitalWrite(ch16_relay[i],HIGH);
//    if(i==0)
//    {
//      digitalWrite(ch16_relay[15],LOW); 
//    }
//    else
//    {
//      digitalWrite(ch16_relay[i-1],LOW);      
//    }
//    delay(50);
//  }


 
  Blynk.begin(auth, wifi, ssid, pass);

  bl_timer.setInterval(5000L,sendMillis);
  //bl_timer.setInterval(30000L, sendDhtSensor);
  //bl_timer.setInterval(30*60*1000L,sendEmailReport);
  
  pinMode(3, OUTPUT);


}

unsigned long last_msec = millis();
void loop() {
  //  Serial.println(BUFFER_SIZE);



//  byte blynk_logs[8];
//  if(blynk_logs = BLYNK_DEBUG_ALL.availableForWrite())
//  {
//    Serial.println(blynk_logs);
//  }




  if(millis() - arduino_smsec > RESET_TIMEOUT)
  {
    Serial.println(RESET_TIMEOUT);
    Serial.println(millis());
    Serial.println(arduino_smsec);
    softwareReset(WDTO_60MS); 
  }

    Blynk.run();
    bl_timer.run();
  
//  if(millis() - last_msec > 10000)
//  {
//    Serial.println(millis() - last_msec / 1000);
//    last_msec =millis();
//  }

//  if(!Blynk.connected())
//  {

//    if(count_blynk_fail == 0)
//    {
//      discon_msec = millis();
//    }
//    float cur_msec = (millis() - discon_msec)/1000;
//    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("WIFI Disconnect");
//    lcd.setCursor(0,1);
//    lcd.print('"'+String(cur_msec) + '"' +" Elapsed");
//    if (count_blynk_fail % 10 == 0)
//    {
//      Serial.print("[Disconnected]Not Connected Blynk :");
//      Serial.println(count_blynk_fail);
//      
//    }
//    delay(50);
//    count_blynk_fail+=1;
//    
//    if(count_blynk_fail > 100)
//    {
//      Blynk.begin(auth, wifi, ssid, pass);
//    }
//  }

  
  
  
  //  while(Serial.available()){
  //    Serial2.write(Serial.read());
  //  }
  //
  //  while(Serial2.available()){
  //    Serial.write(Serial2.read());
  //  }

  // put your main code here, to run repeatedly

  //  String bl_message;
  //
  //  while(Serial.available()){
  //    bl_message+=char(Serial.read());
  //  }
  //  if(!Serial.available()){
  //    if(bl_message!="")
  //    {
  //      Serial.println("reply:"+bl_message);
  //      bl_message="";
  //    }
  //  }

  //
  //
  //  delay(3000);
  //  float *dht_data = getDHT();
  //  unsigned int ldr_val = getLDR();
  //  float ds18_val = getDS18B20();
  //  float co2_con = getCO2();
  //  Serial.print("Humidity : ");
  //  Serial.print(dht_data[0]);
  //  Serial.print(", Temp : " );
  //  Serial.print(dht_data[1]);
  //  Serial.println();
  //  Serial.print("LDR : " );
  //  Serial.print(ldr_val);
  //  Serial.println();
  //  Serial.print("DS18B20 : ");
  //  Serial.print(ds18_val);
  //  Serial.println();
  //  Serial.print("CO2 : " );
  //  Serial.print(co2_con);
  //  Serial.println();
  //  Serial.println("----------------");
}

void LCD_display_connected()
{
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("[PFC] Connected");
   lcd.setCursor(1,1);
   lcd.print("> ");
   lcd.print( (millis() - last_connect_start)/1000);
   lcd.print(" Sec.");
//   lcd.print("On Internet!");    
  
}
void sendDiscon(){
  Blynk.disconnect();
}

void sendEmailReport() {
  float *dht_data = getDHT();
  float ds18_val = getDS18B20();
  float co2_con = getCO2();
  unsigned int ldr_val = getLDR();

  String mail_message = "Dear NerdFarmer! \r\n PFC send to message to you!\r\n\r\n\r\n";
  mail_message +="\tHumidity : "+String(dht_data[0]) +"\r\n\tTemperature:"+ String(dht_data[1]) + "\r\n\tCo2:" +String(co2_con) +"\r\n\tLDR :" + String(ldr_val) +"\r\n\tDS18B20" + String(ds18_val);
  mail_message +="\r\n\r\n From PFC on EZFARM";
  
  Blynk.email("hkh9737@naver.com", "[{DEVICE_NAME}]Alarm _ From Blynk", mail_message);
}

void sendMillis(){

  unsigned long cur_msec = millis();
  Blynk.virtualWrite(V20, cur_msec / 1000);
  Blynk.virtualWrite(V22, eeprom_reset_struct.reset_cnt);

  float ds18_val = 25.32;
  float ph_val = 6.2;
  float ec_val = 3.6;

  // WebHook with Blynk => It will throw to the Data Lake server
//  Blynk.virtualWrite(V50, 
//    "http://210.92.91.225:5000/v1/" 
//    + String(auth) +"/insert/"
//    + "?water_temp=" + String(ds18_val)
//    + "&ph=" + String(ph_val)
//    + "&ec=" + String(ec_val)
//  );
//  Serial.println("http://210.92.91.225:5000/v1/" 
//    + String(auth) +"/insert/"
//    + "?water_temp=" + String(ds18_val)
//    + "&ph=" + String(ph_val)
//    + "&ec=" + String(ec_val));



}

void sendProbeSensor() {
  // PH , EC
  // V6 = PH
  // V7 = EC
  
  float ph_val = getPH();
  float ds18_val = getDS18B20();
  float ec_val = getEC(ds18_val);
  

  Serial.print("DS18B20: " );
  Serial.println(ds18_val);
  Serial.print("PH: ");
  Serial.println(ph_val);
  Serial.print("EC: ");
  Serial.println(ec_val);
  Serial.println("==============");

  Blynk.virtualWrite(V5, ds18_val);
  Blynk.virtualWrite(V6, ph_val);
  Blynk.virtualWrite(V7, ec_val);

  bl_timer.setTimeout(1000, LCD_display_connected);
}

void sendAirSensor() {
  //Co2, LDR
  float co2_con = getCO2();
  unsigned int ldr_val = getLDR();
  Serial.print("Ligth Density Resistor : ");
  Serial.println(ldr_val);
  Serial.print("Co2 : ");
  Serial.println(co2_con);
  Blynk.virtualWrite(V3, co2_con);
  Blynk.virtualWrite(V4, ldr_val);

  bl_timer.setTimeout(1200, sendProbeSensor);
}

void sendDhtSensor() {
  //Humidity,Temperature,DS18B20

  float *dht_data = getDHT();
  Serial.print("Humidity: ");
  Serial.println(dht_data[0]);
  Serial.print("Temperature: ");
  Serial.println(dht_data[1]);


  Blynk.virtualWrite(V1, dht_data[0]);
  Blynk.virtualWrite(V2, dht_data[1]);

  bl_timer.setTimeout(1200, sendAirSensor);

  //  Blynk.virtualWrite(V5,millis());
}

float *getDHT()
{
  DHT dht(DHT_IN, DHT11);

  float *dht_data = malloc(2 * sizeof(float));

  float air_hum = dht.readHumidity();
  float air_temp = dht.readTemperature();

  dht_data[0] = air_hum;
  dht_data[1] = air_temp;

  return dht_data;
}

unsigned int getLDR()
{
  unsigned int ldr_val = analogRead(LDR_IN);
  return ldr_val;
}

float getDS18B20()
{
  OneWire oneWire(DS18_IN);
  DallasTemperature sensors(&oneWire);
  float temp = 0;
  float temp_arr[10] = {0.0};
  float min_v = 0;
  float max_v = 0;
  float amount = 0;
  int max_iter = 10;

  for(int in_i = 0; in_i < 20; in_i++)
  {
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    if(temp >= 60 || temp <= -50)
    {
      continue;
    }
    else
    {
      min_v = temp;
      max_v = temp;
      break;
    }
  }

  for (int in_i = 0; in_i < max_iter; in_i++)
  {
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    
    if(temp >= 60 || temp <= -50)
    {
      in_i--;
//      Serial.println("Abnomaly temp Skipped : " + String(temp));
      continue;
    }
  
//    Serial.println("[" + String(in_i) + "]" + temp);


    if (temp < min_v )
    {
      min_v = temp;
    }
    else if (temp > max_v )
    {
      max_v = temp;
    }
    amount += temp;
    delay(50);
  }
  float avg = (amount - min_v - max_v) / (max_iter - 2);
  return avg;
}

float getCO2()
{
  int co2_vol = analogRead(CO2_IN);
  float voltage = co2_vol * ( 5000 / 1024.0 );
  if (voltage == 0 )
  {
    Serial.println("Fault");
  }
  else if (voltage < 400)
  {
    Serial.println("preheating");
  }
  else
  {
    //    Serial.print("Co2 Voltage : " );
    //    Serial.println(voltage);
    int voltage_diference = voltage - 400;
    float concentration = voltage_diference * 50.0 / 16.0;
    return concentration;
  }
}

float getEC(float temperature)
{
  const byte numReadings = 20;//the number of sample times
  unsigned int AnalogSampleInterval = 25;
  unsigned int printInterval = 700;
  unsigned int tempSampleInterval = 850;
  unsigned int readings[numReadings];
  byte index = 0;
  unsigned long AnalogValueTotal = 0;
  unsigned int AnalogAverage = 0;
  unsigned int averageVoltage = 0;
  unsigned long AnalogSampleTime;
  unsigned long printTime;
  unsigned long tempSampleTime;
  float ECcurrent;

  for (byte thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;
  }
  AnalogSampleTime = millis();
  printTime = millis();
  tempSampleTime = millis();

  for (int in_i = 0; in_i < 50000; in_i++)
  {

    if (millis() - AnalogSampleTime >= AnalogSampleInterval)
    {
      AnalogSampleTime = millis();
      AnalogValueTotal = AnalogValueTotal - readings[index];
      readings[index] = analogRead(EC_IN);
      AnalogValueTotal = AnalogValueTotal + readings[index];
      index = index + 1;
      if (index >= numReadings)
      {
        index = 0;
      }

      AnalogAverage = AnalogValueTotal / numReadings;
    }

    if (millis() - printTime >= printInterval)
    {
      printTime = millis();
      averageVoltage = AnalogAverage * (float)5000 / 1024;

      float TempCoefficient = 1.0 + 0.0185 * (temperature - 25.0);
      float CoefficientVoltage = (float)averageVoltage / TempCoefficient;

      if (CoefficientVoltage < 150)
      {
        ECcurrent = 0;
      }
      else if (CoefficientVoltage <= 1457)
      {
        ECcurrent = 6.98 * CoefficientVoltage - 127;
      }
      else
      {
        ECcurrent = 5.3 * CoefficientVoltage + 2278;
      }

      ECcurrent = ECcurrent / 1000;
      return ECcurrent;
    }
    delay(10);
  }
}

float getPH()
{
  float offset = 0.00;
  int samplingInterval = 20;
  int printInterval = 900;
  int numReadings = 40;
  int pHArray[numReadings];
  int pHArrayIndex = 0;

  unsigned long samplingTime = millis();
  unsigned long printTime = millis();
  float pHValue;
  float voltage;

  while(true)
  {
    if(millis() - samplingTime > samplingInterval)
    {
      pHArray[pHArrayIndex++] = analogRead(PH_IN);
      if(pHArrayIndex == numReadings)
      {
        pHArrayIndex = 0;
      }
      voltage = averageArray(pHArray, numReadings) * 5.0 / 1024;
      pHValue = 3.5 * voltage + offset;
      
    }

    if (millis() - printTime > printInterval)
    {
      return pHValue;
    } 
  }
}


float averageArray(int* arr, int number)
{
  int i;
  int max_v;
  int min_v;
  float avg;
  float amount = 0;

  if(number <= 0 )
  {
    return 0;
  }
  if (number < 5 )
  {
    for(i=0; i < number; i++)
    {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  }
  else
  {
    if(arr[0] < arr[1])
    {
      min_v = arr[0];
      max_v = arr[1];
    }
    else
    {
      min_v = arr[1];
      max_v = arr[0];
    }
    amount = arr[0] + arr[1];

    for(i=2; i<number; i++)
    {
      if(arr[i] < min_v)
      {
        min_v = arr[i]; 
      }
      else if(arr[i] > max_v)
      {
        max_v = arr[i];
      }

      amount += arr[i];
    }
    amount = amount - max_v - min_v;
    avg = (float)amount / (number - 2);
    return avg;
  }


   
}


void softwareReset( uint8_t prescaller) {
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>");
  Serial.println(">>>>[Software Reset] From Arduino Mega 2560");
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>");

//  blynk_lcd.clear();
//  blynk_lcd.print(0,0, "Software Reset");
//  blynk_lcd.print(0,1, millis());

  delay(100);
  // start watchdog with the provided prescaller
  wdt_enable(prescaller);

  // wait for the prescaller time to expire
  // without sending the reset signal by using
//  the wdt_reset() method
//  wdt_reset();
  while(1) {};
}


//V26,V27,V28,V29,V30,V31,V32,V33,V34,V35,V36,V37,V38,V39,V40,V41
BLYNK_WRITE(V26)
{
  int pin_num = 26;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}

BLYNK_WRITE(V27)
{
  int pin_num = 27;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V28)
{
  int pin_num = 28;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V29)
{
  int pin_num = 29;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V30)
{
  int pin_num = 30;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V31)
{
  int pin_num = 31;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V44)
{
  int pin_num = 44;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V32)
{
  int pin_num = 32;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V33)
{
  int pin_num = 33;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V34)
{
  int pin_num = 34;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}









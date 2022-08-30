#include<SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <dht.h>
// GSM pin define
SoftwareSerial SIM900A(3,4);

// Analog input port soil moisture
#define SENSOR_IN A0

int PR = 0;
int PAST =0;
float T;
float H;
float SMV;
float moisture_percentage;

// Variable for soil moisture sensor value
int sensorval;

// Digital pin for DH11
#define dht_apin 2
dht DHT;

// Motor control through relay
// Define input pin for relay
int Relaypin= 7; 
 
// LCD pin define
const int rs = 8, en = 13, d4 = 12, d5 = 11, d6 = 10, d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// set up
void setup() {
  // Define the Relaypin as output pin
  pinMode(Relaypin, OUTPUT); 

  // Open Serial Monitor
  Serial.begin(9600);

  // Setting the baud rate of GSM Module 
  SIM900A.begin(9600);    
  Serial.println ("SIM900A Ready");
  delay(100);
  Serial.println ("Type s to send message or r to receive message");
 
  // Set ADC to use 3.3-volt AREF input
  analogReference(EXTERNAL);
}

void SOIL_MOISTURE(){
  // Read sensor value
  sensorval = analogRead(SENSOR_IN);
  moisture_percentage = (100.0 - (sensorval/1023.0)*100.0);

  // Print to Serial Monitor
  Serial.println(sensorval);
  Serial.print("Soil Moisture persentage (%) = ");
  Serial.println(moisture_percentage);
   lcd.begin(16, 2);
   lcd.setCursor(0,0);
   lcd.print("Moisture =" +String(moisture_percentage)+" %"); 
   SMV = moisture_percentage;
   delay(1000);
}

void MOTOR_TURNON(){
   digitalWrite(Relaypin, LOW); // Sends high signal 
   delay(100); // Waits for 1 second
   lcd.setCursor(0,0);
   Serial.print("Motor turn on");
   lcd.begin(16, 2);
   lcd.setCursor(0,0);
   lcd.print("Motor turn on"); 
   delay(1000);   
  
}

void MOTOR_TURNOFF(){
    digitalWrite(Relaypin, HIGH); // Makes the signal low
    delay(100); // Waits for 1 second
    lcd.begin(16, 2);
    lcd.setCursor(0,0);
    lcd.print("Motor turn-off"); 
    Serial.print("Motor turn off");
    delay(1000);
}

void HUM_TEM(){
  
    DHT.read11(dht_apin);
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");
    lcd.begin(16, 2);
    lcd.setCursor(0,0);
    lcd.print("Humidity =" +String(DHT.humidity)+  "% ");
    lcd.setCursor(0,1);   
    lcd.print("Temp =" +String(DHT.temperature) +  "C  ");
    H = DHT.humidity;
    T = DHT.temperature;
    delay(1000);
  
}

void SendMessageON(float SMV, float T, float H)
{
  Serial.println ("Sending Message");
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  Serial.println ("Set SMS Number");
  SIM900A.println("AT+CMGS=\"+8801700563512\"\r"); //Mobile phone number to send message
  delay(1000);
  Serial.println ("Set SMS Content");
  SIM900A.println("Motor Turn on. Soil Moisture value is "+ String(SMV)+" %. Temperature value is " + String(T)+" C. Humidity value is "+ String(H)+" %.");// Messsage content
  delay(100);
  Serial.println ("Finish");
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println ("Message has been sent ->SMS Selesai dikirim");
}

void SendMessageOFF(float SMV,float T,float H)
{
  Serial.println ("Sending Message");
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  Serial.println ("Set SMS Number");
  SIM900A.println("AT+CMGS=\"+8801700563512\"\r"); //Mobile phone number to send message
  delay(1000);
  Serial.println ("Set SMS Content");
  SIM900A.println("Motor Turn off. Soil Moisture value is "+ String(SMV)+" %. Temperature value is " + String(T)+" C. Humidity value is "+ String(H)+" %.");// Messsage content
  delay(100);
  Serial.println ("Finish");
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println ("Message has been sent ->SMS Selesai dikirim");
}

 void RecieveMessage()
{
  Serial.println ("SIM900A Membaca SMS");
  delay (1000);
  SIM900A.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
  Serial.write ("Unread Message done");
 }

// Soil moisture
void loop() {
         SOIL_MOISTURE();
         if(sensorval>600){
          PR=1;
          if(PR!=PAST){
            HUM_TEM();
            SendMessageON(SMV, T, H);
            PAST =1;
          }
         MOTOR_TURNON();
         return;
         }
         else{
         MOTOR_TURNOFF();
         if(PR==PAST){
          SendMessageOFF(SMV, T, H);
          PAST=0;
         }
         HUM_TEM();
         return;
         }   
}
  

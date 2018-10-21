#include "Wire.h"

byte decToBcd(byte val)                                           //value converter
{
  return( (val/10*16) + (val%10) );
}
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

byte minutes      = 0;                                            //holds the minutes read from the DS3231
byte hours        = 0;                                            //holds the hours read from the DS3231
byte counter      = 0;                                            //used for incrementing the values mentioned above
bool flag         = 0;                                            //used for navigating betveen menus

void setup() {
  DDRB = 0xFF;                                                    //The DDRB registers first four outputs are wired to the D registers inputs
  pinMode(2,INPUT_PULLUP);                                        //set
  pinMode(3,INPUT_PULLUP);                                        //increase
  pinMode(4,OUTPUT);                                              //minutes_lower_clk
  pinMode(5,OUTPUT);                                              //minutes_upper_clk
  pinMode(6,OUTPUT);                                              //hours_lower_clk
  pinMode(7,OUTPUT);                                              //hours_upper_clk
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
  Wire.begin();
  Serial.begin(9600);
  }

void loop() {
  
  Wire.beginTransmission(0x68);                                   //read the time from the DS3231
  Wire.write(1);
  Wire.endTransmission();
  Wire.requestFrom(0x68,1);
  minutes = Wire.read();
  
  Wire.beginTransmission(0x68);
  Wire.write(2);
  Wire.endTransmission();
  Wire.requestFrom(0x68,1);
  hours = (Wire.read() & 0x3f);

  Serial.print(bcdToDec(hours));
  Serial.print(":");
  Serial.println(bcdToDec(minutes));
  
  PORTB = hours;                                                  //second half of the hours
  delay(5);
  digitalWrite(6,HIGH);
  delay(5);
  digitalWrite(6,LOW);
  
  PORTB = hours>>4;                                               //shift by four the get the first half of the hours to the D registers 
  delay(5);
  digitalWrite(7,HIGH);
  delay(5);
  digitalWrite(7,LOW);
  
  PORTB = minutes;                                                //second half of the minutes
  delay(5);
  digitalWrite(4,HIGH);
  delay(5);
  digitalWrite(4,LOW);
  
  PORTB = minutes>>4;                                             //shift by four the get the first half of the minutes to the D registers
  delay(5);
  digitalWrite(5,HIGH);
  delay(5);
  digitalWrite(5,LOW);
  
  if(digitalRead(2)==LOW){                                        //if set is low enter the time setting menu
    Serial.println("set");
    do{
      PORTB=0xFF;                                                 //turn "off" the displays, the CD4028 will not turn on any of its outputs if the input value is above 9 or 1001.
      delay(5);
      digitalWrite(4,HIGH);
      digitalWrite(5,HIGH);
      digitalWrite(6,HIGH);
      digitalWrite(7,HIGH);
      delay(5);
      digitalWrite(4,LOW);
      digitalWrite(5,LOW);
      digitalWrite(6,LOW);
      digitalWrite(7,LOW);
      delay(50);                                                  //de-bounce
      while(digitalRead(2)==HIGH){                                //if set is released enter the hour incrementing part of the code
        flag=1;
         Serial.print("hours");
         Serial.print(": ");
         Serial.println(bcdToDec(hours));
         PORTB = hours;
         delay(5);
         digitalWrite(6,HIGH);
         delay(5);
         digitalWrite(6,LOW);
  
         PORTB = hours>>4;
         delay(5);
         digitalWrite(7,HIGH);
         delay(5);
          digitalWrite(7,LOW);
          if(digitalRead(3)==LOW){                                //hours and minutes are stored in BCD form, for me it was easier to increment the values this way.
            counter = bcdToDec(hours);
            counter++;
            hours = decToBcd(counter);
              if((bcdToDec(hours))==24){
                hours=0;
              }
            delay(200);
              PORTB = hours;
              delay(5);
              digitalWrite(6,HIGH);
              delay(5);
              digitalWrite(6,LOW);
  
              PORTB = hours>>4;
              delay(5);
              digitalWrite(7,HIGH);
              delay(5);
              digitalWrite(7,LOW);
              Serial.print("hours");
              Serial.print(": ");
              Serial.println(bcdToDec(hours));
          }
      } 
        if(flag==1){                                              //pretty much the same as hours, so not much to comment
            do{
              PORTB=0xFF;
              delay(5);
              digitalWrite(4,HIGH);
              digitalWrite(5,HIGH);
              digitalWrite(6,HIGH);
              digitalWrite(7,HIGH);
              delay(5);
              digitalWrite(4,LOW);
              digitalWrite(5,LOW);
              digitalWrite(6,LOW);
              digitalWrite(7,LOW);
              flag=0;
              counter=0;
              delay(50);
                while(digitalRead(2)==HIGH){
                  flag=1;
                   Serial.print("minutes");
                   Serial.print(": ");
                   Serial.println(bcdToDec(minutes));
                   PORTB = minutes;
                   delay(5);
                   digitalWrite(4,HIGH);
                   delay(5);
                   digitalWrite(4,LOW);
            
                   PORTB = minutes>>4;
                   delay(5);
                   digitalWrite(5,HIGH);
                   delay(5);
                    digitalWrite(5,LOW);
                    if(digitalRead(3)==LOW){
                     counter = bcdToDec(minutes);
                     counter++;
                     minutes = decToBcd(counter);
                        if((bcdToDec(minutes))==60){
                          minutes=0;
                        }
                      delay(200);
                        PORTB = minutes;
                        delay(5);
                        digitalWrite(4,HIGH);
                        delay(5);
                        digitalWrite(4,LOW);
            
                        PORTB = minutes>>4;
                        delay(5);
                        digitalWrite(5,HIGH);
                        delay(5);
                        digitalWrite(5,LOW);
                        Serial.print("minutes");
                        Serial.print(": ");
                        Serial.println(bcdToDec(minutes));
                    }
                }

                      if(flag==1){                                //send the new values to the DS3231, year day of month and the rest of the registers are not set every time because I don't think that it's necessary.
                        do
                         {
                            Wire.beginTransmission(0x68);
                            Wire.write(0);
                            Wire.write(decToBcd(0));
                            Wire.write(minutes);
                            Wire.write(hours);
                            Wire.endTransmission();
                            Wire.beginTransmission(0x68);
                            Wire.write(4);
                            Wire.write(1);
                            Wire.write(1);
                            Wire.write(18);
                            Wire.endTransmission();
                            flag=0;
                            counter=0;
                                                            
                            } while(digitalRead(2)==LOW);
                            }
                  
            } while(digitalRead(2)==LOW);
          }
        
    } while(digitalRead(2)==LOW);
  }
  delay(1000);
}
  

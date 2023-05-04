#include <IRremote.h>
#include <Servo.h>
Servo servo;
IRrecv IR(8);
int irLed = 12;
int checker = 0;
int is_hot = 0;
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        
//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  
//PIR - Passive Infrared Sensor
int pirPin = 7;    //the digital pin connected to the PIR sensor's output
int ledPin = 5;    //the digital pin connected to the LED output
int Buzzer = 6;    //the digital pin connected to the BUZZER output
int greenLed = 2;
const int lm35_pin = A1;	/* LM35 O/P pin */

float arr [100]; //declare an empty array
int count = 0;
///////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(greenLed, OUTPUT);
  servo.attach(4);
  digitalWrite(pirPin, LOW);
  Serial.begin(9600);
  Serial.println("IR Receive test");
  pinMode(irLed,OUTPUT);
  IR.enableIRIn();
  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
  }

////////////////////////////
//LOOP
void loop(){
          int reading;
          float temp_val;
          reading = analogRead(lm35_pin);	/* Read Temperature */
          temp_val = reading * (5.0/1024.0);	/* Convert adc value to equivalent voltage */
          // temp_val = (temp_val/10);	/* LM35 gives output of 10mv/°C */
          float temperatureC = temp_val * 100;
          Serial.print("Temperature = ");
          Serial.print(temperatureC);
          Serial.print(" Degree Celsius\n");
          delay(1000);

         
            if (temperatureC < 45.00 && temperatureC > 15.00){
              digitalWrite(greenLed, HIGH); 
              digitalWrite(irLed, LOW);
              // digitalWrite(ledPin,HIGH);
              Serial.print("Normal temperature\n");
              delay(1000);
              if (checker==1){
                servo.write(90);  // Stop
                delay(500);
                servo.write(0);  // Spin in opposite direction
                delay(3000);
                checker = checker + 1;
              }
              if(IR.decode()){
              if(IR.decodedIRData.decodedRawData == 0xEA15FF00){
                // PLUS - DOWN
              servo.write(0); //Spin in one direction
              delay(3000);
              servo.write(90); // Stop
              delay(500);
              servo.write(180);  // Spin in opposite direction
              // digitalWrite(ledPin, HIGH);
              }
              if(IR.decodedIRData.decodedRawData == 0xF807FF00){
                // MINUS - UP
              servo.write(90);  // Stop
              delay(500);
              servo.write(0);  // Spin in opposite direction
              delay(3000);
              // digitalWrite(ledPin, LOW);
              }
              
              IR.resume();
            }
            }
            
            if (temperatureC < 15.00){
              arr[count] = temperatureC + 45.00;
              count += 1;
              while(digitalRead(pirPin) == LOW){
                // Serial.print(temperatureC);
                delay(500);
                digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
                tone(Buzzer,500);
                digitalWrite(greenLed, LOW);
                delay(3000);
                digitalWrite(irLed, HIGH);
                while (checker <= 0){
                servo.write(0); //Spin in one direction
                delay(3000);
                servo.write(90); // Stop
                delay(500);
                servo.write(180);  // Spin in opposite direction
                delay(3000);
                checker = checker+1;
                is_hot = is_hot + 1;
                if (is_hot>5){
                  break;
                }
                }
                if(IR.decode()){
                  if(IR.decodedIRData.decodedRawData == 0xEA15FF00){
                    // PLUS - DOWN
                  servo.write(0); //Spin in one direction
                  delay(3000);
                  servo.write(90); // Stop
                  delay(500);
                  servo.write(180);  // Spin in opposite direction
                  // digitalWrite(ledPin, HIGH);
                  }
                  if(IR.decodedIRData.decodedRawData == 0xF807FF00){
                    // MINUS - UP
                  servo.write(90);  // Stop
                  delay(500);
                  servo.write(0);  // Spin in opposite direction
                  delay(3000);
                  // digitalWrite(ledPin, LOW);
                  }
                  if(IR.decodedIRData.decodedRawData == 0XE718FF00){
                  // 2
                  noTone(Buzzer);
                }
                  IR.resume();
                }


                if(lockLow){  
                  //makes sure we wait for a transition to LOW before any further output is made:
                  lockLow = false;            
                  }         
                  takeLowTime = true;
                }

              while(digitalRead(pirPin) == HIGH){       
                digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
                noTone(Buzzer);
                delay(4000);
                if(takeLowTime){
                  lowIn = millis();          //save the time of the transition from high to LOW
                  takeLowTime = false;       //make sure this is only done at the start of a LOW phase
                  }
                //if the sensor is low for more than the given pause, 
                //we assume that no more motion is going to happen
                if(!lockLow && millis() - lowIn > pause){  
                    //makes sure this block of code is only executed again after 
                    //a new motion sequence has been detected
                    lockLow = true;                        
                     Serial.print("motion detected at ");      //output
                     Serial.print((millis() - pause)/1000);
                     Serial.println(" sec");
                     delay(50);
                    }
                }
                  Serial.println("RECORD OF HIGH TEMPERATURE:\n");
                  for(int i = 0; i < 100; i++)
                    {
                      if(arr[i]>1.00){
                      Serial.println(arr[i]);
                      }
                    }
                  Serial.println("\n---\n");
                // break;
            }
}

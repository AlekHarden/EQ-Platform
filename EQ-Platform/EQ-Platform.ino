#include <FlexyStepper.h>
#include <ezBuzzer.h>
#include <math.h>


//DIGITAL INPUTS
const int LIMIT_SWITCH_PIN = 11;
const int USER_BTN_PIN = 10;
const int DEBUG_SW_PIN = 16;


//ANALOG INPUTS
const int VOLTAGE_DIVIDER_PIN = 28;


//OUTPUTS
const int LED_PIN = 17;
const int BEEPER_PIN = 30;


//STEPPER PINS
const int MOTOR_STEP_PIN = 3;
const int MOTOR_DIRECTION_PIN = 2;
const int MOTOR_ENABLE = 26;



//Loop Vars
int limitSwtichState = 0;
int buttonState = 0;

int buzzer = 0;


float voltage = 0;
const float voltageDividerRatio = 0.146355;


float trackingTargetAngle = 0;
float trackingTargetPos = 0;



int frames = 0;;

unsigned long int time; //used to store output of millis()

unsigned long int trackingTimeStart;
unsigned long int trackingTime;

unsigned long int serialTimerStart;
unsigned long int serialTimer;




//Library Setup
FlexyStepper stepper;





//Angle in Degrees
float angleToPos(float angle){

  float a1 = angle;
  float a2 = a1 * a1;
  float a3 = a2 * a1;
  float a4 = a3 * a1;


  return (0.0007 * a4) - (0.006 * a3) + (0.0165 * a2) + (16.683 * a1) + 112.59;

}




void home(){

  digitalWrite(LED_PIN,HIGH);

  const float homingSpeedInMMPerSec = 24;
  const float maxHomingDistanceInMM = 290;   // since my lead-screw is 38cm long, should never move more than that
  const int directionTowardHome = -1;

  bool success = stepper.moveToHomeInMillimeters(directionTowardHome, homingSpeedInMMPerSec, maxHomingDistanceInMM, LIMIT_SWITCH_PIN);
  if (!success) {


    //RESTART REQUIRED
    while(true) {
      digitalWrite(LED_PIN, HIGH);
      delay(50);
      digitalWrite(LED_PIN, LOW);
      delay(50);
    }

  }

  stepper.setCurrentPositionInMillimeters(0);
  stepper.moveToPositionInMillimeters(0);
  delay(500);

  trackingTimeStart = millis();
  digitalWrite(LED_PIN,LOW);
  


}


void setup() {

  //Time Stuff
  serialTimerStart = millis();
  


  Serial.begin(112050);



  
  //Turn on LED
  pinMode(LED_PIN,OUTPUT);

  //SetupBeeper
  pinMode(BEEPER_PIN,OUTPUT);


  //setup Inputs
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP); //Only Pullup is available
  pinMode(USER_BTN_PIN, INPUT_PULLUP);
  pinMode(DEBUG_SW_PIN, INPUT_PULLUP); 





  //STEPPER SETUP
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);
  pinMode(MOTOR_ENABLE,OUTPUT);
  digitalWrite(MOTOR_ENABLE,LOW);
  stepper.setSpeedInStepsPerSecond(600*16); //MAX SPEED 600
  stepper.setAccelerationInStepsPerSecondPerSecond(1000*16);

  stepper.setStepsPerMillimeter(400);  //TODO: Measure steps/mm


  //Home on power Start
  home();
}



void loop() {

  stepper.processMovement();


  if (!digitalRead(USER_BTN_PIN)){
    home();
  }

  //Tracking
  trackingTime = millis() - trackingTimeStart;
  trackingTargetAngle = 7 - (360.0 * (1.0/24.0) * (1.0/3600.0) * trackingTime/1000);
  trackingTargetPos = angleToPos(trackingTargetAngle);
  stepper.setTargetPositionInMillimeters(trackingTargetPos - (fmod(trackingTargetPos,0.004)) );
  //stepper.setTargetPositionInMillimeters(trackingTargetPos);




  //Check if finished
  if (trackingTargetAngle <= -7){

    //Movement Finished. Wait for user to reset.
    while(digitalRead(USER_BTN_PIN)) {
      digitalWrite(LED_PIN, HIGH);
      delay(300);
      digitalWrite(LED_PIN, LOW);
      delay(300);
    }
    home();
    trackingTargetAngle = -7;
    trackingTimeStart = millis();
  }
 


  //Debug Stuff
  if(digitalRead(DEBUG_SW_PIN)){

    frames++;
    serialTimer = millis() - serialTimerStart;
      
    //Report button states every so often
    if (serialTimer >= 1000){

      //tone(BEEPER_PIN, 5000,500);

      serialTimerStart = millis();

      limitSwtichState = digitalRead(LIMIT_SWITCH_PIN);
      buttonState = digitalRead(USER_BTN_PIN);

      voltage = (analogRead(VOLTAGE_DIVIDER_PIN)/ 1024.0) * 5 / voltageDividerRatio;

      Serial.println("-----------------");
      Serial.print("  Frames: "); Serial.println(frames);
      Serial.print("  LimitSwtich: "); Serial.println(limitSwtichState);
      Serial.print("  Button: "); Serial.println(buttonState);
      Serial.print("  Voltage: "); Serial.println(voltage);
      Serial.print("  TargetAngle: "); Serial.println(trackingTargetAngle,6);
      Serial.print("  TargetPos: "); Serial.println(trackingTargetPos,6);
      Serial.print("  Stepper: "); Serial.println(stepper.getCurrentPositionInMillimeters());

      frames = 0;
  }



    
  }

  
  




}

#include <Servo.h>
#include <AFMotor.h>
#include <NewPing.h>

// --- Definiri Constante ---
#define TRIG_PIN A3
#define ECHO_PIN A5
#define MAX_DISTANCE 100
#define MAX_SPEED 200
#define SERVO_CENTER 115 
#define AUTO_STOP_THRESHOLD 25 
#define TURN_DURATION_MS 800   
#define BACKUP_DURATION_MS 300 

// --- Senzor ---
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// --- Motoare ---
AF_DCMotor motor1(1, MOTOR12_64KHZ);
AF_DCMotor motor2(2, MOTOR12_64KHZ);
AF_DCMotor motor3(3, MOTOR34_64KHZ);
AF_DCMotor motor4(4, MOTOR34_64KHZ);

// --- Servo ---
Servo myservo;

// --- Mod robot ---
enum RobotMode { MANUAL, AUTOMATIC };
// AUTOMATIC ca DEFAULT
RobotMode currentMode = AUTOMATIC;

// --- State machine automat ---
enum AutoState { CRUISE, BACKUP, TURN_RIGHT, TURN_LEFT };
AutoState autoState = CRUISE;
unsigned long stateStartTime = 0;

// --- Variabile generale ---
boolean goesForward = false;

// --- Functii motoare ---
void setMotorSpeed(int speed){
  motor1.setSpeed(speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(speed);
}

void moveStop(){
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
  goesForward = false;
}

void moveForward(){
  if(!goesForward){
    goesForward = true;
    setMotorSpeed(MAX_SPEED);
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
  }
}

void moveBackward(){
  goesForward = false;
  setMotorSpeed(MAX_SPEED);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void turnRight(){
  goesForward = false;
  setMotorSpeed(MAX_SPEED);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void turnLeft(){
  goesForward = false;
  setMotorSpeed(MAX_SPEED);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

// --- Functii servo + senzor ---
int readPing(){
  int cm = sonar.ping_cm();
  if(cm == 0) cm = MAX_DISTANCE;
  return cm;
}

int lookRight(){
  myservo.write(50);
  delay(200);
  int cm = readPing();
  myservo.write(SERVO_CENTER);
  delay(100);
  return cm;
}

int lookLeft(){
  myservo.write(170);
  delay(200);
  int cm = readPing();
  myservo.write(SERVO_CENTER);
  delay(100);
  return cm;
}

// --- Modul Automat ---
void automaticMode(){
  int distance = readPing();
  unsigned long currentTime = millis();

  switch(autoState){
    case CRUISE:
      if(distance <= AUTO_STOP_THRESHOLD){
        moveStop();
        autoState = BACKUP;
        stateStartTime = currentTime;
      } else {
        moveForward();
      }
      break;

    case BACKUP:
      moveBackward();
      if(currentTime - stateStartTime >= BACKUP_DURATION_MS){
        moveStop();
        int dR = lookRight();
        int dL = lookLeft();
        if(dR >= dL){
          autoState = TURN_RIGHT;
        } else {
          autoState = TURN_LEFT;
        }
        stateStartTime = currentTime;
      }
      break;

    case TURN_RIGHT:
      turnRight();
      if(currentTime - stateStartTime >= TURN_DURATION_MS){
        moveStop();
        autoState = CRUISE;
      }
      break;

    case TURN_LEFT:
      turnLeft();
      if(currentTime - stateStartTime >= TURN_DURATION_MS){
        moveStop();
        autoState = CRUISE;
      }
      break;
  }
}

// -- Modul Manual (Bluetooth) --
void manualMode(){

  if(Serial.available()){

    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    Serial.print("Comanda: "); Serial.println(command);
    if(command == "fata" || command == "f") moveForward();
    else if(command == "spate" || command == "b") moveBackward();
    else if(command == "stanga" || command == "l"){ turnLeft(); delay(TURN_DURATION_MS); moveStop();}
    else if(command == "dreapta" || command == "r"){ turnRight(); delay(TURN_DURATION_MS); moveStop();}
    else if(command == "stop"|| command=="s") moveStop();

    else if(command == "auto" || command == "a"){
      currentMode = AUTOMATIC;
      autoState = CRUISE; 
      Serial.println("--- Entered AUTOMATIC Mode ---");
      moveForward(); 
    }
    else if(command == "manual" || command == "m"){
      currentMode = MANUAL;
      Serial.println("--- Entered MANUAL Mode ---");
      moveStop();

    }
  }
}

// --- Setup ---
void setup(){
  Serial.begin(9600);
  myservo.attach(10);
  myservo.write(SERVO_CENTER);
  delay(500);
  setMotorSpeed(MAX_SPEED);
  moveStop();
  Serial.println("Robot ready. AUTOMATIC Mode active. Send manual commands (f/s/l/d/m).");
}

// --- Loop principal ---
void loop(){

  if(currentMode == AUTOMATIC){

    automaticMode();


    if (Serial.available()) {

        String stopCommand = Serial.readStringUntil('\n');

        stopCommand.trim();

        stopCommand.toLowerCase();


        // Comanda de iesire din modul automat (m, manual, stop)

        if (stopCommand.equals("manual") || stopCommand.equals("m") || stopCommand.equals("stop")) {

            currentMode = MANUAL;

            moveStop();

            Serial.println("--- Returned to MANUAL Mode ---");

        }

    }


  } else { // MANUAL

    manualMode();

  }
  delay(20);

}

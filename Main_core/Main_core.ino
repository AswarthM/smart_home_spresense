/*TODO
   implement touch switches
   implement door state done
   added servo instead of solenoid
*/

#include <MP.h>
#include <Servo.h>

static Servo myServo;

#define MSGLEN      64
#define MY_MSGID    10
struct MyPacket {
  volatile int status; /* 0:ready, 1:busy */
  char message[MSGLEN];
};
String message = "";
String prevMessage = "";
String readString = "";
String message2 = "";
int sent = 0;
//outputs
int sw[4] = {10, 11, 12, 13};
int lockPin = 9; //solenoid lock pin

//inputs
int doorPin = 8;
int touch[4] = {7, 6, 5, 4};

int swState[4];
int touchState[4];
int lockState; //lock state
int pir1 = 0;
int pir2;
int pirSwState;

unsigned long time1 = 0;

int mq2;
int ind1; // locations
int ind2;
int ind3;
int ind4;

void setup()
{
  int ret1 = 0;
  int ret2 = 0;
  int subid1 = 1;
  int subid2 = 2;

  myServo.attach(PIN_D09);

  Serial.begin(9600);
  Serial2.begin(9600);
  while (!Serial);

  Serial.println("1From Main Core Start");

  /* Boot SubCore */
  ret1 = MP.begin(subid1);
  ret2 = MP.begin(subid2);
  if (ret1 < 0) {
    printf("MP.begin(%d) error = %d\n", subid1, ret1);
  }
  if (ret2 < 0) {
    printf("MP.begin(%d) error = %d\n", subid2, ret2);
  }
  MP.RecvTimeout(MP_RECV_POLLING);
  for (int i = 0; i < 4; i++) {
    pinMode(sw[i], OUTPUT);
    digitalWrite(sw[i], HIGH);
  }
  pinMode(doorPin, INPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(touch[i], INPUT);
  }

}

void loop()
{
  //readTouch();
  readPir1();

  int      ret1;
  int      subid1 = 1;
  int8_t   msgid;
  MyPacket *packet;

  /* Receive message from SubCore */
  ret1 = MP.Recv(&msgid, &packet, subid1);
  if (ret1 > 0) {
    message = (String)packet->message;
    //Serial.print(message);
    message += ",";
    message += "0";// add 2pir and mq02 sensor values
    message += ",";
    message += "0";
    message += ",";
    message += readMq2(); //add mq2 values
    message += ",";
    message += readDoor();
    message += "*";
    packet->status = 0;
    Serial.println("From Main Core Recieved message from Sub Core1");
    //    //prevMessage = message;
    //    //while(!Serial2.available()); //wait till serial2 is available
    //    delay(1000);
  }

  int      ret2;
  int      subid2 = 2;
  MyPacket *packet2;

  /* Receive message from SubCore2 */
  ret2 = MP.Recv(&msgid, &packet2, subid2);
  if (ret2 > 0) {
    message2 = (String)packet2->message;
    Serial.print("2From Main Core");
    Serial.println(message2);
    for (int i = 0; i < 4; i++) {
      if (message2.substring(i, i + 1) == "1") {
        swState[i] = !swState[i];
      }
    }
    updateSw();
    packet2->status = 0;
    Serial.println("From Main Core Recieved message from Sub Core2");
  }
  if (!sent) {
    Serial.print("2From Main Core");
    Serial.println(message);
    Serial2.println(message); //send nearby devices list to nodemcu
    sent = 1;
  }

  prevMessage = message;
  message = "";
  //while (!Serial2.available());
  while (Serial2.available())  {
    sent = 0;
    readString = Serial2.readString();
    //do stuff
    Serial.print("captured String is : ");
    Serial.println(readString); //prints string to serial port out
    for (int i = 0; i < 4; i++) {
      if (readString.charAt(i) == '1') {
        swState[i] = !swState[i];
      }
    }
    lockState = readString.charAt(4);

    updateSw();
    updateLock();
    //    ind1 = readString.indexOf(',');  //finds location of first ,
    //    swState[0] = readString.substring(0, ind1).toInt();   //captures first data String
    //    ind2 = readString.indexOf(',', ind1 + 1 ); //finds location of second ,
    //    swState[1] = readString.substring(ind1 + 1, ind2 + 1).toInt(); //captures second data String
    //    ind3 = readString.indexOf(',', ind2 + 1 );
    //    swState[2] = readString.substring(ind2 + 1, ind3 + 1).toInt();
    //    ind4 = readString.indexOf(',', ind3 + 1 );
    //    swState[3] = readString.substring(ind3 + 1, ind4 + 1).toInt(); //captures remain part of data after last (,)
    //    lockState = readString.substring(ind4 + 1).toInt();
    delay(1000);
    readString = ""; //clears variable for new input
  }


}

void updateSw() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(sw[i], !swState[i]);
  }
}
void updateLock() {
  //digitalWrite(lockPin, lockState);
  if(lockState){
    myServo.write(90);
  }
  else{
    myServo.write(0);
  }
}

void readPir1() {
    Serial.println(analogRead(A0));
  if (pir1) {
    if ((unsigned long)(millis() - time1) >= 1000 * 5) { //delay 5 seconds
      digitalWrite(sw[3], !swState[3]);
      pir1 = 0;
    }
  }
  else{
    if (analogRead(A0) >= 500) {
      pir1 = 1;
      pirSwState = swState[3];
      digitalWrite(sw[3], LOW);
      time1 = millis();
    }
  }
}

String readDoor() {
  if (digitalRead(doorPin) == HIGH) {
    return "1";
  }
  else {
    return "0";
  }
}

String readMq2() {
  if (analogRead(A2) >= 500) {
    return "1";
  }
  else {
    return "0";
  }
}
void readTouch() {
  for (int i = 0; i < 4; i++) {
    swState[i] = digitalRead(touch[i]);
  }
}

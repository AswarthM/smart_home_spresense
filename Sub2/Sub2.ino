#include <MP.h>

#define MSGLEN 64
#define MY_MSGID 10
struct MyPacket {
  volatile int status; /* 0:ready, 1:busy */
  char message[MSGLEN];
};
#define SUBID "Sub1"
MyPacket packet2;

int touchPin[4] = {4, 5, 6, 7};
int touchState[4];
int prevTouchState[4];
bool changed[4];
String message = "";
String prevMessage = "";
int check = 0;

void setup() {
  Serial.begin(9600);
  memset(&packet2, 0, sizeof(packet2));
  MP.begin();
  for (int i = 0; i < 4; i ++) {
    pinMode(touchPin[i], INPUT);
    touchState[i] = 0;
    prevTouchState[i] = 0;
    changed[i] = 0;
  }
  Serial.println("From Sub Core 2 Start");
}

void loop() {
  int ret;
  int msgReady = 0;

  touchState[0] = digitalRead(touchPin[0]);
  if (touchState[0] != prevTouchState[0]) {
    if (touchState[0] == 1) { //detect rising edge
      changed[0] = 1;
    }
    delay(50);
  }
  prevTouchState[0] = touchState[0];


  touchState[1] = digitalRead(touchPin[1]);
  if (touchState[1] != prevTouchState[1]) {
    if (touchState[1] == 1) { //detect rising edge
      changed[1] = 1;
    }
    delay(50);
  }
  prevTouchState[1] = touchState[1];



  touchState[2] = digitalRead(touchPin[2]);
  if (touchState[2] != prevTouchState[2]) {
    if (touchState[2] == 1) { //detect rising edge
      changed[2] = 1;
    }
    delay(50);
  }
  prevTouchState[2] = touchState[2];


  touchState[3] = digitalRead(touchPin[3]);
  if (touchState[3] != prevTouchState[3]) {
    if (touchState[3] == 1) { //detect rising edge
      changed[3] = 1;
    }
    delay(50);
  }
  prevTouchState[3] = touchState[3];


  for (int i = 0; i < 4; i++) {
    check += changed[i];
  }
  
  for (int i = 0; i < 4; i++) { //build message to be sent to main core
    if (changed[i]) {
      message += "1";
      changed[i] = 0;
    }
    else {
      message += "0";
    }
  }
  if ((packet2.status == 0) && (check > 0) && (message != prevMessage)) {

    /* status -> busy */
    packet2.status = 1;
    //send message to main core
    //if 1 then clicked, invert current relay state
    Serial.print("From Sub Core 2 ");
    Serial.println(message);
    message.toCharArray(packet2.message, MSGLEN);
    /*snprintf(packet.message, MSGLEN, "%s", msgStr, count++);*/

    /* Send to MainCore */
    ret = MP.Send(MY_MSGID, &packet2);
    Serial.println("3From Sub Core2, Sent to main core");
    if (ret < 0) {
      printf("MP.Send error = %d\n", ret);
    }
    check = 0;
  }
  prevMessage = message;
  message = "";

}

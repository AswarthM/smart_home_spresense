#include <MP.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);  //RX, TX
#define MSGLEN      64
#define MY_MSGID    10
struct MyPacket {
  volatile int status; /* 0:ready, 1:busy */
  char message[MSGLEN];
};
String msgStr = "hiii";
String prevMessage = "";

#define SUBID "Sub1"

MyPacket packet;

void setup()
{
  Serial.begin(9600);
  memset(&packet, 0, sizeof(packet));
  MP.begin();
  mySerial.begin(9600);
  Serial.println("1From Sub Core Start");
}

void loop()
{
  int        ret;
  int msgReady = 0;
  while (mySerial.available()) {
    msgStr = mySerial.readString();
    msgStr = msgStr.substring(0, 5); // remove trailing newline
    msgReady = 1;
//    Serial.print("2From Sub Core");
//    Serial.println(msgStr);
  }

  if ((msgStr != prevMessage) || 1) {  /////////////////////debug
    if (packet.status == 0 && msgReady == 1) {

      /* status -> busy */
      packet.status = 1;

      /* Create a message */
      msgStr.toCharArray(packet.message, MSGLEN);
      /*snprintf(packet.message, MSGLEN, "%s", msgStr, count++);*/

      /* Send to MainCore */
      ret = MP.Send(MY_MSGID, &packet);
      Serial.println("3From Sub Core, Sent to main core");
      if (ret < 0) {
        printf("MP.Send error = %d\n", ret);
      }
      prevMessage = msgStr;
      msgStr = "";
      msgReady = 0;
    }
  }
  delay(500);
}

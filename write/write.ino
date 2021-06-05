#include <SPI.h>
#include <mcp2515.h>

struct can_frame can_rpm;
struct can_frame can_v;
struct can_frame can_t;
struct can_frame request;
struct can_frame msg[3];

const byte interruptPin = 2;

MCP2515 mcp2515(10);

byte i;


void setup() {
  //pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), OBD2, LOW);

  can_rpm.can_id  = 0x7E8;
  can_rpm.can_dlc = 8;
  can_rpm.data[0] = 4; // dlc
  can_rpm.data[1] = 0x41;
  can_rpm.data[2] = 0x0C; // PID
  can_rpm.data[3] = 0x6C; // A
  can_rpm.data[4] = 0xE4; // B
  can_rpm.data[5] = 0x23;
  can_rpm.data[6] = 0x23;
  can_rpm.data[7] = 0x23;

  can_v.can_id  = 0x7E8;
  can_v.can_dlc = 8;
  can_v.data[0] = 3; // dlc
  can_v.data[1] = 0x41;
  can_v.data[2] = 0x0D; // PID
  can_v.data[3] = 0x45; // A
  can_v.data[4] = 0x23; // B
  can_v.data[5] = 0x23;
  can_v.data[6] = 0x23;
  can_v.data[7] = 0x23;

  can_t.can_id  = 0x7E8;
  can_t.can_dlc = 8;
  can_t.data[0] = 3; // dlc
  can_t.data[1] = 0x41;
  can_t.data[2] = 0x5C; // PID
  can_t.data[3] = 0x3F; // A
  can_t.data[4] = 0x23; // B
  can_t.data[5] = 0x23;
  can_t.data[6] = 0x23;
  can_t.data[7] = 0x23;
  Serial.begin(115200);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  msg[0] = can_t;
  msg[1] = can_v;
  msg[2] = can_rpm;
  i = 0;
}

void loop() {

  mcp2515.sendMessage(&msg[i]);
  Serial.println(msg[i].data[2]);
  i++;
  if (i>2) {
    i=0;
  }
  delay(1000);

  //mcp2515.sendMessage(&can_v);
  //Serial.println("Sent v");
  //delay(50);
  //mcp2515.sendMessage(&can_t);
  //Serial.println("Sent t");
  //delay(50);
  //mcp2515.sendMessage(&can_rpm);
  //Serial.println("Sent rpm");
  //delay(50);
}

void OBD2(){
  if (mcp2515.readMessage(&request) == MCP2515::ERROR_OK) {
    if (request.can_id==0x7DF){
      switch (request.data[2]) {
        case 0x0C://rpm
        mcp2515.sendMessage(&can_rpm);
        break;
        case 0x0D://vel
        mcp2515.sendMessage(&can_v);
        break;
        case 0x5C://temp
        mcp2515.sendMessage(&can_t);
        break;
      }
    }
  }
}

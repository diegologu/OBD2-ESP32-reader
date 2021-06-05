#include <SPI.h>
#include <mcp2515.h>

const byte interruptPin = 2;
const byte modo=0;
uint8_t MSGCAN[12];
volatile uint32_t tiempostamp=0;
const byte numPIDs=5;
uint8_t PIDs[numPIDs]={0x10, 0x0B, 0X0F, 0X44, 0x0C};

struct can_frame canMsgOBD2; //escribir obd2 (por supuesto obd2 esta sobre can)
struct can_frame canMsg; //leer can
MCP2515 mcp2515(10);

void setup() {
  Serial.begin(9600);
  Serial.flush();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  canMsgOBD2.can_id  = 0x7DF;
  canMsgOBD2.can_dlc = 8;
  canMsgOBD2.data[0] = 0x02;
  canMsgOBD2.data[1] = 0x01;
  canMsgOBD2.data[2] = PIDs[0]; // PIDs
  canMsgOBD2.data[3] = 0x55;
  canMsgOBD2.data[4] = 0x55;
  canMsgOBD2.data[5] = 0x55;
  canMsgOBD2.data[6] = 0x55;
  canMsgOBD2.data[7] = 0x55;

  pinMode(interruptPin, INPUT_PULLUP);

  switch(modo){
    case 0: //snifeer
      attachInterrupt(digitalPinToInterrupt(interruptPin), snifeer, LOW);
      break;
    case 2: //OBD2
      TCNT2 = 0;
      TIMSK2 |= 1 << TOIE2;
      TCCR2B = 0b00000111;//33mseg
      attachInterrupt(digitalPinToInterrupt(interruptPin), OBD2, LOW);
      break;
  }
}

void loop() {
}

uint8_t iPIDs=0;
ISR(TIMER2_OVF_vect) {
  Serial.println(".");
  canMsgOBD2.data[2] = PIDs[iPIDs];
  mcp2515.sendMessage(&canMsgOBD2);
  iPIDs++;
  iPIDs=iPIDs%numPIDs;
}

void snifeer(){
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    send2ESP();
  }
}

void OBD2(){
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if (canMsg.can_id==0x7E8){
      send2ESP();
    }
  }
}

void send2ESP(){
    Serial.write(canMsg.data[2]);
    Serial.write(canMsg.data[3]);
    Serial.write(canMsg.data[4]);
    mcp2515.clearRXnOVR();
}

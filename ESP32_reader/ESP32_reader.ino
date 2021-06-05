

#define RXD2 16

#define TXD2 17

// Set these to your desired credentials.


uint8_t MSGCAN[3];
short temp;
unsigned char vel;
unsigned short rpm;
byte ind;

void setup() {

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  ind = 0;

}

void loop() {
 if (Serial2.available() > 0) {
   MSGCAN[ind] = Serial2.read();
   ind++;
   if (ind == 3) {
      switch (MSGCAN[0]) {
        case 0x0C: //rpm
          rpm = ((256*MSGCAN[1])+MSGCAN[2])/4;
        break;
        case 0x0D: //vel
          vel = MSGCAN[1];
        break;
        case 0x5C: //temp
          temp = MSGCAN[1]-40;
        break;
      }
      ind = 0;
   }
   Serial.print("RPM:\t");
   Serial.print(rpm);
   Serial.print("\tVelocidad:  ");
   Serial.print(vel);
   Serial.print("\tT aceite:  ");
   Serial.print(temp);
   Serial.println();
 }
}

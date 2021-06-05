

#define RXD2 16

#define TXD2 17

// Set these to your desired credentials.


uint8_t MSGCAN[3];
byte ind;

void setup() {

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  ind = 0;

}

void loop() {
 //MSGCAN = Serial2.read();
 //Serial.print(MSGCAN[8]);
 //Serial.println();
 //Serial.print(MSGCAN[9]);
 //Serial.println();
 //Serial.print(MSGCAN[10]);
 //Serial.println();
 if (Serial2.available() > 0) {
   MSGCAN[ind] = Serial2.read();
   ind++;
   if (ind == 3) {
      for (byte i = 0; i <3; i++)
      {
         Serial.write(MSGCAN[i]);
      }
      Serial.println();
      ind = 0;
   }
 }
}


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define RXD2 16

#define TXD2 17

char auth[] = "hwc8WcJ3m34SdJuP_iEATW3_12Q0By_a";

char ssid[] = "LOPEZGERRERO*";
char pass[] = "Lg104562";

BlynkTimer timer;

short temp;
unsigned char vel;
unsigned short rpm;
uint8_t MSGCAN[3];

void resfreshData()
{
  if (Serial2.available() > 0) {
    getData();
  }
  Blynk.virtualWrite(V1, rpm);
  Serial.print("RPM: ");
  Serial.print(rpm);
  Blynk.virtualWrite(V2, temp);
  Serial.print("\tTemp: ");
  Serial.print(temp);
  Blynk.virtualWrite(V3, vel);
  Serial.print("\tVel: ");
  Serial.print(vel);

  Serial.println();
}

void getData(){
  for (byte i = 0; i < 3; i++) {
    MSGCAN[i] = Serial2.read();
  }
  Serial.write(MSGCAN,3);
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
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, resfreshData);

  temp = 20;
  vel = 50;
  rpm = 3000;
}

void loop()
{
  Blynk.run();
  timer.run();
}

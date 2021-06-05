#include <WiFi.h>

#define RXD2 16

#define TXD2 17

const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

WiFiServer server(80);

String header;

uint8_t MSGCAN[3];
short temp;
unsigned char vel;
unsigned short rpm;


void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  temp = 18;
  vel = 30;
  rpm = 3000;

  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
}

void getData(){
  for (byte i = 0; i < 3; i++) {
    MSGCAN[i] = Serial2.read();
  }
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

void loop(){
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (Serial2.available() > 0) {
        getData();
      }
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");

            // Web Page Heading
            client.println("<body><h1>OBD2 Reader Web Server</h1>");

            client.println("<p>RPM: " + (String) rpm + "</p>");
            client.println("<p>Velocity: " + (String) vel + "</p>");
            client.println("<p>Oil Temperature: " + (String) temp + "</p>");

            client.println("</body></html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

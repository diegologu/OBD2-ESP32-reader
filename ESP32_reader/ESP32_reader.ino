#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define RXD2 16

#define TXD2 17

const char* ssid     = "LOPEZGERRERO*";
const char* password = "Lg104562";

AsyncWebServer server(80);
AsyncEventSource events("/events");
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;


uint8_t MSGCAN[3];
short temp;
unsigned char vel;
unsigned short rpm;

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

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

String processor(const String& var){
  if (Serial2.available() > 0) {
    getData();
  }
  if(var == "OIL TEMPERATURE"){
    return String(temp);
  }
  else if(var == "RPM"){
    return String(rpm);
  }
  else if(var == "VELOCITY"){
    return String(vel);
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>OBD2 reader Web Server</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> OIL TEMPERATURE</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-tint" style="color:#00add6;"></i> RPM</p><p><span class="reading"><span id="rpm">%RPM%</span></span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-angle-double-down" style="color:#e1e437;"></i> VELOCITY</p><p><span class="reading"><span id="vrl">%VELOCITY%</span> km/h</span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');

 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);

 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);

 source.addEventListener('temp', function(e) {
  console.log("temp", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);

 source.addEventListener('rpm', function(e) {
  console.log("rpm", e.data);
  document.getElementById("rpm").innerHTML = e.data;
 }, false);

 source.addEventListener('velocity', function(e) {
  console.log("velocity", e.data);
  document.getElementById("vel").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";

void setup() {

  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  initWiFi();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (Serial2.available() > 0) {
      getData();
    }
    Serial.printf("Oil Temperature = %d ºC \n", temp);
    Serial.printf("RPM = %d \n", rpm);
    Serial.printf("Velocity = %d Km/h \n", vel);
    Serial.println();

    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    events.send(String(temp).c_str(),"oil temperature",millis());
    events.send(String(rpm).c_str(),"rpm",millis());
    events.send(String(vel).c_str(),"velocity",millis());

    lastTime = millis();
  }
}

#include <ESP8266WiFi.h>          // ESP32 pakai <WiFi.h>
#include <ESP8266WebServer.h>     // ESP32 pakai <WebServer.h>

ESP8266WebServer server(80);

// Pin motor
const int IN1 = 12;
const int IN2 = 13;
const int IN3 = 14;
const int IN4 = 15;

// Wi-Fi Hotspot
const char* ssid = "MobilWiFi";
const char* password = "12345678";

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopMotor();

  WiFi.softAP(ssid, password);
  Serial.print("IP Address: "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/move", handleMove);

  server.begin();
}

void loop() {
  server.handleClient();
}

// Motor functions
void moveForward()  { digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);}
void moveBackward() { digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);}
void turnLeft()     { digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);}
void turnRight()    { digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);}
void stopMotor()    { digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);}

// Handle motor commands
void handleMove() {
  String dir = server.arg("dir");
  String action = server.arg("action"); // start / stop

  if(action == "start"){
    if(dir=="forward") moveForward();
    else if(dir=="backward") moveBackward();
    else if(dir=="left") turnLeft();
    else if(dir=="right") turnRight();
  } else {
    stopMotor();
  }
  server.send(200,"text/plain","OK");
}

// Halaman web joystick
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Mobil Remote</title>
<style>
body { font-family: Arial; text-align:center; background:#eef2f5; margin:0; padding:0; }
h2 { margin-top:20px; color:#333; }
.joystick { display:grid; grid-template-columns:100px 100px 100px; grid-gap:15px; justify-content:center; margin-top:20px; }
button { width:100px; height:100px; font-size:28px; border-radius:16px; border:none; background:#28a745; color:#fff; box-shadow:0 5px #1e7e34; cursor:pointer; user-select:none; }
button:active { background:#218838; box-shadow:0 2px #1e7e34; transform:translateY(3px);}
@media(max-width:400px){ .joystick { grid-template-columns:70px 70px 70px;} button{width:70px;height:70px;font-size:20px;} }
</style>
</head>
<body>
<h2>Kontrol Mobil Remote</h2>
<div class="joystick">
  <div></div>
  <button id="up">↑</button>
  <div></div>
  <button id="left">←</button>
  <button id="stop">■</button>
  <button id="right">→</button>
  <div></div>
  <button id="down">↓</button>
  <div></div>
</div>

<script>
const mapping = { up:'forward', down:'backward', left:'left', right:'right', stop:'stop' };

// Bisa menekan 2 tombol bersamaan
function sendCmd(dir, action){
  fetch('/move?dir='+dir+'&action='+action);
}

// Bind tombol touch & hold
['up','down','left','right','stop'].forEach(id=>{
  const btn = document.getElementById(id);
  btn.addEventListener('mousedown', ()=>sendCmd(mapping[id],'start'));
  btn.addEventListener('mouseup', ()=>sendCmd(mapping[id],'stop'));
  btn.addEventListener('touchstart', ()=>sendCmd(mapping[id],'start'));
  btn.addEventListener('touchend', ()=>sendCmd(mapping[id],'stop'));
});
</script>
</body>
</html>
)rawliteral";

  server.send(200,"text/html",html);
}

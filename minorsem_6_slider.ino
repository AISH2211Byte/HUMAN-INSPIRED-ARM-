#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_Hand";
const char* password = "12345678";

WebServer server(80);

// Servo pins
int servoPins[7] = {13,12,14,27,26,25,33};

// PWM channels
int channels[7] = {0,1,2,3,4,5,6};

// Convert angle to duty
int angleToDuty(int angle) {
  return map(angle, 0, 180, 1638, 8192);
}

// ================= WEBPAGE =================
void handleRoot() {

String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">


<style>
body {
  margin: 0;
  font-family: Arial;
  color: white;
  text-align: center;

  /* BLACK BACKGROUND */
  background-color: black;
}

.slider {
  width: 80%;
}

/* makes text look better */
p, h2 {
  text-shadow: 2px 2px 5px black;
}
</style>

</head>

<body>

<h2> Servo Control Panel</h2>

<script>
function send(s,v){
  fetch(`/set?s=${s}&v=${v}`);
  document.getElementById("val"+s).innerText = v;
}
</script>
)rawliteral";

String colors[7] = {"red","orange","yellow","green","cyan","blue","violet"};
String names[7] = {"Servo 1","Servo 2","Servo 3","Servo 4","Servo 5","Servo 6","Servo 7"};

for(int i=0;i<7;i++){
  html += "<p>"+names[i]+": <span id='val"+String(i)+"'>90</span></p>";

  html += "<input type='range' min='0' max='180' value='90' ";
  html += "oninput='send("+String(i)+",this.value)' ";
  html += "style='accent-color:"+colors[i]+";' class='slider'><br><br>";
}

html += "</body></html>";

server.send(200, "text/html", html);
}

// ================= CONTROL =================
void handleSet() {
  int s = server.arg("s").toInt();
  int v = server.arg("v").toInt();

  if(s >= 0 && s < 7){
    int duty = angleToDuty(v);
    ledcWrite(channels[s], duty);
  }

  server.send(200, "text/plain", "OK");
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());

  for(int i=0;i<7;i++){
    ledcSetup(channels[i], 50, 16);
    ledcAttachPin(servoPins[i], channels[i]);
    ledcWrite(channels[i], angleToDuty(90));
  }

  server.on("/", handleRoot);
  server.on("/set", handleSet);

  server.begin();
}

// ================= LOOP =================
void loop() {
  server.handleClient();
}
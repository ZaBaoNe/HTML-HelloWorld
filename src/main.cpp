// ESP32 Web Server: Hello World!
#include <WiFi.h>
#include<Arduino.h>
#include<ESPAsyncWebServer.h>
// Replace with your network credentials
const char* ssid = "ASUS_90";
const char* password = "harbor_2342";
const int ledPin =2;

bool blinkActive = false;
AsyncWebServer server(80);

const char index_html[]PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Web Server</title>
    <style>
        body { font-family: Times New Roman, sans-serif; text-align: center; margin-top: 50px; }
    </style>
</head>
<body>
    <h1>Hello, World!</h1>
    <p>This is an HTML page hosted on the ESP32-S3.</p>
    
   <marquee behavior="scroll" bgcolor="blue" loop="-1" width="30%">
   <i>
      <font color="white">
        COUNTING TIME:
      </font>
    </i>
    </marquee>

    <div id="counter" style = "font-size: 24px; font-weight: bold;
    color: blue; margin-top: 10px;">0</div>

<!-- 🔄 Toggle Counter Button -->
<button id="countBtn" onclick="toggleCounting()">Stop Counting</button>

    <p>Your IP Address: <span id="ip">Loading...</span></p>

<!-- ✅ Toggle Button -->
    <button id="toggleBtn" onclick="toggleBlink()">Start Blinking</button>

  <script>
    let count = 0;
    let isCounting = true;
    const counterElement = document.getElementById('counter');

    function updateCounter() {
      if (isCounting){
      count++;
      counterElement.textContent = count;
        }
      setTimeout(updateCounter, 1000); // Update every 1 second
    }

    updateCounter();

    //fetch ip from ESP32
    async function fetchIP() {
            const response = await fetch('/get-ip');
            const ip = await response.text();
            document.getElementById("ip").textContent = ip;
            }
    fetchIP();

    // 🔄 Toggle Counting
      function toggleCounting(){
        isCounting =!isCounting;
        document.getElementById("countBtn").textContent = isCounting ? "Stop Counting" : "Start Counting";
        }

    // ✅ Control LED Blinking
        let isBlinking = false;
        async function toggleBlink(){
            isBlinking = !isBlinking;
            document.getElementById("toggleBtn").textContent = isBlinking ? "Stop Blinking" : "Start Blinking";
            await fetch('/led?state=' + (isBlinking ? 'start' : 'stop'));
            }
  
  </script>

</body>
</html>
)rawliteral";

void setup(){
  Serial.begin(115200);
  delay(1000);

  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);

  Serial.print("Connecting to Wifi...");
  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());

  server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(200, "text/html",index_html);
  });

  server.on("/get-ip",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(200, "text/plain", WiFi.localIP().toString());
  });

  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("state")) {
        String state = request->getParam("state")->value();
        blinkActive = (state =="start");
        if(!blinkActive){
          digitalWrite(ledPin,LOW);
        }
        }
    request->send(200, "text/plain", "OK");
});

  server.begin();
  
}

void loop() {
  if (blinkActive) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}



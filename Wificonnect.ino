#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "txLogiMesh";
const char* password = "2017tx11";

IPAddress local_IP(192, 168, 50, 139);
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

#define IO_Pin 16

void handleOn() {
  digitalWrite(IO_Pin, HIGH);
  server.send(200, "text/plain", "\n ON");
}

void handleOff() {
  digitalWrite(IO_Pin, LOW);
  server.send(200, "text/plain", "\n OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(IO_Pin, OUTPUT);

    if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.on("/", [](){
  server.send(200, "text/plain", "Switch is Online");
});

  server.begin();
}

void loop() {
  server.handleClient();
}
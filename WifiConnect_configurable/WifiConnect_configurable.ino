#include <WiFi.h>
#include <WebServer.h>
#include <string.h>
#include "esp_partition.h"

#define led_pin 23

// ---------------------------------------------------------------------
// Config block read from a dedicated "usrcfg" flash partition (written
// separately by make_config.py + esptool, no recompile needed).
// If that partition has never been written (or is missing), the
// defaults below are used instead.
// ---------------------------------------------------------------------
struct DeviceConfig {
  char    magic[4];      // "CFG1" marks a valid, written config block
  char    ssid[32];
  char    password[64];
  uint8_t ip[4];
  uint8_t gateway[4];
  uint8_t subnet[4];
};

static const char* DEFAULT_SSID = "txLogiMesh";
static const char* DEFAULT_PASS = "2017tx11";
static IPAddress DEFAULT_IP(192, 168, 50, 139);
static IPAddress DEFAULT_GW(192, 168, 50, 1);
static IPAddress DEFAULT_SUBNET(255, 255, 255, 0);

#define IO_Pin 16
WebServer server(80);

DeviceConfig cfg;
bool haveConfig = false;

bool loadConfig() {
  const esp_partition_t* part = esp_partition_find_first(
      (esp_partition_type_t)0x01,   // data partition
      (esp_partition_subtype_t)0x40, // custom subtype, must match partitions.csv
      "usrcfg");
  if (!part) return false;
  if (esp_partition_read(part, 0, &cfg, sizeof(cfg)) != ESP_OK) return false;
  return memcmp(cfg.magic, "CFG1", 4) == 0;
}

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
  pinMode(led_pin, OUTPUT);


  const char* ssid;
  const char* password;
  IPAddress local_IP, gateway, subnet;

  haveConfig = loadConfig();
  if (haveConfig) {
    ssid     = cfg.ssid;
    password = cfg.password;
    local_IP = IPAddress(cfg.ip[0], cfg.ip[1], cfg.ip[2], cfg.ip[3]);
    gateway  = IPAddress(cfg.gateway[0], cfg.gateway[1], cfg.gateway[2], cfg.gateway[3]);
    subnet   = IPAddress(cfg.subnet[0], cfg.subnet[1], cfg.subnet[2], cfg.subnet[3]);
    Serial.println("Loaded configuration from flash.");
  } else {
    ssid     = DEFAULT_SSID;
    password = DEFAULT_PASS;
    local_IP = DEFAULT_IP;
    gateway  = DEFAULT_GW;
    subnet   = DEFAULT_SUBNET;
    Serial.println("No stored configuration found, using built-in defaults.");
  }

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(led_pin, HIGH);  
    delay(1000);                  
    digitalWrite(led_pin, LOW);  
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());
   digitalWrite(led_pin, HIGH); //to show that the wifi setup is completed

  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/", []() {
    server.send(200, "text/plain", "Switch is Online");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}

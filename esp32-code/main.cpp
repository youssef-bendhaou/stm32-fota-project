
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// --- CONFIGURATION ---
#define SSID_NAME "Redmi 13C"
#define SSID_PASS "youssef12"
#define MQTT_SERVER "test.mosquitto.org"
#define TOPIC_SUB "/FOTA/test"
#define TOPIC_PUB "/FOTA/status"

// --- PINS & BOOTLOADER ---
#define PIN_RX 18
#define PIN_TX 17
#define PIN_RST 4
#define ADDR_APP 0x08008000

// --- GLOBALS ---
String gitURL = "https://raw.githubusercontent.com/youssef-bendhaou/projetsem/main/ota0.0.bin";
WiFiClient espClient;
PubSubClient client(espClient);
bool doErase = false, doUpload = false;

// --- LOGGING (USB + MQTT) ---
void logM(String msg)
{
  Serial.println(msg);
  if (client.connected())
    client.publish(TOPIC_PUB, msg.c_str());
}

// --- OUTILS STM32 (CRC & Reset) ---
uint32_t calcCRC(const uint8_t *data, size_t len)
{
  uint32_t crc = 0xFFFFFFFF;
  for (size_t i = 0; i < len; i++)
  {
    crc ^= (uint32_t)data[i];
    for (int j = 0; j < 32; j++)
      crc = (crc & 0x80000000) ? (crc << 1) ^ 0x04C11DB7 : (crc << 1);
  }
  return crc;
}

void resetSTM32()
{
  logM(">>> Reset STM32...");
  digitalWrite(PIN_RST, LOW);
  delay(150);
  digitalWrite(PIN_RST, HIGH);
  delay(1000);
  while (Serial1.available())
    Serial1.read(); // Purge buffer
}

// --- COMMUNICATION UART ---
void sendPacket(uint8_t cmd, uint32_t addr, uint8_t *payload, uint8_t len)
{
  uint8_t buf[255];
  buf[0] = 1 + 4 + 1 + len + 4; // Length field
  buf[1] = cmd;
  memcpy(&buf[2], &addr, 4); // Address (Little Endian natif ESP32)
  buf[6] = len;
  if (len > 0)
    memcpy(&buf[7], payload, len);

  uint32_t crc = calcCRC(&buf[1], 1 + 4 + 1 + len);
  memcpy(&buf[7 + len], &crc, 4);

  Serial1.write(buf, buf[0] + 1);
}

bool waitACK(int timeout = 2000)
{
  uint32_t start = millis();
  while (millis() - start < timeout)
  {
    if (Serial1.available() >= 2)
    {
      uint8_t ack = Serial1.read();
      Serial1.read(); // Skip len byte
      delay(5);
      while (Serial1.available())
        Serial1.read(); // Purge rest
      return (ack == 0xCD);
    }
  }
  return false;
}

// --- TACHES FOTA ---
bool downloadBin()
{
  logM("Download: " + gitURL);
  WiFiClientSecure sClient;
  sClient.setInsecure();
  HTTPClient http;
  http.begin(sClient, gitURL);

  if (http.GET() == HTTP_CODE_OK)
  {
    File f = LittleFS.open("/update.bin", "w");
    http.writeToStream(&f);
    f.close();
    http.end();
    logM("Download OK");
    return true;
  }
  logM("Download Failed");
  http.end();
  return false;
}

void handleErase()
{
  resetSTM32();
  logM("Erasing Flash...");
  sendPacket(0x15, 0xFFFFFFFF, NULL, 0); // Erase Cmd
  if (waitACK(10000))
    logM("Erase Success");
  else
    logM("Erase Failed");
}

void handleUpload()
{
  if (!downloadBin())
    return;

  File f = LittleFS.open("/update.bin", "r");
  long total = f.size();
  logM("Flashing " + String(total) + " bytes...");

  resetSTM32();

  uint32_t addr = ADDR_APP;
  uint8_t buf[64];
  int pkts = 0;

  while (f.available())
  {
    int len = f.read(buf, 64);
    while (Serial1.available())
      Serial1.read();
    sendPacket(0x16, addr, buf, len); // Write Cmd

    if (!waitACK())
    {
      logM("Write Fail @ " + String(addr, HEX));
      f.close();
      return;
    }

    addr += len;
    if (++pkts % 20 == 0)
    {
      client.loop(); // Keep MQTT alive
      client.publish(TOPIC_PUB, String("Progress: " + String((pkts * 64 * 100) / total) + "%").c_str());
    }
  }
  f.close();

  logM("Jumping to App...");
  sendPacket(0x14, ADDR_APP, NULL, 0); // Jump Cmd
  if (waitACK())
    logM("Update FINISHED");
}

// --- SYSTEME ---
void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  String msg = "";
  for (unsigned int i = 0; i < len; i++)
    msg += (char)payload[i];
  Serial.println("MQTT: " + msg);

  if (msg == "erase")
    doErase = true;
  else if (msg.startsWith("http"))
  {
    gitURL = msg;
    doUpload = true;
  }
}

void setup()
{
  pinMode(PIN_RST, OUTPUT);
  digitalWrite(PIN_RST, HIGH);
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX);
  LittleFS.begin(true);

  WiFi.begin(SSID_NAME, SSID_PASS);
  while (WiFi.status() != WL_CONNECTED)
    delay(500);

  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(mqttCallback);
}

void loop()
{
  if (!client.connected())
  {
    if (client.connect(("ESP32-" + String(random(0xffff), HEX)).c_str()))
    {
      client.subscribe(TOPIC_SUB);
      logM("Ready");
    }
    else
      delay(2000);
  }
  client.loop();

  if (doErase)
  {
    doErase = false;
    handleErase();
  }
  if (doUpload)
  {
    doUpload = false;
    handleUpload();
  }
  delay(10);
}
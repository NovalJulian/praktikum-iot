#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

// ============================
//   KONFIGURASI SENSOR & PIN
// ============================
#define DHTPIN  D3          //
#define DHTTYPE DHT11

#define LED_SUHU D6         // Indikator suhu
#define LED_HUM  D7         // Indikator kelembapan

// ============================
//        IDENTITAS
// ============================
String NIM = "G.231.23.0037";
String topic_pub = "iot/" + NIM;
String clientID  = NIM;

// ============================
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "Nopal";
const char* password = "12345678";

const char* mqttServer = "x2.revolusi-it.com";
const char* mqttUser   = "usm";
const char* mqttPass   = "usmjaya25";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastSend = 0;
const long gmtOffset_sec = 7 * 3600;

// ============================
//   FUNGSI CENTER TEKS
// ============================
void printCentered(int row, String text) {
  int pos = (16 - text.length()) / 2;
  if (pos < 0) pos = 0;
  lcd.setCursor(pos, row);
  lcd.print(text);
}

// ============================
//     FUNGSI KEDIP LED
// ============================
void blinkLED(int pin, int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
  }
}

// ============================
//        MQTT RECONNECT
// ============================
void reconnectMQTT() {
  lcd.clear();
  printCentered(0, "MQTT");
  printCentered(1, "Connecting...");
  while (!client.connected()) {
    client.connect(clientID.c_str(), mqttUser, mqttPass);
    delay(1000);
  }
  lcd.clear();
  printCentered(0, "MQTT");
  printCentered(1, "Connected");
  delay(1500);
}

// ============================
//   CETAK WAKTU TENGAH
// ============================
void printCenteredTime() {
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);
  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
  lcd.setCursor(4, 1);
  lcd.print(buffer);
}

// ============================
//     OPENING LCD
// ============================
void openingLCD() {
  lcd.clear();
  printCentered(0, "THERMOMETER");
  printCentered(1, "DIGITAL");
  delay(2500);
}

// ============================
//             SETUP
// ============================
void setup() {
  Serial.begin(115200);

  pinMode(LED_SUHU, OUTPUT);
  pinMode(LED_HUM, OUTPUT);
  digitalWrite(LED_SUHU, LOW);
  digitalWrite(LED_HUM, LOW);

  Wire.begin(D2, D1);
  lcd.init();
  lcd.backlight();

  openingLCD();

  WiFi.begin(ssid, password);
  lcd.clear();
  printCentered(0, "WiFi");
  printCentered(1, "Connecting...");
  while (WiFi.status() != WL_CONNECTED) delay(500);

  lcd.clear();
  printCentered(0, "WiFi");
  printCentered(1, "Connected");
  delay(1500);

  client.setServer(mqttServer, 1883);
  reconnectMQTT();

  lcd.clear();
  printCentered(0, "Time");
  printCentered(1, "Sync...");
  configTime(gmtOffset_sec, 0, "pool.ntp.org", "time.nist.gov");
  while (time(nullptr) < 100000) delay(500);

  lcd.clear();
  printCentered(0, "Time");
  printCentered(1, "OK");
  delay(1500);

  dht.begin();
}

// ============================
//             LOOP
// ============================
void loop() {

  if (!client.connected()) reconnectMQTT();
  client.loop();

  if (millis() - lastSend > 2000) {
    lastSend = millis();

    float suhu = dht.readTemperature();
    float hum  = dht.readHumidity();
    if (isnan(suhu) || isnan(hum)) return;

    // MQTT
    String json = "{\"nim\":\"" + NIM + "\",\"suhu\":" + String(suhu,1) +
                  ",\"lembab\":" + String(hum,1) + "}";
    client.publish(topic_pub.c_str(), json.c_str());

    // HTTP POST
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient httpClient;
      HTTPClient http;
      http.begin(httpClient, "http://10.99.189.254/Nopal/api_post.php");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.POST("suhu=" + String(suhu) + "&kelembapan=" + String(hum));
      http.end();
    }

    // =========================
    //   LOGIKA LED SUHU (D6)
    // =========================
    digitalWrite(LED_SUHU, LOW);
    if (suhu > 29 && suhu < 30) blinkLED(LED_SUHU, 1);
    else if (suhu >= 30 && suhu <= 31) blinkLED(LED_SUHU, 2);
    else if (suhu > 31) blinkLED(LED_SUHU, 3);

    // =========================
    //   LOGIKA LED KELEMBAPAN (D7)
    // =========================
    digitalWrite(LED_HUM, LOW);
    if (hum >= 60 && hum < 70) blinkLED(LED_HUM, 1);
    else if (hum >= 70) blinkLED(LED_HUM, 3);

    // LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("T:%.1fC H:%.1f%%", suhu, hum);
    printCenteredTime();
  }
}

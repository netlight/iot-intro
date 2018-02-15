#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 512
#endif


#include <Arduino.h>

// ENM deps
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

// application deps
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#include <ArduinoJson.h>

// sensor defines
#define DHTPIN 5
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
#define INTERVAL 15000

const char* applicationUUID = "nl_iot_8-webupdate";

const char* tenantId = "1";
const char* deviceId = "ybmFtZSI6ImNlbyJ9.jZM5Ogdh9aWCBYth7e5m5N6SbAAdu3LstVRxG0a804J58n3Fb2O9M8I9U8Vt0XxO044T_p9R65-JdgiMqxWBe";
const char* tempType= "urn:olt:temperature";
const char* humType= "urn:olt:humidity";
const char* timestamp= "2017-11-24T14:10:12.318570486Z";

const char* ssid = "_";
const char* password = "043022626";
const char* mqtt_server = "api.int.oltd.de";
const char* TOPIC = "sensordata";

char tmp[75];
char hum[75];
char chipId[100];
char topic[100];
bool new_reading = false;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

void reconnect() {
 if (client.connected()) {
   return;
 }

 Serial.print("Attempting MQTT connection...");
 Serial.print(mqtt_server);

 // Create a random client ID
 String clientId = "ESP8266Client-";
 clientId += String(random(0xffff), HEX);

 // Attempt to connect
 if (client.connect(clientId.c_str())) {
   Serial.println("Connected to MQTT gateway");
   return;
 }

 Serial.print("failed, rc=");
 Serial.print(client.state());
}

void readSensor() {
 Serial.println("\nReading sensor...");
 // Reading temperature or humidity takes about 250 milliseconds!
 // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 float h = dht.readHumidity();

 // Read temperature as Celsius (the default)
 float t = dht.readTemperature();
 Serial.println("\n Read sensor completed...");
 // Read temperature as Fahrenheit (isFahrenheit = true)
 //float f = dht.readTemperature(true);

 // Check if any reads failed and exit early (to try again).
 if (isnan(h) || isnan(t)) {
   Serial.println("Failed to read from DHT sensor!");
   return;
 }

 dtostrf(h, 3, 1, hum);
 dtostrf(t, 3, 1, tmp);
 Serial.println("\nValue:");
 Serial.println(tmp);

 new_reading = true;
}

void transmit() {

 Serial.println("transmitting");


 StaticJsonBuffer<500> jsonBuffer;


 JsonArray& root = jsonBuffer.createObject().createNestedArray("");
 JsonObject& tempPayload = root.createNestedObject();

 tempPayload["time"] = timestamp;
 tempPayload["value"] = tmp;
 tempPayload["tenantId"] = tenantId;
 tempPayload["deviceId"] = deviceId;
 tempPayload["type"] = tempType;


 char JSONmessageBuffer[500];
 root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer)+1);

 Serial.print(topic);
 Serial.print(" ");
 Serial.println(sizeof(JSONmessageBuffer));



 if (!client.publish(topic, JSONmessageBuffer)){
     Serial.println("FAILED!!!!");
 }

 new_reading = false;
}


void setup() {
 Serial.begin(115200);
 Serial.println();

 // Get the unique chip ID
 itoa(ESP.getChipId(), chipId, 16);
 Serial.print("chip ID: ");
 Serial.println(chipId);

 // Set the per-device sensor topic
 snprintf(topic, 100, "%s", TOPIC);
 Serial.print("using the following topic to transmit readings: ");
 Serial.println(topic);

 pinMode(LED_BUILTIN, OUTPUT);
 pinMode(2, OUTPUT);
 digitalWrite(LED_BUILTIN, HIGH);
 digitalWrite(2, HIGH);

 WiFi.setAutoConnect(true);
 WiFi.setAutoReconnect(true);
 WiFi.hostname(applicationUUID);

 Serial.print("Connecting to gateway...");
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED)
 {
   delay(500);
   Serial.print(".");
 }
 Serial.println("\nConnected to gateway");

 // set up mqtt stuff
 client.setServer(mqtt_server, 1883);

 httpServer.on("/id", [](){
   httpServer.send(200, "text/plain", applicationUUID);
 });
 httpUpdater.setup(&httpServer);
 httpServer.begin();

 reconnect();

}

void loop() {
 if (WiFi.isConnected() == true) {
   digitalWrite(2, LOW);
 } else {
   // turn the LED off
   digitalWrite(2, HIGH);
 }

 if (new_reading == true) {
   // only try to reconnect when there's a new reading
   digitalWrite(LED_BUILTIN, LOW);
   delay(100);
   reconnect();
   digitalWrite(LED_BUILTIN, HIGH);
   transmit();
 }

 long now = millis();

 if (now - lastMsg > INTERVAL) {
   lastMsg = now;
   readSensor();
 }

 httpServer.handleClient();
}

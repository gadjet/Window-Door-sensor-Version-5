
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

/* ------------------------ Change Me --------------------------------------- */

const char*     ssid              = "xxxxxxxxx";
const char*     password          = "xxxxxxxxx";
const char*     mqtt_server       = "X.X.X.X";
const char*     mqtt_username     = "xxxxxxxxx";
const char*     mqtt_password     = "xxxxxxxxx";
bool            use_fahrenheit    = true;


/* ------------------------------------------------------------------ */

#ifdef DEBUG_FLAG
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

/*--------------------------ADDED IN----------------------------------*/
unsigned long previousMillis = 0;
unsigned long interval = 30000;


/* ------------------------ Messages --------------------------------------- */

#define MESH_ID               6734922

typedef struct struct_message {
  int mesh_id;
  int id;
  char state[7];
  int vBatt;
  float Temp;
} struct_message;

struct_message myData;


//struct_message  msg;
uint8_t         incomingData[sizeof(struct struct_message)];
size_t          received_msg_length;

/* --------------------------- Wifi/MQTT ---------------------------------------------- */

WiFiClient    wifiClient;
#define       MQTT_MSG_SIZE    200
char          mqttTopic[MQTT_MSG_SIZE];
#define       MSG_BUFFER_SIZE  (50)
PubSubClient  mqttClient(wifiClient);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

/* ############################ Setup ############################################ */

void setup() {

  Serial.begin(115200);
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

/*  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  while (WiFi.status() != WL_CONNECTED) {
    debug(".");
    delay(1000);
  }
  debug("IP address:\t");
  debugln(WiFi.localIP());*/

  mqttClient.setServer( mqtt_server, 1883 );
  String MAC = WiFi.macAddress(); 
  MAC.replace(":", "");

}

/* ############################ Loop ############################################# */

void loop() {

  wifiReconnect();            
  mqttReconnect();
  mqttClient.loop();

  if (Serial.available()) {
    received_msg_length = Serial.readBytesUntil('\n', incomingData, sizeof(incomingData));
    if (received_msg_length == sizeof(incomingData)) {  // got a msg from a sensor
      memcpy(&myData, incomingData, sizeof(myData));
      if ( myData.mesh_id == MESH_ID ) sensorMessageOverMQTT();
    }
  }
}


/* ############################ Sensors ############################################# */

void sensorMessageOverMQTT() {

Serial.println(myData.id);
String payload = "{";
payload += "\"id\":\"" + String(myData.id);
payload += "\",\"state\":\""+String(myData.state);
payload += "\",\"vBatt\":\""+String(myData.vBatt);
payload += "\",\"Temp\":\""+String(myData.Temp);
payload += "\"}";  
Serial.println(payload);

String topic = "ESPNow/board/";           
    topic += String(myData.id);

  
  mqttPublish(topic.c_str(),payload);                                      
}


/* ################################# MQTT ########################################### */

void mqttPublish(String topic, String message) {
  Serial.println("Publish");
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.publish(topic.c_str(), message.c_str());    
}


void mqttReconnect() {
  Serial.println("Waiting for ESP-NOW Message");
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP-NOW-Board-";
    clientId += String(myData.id);

    if (mqttClient.connect(clientId.c_str(),mqtt_username, mqtt_password)) {               
      Serial.println("connected");
  } else {
      Serial.print("failed to connect to mqtt server, rc = ");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying    
      delay(5000);
  }
 } 
}
/* ############################ ADDED THIS - SEE LOOP ############################################# */

void wifiReconnect()  {
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
  //  WiFi.config(localIP, gateway, subnet, primaryDNS); //////////////////////////////////////////////// added on 11/15
    WiFi.begin(ssid, password);
    Serial.println(WiFi.localIP());
    //Alternatively, you can restart your board
    //ESP.restart();  /////////////////////////////////////////////////////////////////////////////////added back in 11/15
    Serial.println(WiFi.RSSI());
    previousMillis = currentMillis;
  }
}
  

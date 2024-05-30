// Last updated: May 30 2:53 PM

// Dependencies
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Hardware Constants
#define RELAY D7
#define LED D8
#define BUTTON D6

// MQTT Configuration
const char* SSID = "DragonsDen";
const char* PASSWORD = "iotcup2024fusrodah";
const char* MQTT_SERVER = "192.168.0.7";
const int MAIN_PORT = 1889;
const String serverPing = "https://cs145-firewall.vercel.app/api/ping";
const String serverActions = "https://cs145-firewall.vercel.app/api/actions";
const String serverConnect = "https://cs145-firewall.vercel.app/api/connect";
String serverStatus = "https://cs145-firewall.vercel.app/api/status?m=34:38:3a:45:37:3a&s=";
const int httpsPort = 443; 

// MQTT Topics
#define OUTTOPIC "cs145/Sinigang/socket/out"            // This will be used for publishing to our own
#define INTOPIC "cs145/Sinigang/server/#"
#define OUTTOPIC_SERVER "cs145/Sinigang/server/out"     // This will be used for publishing to the server

//Wi-Fi Variables
WiFiClient espClient;
PubSubClient client(espClient);

// Payload to send out
const char* SOCKET_ON = "100001";
const char* SOCKET_OFF = "100000";

// Global condition variables
int state = 1;
String inmessage_string = "------";
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long lastPressTime = 0;
unsigned long pressDelay = 2000;
unsigned long holdStart = 0;
unsigned long holdThreshold = 3000;
bool buttonSwitched = false;
bool lastButtonState = 1;
unsigned long lastPollTime = 0;
unsigned long pollDelay = 30000;
bool isConnected = false;
String MAC = "";

void setup(){
  // Setup
  Serial.begin(115200);

  // Setup Relay Module
  pinMode(RELAY, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  digitalWrite(LED, 0);
  digitalWrite(RELAY, HIGH); // Initialize relay to on

  // wifi setup for MQTT and server
  setup_wifi();
	client.setServer(MQTT_SERVER, MAIN_PORT);
	client.setCallback(callback);

  // Check for ping
  String payload = getData(serverPing);
  Serial.println(payload);

  // Check if already connected
  payload = getData(serverStatus + (String) state);
  Serial.println(payload);
  
  for (int i = 0; i < 6; ++i) {
    MAC += String(WiFi.macAddress()[i], 16);
    if (i < 5)
      MAC += ':';
  }
}

void loop(){
  String payload;

  if (!client.connected()){
    reconnect();
  }
  client.loop();

  // Polling implementation
  if(millis() - lastPollTime > pollDelay && isConnected){
    digitalWrite(LED, LOW);
    client.publish(OUTTOPIC, "POLLING");
    lastPollTime = millis();

    String server = serverStatus + (String) state;
    payload = getData(server);
    Serial.print("Payload from polling: ");
    Serial.println(payload);
    
    client.publish(OUTTOPIC, payload.c_str());
    Serial.println(payload);
    digitalWrite(LED, HIGH);
    client.publish(OUTTOPIC, "END OF POLLING");
  }
  
  // Button press implementation
  int reading = digitalRead(BUTTON);
  buttonSwitched = false;
	if (reading != lastButtonState){
    lastButtonState = reading;
    buttonSwitched = true;
    if(reading == 0) holdStart = millis();
    //if(reading == LOW) lastDebounceTime = millis();
	}
  // READING = 0 WHEN HIGH
  int debouncedReading = (reading == 1 && buttonSwitched);

  // Sending MAC Address to server. Long press of 3s or more
  // if(debouncedReading == 1) Serial.println(debouncedReading);
  if(debouncedReading == 1 && millis() - holdStart > holdThreshold){
    // send out stuff
    Serial.println("Sending MAC now");
    client.publish(OUTTOPIC_SERVER, MAC.c_str());

    String M = "{\"mac\" : \"" + MAC + "\"}";
    bool success = sendToServer(M, serverConnect, 1);
    if(success) {
      isConnected = true;
      digitalWrite(LED, LOW);
      delay(250);
      digitalWrite(LED, HIGH);
      delay(250);
      digitalWrite(LED, LOW);
      delay(250);
      digitalWrite(LED, HIGH);
      delay(250);
      digitalWrite(LED, LOW);
      delay(250);
      digitalWrite(LED, HIGH);
      delay(250);
    }

    if(isConnected)
      client.publish(OUTTOPIC, "Connected");
  }

  // Turn off switch either by smoke or user
  else if((inmessage_string == "011001" || payload == "OFF" || (state == 1 && debouncedReading == 1)) && (millis() - lastPressTime > pressDelay)){
    // trigger = 0 if smoke detected, else anything else
    int trigger = 2;
    if(inmessage_string == "011001") trigger = 0;
    else if(payload == "OFF") trigger = 1;
    
    // Sending a signal to other hardware and server
    Serial.println("Turning off now");
    digitalWrite(RELAY, LOW);   // Turn off relay
    state = 0;
    client.publish(OUTTOPIC_SERVER, SOCKET_OFF); //send to smoke detector
    if(trigger != 1){
      String content = makeActionContent(trigger); 
      bool _ = sendToServer(content, serverActions, 0); // send to server
    }

    lastPressTime = millis();
    inmessage_string = "------";
  }

  // Turn on switch through user or button press.
  else if(((state == 0 && debouncedReading == 1) || payload == "ON" ) && (millis() - lastPressTime > pressDelay)){
    int trigger = 0;
    if(payload == "ON") trigger = 1;

    Serial.println("Turning on now");
    digitalWrite(RELAY, HIGH);  // Turn on relay
    state = 1;
    client.publish(OUTTOPIC_SERVER, SOCKET_ON); // send to smoke detector

    if(trigger == 0){
      String content = makeActionContent(0);
      bool _ = sendToServer(content, serverActions, 0); // send to server
    }

    lastPressTime = millis();
    inmessage_string = "------";
  }
}

String makeActionContent(int trigger){
  return "{\"mac\" : \"" + MAC + "\", \"trigger\" : \"" + (String) trigger + "\", \"state\" : \"" + (String) state + "\"}";
}

void setup_wifi() {
  //Wi-Fi Setup. Copied from sketch template.

  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Reconnect Function. Code was copy pasted from sketch template.
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(OUTTOPIC, "connected");
      // ... and resubscribe
      client.subscribe(INTOPIC);
      digitalWrite(LED, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      digitalWrite(LED, 0);
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  // Reader Function. Modified code from callback function from sketch template.

  inmessage_string = "------";
  for(int i = 0; i < length; i++){
    inmessage_string[i] = (char)payload[i];
  }

  Serial.println(inmessage_string);
}

bool sendToServer(String content, String serverName, int type){
  if(WiFi.status()== WL_CONNECTED){
    WiFiClientSecure client;
    HTTPClient http;

    int httpResponseCode;

    digitalWrite(LED, 0);
    client.setInsecure(); //the magic line, use with caution
    client.connect(serverName, httpsPort);

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    if(type == 0){
      http.addHeader("Content-Type", "application/json");
      httpResponseCode = http.POST(content);
    }

    else{
      http.addHeader("Content-Type", "application/text");
      httpResponseCode = http.POST(content);
    }

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String payload = http.getString();   //Get the request response payload
    Serial.println(payload);  
      
    // Free resources
    http.end();
    digitalWrite(LED, 1);
    if(httpResponseCode == 200) return true;
  }
  return false;
}

String getData(String serverName){
  if(WiFi.status()== WL_CONNECTED){
    WiFiClientSecure client;
    HTTPClient http;

    client.setInsecure(); //the magic line, use with caution
    client.connect(serverName, httpsPort);

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    int httpCode = http.GET();     //Send the request     
    Serial.println(httpCode)     ;                       
    if (httpCode == 200) { //Check the returning code
      Serial.println("Getting info");
      String payload = http.getString();   //Get the request response payload
      return payload;
    } 

    else if(httpCode == 204){
      Serial.println("Device and server is connected");
      String payload = http.getString();   //Get the request response payload
      isConnected = true;
      return payload;
      
    }
    
    // Free resources
    http.end();
  }

  return "";
}
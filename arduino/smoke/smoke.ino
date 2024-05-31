// Last updated: May 31 3:52 PM

//Dependencies
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Hardware Constants
#define BUZZER D6
#define SENSOR A0
#define LED     D8
#define MQT  D9
#define LOAD_RES 10    
#define AIR_FACTOR 9.83  
                                                    
// Smoke sensor Global variables         
const float SMOKECURVE[3] = {3.1,1.65,-0.2};
const int THRESHOLD = 200;                                                                              
float Res = 0;

// MQTT Configuration
const char* SSID = "DragonsDen";
const char* PASSWORD = "iotcup2024fusrodah";
const char* MQTT_SERVER = "192.168.0.7";
const int MAIN_PORT = 1889;

// MQTT Topics
#define OUTTOPIC "cs145/Sinigang/smoke/out"           // This will be used for publishing to our own
#define INTOPIC "cs145/Sinigang/server/#"
#define OUTTOPIC_SERVER "cs145/Sinigang/server/out"   // This will be used for publishing to the server

//Wi-Fi Variables
WiFiClient espClient;
PubSubClient client(espClient);

// Payload to send out
const char* SMOKE_DETECTED = "011001";
const char* NO_SMOKE_DETECTED = "011000";

unsigned long buzzerStart = 0;
int buzzLength = 5000;
int maxSound = 1000;
bool first = true;

// Global condition variables
int outmessage_sent = 0;
String inmessage_string = "------";

void setup(){   
  // Setup
  Serial.begin(115200);

  // Setup BUZZER and LED
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(MQT, OUTPUT);
  digitalWrite(BUZZER, 0);
  digitalWrite(LED, 0);
  digitalWrite(MQT, 0);

  // Calibrate smoke detection unit
  Serial.println();
  Serial.print("Calibrating.....");
  Res = SensorCalibration();
  printCalibration(Res);

  // wifi setup
  setup_wifi();
	client.setServer(MQTT_SERVER, MAIN_PORT);
	client.setCallback(callback);
}

void loop(){ 

  if (!client.connected()){
    reconnect();
  }
  client.loop();

  if(first){
    client.publish(OUTTOPIC, "FINISHED CALIBRATING");
    first = false;
  }
  
  Serial.println();
  float result = checkForSmoke();
  // int result = analogRead(SENSOR);
  // client.publish(OUTTOPIC, "");
  Serial.print("result");
  Serial.println(result);
  String r = (String) result;
  client.publish(OUTTOPIC, r.c_str());

  // Message not yet sent and Smoke detected
  if(outmessage_sent == 0 && result > THRESHOLD){ // FOR SMOKE SENSOR
  // if(outmessage_sent == 0 && result < 512){ // For Light SENSOR
    digitalWrite(LED, 1);
    tone(BUZZER, maxSound);
    buzzerStart = millis();
    outmessage_sent = 1;

    for(int i = 0; i < 4; i++){
      Serial.println("Publishing");
      client.publish(OUTTOPIC_SERVER, SMOKE_DETECTED);
      client.publish(OUTTOPIC, "SMOKE DETECTED");
      delay(3000);
    }
  }

  // Smoke is below threshold
  else if(result <= THRESHOLD){
    outmessage_sent = 0;
    client.publish(OUTTOPIC, "NO SMOKE DETECTED");
  }

  // Smoke is detected and message already sent
  else{
    digitalWrite(LED, 1);
    tone(BUZZER, maxSound);
    buzzerStart = millis();
  }


  if(millis() - buzzLength > buzzerStart){
    tone(BUZZER, 0);
  }
}

float checkForSmoke(){
  float res = resistance(6,50);
  res /= Res;
  float result = pow(10,(((log(res)-SMOKECURVE[1])/SMOKECURVE[2]) + SMOKECURVE[0]));
  return result;
}

void printCalibration(float Res){
  Serial.print("Calibration done.");
  Serial.print("Res=");
  Serial.print(Res);
  Serial.print("kohm");
  delay(2000);
}

void printSmokePPM(float result){
  Serial.print("SMOKE:"); 
  Serial.print(result);
  Serial.println( " ppm        ");
}

float SensorCalibration(){
  int i;
  float val=0;    
  val=resistance(100,500);                
  val = val/AIR_FACTOR;  
  return val; 
}

float resistance(int samples, int interval){
   int i;
   float res=0; 
   for (i=0;i<samples;i++) 
   {
      int adc_value=analogRead(SENSOR);
      Serial.println(adc_value);
      res+=((float)LOAD_RES*(1023-adc_value)/adc_value);
      delay(interval);
   }
   res/=samples;
   return res;
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
      client.publish(OUTTOPIC, "sensor is connected to mqtt");
      // ... and resubscribe
      client.subscribe(INTOPIC);
      digitalWrite(MQT, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      digitalWrite(MQT, 0);
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

  if(inmessage_string == "100001"){
    digitalWrite(BUZZER, 0);
    digitalWrite(LED, 0);
  }
}
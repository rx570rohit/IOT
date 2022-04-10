#include <ssl_client.h>
#include <ESP32HTTPUpdateServer.h>
#include <EspMQTTClient.h>
#include <ssl_client.h>

#include <WiFiServer.h>
#include <WiFi.h> 
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
//---- WiFi settings
String myString;
char rdata; // received charactors

String S1, S2,S3;
const char* ssid = "moto";
const char* password = "12345678";
//---- MQTT Broker settings
const char* mqtt_server = "115c079051424aaeb34ce9d86c06fd52.s1.eu.hivemq.cloud"; // replace with your broker url
const char* mqtt_username = "rx570";
const char* mqtt_password = "Rx570soni";
const int mqtt_port =8883;
#define ADC_VREF_mV  3300.0 // in millivolt
#define ADC_RESOLUTION 4096.0
#define PIN_LM35       36
static const uint8_t lpg = 35;
static const uint8_t ldr = 39;
static const uint8_t led  = 16;
static const uint8_t fan  = 25;
int data=0;
static const uint8_t ir = 14;
static const uint8_t lck  = 12;
static const uint8_t light  = 27;
String flamed="";
String locke="";
WiFiClientSecure espClient;    
//WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];


String sensor1 = "";
String sensor2 = "";
String sensor3 = "";
String sensor4 = "";
String sensor5 = "";
int command1 =0;

const char* sensor1_topic= "sensor1";
const char*  sensor2_topic="sensor2";
const char*  sensor3_topic="sensor3";
const char*  sensor4_topic="sensor4";
const char*  sensor5_topic="sensor5";
const char* command1_topic="command1";

const char* root_ca = \ 
"-----BEGIN CERTIFICATE-----\n" \ 
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \ 
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \ 
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \ 
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \ 
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \ 
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \ 
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \ 
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \ 
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \ 
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \ 
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \ 
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \ 
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \ 
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \ 
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \ 
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \ 
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \ 
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \ 
"-----END CERTIFICATE-----\n" ;

void setup_wifi() {
  delay(10); 
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED)
    {delay(500);
     
    Serial.print(".,.");
  }
  
  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());

}
void reconnect() {
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";   
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");

      client.subscribe(command1_topic);  
      //client.subscribe(command2_topic);   
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds"); 
      delay(5000);
    }
  }
}
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);
  setup_wifi();
  espClient.setCACert(root_ca);
  pinMode(ldr,INPUT);
     pinMode(ir,INPUT);
  pinMode(lck, INPUT);
  pinMode(lpg,INPUT);
   pinMode(fan,OUTPUT);
    pinMode(light,OUTPUT);
   

  //#ifdef ESP8266
   // espClient.setInsecure();
  //#else   
  //#endif
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
 
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}  
void loop() {

  if (!client.connected()) reconnect();
  client.loop();
  int adcVal = analogRead(PIN_LM35);
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
  float tempC = milliVolt / 10;
  sensor3  = tempC;
  int ldrv= analogRead(ldr);
  
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;  
    if(ldrv<500){
      sensor1="lights_ON";
      digitalWrite(light,HIGH);
      }
      else
      {
        sensor1="lights_OFF";
        digitalWrite(light,LOW);}
      
      
    data = analogRead(lpg);
    Serial.print("Gas Level: ");
    Serial.println(data);
    if(data >2000){sensor2="lpg_lekage";
   
   digitalWrite(fan,HIGH);}
    else{sensor2="No_lekage";
  digitalWrite(fan,LOW); }
  int irv= analogRead(ir);
Serial.println(irv);
  if(analogRead(ir))
  {
    flamed="no fire";
    }
    else
    {
     flamed="alert fire";
     }
      if(digitalRead(lck)==LOW)
  {
    locke=" Door Closed";
    }
    else
    {
     locke="Door Open";
     }
  
    publishMessage(sensor1_topic,String(sensor1),15,true);    
    publishMessage(sensor2_topic,String(sensor2),15,true);
    publishMessage(sensor3_topic,String(sensor3)+" Deg C",8,false);
    publishMessage(sensor4_topic,flamed,10,true);
    publishMessage(sensor5_topic,locke,12,true);
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
  
    if( strcmp(topic,command1_topic) == 0){
     if (incommingMessage.equals("1")) digitalWrite(light, LOW);   // Turn the LED on 
     else digitalWrite(light, HIGH);  // Turn the LED off 
  }

}
void publishMessage(const char* topic, String payload ,int no ,boolean retained){
  client.publish(topic, (byte*) payload.c_str(),no, retained);
  Serial.println("Message publised ["+String(topic)+"]: "+payload);
  payload="";
}

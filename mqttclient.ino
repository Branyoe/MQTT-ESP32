//ESP32 MQTT Client

//Librarys
#include <WiFi.h>
#include <Adafruit_MQTT_Client.h>
#include <DHTesp.h>

//Constants
//*Wifi
// const char* SSID = "Megacable_WmUj3k7";
// const char* PASSWORD = "TejznyTBXRXT3XGX78";
const char* SSID = "POCO";
const char* PASSWORD = "sexoseguro";

//*MQTT
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
  
//*Topics
const char* TEMPERATURE_TOPIC_DIRECTION = "RE4g/temp";
const char* HUMIDITY_TOPIC_DIRECTION = "RE4g/hum";

//*Pins
const int pinDHT = 15;
const int pinWifi = 23;
const int pinMqtt = 4;
const int pinPublish = 2;

//Instances
WiFiClient client; //Wifi Client
Adafruit_MQTT_Client mqtt(&client, mqtt_server, mqtt_port); //MQTT Client
DHTesp dht; //DHT11
Adafruit_MQTT_Publish tempTopic = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_TOPIC_DIRECTION);//MQTT Publish temperature
Adafruit_MQTT_Publish humTopic = Adafruit_MQTT_Publish(&mqtt, HUMIDITY_TOPIC_DIRECTION);//MQTT Publish humidity

//Functions
void setup() { // the setup function runs once when you press reset or power the board
  Serial.begin(115200); //initialize serial communication
  dht.setup(pinDHT, DHTesp::DHT11); //initialize DHT11
  
  //*pins setup
  pinMode(pinWifi, OUTPUT); //wifi indicator
  pinMode(pinMqtt, OUTPUT); //Mqtt indicator
  pinMode(pinPublish, OUTPUT); //Publish indicator

  Serial.println("");
  Serial.println("");
  Serial.println("************Iniciando************");
  Serial.println("");
  Serial.println("");
}

void loop() { // the loop function runs over and over again forever
  wifiConnect(); //connect and reconnect wifi
  mqttConect(); //connect and reconnect mqtt

  TempAndHumidity measure = dht.getTempAndHumidity(); //get temperature and humidity

  //*temperature management
  if(isnan(measure.temperature)) { //check if temperature is valid
    Serial.println("Error en la lectura de temperatura");
  }else{
    if(tempTopic.publish(measure.temperature)){ //check if temperature is published
      Serial.print("Temperatura: ");
      Serial.println(measure.temperature);
      digitalWrite(pinPublish, HIGH);
      delay(200);
      digitalWrite(pinPublish,LOW);
      delay(200);
    }else{
      Serial.println("Error en la publicación de temperatura");
    }
  }

  //*humidity management
  if(isnan(measure.humidity)){ //check if humidity is valid
    Serial.println("Error en la lectura de humedad");
  }else{
    if(humTopic.publish(measure.humidity)){ //check if humidity is published
      Serial.print("Humedad: ");
      Serial.println(measure.humidity);
      digitalWrite(pinPublish, HIGH);
      delay(200);
      digitalWrite(pinPublish,LOW);
    }else{
      Serial.println("Error en la publicación de humedad");
    }
  }

  Serial.println();
  delay(2400);
}

void wifiConnect() { //connect and reconnect wifi
  if (WiFi.status() == WL_CONNECTED) { //check if wifi is connected
    return;
  }
  Serial.println();
  Serial.print("------conectando a ");
  Serial.print(SSID);
  Serial.println("------");

  delay(1000);

  WiFi.begin(SSID, PASSWORD); //connect to wifi
  delay(2000);

  while (WiFi.status() != WL_CONNECTED) { //wait for wifi connection
    digitalWrite(pinWifi, LOW);
    Serial.print(".");
    delay(500);
    digitalWrite(pinWifi, HIGH);
    delay(500);
  }

  Serial.println();
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void mqttConect() { //connect and reconnect mqtt
  digitalWrite(pinMqtt, HIGH);

  if (mqtt.connected()) { //check if mqtt is connected
    return;
  }
  int8_t ret;
  Serial.println();
  Serial.print("------Conectando a ");
  Serial.print(mqtt_server);
  Serial.print(" on port ");
  Serial.print(mqtt_port);
  Serial.println("------");

  while ((ret = mqtt.connect()) != 0) {  // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret)); //otherwise return a error code
    Serial.println("Reintentado conexión en 5s...");
    mqtt.disconnect(); // wait 5 seconds
    digitalWrite(pinMqtt, LOW);
    delay(800);
    digitalWrite(pinMqtt, HIGH);
    delay(800);
    digitalWrite(pinMqtt, LOW);
    delay(800);
    digitalWrite(pinMqtt, HIGH);
    delay(800);
    digitalWrite(pinMqtt, LOW);
    delay(800);
    digitalWrite(pinMqtt, HIGH);
    delay(800);
  }

  Serial.println("MQTT Connected!");
  Serial.print("Temperatura topic: ");
  Serial.println(TEMPERATURE_TOPIC_DIRECTION);
  Serial.print("Humedad topic: ");
  Serial.println(HUMIDITY_TOPIC_DIRECTION);
  Serial.println();
}
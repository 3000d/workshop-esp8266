/**
   Code d'exemple permettant de publier des données sur adafruit.io via MQTT
*/
#include "settings.h"
// Bibliothèques
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define LED_PIN 14

// définir le client pour la connexion au wifi
WiFiClient client;

// définir le client MQTT auquel on passe la connection Wifi, l'url du serveur, le port et les paramètres de connexion
Adafruit_MQTT_Client mqtt(&client, IO_SERVER, IO_SERVERPORT, IO_USERNAME, IO_KEY);

// définition du feed sur lequel on va publier les données
Adafruit_MQTT_Publish test = Adafruit_MQTT_Publish(&mqtt,  IO_USERNAME "/feeds/workshop-trakk.test");

// définitioin du feed auquel on va souscrire pour recevoir les données
Adafruit_MQTT_Subscribe led = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/workshop-trakk.led");

void setup() {
  Serial.begin(115200);
  Serial.println("exemple mqtt");

  pinMode(LED_PIN, OUTPUT);

  Serial.print("Connexion à ");
  Serial.println(WLAN_SSID);
  // connexion au Wifi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connecté !");


  mqtt.subscribe(&led);
  // connexion à adafruit.io

  connect();

  // alimente le générateur de nombres (pseudo) aléatoires
 randomSeed(analogRead(0));
}

void loop() {

  
  if(! mqtt.ping(3)){
    if(! mqtt.connected()){
      connect();
    }
  }
  
  float value = random(1000);

  // publication des données vers le feed "test"
  if(! test.publish(value)){
    Serial.println("échec de la publication des données");
  }else{
    Serial.println("données publiées");
  }

  // tentative de lecture d'un packet entrant
  Adafruit_MQTT_Subscribe *subscription;
  while(subscription = mqtt.readSubscription(5000)){
    if(subscription == &led){ // si le feed correspond
      // conversion du message reçu dans un format utilisable via arduino
      char *value = (char *)led.lastread;
      String message = String(value);
      message.trim();
      Serial.print("message reçu : ");
      Serial.println(message);

      // en fonction du message reçu action sur la Led
      if(message == "ON") digitalWrite(LED_PIN, HIGH);
      if(message == "OFF") digitalWrite(LED_PIN, LOW);
    }
  }
  

  delay(5000);
}


void connect() {
  Serial.println("connexion à adafruit.io");
  int8_t retour;

  while ((retour = mqtt.connect()) != 0) {
    switch (retour) {
      case 1: Serial.println(F("protocole incorrect")); break;
      case 2: Serial.println(F("id rejeté")); break;
      case 3: Serial.println(F("serveur indisponible")); break;
      case 4: Serial.println(F("Mauvais user/pass")); break;
      case 5: Serial.println(F("Non authentifié")); break;
      case 6: Serial.println(F("impossible de souscrire")); break;
      default: Serial.println(F("échec de connexion")); break;
    }
    if(retour >= 0){
      mqtt.disconnect();
    }
    Serial.println("nouvelle tentative de connexion");
    delay(5000);
      
  }

  Serial.println("connecté à  adafriut.io");
  
}

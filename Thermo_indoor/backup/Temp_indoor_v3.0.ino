#include <SimpleDHT.h>
//#include <LiquidCrystal.h>
#include <ESP8266WiFi.h> 
//#include <SPI.h>
#include <PubSubClient.h> //Librairie pour la gestion Mqtt
#include <ArduinoOTA.h>


// DEBUT SECTION DECLARATION - CONNEXION
// DECLARATION CONNEXION I - WiFi

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// WiFi paramètre
char ssid[] = "Livebox-1F90";    // your SSID
char pass[] = "o3jwTuDzadcmQAtZ2r";       // your SSID Password

// MQTT Broker  
#define MQTT_BROKER       "192.168.1.61"
#define MQTT_BROKER_PORT  1883
#define MQTT_USERNAME     "frdev66"
#define MQTT_KEY          "Lenems66!!"

WiFiClient espClient;            // Use this for WiFi instead of EthernetClient
PubSubClient client(espClient);

// FIN SECTION DECLARATION - CONNEXION

// DEBUT SECTION DECLARATION - MESURES
// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 13;
SimpleDHT11 dht11(pinDHT11);

// FIN SECTION DECLARATION - MESURES

// DEBUT SECTION DECLARATION - ECRAN LCD
// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// FIN SECTION DECLARATION - ECRAN LCD 

// DEBUT SECTION DECLARATION - CONSTANTE
// Declaration Const --> Bouton
//nt buttonpin = 2;

// Declaration variables
unsigned long tempoMesure = 0;

unsigned long previousMillis;

// FIN SECTION DECLARATION - CONSTANTE

// EXEMPLE REQUETE SQL
// Sample query
//char INSERT_SQL[] = "INSERT INTO station_meteo.temperature (id_mesure_temp, date_mesure, mesure) VALUES (NULL, current_timestamp(), '15.8');";


void setup() {
  Serial.begin(115200);

  // DEBUT SECTION SETUP - CONNEXION
  Serial.println();

  WiFi.begin(ssid,pass);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // End WiFi section

  // FIN SECTION SETUP - CONNEXION

  // DEBUT SECTION - MQTT
  setup_mqtt();
  client.publish("esp/init", "Hello from ESP8266_IN1");

  // END SECTION - MQTT

  

  // set up the LCD's number of columns and rows:

  //lcd.begin(16, 2);
  
  // Print a message to the LCD.
  //lcd.print("Hello, World!");
  //lcd.noDisplay();

  // initialisation bouton = input
  //pinMode(buttonpin, INPUT_PULLUP); 

  // initialisation alimentation retro-eclairage LCD sur la PIN 6
  //pinMode(6,OUTPUT);

  initOTA();

}



void loop() {
  
  ArduinoOTA.handle();
  
  // Prise de Mesure - TOUTES LES 10 secondes
  if((millis() - tempoMesure) >= 10000){
    // start working...
    Serial.println("=================================");
    Serial.println("Sample DHT11...");
    
    
    // read without samples.
    byte temperature = 0;
    byte humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
      Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
      return;
    }
    
    // Affichage des Données sur le Serial
    Serial.print("Sample OK: ");
    Serial.print((int)temperature); Serial.print(" *C, "); 
    Serial.print((int)humidity); Serial.println(" H");
    
    mqtt_publish("esp/temperatureIn1",temperature);
    //client.publish("esp2/temperatureExt",temperatureext);
    mqtt_publish("esp/humiditeIn1",humidity);

    //Serial.println(digitalRead(buttonpin));
    /*
    if (digitalRead(buttonpin) == LOW)
    {
      Serial.println(digitalRead(buttonpin)); // Affichage du Statut Bouton sur le Serial
      digitalWrite(6, HIGH); // Allumage Retro-Eclairage LCD
      lcd.display(); // Activation Display LCD
      // set the cursor to column 0, line 1
      // (note: line 1 is the second row, since counting begins with 0):
      lcd.setCursor(0, 0);
      // print the number of seconds since reset:
      //lcd.print(millis() / 1000);
      lcd.print("Temperature = ");
      lcd.print(temperature);
      lcd.setCursor(1,1);
      lcd.print("Humidity ");
      lcd.setCursor(12,1);
      lcd.print("=");
      lcd.setCursor(14,1);
      lcd.print(humidity);
    }
    else {
      //lcd.clear();
      lcd.noDisplay(); // Desactivation Display LCD
      digitalWrite(6, LOW); // Eteindre Retro-Eclairage LCD
    }
    */

    tempoMesure = millis();
    // DHT11 sampling rate is 1HZ.
    //delay(10000);
  }
  
}

void setup_mqtt() {
  client.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  reconnect();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connection au serveur MQTT ...");
    if (client.connect("ESPClientIN1", MQTT_USERNAME, MQTT_KEY)) {
      Serial.println("MQTT connecté");
    }
    else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
    delay(2000);
    }
  }
  //client.subscribe("esp2/temperatureExt"); //souscription au topic du esp2
}

//Fonction pour publier un float sur un topic
void mqtt_publish(String topic, float t) {
  char top[topic.length()+1];
  topic.toCharArray(top,topic.length()+1);
  char t_char[50];
  String t_str = String(t);
  t_str.toCharArray(t_char, t_str.length() + 1);
  client.publish(top,t_char);
  
  Serial.print("topic = ");
  Serial.println(top);
  Serial.print("valeur topic MQTT = ");
  Serial.println(t);
  Serial.println(t_char);
}

void initOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}




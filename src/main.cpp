#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include "RTClib.h"
#include <Arduino.h>
#include <PubSubClient.h>
#include <SparkFun_VEML7700_Arduino_Library.h>

const char* ssid           = "Galaxy_A70";
const char* password       = "abcd1234";
const char* mqtt_server    = "127.0.0.1"; 
const int   mqtt_port      = 1883;
const char* mqtt_user      = "Santiago";
const char* mqtt_password  = "santiago";
const char* mqtt_client_id = "esp32_client";

//Pines de la tarjeta SD
#define SD_SS 15 //(CS)
/*
define SD_SCK 14
#define SD_MISO 12
#define SD_MOSI 13
*/ 
//Pines del Sensor de Luz
#define Lux_SDA 33
#define Lux_SCL 32
//Pines del Sensor RTC
#define Time_SDA 27
#define Time_SCL 26

File myFile;           //Definimos el archivo de la SD
RTC_DS3231 rtc;        //Definimos el objeto de la RTC
VEML7700 mySensor;     //Definimos el objeto del Sensor
SPIClass SPI2(HSPI);   //Objeto para la comunicacion SPI
WiFiClient espClient;  //Objeto de conxción WIFI
//Comunicación I2C 
TwoWire Lux   = TwoWire(0); //Primer Bus I2C
TwoWire sTime = TwoWire(1); //Primer Bus I2C
//Se publica los datos con client
PubSubClient client(espClient);
String daysOfTheWeek[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
//Función para la conexion WIFI
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a " + String(ssid));
  //Se inicia la conexión con nombre y contraseña
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Despues de hacer la conexión inicial se muestra la direccion IP
  Serial.println("");
  Serial.println("Conectado a la red WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}
// Si el sistema se desconecta del servidor MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("Conectado al broker MQTT");
      client.subscribe("val2");
    } else {
      Serial.print("Error de conexión, rc = ");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}
// Escribimos en el un mensaje en la tarjeta SD
void WriteFile(String path, String message){
  // Se abre el documento con la ruta especifica
  myFile = SD.open(path, FILE_APPEND);
  // Si el archivo se abre correctamente, procedemos a escribir en el
  if (myFile) {
    myFile.println(message);
    myFile.close(); // Cerramos al archivo
  } else {
    Serial.println("A ocurrido un error abriendo el archivo ");
  }
}
//Función que lee que hay dentro de la tarjeta SD
void ReadFile(const char * path){
  // Abrir el archivo antes de leerlo
  myFile = SD.open(path);
  if (myFile) {
     Serial.printf("Leyendo archivo desde:  %s\n", path);
     // Leemos hasta que el archivo este vacio
    while (myFile.available()) {
      // Lee línea por linea completa del archivo
      String line = myFile.readStringUntil('\n'); 
      Serial.println(line);
    }
    myFile.close(); // Cerramos el archivo
  } 
  else {
    Serial.println("error opening test.txt");
  }
}
//Función que manda los datos a Node - Red
void sendData(String data) {
  char payload[100];              // El payload en el cual va a it el mensaje
  data.toCharArray(payload, 100); //Pasamos de String a CharArray

  Serial.println("Publicando: " + String(payload)); //Observamos que esta mandando
  client.publish("val2", payload);                  // Publica el valor en el topic "val2"
  
  //WriteFile("/test.txt", data);
}
//Función que me da la fecha completa que me genera el sensor 
void printDate(DateTime date){
  Serial.print(date.year(), DEC);
  Serial.print('/');
  Serial.print(date.month(), DEC);
  Serial.print('/');
  Serial.print(date.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[date.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(date.hour(), DEC);
  Serial.print(':');
  Serial.print(date.minute(), DEC);
  Serial.print(':');
  Serial.print(date.second(), DEC);
  Serial.println();
}
// Función que envía datos almacenados en SD al broker
void sendStoredData() {
  myFile = SD.open("/test.txt");
  if (myFile) {
    while (myFile.available()) {
      String line = myFile.readStringUntil('\n');
      line.remove(line.length() - 1);
      char payload[100];
      line.toCharArray(payload, 100);
      Serial.println("Enviando almacenado: " + String(payload));
      client.publish("val2", payload);
      delay(100); // Pequeña demora entre envíos
    }
    myFile.close();
    SD.remove("/test.txt"); // Borra el archivo después de enviar los datos
  }
}
// SetUp del programa
void setup() {
  Serial.begin(115200);
  setup_wifi(); // Se inicia la conexión WiFi
  // Se inicializa el sensor de luz
  Lux.begin(Lux_SDA, Lux_SCL, 100000);
  if (!mySensor.begin(Lux)) {
    Serial.println("Unable to communicate with the VEML7700. Please check the wiring. Freezing...");
  }
  // Se inicializa el sensor RTC
  sTime.begin(Time_SDA, Time_SCL, 100000);
  if (!rtc.begin(&sTime)) {
    Serial.println(F("Couldn't find RTC"));
  }
  // Se inicializa la tarjeta SD
  if (!SD.begin(SD_SS, SPI2)) {
    Serial.println("Error al iniciar el Módulo MicroSD");
  }
  // Se realiza la conexión con el servidor MQTT
  client.setServer(mqtt_server, mqtt_port);
  // rtc.adjust(DateTime(2024, 5, 13, 27, 0, 0));
}
//El loop del sistema
void loop() {
  static unsigned long lastSendTime = 0;
  static bool wasWifiConnected = true;
  //Verificamos que exista la conexión a internet
  if (WiFi.status() != WL_CONNECTED) {
    if (wasWifiConnected) {
      Serial.println("Conexión WiFi perdida. Guardando datos en la tarjeta SD.");
      wasWifiConnected = false;
    }
  } else {
    if (!wasWifiConnected) {
      Serial.println("Conexión WiFi recuperada. Enviando datos almacenados al broker.");
      wasWifiConnected = true;
      reconnect(); // Reconectar al servidor MQTT
      sendStoredData();
    }
    // Si esta conectado a internet pero se perdio la conexión con el broker
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }  
  // Envía un valor de luz y de fecha cada 5 segundos
  if (millis() - lastSendTime > 5000) {
    DateTime now = rtc.now();
    float value = mySensor.getLux();
    String data = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + "," + String(value);
    //Si esta tanto conectado a WIFI como al broker manda los datos 
    if (WiFi.status() == WL_CONNECTED && client.connected()) {
      sendData(data);
    } else {
      //Sino lo escribe en la SD
      WriteFile("/test.txt", data);
      Serial.println("Datos por enviar: " + data);
    }
    lastSendTime = millis();
  }
}
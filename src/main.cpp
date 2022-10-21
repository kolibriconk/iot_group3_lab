#include <Arduino.h>
#include <DFRobot_DHT11.h>
#include <SPI.h>
#include <BLEPeripheral.h>
#include "nrf52.h"

#define DHT11_PIN 20

DFRobot_DHT11 dht;
BLEPeripheral blePeripheral = BLEPeripheral();
BLEService environmentService = BLEService("00000000000000000000000000000000");
BLEIntCharacteristic temperatureCharacteristic = BLEIntCharacteristic("00000000000000000000000000000001", BLERead | BLENotify);
BLEIntCharacteristic humidityCharacteristic = BLEIntCharacteristic("00000000000000000000000000000002", BLERead | BLENotify);

void updateTemperature(int temp);
void updateHumidity(int humidity);

void setup(){
  Serial.begin(9600);
  blePeripheral.setLocalName("Group_3_IoT");
  blePeripheral.setAdvertisedServiceUuid(environmentService.uuid());

  blePeripheral.addAttribute(environmentService); //Adding service to wrap temp and humid characteristics to blePeripheral
  blePeripheral.addAttribute(temperatureCharacteristic); //Adding temp characteristic
  blePeripheral.addAttribute(humidityCharacteristic); //Adding humid characteristic

  blePeripheral.begin();
  Serial.println("BLE set up completed");
}

void loop(){

  BLECentral central = blePeripheral.central();
  dht.read(DHT11_PIN);
  int temperature = dht.temperature;
  int humidity = dht.humidity;

  temperatureCharacteristic.setValue(temperature);
  humidityCharacteristic.setValue(humidity);

  if (central) {
  // central connected to peripheral
  Serial.print(F("Connected to central: "));
  Serial.println(central.address());

    while (central.connected()) {
      dht.read(DHT11_PIN);
      temperature = dht.temperature;
      humidity = dht.humidity;

      if (temperatureCharacteristic.value() != temperature && temperature != 255)
        if (temperatureCharacteristic.subscribed()) updateTemperature(temperature);

      if (humidityCharacteristic.value() != humidity && humidity != 255)
        if (humidityCharacteristic.subscribed()) updateHumidity(humidity);
    }

  Serial.print(F("Disconnected from central: "));
  Serial.println(central.address());
  }
}

void updateTemperature(int temperature){
  temperatureCharacteristic.setValue(temperature);
  Serial.println("Temperature has changed! (" + String(temperature) + "ºC)");
}

void updateHumidity(int humidity){
  humidityCharacteristic.setValue(humidity);
  Serial.println("Humidity has changed! (" + String(humidity) + "%)");
}
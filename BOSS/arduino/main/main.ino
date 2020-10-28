#define PJON_INCLUDE_TSA true
#include <PJON.h>
#include <PJONDefines.h>
#include <PJONLocal.h>


PJON<ThroughSerialAsync> bus(45);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
 /* Make use of the payload before sending something, the buffer where payload points to is
    overwritten when a new message is dispatched */
  digitalToggle(LED_BUILTIN);
  Serial.println((char*)payload);
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Initialize LED 13 to be off
  Serial.begin(9600);
  Serial1.begin(1000000);
  bus.strategy.set_serial(&Serial1);
  bus.set_receiver(receiver_function);
  bus.set_communication_mode(PJON_HALF_DUPLEX);
  bus.begin();
};

void loop() {

  //Serial.print("loop");
  bus.send(44,"world",5);
  bus.update();
  bus.receive(1000);
 
};
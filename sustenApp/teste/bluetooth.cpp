#include <SoftwareSerial.h>

int LED = 3, TX = 0, RX = 1;

SoftwareSerial bluetooth(RX, TX); // RX(Modulo) -> TX(Placa), TX(Modulo) -> RX(Placa)

void setup() {
    bluetooth.begin(9600);
    pinMode(LED, OUTPUT);
}

void loop() {
    if(bluetooth.available()) {
        if(bluetooth.read() == '3'){
            digitalWrite(LED, HIGH);
            delay(1000);
            digitalWrite(LED, LOW);
        }
    }
}
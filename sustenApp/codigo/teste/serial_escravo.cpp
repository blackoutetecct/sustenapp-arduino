// ESP32 B
#include <HardwareSerial.h>

HardwareSerial serial(0); // UART2

void setup() {
  Serial.begin(115200); // Inicializa a porta serial para depuração
  serial.begin(9600);  // Inicializa a UART2 com baud rate de 9600
}

void loop() {
  // Lê dados do outro ESP32
  if (serial.available()) {
    String receivedData = serial.readString();
    Serial.println("Dados recebidos: " + receivedData);
  }

  // Envia dados para o outro ESP32
  serial.println("Hello, ESP32 A!");
  delay(1000);
}

int LED = 3;

void setup() {
    pinMode(LED, OUTPUT);
}

void loop() {
  controlaDispositivo(LED);
  delay(500);
}

void controlaDispositivo(int porta) {
    digitalWrite(porta, !retornaEstadoDispositivo(porta));
}

int retornaEstadoDispositivo(int porta) {
    return digitalRead(porta);
}
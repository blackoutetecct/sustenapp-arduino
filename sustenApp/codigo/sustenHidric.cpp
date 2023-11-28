#include <HardwareSerial.h>

const float FATOR_CALIBRACAO = 7.5;
const int SENSOR = 2, LED_ACIONAMENTO = 23;

double VOLUME_TOTAL = 0;
int CONTADOR_AGUA = 0, INTERRUPCAO_SENSOR = 0;
unsigned long ULTIMA_EXECUCAO = 0;

HardwareSerial SerialPort(2);

void setup() {
    Serial.begin(9600);
    SerialPort.begin(9600, SERIAL_8N1, 1, 3);
    pinMode(SENSOR, INPUT_PULLUP);

    /* LED ACIONAMENTO */
    pinMode(LED_ACIONAMENTO, OUTPUT);
    digitalWrite(LED_ACIONAMENTO, HIGH);
}

void loop() {
    SerialPort.println(getLeitura());
    leituraHidrica();
}

void leituraHidrica() {
    unsigned long INTERVALO = millis() - ULTIMA_EXECUCAO;

    if (INTERVALO > 1000) {
        detachInterrupt(INTERRUPCAO_SENSOR);

        double FLUXO = ((1000.0 / INTERVALO) * CONTADOR_AGUA) / FATOR_CALIBRACAO;
        double VOLUME = FLUXO / 60;

        VOLUME_TOTAL += VOLUME / 1000; //(VOLUME / 1000);
        CONTADOR_AGUA = 0;
        ULTIMA_EXECUCAO = millis();

        attachInterrupt(INTERRUPCAO_SENSOR, setContador, FALLING);
    }
}

void setContador() {
    CONTADOR_AGUA++;
}

double getLeitura() {
    return VOLUME_TOTAL;
}

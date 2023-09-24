#include <EmonLib.h>

const int SENSOR = A0, TENSAO = 110, CALIBRACAO = 6.0606, LED_ACIONAMENTO = 2;
double W;

EnergyMonitor energia;

void setup() {
    Serial.begin(4800);
    energia.current(SENSOR, CALIBRACAO);

    /* LED ACIONAMENTO */
    pinMode(LED_ACIONAMENTO, OUTPUT);
    digitalWrite(LED_ACIONEMENTO, HIGH);
}

void loop() {
    if(Serial.available()) {
        while(Serial.available()) {
            Serial.println(getLeitura());
        }
    }

    leituraEletrica();
}

void leituraEletrica() {
    double IRMS = energia.calcIrms(1480);

    if (IRMS < 0.02) {
        return;
    }

    W += (IRMS * TENSAO);
}

double getLeitura() {
    return VOLUME_TOTAL;
}
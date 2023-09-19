#include <EmonLib.h>

const int SENSOR = A0, TENSAO = 110, CALIBRACAO = 6.0606;
double W;

EnergyMonitor energia;

void setup() {
    Serial.begin(9600);
    energia.current(SENSOR, CALIBRACAO);
}

void loop() {
    if(Serial.available()) {
        while(Serial.available()) {
            if(Serial.readString() == "relatorio_eletrico") {
                Serial.println(getLeitura());
            }
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
    double W_RETURN = W;
    W = 0;
    return W_RETURN;
}
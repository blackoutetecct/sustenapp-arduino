#include <EmonLib.h>

const int SCT = A0, TENSAO = 110;
double KWH = 0, CALIBRACAO = 0, CORRENTE = 0;

EnergyMonitor energia;

void setup() {
    Serial.begin(9600);
}

void loop() {
  leituraEletrica();
  energia.current(SCT, CALIBRACAO);
}

void leituraEletrica() {
    double IRMS = energia.calcIrms(1480); 

    if(IRMS == CORRENTE) {
        Serial.println(CALIBRACAO);
    }

    CALIBRACAO += 0.01;
}
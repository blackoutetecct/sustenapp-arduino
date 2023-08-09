#include <EmonLib.h>

const int SCT = A0, TENSAO = 110;
double KW;

EnergyMonitor energia;

void setup() {
    Serial.begin(9600);
    energia.current(SCT, 6.0606);
}

void loop() {
  leituraEletrica();
  Serial.println(KW);
}

void leituraEletrica() {
    double IRMS = energia.calcIrms(1480); 

    if (IRMS < 0.02) {
        return;
    }

    KW = ((IRMS * TENSAO) / 1000);
}

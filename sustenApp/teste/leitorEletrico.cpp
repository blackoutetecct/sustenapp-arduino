#include <EmonLib.h>

const int SCT = A0, TENSAO = 110;
double KWH = 0;

EnergyMonitor energia;

void setup() {
    Serial.begin(9600);
    energia.current(SCT, 60);
}

void loop() {
  leituraEletrica();
  Serial.println(KWH);
}

void leituraEletrica() {
    double IRMS = energia.calcIrms(1480); 

    if (IRMS < 0.02) {
        return;
    }

    KWH += (IRMS * TENSAO) / 1000;
}
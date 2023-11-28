#include <EmonLib.h>

const int SCT = A1, TENSAO = 110;
double WH;

EnergyMonitor energia;

void setup() {
    Serial.begin(9600);
    energia.current(SCT, 6.0607);
}

void loop() {
  leituraEletrica();
  Serial.println(WH);
  delay(1000);
}

void leituraEletrica() {
    double IRMS = energia.calcIrms(1480); 

    if (IRMS < 0.02) {
        return;
    }

    WH += (IRMS * TENSAO) / 1000;
}

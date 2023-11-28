#include <EmonLib.h>
#include <HardwareSerial.h>

const int SENSOR = 2, TENSAO = 110, CALIBRACAO = 6.0607, LED_ACIONAMENTO = 23;
double WH;

HardwareSerial SerialPort(2);
EnergyMonitor energia;

void setup() {
    Serial.begin(4800);
    SerialPort.begin(4800, SERIAL_8N1, 16, 17);
    energia.current(SENSOR, CALIBRACAO);

    /* LED ACIONAMENTO */
    pinMode(LED_ACIONAMENTO, OUTPUT);
    digitalWrite(LED_ACIONAMENTO, HIGH);
}

void loop() {
    leituraEletrica();
    SerialPort.println(getLeitura());
    delay(1000);
}

void leituraEletrica() {
    double IRMS = energia.calcIrms(1480);

    if (IRMS < 0.02) {
        return;
    }

    WH += (IRMS * TENSAO) / 1000;
}

double getLeitura() {
    return WH;
}

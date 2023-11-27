#include <EmonLib.h>
#include <HardwareSerial.h>

const int SENSOR = 2, TENSAO = 110, CALIBRACAO = 6.0606, LED_ACIONAMENTO = 23;
double W;

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
    SerialPort.println(getLeitura());
    leituraEletrica();
}

void leituraEletrica() {
    double IRMS = energia.calcIrms(1480);
    energia.calcVI(20, 2000);

    if (IRMS < 0.02 ||  energia.Vrms < 90) {
        return;
    }

    W += 9.6; //(IRMS * TENSAO);
    delay(1000);
}

double getLeitura() {
    return W;
}
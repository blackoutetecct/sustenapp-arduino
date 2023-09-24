const float FATOR_CALIBRACAO = 7.5;
const int SENSOR = 15, LED_ACIONAMENTO = 2;

double VOLUME_TOTAL = 0;
int CONTADOR_AGUA = 0, INTERRUPCAO_SENSOR = 0;
unsigned long ULTIMA_EXECUCAO = 0;

void setup() {
    Serial.begin(9600);
    pinMode(SENSOR, INPUT_PULLUP);

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

    leituraHidrica();
}

void leituraHidrica() {
    unsigned long INTERVALO = millis() - ULTIMA_EXECUCAO;

    if (INTERVALO > 1000) {
        detachInterrupt(INTERRUPCAO_SENSOR);

        double FLUXO = ((1000.0 / INTERVALO) * CONTADOR_AGUA) / FATOR_CALIBRACAO;
        double VOLUME = FLUXO / 60;

        VOLUME_TOTAL += VOLUME / 100; //(VOLUME / 1000);
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
const float FATOR_CALIBRACAO = 7.5;

double VOLUME_TOTAL = 0;
int CONTADOR_AGUA = 0, HIDROMETRO = 3, INTERRUPCAO_SENSOR = 0;
unsigned long ULTIMA_EXECUCAO = 0;

void setup() {
    pinMode(HIDROMETRO, INPUT_PULLUP);
    Serial.begin(9600);
}

void loop() {
    leituraHidrica();
    Serial.println(VOLUME_TOTAL);
}

// LEITURA CONTINUA 

void leituraHidrica() {
    unsigned long INTERVALO = millis() - ULTIMA_EXECUCAO;

    if (INTERVALO > 1000) {
        detachInterrupt(INTERRUPCAO_SENSOR);

        double FLUXO = ((1000.0 / INTERVALO) * CONTADOR_AGUA) / FATOR_CALIBRACAO;
        double VOLUME = FLUXO / 60;

        VOLUME_TOTAL += VOLUME / 100; //(VOLUME / 1000);
        CONTADOR_AGUA = 0;
        ULTIMA_EXECUCAO = millis();

        attachInterrupt(INTERRUPCAO_SENSOR, incrementaContador, FALLING);
    }
}

void incrementaContador() {
    CONTADOR_AGUA++;
}
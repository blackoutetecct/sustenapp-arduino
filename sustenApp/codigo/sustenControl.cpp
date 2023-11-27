#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <Ultrasonic.h>
#include <HardwareSerial.h>

/* WIFI */

const char * ssid = "REDE WIFI";
const char * password = "SENHA";

IPAddress staticIP(192, 168, 18, 226);
IPAddress gateway(192, 168, 18, 1); 
IPAddress subnet(255, 255, 255, 0); 

AsyncWebServer server(80); 
AsyncWebServerRequest * requestStatic;

/* SERIAL */

HardwareSerial serial_hidrico(0);
HardwareSerial serial_eletrico(2);

/* PORTAS */

const int RESERVATORIO_ECHO = 22, RESERVATORIO_TRIGGER = 23, LED_ACIONEMENTO = 25;
int portasDisponiveis[18] = {4,5,12,13,14,15,25,26,27,32,33,34,35,36,39,18,19};

/* VARIAVEIS */

long double ALTURA_RESERVATORIO = 0, CAPACIDADE_RESERVATORIO = 0, KW = 0;

/* INSTANCIACAO */

Ultrasonic ultrasonic(RESERVATORIO_TRIGGER, RESERVATORIO_ECHO);
const int LED_ACIONAMENTO = 2;

/* CONTROLE DE LEITURA */

double ULTIMA_LEITURA_HIDRICA = 0, ULTIMA_LEITURA_ELETRICA = 0;

void setup() {
    inicializaWifi();

    /* SERIAL */
    Serial.begin(115200);
    serial_hidrico.begin(9600, SERIAL_8N1, 1, 3);
    serial_eletrico.begin(4800, SERIAL_8N1, 16, 17);

    /* LED ACIONAMENTO */
    pinMode(LED_ACIONAMENTO, OUTPUT);
    digitalWrite(LED_ACIONEMENTO, HIGH);
}

void loop() {
    contadorEletrico();
    delay(1000);
}

/* COMUNICACAO SERIAL */

double recebeInformacaoSerial(String tipo) {
    if (tipo == "hidrico") {
        recebeInformacaoHidrico();
    } else if (tipo == "eletrico") {
        recebeInformacaoEletrico();
    }

    return 0.0;
}

double recebeInformacaoHidrico() {
    while (serial_hidrico.available()) {
        double leitura = serial_hidrico.readString().toDouble();
        double estimativa = leitura - ULTIMA_LEITURA_HIDRICA;
        ULTIMA_LEITURA_HIDRICA = leitura;

        return estimativa;
    }

    enviaStatus(400, "FALHA NA OBTENCAO DE CONSUMO");
}

void contadorEletrico() {
    ULTIMA_LEITURA_ELETRICA += 9.6;
}

double recebeInformacaoEletrico() {
    while (serial_eletrico.available()) {
        double leitura = serial_eletrico.readString().toDouble();
        double estimativa = leitura - ULTIMA_LEITURA_ELETRICA;
        ULTIMA_LEITURA_ELETRICA = leitura;

        return estimativa;
    }

    return ULTIMA_LEITURA_ELETRICA;

    enviaStatus(400, "FALHA NA OBTENCAO DE CONSUMO");
}

/* COMUNICACAO EXTERNA */

void enviaInformacao(String informacao) {
    requestStatic->send(200, "application/json", informacao);
}

/* WIFI */

AsyncCallbackJsonWebHandler * handle = new AsyncCallbackJsonWebHandler(
    "/", [](AsyncWebServerRequest * request, JsonVariant & jsonReceiver) {
        requestStatic = request;

        if (jsonReceiver["comando"] == "consumo") {
                if (jsonReceiver["tipo"] == "hidrico") {
                    if (jsonReceiver["renovavel"]) {
                        enviaRelatorio(retornaVolumeReservatorio(), "L");
                    } else {
                        enviaRelatorio(retornaConsumoHidrico(), "M3");
                    }
                } else if (jsonReceiver["tipo"] == "eletrico") {
                    enviaRelatorio(retornaConsumoEletrico(), "W");
                }
            } else if (jsonReceiver["comando"] == "controlador") {
                controlaDispositivo(jsonReceiver["porta"]);
            } else if (jsonReceiver["comando"] == "dispositivo") {
                enviaEstadoDispositivo(200, retornaEstadoDispositivo(jsonReceiver["porta"]));
            } else if (jsonReceiver["comando"] == "declaracao") {
                if (jsonReceiver["tipo"] == "reservatorio") {
                    declaraReservatorio(jsonReceiver["capacidade"]);
                } else if (jsonReceiver["tipo"] == "dispositivo") {
                    declaracaoDispositivo(jsonReceiver["porta"]);
                } else if (jsonReceiver["tipo"] == "eletricidade") {
                    declaraEletricidade();
                } else if (jsonReceiver["tipo"] == "hidricidade") {
                    declaraHidricidade();
                }
            } else if (jsonReceiver["comando"] == "disponibilidade") {
                informaPortaDisponivel();
            } else {
                enviaStatus(500, "FALHA NA COMUNICACAO");
            }
    }
);

void handleRequest(AsyncWebServerRequest * request) {}
 
void inicializaWifi() {
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    server.addHandler(handle);
    server.on("/", HTTP_POST, handleRequest);
    server.begin();
}

/* CONTROLADOR DE DISPOSITIVO */

void controlaDispositivo(int porta) {
    int estado = !retornaEstadoDispositivo(porta);
    digitalWrite(porta, estado);
    enviaEstadoDispositivo(200, !estado);
}

/* STATUS */

double retornaConsumoEletrico() {
    return ULTIMA_LEITURA_ELETRICA/1000; //recebeInformacaoSerial("eletrico");
}

double retornaConsumoHidrico() {
    return recebeInformacaoSerial("hidrico");
}

double retornaVolumeReservatorio() {
    return ((CAPACIDADE_RESERVATORIO / ALTURA_RESERVATORIO) * (ultrasonic.read() * 100) / ALTURA_RESERVATORIO);
}

int retornaEstadoDispositivo(int porta) {
    return digitalRead(porta);
}

/* DECLARACAO */

void declaraReservatorio(double capacidade) {
    CAPACIDADE_RESERVATORIO = capacidade; // L
    ALTURA_RESERVATORIO = ultrasonic.read(); // CM

    enviaStatus(200, "RESERVATORIO CONFIGURADO");
}


void declaraEletricidade() {
  enviaStatus(200, "ELETRICIDADE MONITORAVEL");
}

void declaraHidricidade() {
  enviaStatus(200, "ELETRICIDADE MONITORAVEL");
}

/* GERENCIAMENTO DE PORTAS */

void informaPortaDisponivel() {
    for (int i = 0; i < 18; i++) {
        if (portasDisponiveis[i]) {
            enviaPortaDisponivel(200, portasDisponiveis[i]);
            return;
        }
    }

    enviaStatus(200, "ALCANCE DE LIMITE DE PORTAS UTILIZAVEIS");
}

void declaracaoDispositivo(int porta) {
    for (int i = 0; i < 18; i++) {
        if (portasDisponiveis[i] == porta) {
            portasDisponiveis[i] = NULL;
            declaraPorta(porta, OUTPUT);
            enviaStatus(200, "DISPOSITIVO ADICIONADO");
            return;
        }
    }

    enviaStatus(402, "FALHA NA ADICAO DO DISPOSITIVO");
}

void declaraPorta(int porta, int fluxo) {
    pinMode(porta, fluxo);

    if (fluxo == OUTPUT) {
        digitalWrite(porta, LOW);
    }
}

/* RELATORIO */

void enviaRelatorio(double consumo, String unidade) {
    enviaInformacao(
        "'consumo':" + String(consumo) + ",'unidade':'" + unidade + "'"
    );
}

void enviaStatus(int status, String mensagem) {
    enviaInformacao(
        "'status':" + String(status) + ",'mensagem':'" + mensagem + "'"
    );
}

void enviaEstadoDispositivo(int status, int estado) {
    enviaInformacao(
        "'status':" + String(status) + ",'estado':" + String(estado)
    );
}

void enviaPortaDisponivel(int status, int porta) {
    enviaInformacao(
        "'status':" + String(status) + ",'porta':" + String(porta)
    );
}
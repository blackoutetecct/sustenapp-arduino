<h1 align=center>SUSTENAPP-ARDUINO</h1>

<p align="center">
  <img src="logo_sustenapp.png" width="500">
</p>

#
### TECNOLOGIAS

![Arduino](https://img.shields.io/badge/Arduino-0D1117?style=for-the-badge&logo=Arduino&logoColor=00979D&labelColor=0D1117)&nbsp;
![C++](https://img.shields.io/badge/C%2B%2B-0D1117?style=for-the-badge&logo=C%2B%2B&logoColor=00599C&labelColor=0D1117)&nbsp;
![JSON](https://img.shields.io/badge/json-0D1117?style=for-the-badge&logo=json&logoColor=5E5C5C&labelColor=0D1117)&nbsp;

# 
### BIBLIOTECAS E REFERENCIAS

- https://www.arduino.cc/reference/en/libraries/emonlib/
- https://github.com/MatheusAlvesA/ArduinoAsync
- https://www.arduino.cc/reference/en/libraries/ultrasonic/
- https://www.arduino.cc/reference/en/libraries/arduinojson/

#
### DOCUMENTACAO DA APLICACAO

- CONTROLE DE DISPOSITIVO
```
{
	"comando": "controlador",
	"porta": <int>
}
```

- ESTADO DE DISPOSITIVO
```
{
	"comando": "dispositivo",
	"porta": <int>,
	"estado": <int>
}
```
- DECLARACAO DE DISPOSITIVO
```
{
    	"comando": "declaracao",
    	"tipo": "dispositivo",
	"porta" : int
}
```

- DECLARACAO DE VARIAVEL DE RESERVATORIO
```
{
    	"comando": "declaracao",
    	"tipo": "reservatorio",
	"capacidade" : <double>
}
```

- ATUALIZACAO DE CONSUMO
```
{
	"comando": "consumo",
	"tipo": "<hidrico/eletrico>",
	"renovavel": <bool>,
	"consumo": <float>,
	"unidade": "<str>"
}
```

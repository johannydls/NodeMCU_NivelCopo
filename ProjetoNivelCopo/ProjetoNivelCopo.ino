/*
 * Projeto para a disciplina LOAC 2018.1 - Turma 3
 * Aluno: Johanny de Lucena Santos - 113210425
 * Projeto: reconhecer o nível de líquido em um
 * copo e disponibilizar via internet, através do ESP-12E
 */

#include "indexHTML.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "ssid";
const char *password = "senha";

const char *ssidAP = "ESP-JLS";
const char *passwordAP = "123456789";

const int porta = 80;

ESP8266WebServer server (porta);

#define BASE 5 // Base do copo - Faz contato com os outros fios
#define NVL_BAIXISSIMO 4
#define NVL_BAIXO 14
#define NVL_MEDIO 12
#define NVL_ALTO 13

void handleRoot() {

  char temp[600];

  snprintf(temp, 600,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Monitoramento de Copos</title>\
    <style>%s</style>\
  </head>\
  <body align='center'>\
    <h1>Gerenciamento de copos<br>LABARC - UFCG 2018.1</h1>\
    <h4>Feito por Johanny de Lucena Santos</h4>\
    <hr />\
    <h2>Copo #1: %s</h2><br>\
    <img src=\"/copos.svg\"/>\
    <br>\
    <p><em>Atualizado a cada 5 segundos</em></p>\
  </body>\
</html>",
  css.c_str(), textoNivel().c_str()
  );
  server.send(200, "text/html; charset=utf-8", temp);
}

void handleNotFound() {
  String message = "Arquivo não encontrado\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i=0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; 
  }

  server.send(404, "text/plain", message);
}

void setup() {
  
  pinMode(BASE, OUTPUT);
  pinMode(NVL_BAIXISSIMO, INPUT);
  pinMode(NVL_BAIXO, INPUT);
  pinMode(NVL_MEDIO, INPUT);
  pinMode(NVL_ALTO, INPUT);

  Serial.begin(115200);

  //Configurações da rede
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  //Configuração Access Point
  WiFi.softAP(ssidAP, passwordAP);
  IPAddress ipAP = WiFi.softAPIP();

  //Esperando por conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Log de conexão
  Serial.println("");
  Serial.print("Conectado a ");
  Serial.println(ssid);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  //Log do Access Point
  Serial.print("SSID Access Point: ");
  Serial.println(ssidAP);
  Serial.print("Wifi Access Point IP: ");
  Serial.println(ipAP);
  Serial.println("");

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/copos.svg", desenhaCopo);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Servidor HTTP disponível em ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(porta);
  Serial.println("");

}

void loop() {
  server.handleClient();
}

/*
 * Função que retorna o valor do contato entre a base e um dos niveis
 */
uint8_t contato(int nivel) {
  
  register uint8_t now=0, prev=1, value=0;

  for (register uint8_t i=0; i < 200; i++) {
    
    now = GPIP(nivel);
    value += now ^ prev;
    prev = now;

    if (now) GPOC = (1 << BASE);
    else     GPOS = (1 << BASE);
  }
  
  return value;
}

/*
 * Função para retornar o texto referente ao nível 
 */
String textoNivel() {

  int baixissimo = contato(NVL_BAIXISSIMO);
  int baixo = contato(NVL_BAIXO);
  int medio = contato(NVL_MEDIO);
  int alto = contato(NVL_ALTO);  
  
  String out = "";

  if (alto > 50 && medio > 50 && baixo > 50 && baixissimo > 50) {
    out += "<span style='color:green;'>CHEIO</span>";
  }

  else if (alto < 50 && medio > 50 && baixo > 50 && baixissimo > 50) {
    out += "<span style='color: yellow;'>NA METADE</span>";
  }

  else if (alto < 50 && medio < 50 && baixo > 50 && baixissimo > 50) {
    out += "<span style='color: orange;'>MENOS DA METADE</span>";
  }

  else if (alto < 50 && medio < 50 && baixo < 50 && baixissimo > 50) {
    out += "<span style='color: red;'>QUASE ACABANDO</span>";
  }

  else {
    out += "VAZIO";
  }

  return out;
}

/*
 * Função para desenhar o copo de acordo com o nível 
 */
void desenhaCopo() {
  
  int baixissimo = contato(NVL_BAIXISSIMO);
  int baixo = contato(NVL_BAIXO);
  int medio = contato(NVL_MEDIO);
  int alto = contato(NVL_ALTO);
  
  String out = "";

  if (alto > 50 && medio > 50 && baixo > 50 && baixissimo > 50) {
    //Desenha o copo no nivel alto

    out += "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='415px' height='520px' viewBox='6 5 127 159' preserveAspectRatio='xMidYMid meet'>";
    out += "<rect id='svgEditorBackground' x='0' y='0' width='870' height='520' style='fill:none; stroke: none;'/>";
    out += "<defs id='svgEditorDefs'>";
    out += "<linearGradient gradientTransform='skewX(-45)' gradientUnits='objectBoundingBox' id='lgrd2-c-repeat' spreadMethod='repeat' x1='0%' x2='5%' y1='0%' y2='0%'>";
    out += "<stop offset='5%' stop-color='red' stop-opacity='0.8'/>";
    out += "<stop offset='75%' stop-color='yellow' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientTransform='rotate(90)' gradientUnits='objectBoundingBox' id='lgrd3-a' spreadMethod='repeat' x1='10%' x2='20%' y1='20%' y2='20%'>";
    out += "<stop offset='5%' stop-color='#babc91' stop-opacity='1'/>";
    out += "<stop offset='50%' stop-color='#c8d591' stop-opacity='1'/>";
    out += "<stop offset='90%' stop-color='#afd591' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<radialGradient cx='50%' cy='50%' fx='50%' fy='50%' id='rgrd2-white-blue' r='50%'>";
    out += "<stop offset='0%' style='stop-color:white;stop-opacity:0'/>";
    out += "<stop offset='100%' style='stop-color:blue;stop-opacity:1'/>";
    out += "</radialGradient>";
    out += "<radialGradient gradientUnits='objectBoundingBox' id='rgrd-wave' r='5%' spreadMethod='reflect'>";
    out += "<stop offset='0%' stop-color='navy' stop-opacity='1.00'/>";
    out += "<stop offset='20%' stop-color='navy' stop-opacity='0.90'/>";
    out += "<stop offset='40%' stop-color='navy' stop-opacity='0.65'/>";
    out += "<stop offset='60%' stop-color='navy' stop-opacity='0.35'/>";
    out += "<stop offset='80%' stop-color='navy' stop-opacity='0.10'/>";
    out += "<stop offset='100%' stop-color='navy' stop-opacity='0.00'/>";
    out += "</radialGradient>";
    out += "<radialGradient cy='60%' fx='45%' fy='45%' gradientUnits='objectBoundingBox' id='rgrd11-b' r='2'>";
    out += "<stop offset='0%' stop-color='#48afc1'/>";
    out += "<stop offset='10%' stop-color='#b4c63b'/>";
    out += "<stop offset='20%' stop-color='#ef5b2b'/>";
    out += "<stop offset='30%' stop-color='#503969'/>";
    out += "<stop offset='40%' stop-color='#ab6294'/>";
    out += "<stop offset='50%' stop-color='#1cb98f'/>";
    out += "<stop offset='60%' stop-color='#48afc1'/>";
    out += "<stop offset='70%' stop-color='#b4c63b'/>";
    out += "<stop offset='80%' stop-color='#ef5b2b'/>";
    out += "<stop offset='90%' stop-color='#503969'/>";
    out += "<stop offset='100%' stop-color='#ab6294'/>";
    out += "</radialGradient>";
    out += "<radialGradient fx='30%' fy='30%' gradientUnits='objectBoundingBox' id='rgrd11-a'>";
    out += "<stop offset='0' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#f79959'/>";
    out += "<stop offset='90%' stop-color='#f97821'/>";
    out += "<stop offset='100%' stop-color='#f97821'/>";
    out += "</radialGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-h' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' style='stop-color:peachpuff;  stop-opacity:1;'/>";
    out += "<stop offset='100%' style='stop-color:sienna;  stop-opacity:1'/>";
    out += "</linearGradient>";
    out += "<path id='svgEditorClosePathDefs' style='stroke:black;stroke-width:1px;fill:khaki;'/>";
    out += "<polygon id='svgEditorShapeDefs' style='fill:khaki;stroke:black;vector-effect:non-scaling-stroke;stroke-width:1px;'/>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lrgd2-e1' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' stop-color='lightblue'/>";
    out += "<stop offset='100%' stop-color='#ef5b2b'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-v' spreadMethod='pad' x1='0%' x2='0%' y1='19.2%' y2='100%' gradientTransform='rotate(0)'>";
    out += "<stop offset='0%' style='stop-color: paleturquoise; stop-opacity: 0.84;'/>";
    out += "<stop offset='0.852' style='stop-color: dodgerblue; stop-opacity: 1;'/>";
    out += "</linearGradient>";
    out += "</defs>";
    out += "<path d='M-29.122807017543884,-23.480701754385983l1.1298245614035096,8h4l1.0736842105263094,-8Z' style='fill:none; stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e13_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "<path d='M-28.932204073401934,-22.478483565234953l0.9743093365597844,6.962694091550702h3.915789473684214l0.9260738052026625,-6.965759225650338Z' style='fill:url(#lgrd2-peachpuff-sienna-v); stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e4_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "</svg>";
  }

  else if (alto < 50 && medio > 50 && baixo > 50 && baixissimo > 50) {
    //Desenha o copo no nivel medio

    out += "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='415px' height='520px' viewBox='6 5 127 159' preserveAspectRatio='xMidYMid meet'>";
    out += "<rect id='svgEditorBackground' x='0' y='0' width='870' height='520' style='fill:none; stroke: none;'/>";
    out += "<defs id='svgEditorDefs'>";
    out += "<linearGradient gradientTransform='skewX(-45)' gradientUnits='objectBoundingBox' id='lgrd2-c-repeat' spreadMethod='repeat' x1='0%' x2='5%' y1='0%' y2='0%'>";
    out += "<stop offset='5%' stop-color='red' stop-opacity='0.8'/>";
    out += "<stop offset='75%' stop-color='yellow' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientTransform='rotate(90)' gradientUnits='objectBoundingBox' id='lgrd3-a' spreadMethod='repeat' x1='10%' x2='20%' y1='20%' y2='20%'>";
    out += "<stop offset='5%' stop-color='#babc91' stop-opacity='1'/>";
    out += "<stop offset='50%' stop-color='#c8d591' stop-opacity='1'/>";
    out += "<stop offset='90%' stop-color='#afd591' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<radialGradient cx='50%' cy='50%' fx='50%' fy='50%' id='rgrd2-white-blue' r='50%'>";
    out += "<stop offset='0%' style='stop-color:white;stop-opacity:0'/>";
    out += "<stop offset='100%' style='stop-color:blue;stop-opacity:1'/>";
    out += "</radialGradient>";
    out += "<radialGradient gradientUnits='objectBoundingBox' id='rgrd-wave' r='5%' spreadMethod='reflect'>";
    out += "<stop offset='0%' stop-color='navy' stop-opacity='1.00'/>";
    out += "<stop offset='20%' stop-color='navy' stop-opacity='0.90'/>";
    out += "<stop offset='40%' stop-color='navy' stop-opacity='0.65'/>";
    out += "<stop offset='60%' stop-color='navy' stop-opacity='0.35'/>";
    out += "<stop offset='80%' stop-color='navy' stop-opacity='0.10'/>";
    out += "<stop offset='100%' stop-color='navy' stop-opacity='0.00'/>";
    out += "</radialGradient>";
    out += "<radialGradient cy='60%' fx='45%' fy='45%' gradientUnits='objectBoundingBox' id='rgrd11-b' r='2'>";
    out += "<stop offset='0%' stop-color='#48afc1'/>";
    out += "<stop offset='10%' stop-color='#b4c63b'/>";
    out += "<stop offset='20%' stop-color='#ef5b2b'/>";
    out += "<stop offset='30%' stop-color='#503969'/>";
    out += "<stop offset='40%' stop-color='#ab6294'/>";
    out += "<stop offset='50%' stop-color='#1cb98f'/>";
    out += "<stop offset='60%' stop-color='#48afc1'/>";
    out += "<stop offset='70%' stop-color='#b4c63b'/>";
    out += "<stop offset='80%' stop-color='#ef5b2b'/>";
    out += "<stop offset='90%' stop-color='#503969'/>";
    out += "<stop offset='100%' stop-color='#ab6294'/>";
    out += "</radialGradient>";
    out += "<radialGradient fx='30%' fy='30%' gradientUnits='objectBoundingBox' id='rgrd11-a'>";
    out += "<stop offset='0' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#f79959'/>";
    out += "<stop offset='90%' stop-color='#f97821'/>";
    out += "<stop offset='100%' stop-color='#f97821'/>";
    out += "</radialGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-h' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' style='stop-color:peachpuff;  stop-opacity:1;'/>";
    out += "<stop offset='100%' style='stop-color:sienna;  stop-opacity:1'/>";
    out += "</linearGradient>";
    out += "<path id='svgEditorClosePathDefs' style='stroke:black;stroke-width:1px;fill:khaki;'/>";
    out += "<polygon id='svgEditorShapeDefs' style='fill:khaki;stroke:black;vector-effect:non-scaling-stroke;stroke-width:1px;'/>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lrgd2-e1' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' stop-color='lightblue'/>";
    out += "<stop offset='100%' stop-color='#ef5b2b'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-v' spreadMethod='pad' x1='0%' x2='0%' y1='19.2%' y2='100%' gradientTransform='rotate(0)'>";
    out += "<stop offset='0%' style='stop-color: paleturquoise; stop-opacity: 0.84;'/>";
    out += "<stop offset='0.852' style='stop-color: dodgerblue; stop-opacity: 1;'/>";
    out += "</linearGradient>";
    out += "</defs>";
    out += "<path d='M-29.122807017543884,-23.480701754385983l1.1298245614035096,8h4l1.0736842105263094,-8Z' style='fill:none; stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e13_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "<path d='M-28.538898971566883,-19.86214962694094l0.581004234724734,4.34636015325669h3.915789473684214l0.566969147005441,-4.383625730994154Z' style='fill:url(#lgrd2-peachpuff-sienna-v); stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e4_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "</svg>";
  }

  else if (alto < 50 && medio < 50 && baixo > 50 && baixissimo > 50) {
    //Desenha o copo no nivel baixo

    out += "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='415px' height='520px' viewBox='6 5 127 159' preserveAspectRatio='xMidYMid meet'  >";
    out += "<rect id='svgEditorBackground' x='0' y='0' width='870' height='520' style='fill:none; stroke: none;'/>";
    out += "<defs id='svgEditorDefs'>";
    out += "<linearGradient gradientTransform='skewX(-45)' gradientUnits='objectBoundingBox' id='lgrd2-c-repeat' spreadMethod='repeat' x1='0%' x2='5%' y1='0%' y2='0%'>";
    out += "<stop offset='5%' stop-color='red' stop-opacity='0.8'/>";
    out += "<stop offset='75%' stop-color='yellow' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientTransform='rotate(90)' gradientUnits='objectBoundingBox' id='lgrd3-a' spreadMethod='repeat' x1='10%' x2='20%' y1='20%' y2='20%'>";
    out += "<stop offset='5%' stop-color='#babc91' stop-opacity='1'/>";
    out += "<stop offset='50%' stop-color='#c8d591' stop-opacity='1'/>";
    out += "<stop offset='90%' stop-color='#afd591' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<radialGradient cx='50%' cy='50%' fx='50%' fy='50%' id='rgrd2-white-blue' r='50%'>";
    out += "<stop offset='0%' style='stop-color:white;stop-opacity:0'/>";
    out += "<stop offset='100%' style='stop-color:blue;stop-opacity:1'/>";
    out += "</radialGradient>";
    out += "<radialGradient gradientUnits='objectBoundingBox' id='rgrd-wave' r='5%' spreadMethod='reflect'>";
    out += "<stop offset='0%' stop-color='navy' stop-opacity='1.00'/>";
    out += "<stop offset='20%' stop-color='navy' stop-opacity='0.90'/>";
    out += "<stop offset='40%' stop-color='navy' stop-opacity='0.65'/>";
    out += "<stop offset='60%' stop-color='navy' stop-opacity='0.35'/>";
    out += "<stop offset='80%' stop-color='navy' stop-opacity='0.10'/>";
    out += "<stop offset='100%' stop-color='navy' stop-opacity='0.00'/>";
    out += "</radialGradient>";
    out += "<radialGradient cy='60%' fx='45%' fy='45%' gradientUnits='objectBoundingBox' id='rgrd11-b' r='2'>";
    out += "<stop offset='0%' stop-color='#48afc1'/>";
    out += "<stop offset='10%' stop-color='#b4c63b'/>";
    out += "<stop offset='20%' stop-color='#ef5b2b'/>";
    out += "<stop offset='30%' stop-color='#503969'/>";
    out += "<stop offset='40%' stop-color='#ab6294'/>";
    out += "<stop offset='50%' stop-color='#1cb98f'/>";
    out += "<stop offset='60%' stop-color='#48afc1'/>";
    out += "<stop offset='70%' stop-color='#b4c63b'/>";
    out += "<stop offset='80%' stop-color='#ef5b2b'/>";
    out += "<stop offset='90%' stop-color='#503969'/>";
    out += "<stop offset='100%' stop-color='#ab6294'/>";
    out += "</radialGradient>";
    out += "<radialGradient fx='30%' fy='30%' gradientUnits='objectBoundingBox' id='rgrd11-a'>";
    out += "<stop offset='0' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#f79959'/>";
    out += "<stop offset='90%' stop-color='#f97821'/>";
    out += "<stop offset='100%' stop-color='#f97821'/>";
    out += "</radialGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-h' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' style='stop-color:peachpuff;  stop-opacity:1;'/>";
    out += "<stop offset='100%' style='stop-color:sienna;  stop-opacity:1'/>";
    out += "</linearGradient>";
    out += "<path id='svgEditorClosePathDefs' style='stroke:black;stroke-width:1px;fill:khaki;'/>";
    out += "<polygon id='svgEditorShapeDefs' style='fill:khaki;stroke:black;vector-effect:non-scaling-stroke;stroke-width:1px;'/>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lrgd2-e1' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' stop-color='lightblue'/>";
    out += "<stop offset='100%' stop-color='#ef5b2b'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-v' spreadMethod='pad' x1='0%' x2='0%' y1='19.2%' y2='100%' gradientTransform='rotate(0)'>";
    out += "<stop offset='0%' style='stop-color: paleturquoise; stop-opacity: 0.84;'/>";
    out += "<stop offset='0.852' style='stop-color: dodgerblue; stop-opacity: 1;'/>";
    out += "</linearGradient>";
    out += "</defs>";
    out += "<path d='M-29.122807017543884,-23.480701754385983l1.1298245614035096,8h4l1.0736842105263094,-8Z' style='fill:none; stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e13_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "<path d='M-28.28239564428316,-17.929824561403535l0.3245009074410099,2.4140350877192844h3.915789473684214l0.3104658197217134,-2.4342004436378275Z' style='fill:url(#lgrd2-peachpuff-sienna-v); stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e4_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "</svg>";
  }

  else if (alto < 50 && medio < 50 && baixo < 50 && baixissimo > 50) {
    //Desenha o copo no nivel baixissomo

    out += "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='415px' height='520px' viewBox='6 5 127 159' preserveAspectRatio='xMidYMid meet'>";
    out += "<rect id='svgEditorBackground' x='0' y='0' width='870' height='520' style='fill:none; stroke: none;'/>";
    out += "<defs id='svgEditorDefs'>";
    out += "<linearGradient gradientTransform='skewX(-45)' gradientUnits='objectBoundingBox' id='lgrd2-c-repeat' spreadMethod='repeat' x1='0%' x2='5%' y1='0%' y2='0%'>";
    out += "<stop offset='5%' stop-color='red' stop-opacity='0.8'/>";
    out += "<stop offset='75%' stop-color='yellow' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientTransform='rotate(90)' gradientUnits='objectBoundingBox' id='lgrd3-a' spreadMethod='repeat' x1='10%' x2='20%' y1='20%' y2='20%'>";
    out += "<stop offset='5%' stop-color='#babc91' stop-opacity='1'/>";
    out += "<stop offset='50%' stop-color='#c8d591' stop-opacity='1'/>";
    out += "<stop offset='90%' stop-color='#afd591' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<radialGradient cx='50%' cy='50%' fx='50%' fy='50%' id='rgrd2-white-blue' r='50%'>";
    out += "<stop offset='0%' style='stop-color:white;stop-opacity:0'/>";
    out += "<stop offset='100%' style='stop-color:blue;stop-opacity:1'/>";
    out += "</radialGradient>";
    out += "<radialGradient gradientUnits='objectBoundingBox' id='rgrd-wave' r='5%' spreadMethod='reflect'>";
    out += "<stop offset='0%' stop-color='navy' stop-opacity='1.00'/>";
    out += "<stop offset='20%' stop-color='navy' stop-opacity='0.90'/>";
    out += "<stop offset='40%' stop-color='navy' stop-opacity='0.65'/>";
    out += "<stop offset='60%' stop-color='navy' stop-opacity='0.35'/>";
    out += "<stop offset='80%' stop-color='navy' stop-opacity='0.10'/>";
    out += "<stop offset='100%' stop-color='navy' stop-opacity='0.00'/>";
    out += "</radialGradient>";
    out += "<radialGradient cy='60%' fx='45%' fy='45%' gradientUnits='objectBoundingBox' id='rgrd11-b' r='2'>";
    out += "<stop offset='0%' stop-color='#48afc1'/>";
    out += "<stop offset='10%' stop-color='#b4c63b'/>";
    out += "<stop offset='20%' stop-color='#ef5b2b'/>";
    out += "<stop offset='30%' stop-color='#503969'/>";
    out += "<stop offset='40%' stop-color='#ab6294'/>";
    out += "<stop offset='50%' stop-color='#1cb98f'/>";
    out += "<stop offset='60%' stop-color='#48afc1'/>";
    out += "<stop offset='70%' stop-color='#b4c63b'/>";
    out += "<stop offset='80%' stop-color='#ef5b2b'/>";
    out += "<stop offset='90%' stop-color='#503969'/>";
    out += "<stop offset='100%' stop-color='#ab6294'/>";
    out += "</radialGradient>";
    out += "<radialGradient fx='30%' fy='30%' gradientUnits='objectBoundingBox' id='rgrd11-a'>";
    out += "<stop offset='0' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#f79959'/>";
    out += "<stop offset='90%' stop-color='#f97821'/>";
    out += "<stop offset='100%' stop-color='#f97821'/>";
    out += "</radialGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-h' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' style='stop-color:peachpuff;  stop-opacity:1;'/>";
    out += "<stop offset='100%' style='stop-color:sienna;  stop-opacity:1'/>";
    out += "</linearGradient>";
    out += "<path id='svgEditorClosePathDefs' style='stroke:black;stroke-width:1px;fill:khaki;'/>";
    out += "<polygon id='svgEditorShapeDefs' style='fill:khaki;stroke:black;vector-effect:non-scaling-stroke;stroke-width:1px;'/>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lrgd2-e1' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' stop-color='lightblue'/>";
    out += "<stop offset='100%' stop-color='#ef5b2b'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-v' spreadMethod='pad' x1='0%' x2='0%' y1='19.2%' y2='100%' gradientTransform='rotate(0)'>";
    out += "<stop offset='0%' style='stop-color: paleturquoise; stop-opacity: 0.84;'/>";
    out += "<stop offset='0.852' style='stop-color: dodgerblue; stop-opacity: 1;'/>";
    out += "</linearGradient>";
    out += "</defs>";
    out += "<path d='M-29.122807017543884,-23.480701754385983l1.1298245614035096,8h4l1.0736842105263094,-8Z' style='fill:none; stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e13_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "<path d='M-28.07719298245618,-16.442105263157924l0.11929824561402924,0.9263157894736729h3.915789473684214l0.10526315789473273,-0.9122807017543835Z' style='fill:url(#lgrd2-peachpuff-sienna-v); stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e4_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "</svg>";    
  }

  else {
    //Desenha o copo seco

    out += "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='415px' height='520px' viewBox='5.99138 5 127.017 159' preserveAspectRatio='xMidYMid meet'>";
    out += "<rect id='svgEditorBackground' x='0' y='0' width='870' height='520' style='fill:none; stroke: none;'/>";
    out += "<defs id='svgEditorDefs'>";
    out += "<linearGradient gradientTransform='skewX(-45)' gradientUnits='objectBoundingBox' id='lgrd2-c-repeat' spreadMethod='repeat' x1='0%' x2='5%' y1='0%' y2='0%'>";
    out += "<stop offset='5%' stop-color='red' stop-opacity='0.8'/>";
    out += "<stop offset='75%' stop-color='yellow' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientTransform='rotate(90)' gradientUnits='objectBoundingBox' id='lgrd3-a' spreadMethod='repeat' x1='10%' x2='20%' y1='20%' y2='20%'>";
    out += "<stop offset='5%' stop-color='#babc91' stop-opacity='1'/>";
    out += "<stop offset='50%' stop-color='#c8d591' stop-opacity='1'/>";
    out += "<stop offset='90%' stop-color='#afd591' stop-opacity='1'/>";
    out += "</linearGradient>";
    out += "<radialGradient cx='50%' cy='50%' fx='50%' fy='50%' id='rgrd2-white-blue' r='50%'>";
    out += "<stop offset='0%' style='stop-color:white;stop-opacity:0'/>";
    out += "<stop offset='100%' style='stop-color:blue;stop-opacity:1'/>";
    out += "</radialGradient>";
    out += "<radialGradient gradientUnits='objectBoundingBox' id='rgrd-wave' r='5%' spreadMethod='reflect'>";
    out += "<stop offset='0%' stop-color='navy' stop-opacity='1.00'/>";
    out += "<stop offset='20%' stop-color='navy' stop-opacity='0.90'/>";
    out += "<stop offset='40%' stop-color='navy' stop-opacity='0.65'/>";
    out += "<stop offset='60%' stop-color='navy' stop-opacity='0.35'/>";
    out += "<stop offset='80%' stop-color='navy' stop-opacity='0.10'/>";
    out += "<stop offset='100%' stop-color='navy' stop-opacity='0.00'/>";
    out += "</radialGradient>";
    out += "<radialGradient cy='60%' fx='45%' fy='45%' gradientUnits='objectBoundingBox' id='rgrd11-b' r='2'>";
    out += "<stop offset='0%' stop-color='#48afc1'/>";
    out += "<stop offset='10%' stop-color='#b4c63b'/>";
    out += "<stop offset='20%' stop-color='#ef5b2b'/>";
    out += "<stop offset='30%' stop-color='#503969'/>";
    out += "<stop offset='40%' stop-color='#ab6294'/>";
    out += "<stop offset='50%' stop-color='#1cb98f'/>";
    out += "<stop offset='60%' stop-color='#48afc1'/>";
    out += "<stop offset='70%' stop-color='#b4c63b'/>";
    out += "<stop offset='80%' stop-color='#ef5b2b'/>";
    out += "<stop offset='90%' stop-color='#503969'/>";
    out += "<stop offset='100%' stop-color='#ab6294'/>";
    out += "</radialGradient>";
    out += "<radialGradient fx='30%' fy='30%' gradientUnits='objectBoundingBox' id='rgrd11-a'>";
    out += "<stop offset='0' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9caaa'/>";
    out += "<stop offset='20%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f9ae7a'/>";
    out += "<stop offset='40%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#f79959'/>";
    out += "<stop offset='60%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#fc9149'/>";
    out += "<stop offset='80%' stop-color='#f79959'/>";
    out += "<stop offset='90%' stop-color='#f97821'/>";
    out += "<stop offset='100%' stop-color='#f97821'/>";
    out += "</radialGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-h' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' style='stop-color:peachpuff;  stop-opacity:1;'/>";
    out += "<stop offset='100%' style='stop-color:sienna;  stop-opacity:1'/>";
    out += "</linearGradient>";
    out += "<path id='svgEditorClosePathDefs' style='stroke:black;stroke-width:1px;fill:khaki;'/>";
    out += "<polygon id='svgEditorShapeDefs' style='fill:khaki;stroke:black;vector-effect:non-scaling-stroke;stroke-width:1px;'/>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lrgd2-e1' spreadMethod='pad' x1='0%' x2='100%' y1='0%' y2='0%'>";
    out += "<stop offset='0%' stop-color='lightblue'/>";
    out += "<stop offset='100%' stop-color='#ef5b2b'/>";
    out += "</linearGradient>";
    out += "<linearGradient gradientUnits='objectBoundingBox' id='lgrd2-peachpuff-sienna-v' spreadMethod='pad' x1='0%' x2='0%' y1='19.2%' y2='100%' gradientTransform='rotate(0)'>";
    out += "<stop offset='0%' style='stop-color: paleturquoise; stop-opacity: 0.84;'/>";
    out += "<stop offset='0.852' style='stop-color: dodgerblue; stop-opacity: 1;'/>";
    out += "</linearGradient>";
    out += "</defs>";
    out += "<path d='M-29.122807017543884,-23.480701754385983l1.1298245614035096,8h4l1.0736842105263094,-8Z' style='fill:none; stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e13_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "<path d='M-27.94039120733816,-15.587094171645449l-0.0004033075434080047,0.07130469796122263h3.9157894736842103l0.0026618274062784053,-0.07436983248588547Z' style='fill:url(#lgrd2-peachpuff-sienna-v); stroke:black; vector-effect:non-scaling-stroke;stroke-width:1px;' id='e4_shape' transform='matrix(17.8125 0 0 17.8125 532.75 430.75)'/>";
    out += "</svg>";
  }

  server.send(200, "image/svg+xml", out);
}


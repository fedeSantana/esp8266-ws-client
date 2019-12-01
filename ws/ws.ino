#include <ESP8266WiFi.h> 
#include <ESP8266WiFiMulti.h>

#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>


const char* ssid = "NOBODY";
const char* password = "pachu1234";

const char *host = "192.168.1.106";
uint16_t port = 3006;
const char *url = "/ws";


ESP8266WiFiMulti wifiMulti;

WebSocketsClient ws;

void connectToWifi(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);
    Serial.print("Conectado a: ");
    Serial.print(ssid);
    
    Serial.println(" ...");

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(++i);
        Serial.print(' ');
    }

    Serial.println('\n');
    Serial.println("Conexion establecida!");
    Serial.print("IP Address:\t");
    Serial.println(WiFi.localIP());
}

void wsEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("[WS] Desconectado.");
        break;
    
    case WStype_CONNECTED:
        Serial.println("[WS] Connectado.");
        ws.sendTXT("Connected.");
    
    case WStype_TEXT:
        
        Serial.printf("Texto obtenido");
        for (int i = 0; i < length; i++)
        {
            Serial.printf("%c", payload[i]);
        }
        Serial.println();
        
    default:
        break;
    }
}

void startWebSocket(const char *host, uint16_t port, const char *url)
{
    ws.begin(host, port, url);
    ws.onEvent(wsEvent);
    ws.setReconnectInterval(5000);
    ws.enableHeartbeat(15000, 3000, 2);
}

void setup()
{
    Serial.begin(115200);
    delay(5000);
    connectToWifi(ssid, password);
    startWebSocket(host, port, url);
}

void loop() {
    ws.loop();

}
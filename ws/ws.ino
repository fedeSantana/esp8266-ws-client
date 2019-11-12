#include <ESP8266WiFi.h> 
#include <ESP8266WiFiMulti.h>

#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>


const char* ssid = "Delfos";
const char* password = "GnothiSeauton";

const char *host = "192.168.0.16";
uint16_t port = 3001;
const char *url = "/ws";


ESP8266WiFiMulti wifiMulti;

WebSocketsClient ws;

void connectToWifi(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);
    Serial.print("Connecting to: ");
    Serial.print(ssid);
    
    Serial.println(" ...");

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(++i);
        Serial.print(' ');
    }

    Serial.println('\n');
    Serial.println("Connection established!");
    Serial.print("IP Address:\t");
    Serial.println(WiFi.localIP());
}

void wsEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("[WS] Disconnect.");
        break;
    
    case WStype_CONNECTED:
        Serial.println("[WS] Connected.");
        ws.sendTXT("Connected.");
    
    case WStype_TEXT:
        
        Serial.printf("get text:");
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

    if (Serial.available() > 0) {
        if (Serial.read() == 'p')
        {
            ws.sendTXT("Hola Pepe");
        }
    }
}
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>


const uint8_t fingerprint[20] = {
    0x56,
    0x26,
    0xb0,
    0xbc,
    0xd7,
    0xca,
    0x44,
    0xbb,
    0xab,
    0x48,
    0x3c,
    0xbd,
    0x01,
    0x57,
    0xea,
    0x0b,
    0x83,
    0xd6,
    0xe9,
    0x05,
};

const uint8_t fingerprint_fabio[20] = {
    0xe6,
    0xef,
    0xe4,
    0x7d,
    0x79,
    0x67,
    0xb7,
    0x41,
    0xd4,
    0x8d,
    0xbb,
    0x3a,
    0x95,
    0x69,
    0x85,
    0x72,
    0x7f,
    0x7f,
    0x81,
    0x62,
};


const char *ssid = "UTN.BA";
const char *url = "https://gw2.frba.utn.edu.ar/unlock.php";
const char *url_captive = "https://gw2.frba.utn.edu.ar";
const char *url_fabio = "https://www.fabio.com.ar";

const char *set_cookie = "Set-Cookie";
const char *headers[] = {set_cookie};

void connectToWifi(const char *ssid)
{
    WiFi.begin(ssid);
    Serial.print("Connecting to: ");
    Serial.print(ssid);

    Serial.println(" ...");

    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(++i);
        Serial.print(' ');
    }

    Serial.println('\n');
    Serial.println("Connection established!");
    Serial.print("IP Address:\t");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    delay(5000);
    connectToWifi(ssid);

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprint);

    String cookie;
    {
        HTTPClient https;
        https.begin(*client, url);
        https.collectHeaders(headers, 1);
        int httpCode = https.GET();
        cookie = https.header(set_cookie);
        delay(2000);
    }

    {
        HTTPClient https;

        Serial.print("[HTTPS] Begin... \n");
        if (https.begin(*client, url))
        {
            Serial.printf("[HTTPS] POST...\n");

            https.addHeader("Content-Type", "application/x-www-form-urlencoded");
            https.addHeader("Cookie", cookie);

            String postData = "user_sinap=atoledo&pass_sinap=wikipedia123&type=sinap&url=www.frba.utn.edu.ar";
            int httpCode = https.POST(postData);

            if (httpCode > 0)
            {
                Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
                {
                    String payload = https.getString();
                    Serial.println(payload);
                }
            }
            else
            {
                Serial.printf(
                    "[HTTPS] POST... Failed, error: %s",
                    https.errorToString(httpCode).c_str());
            }

            https.end();
        }
        else
        {
            Serial.printf("[HTTPS] Unable to connect\n");
        }
    }

    delay(3000);

    HTTPClient https;
    client->setFingerprint(fingerprint_fabio);
    Serial.print("[HTTPS] Begin... \n");
    if (https.begin(*client, url_fabio))
    {
        Serial.printf("[HTTPS] GET...\n");
        int httpCode = https.GET();

        if (httpCode > 0)
        {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
            {
                String payload = https.getString();
                Serial.println(payload);
            }
        }
        else
        {
            Serial.printf(
                "[HTTP] GET... Failed, error: %s",
                https.errorToString(httpCode).c_str());
        }

        https.end();
    }
    else
    {
        Serial.printf("[HTTPS] Unable to connect\n");
    }
}

void loop()
{
}

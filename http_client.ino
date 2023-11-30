#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* SSID   = "your wifi ssid"; 
const char* PASSWD = "your wifi password";

String SERVER_IP         = "192.168.0.0:80";
String TOKEN_KEY         = "";
String TOKEN_VALUE       = "";
String INTEGER_RESOURCES = "/projects/http_client/data_types/integer";
String ARRAY_RESOURCES   = "/projects/http_client/data_types/array";
String json_to_string    = "";

int retries = 0;

void setup()
{
    Serial.begin(115200);
    WiFi.begin(SSID, PASSWD);

    Serial.println("Connecting to Wi-Fi AP...");

    while ((WiFi.status() != WL_CONNECTED) && (retries < 50))
    {
        retries++;
        delay(1000);

        Serial.print(".");

        if (retries > 50)
        {
            Serial.print("[ERROR] Failed to Connect ");
            Serial.println(SSID);

            EspClass esp_module;
            esp_module.restart();
        }       
        else if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("Connected to ");
            Serial.print(SSID);
            Serial.print("--- IP: ");
            Serial.println(WiFi.localIP());
            retries = 0;
        }
    }

    
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClient wifi_client;
        HTTPClient http_client;

        http_client.begin(wifi_client, "http://" + SERVER_IP + ARRAY_RESOURCES);
        // http_client.addHeader(TOKEN_KEY, TOKEN_VALUE);
        http_client.addHeader("Content-Type", "application/json"); //"text/plain"

        convertJsonArrayDocument();
        // convertJsonDocument();

        int server_response_code = http_client.POST(json_to_string);
        json_to_string.clear();

        if (server_response_code > 0)
        {
            String response = http_client.getString();
            Serial.println(response);
        }
        else
        {
            Serial.print("[ERROR] ERROR NUMBER : ");
            Serial.println(server_response_code);
        }

        http_client.end();
    }
    else if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();

        Serial.println("Reconnecting to Wi-Fi AP...");

        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
        
            delay(500);
            retries++;

            if (retries > 50)
            {
                Serial.print("[ERROR] Failed to Connect ");
                Serial.println(SSID);

                EspClass esp_module;
                esp_module.restart();
            }
            else if (WiFi.status() == WL_CONNECTED)
            {
                Serial.print("Reconnected to ");
                Serial.print(SSID);
                Serial.print("--- IP: ");
                Serial.println(WiFi.localIP());
                retries = 0;
            }
        }
    }
}

void convertJsonArrayDocument()
{
    StaticJsonDocument<300> doc;

    JsonArray depth_array = doc.createNestedArray("value");

    for (uint8_t i = 0; i < 15; i++)
    {
        depth_array.add(i);
    }

    serializeJsonPretty(doc, json_to_string);
    doc.clear();
    Serial.print(json_to_string);
}

void convertJsonDocument()
{
    StaticJsonDocument<300> doc;

    doc["value"] = 777;

    serializeJsonPretty(doc, json_to_string);
    doc.clear();
    Serial.print(json_to_string);
}
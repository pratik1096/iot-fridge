#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#endif
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);
long now = millis();
long lastMeasure = 0;
const char* ssid = "ssid";
const char* password = "psswd";
String payload;
const char* host = "0.0.0.0:0000/myfile";
const char* serverName = "0.0.0.0:0000/myfile";
const char* cloudserverName = "0.0.0.0:0000/myfile";
String apiKeyValue = "**********";
String productcode = "************";
String uniquekey = "**********";
String modelname = "LM";
int outputpin= A0;
float millivolts;
int analogValue;
float celsius;
int p1;
String date;
int p2;
String currenttime;
int p3;
String hh;
int p4;
String mm;

void setup() {

  Serial.begin(115200);
  pinMode(outputpin,INPUT);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}
void gettime() {
  HTTPClient http;
  http.begin(host);
  int httpCode = http.GET();
  if (httpCode > 0) {
    payload = http.getString();
    // Serial.println(payload);
    p1 = payload.indexOf('&');
    date = payload.substring(0, p1);
    p2 = payload.indexOf('&', p1 + 1 );
    currenttime = payload.substring(p1 + 1, p2 + 1);
    Serial.println(date);
    currenttime.remove(8);
    //  Serial.println(fuel);
    p3 = currenttime.indexOf(':');
    hh = currenttime.substring(0, p3);
    Serial.println(hh);
    p4 = currenttime.indexOf(':', p3 + 1 );
    mm = currenttime.substring(p3 + 1, p4 + 1);
    mm.remove(2);
    Serial.println(mm);
  }
  http.end();
  delay(1000);
}
void server() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData =  "api_key=" + apiKeyValue + "&product_code=" + productcode + "&model_name=" + modelname + "&unique_key=" + uniquekey + "&temperature=" +  celsius +  "" ;
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }

  delay(2000);
}

void cloudserver() {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(cloudserverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    //    light=analogRead(ldr);
    String httpRequestData =   "api_key=" + apiKeyValue + "&product_code=" + productcode + "&model_name=" + modelname + "&unique_key=" + uniquekey +
                               "&temperature=" + celsius + "&date=" +  date +  "&hh=" + hh +  "&mm=" + mm + "" ;
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }

  delay(2000);
}

void loop()
{
  analogValue = analogRead(outputpin);
  millivolts = (analogValue / 1024.0) * 3300; //3300 is the voltage provided by NodeMCU
  celsius = millivolts / 10;
  Serial.print("in DegreeC=   ");
  Serial.println(celsius);
  gettime();
  server();
  cloudserver();

}

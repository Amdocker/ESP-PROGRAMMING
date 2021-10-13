#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <WiFiClientSecure.h>
#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif
#include <ESP8266WiFi.h>


String GAS_ID = "AKfycbwagOm99qeDxLk8VEYL7XCwp7Hfs7bOTT8Qx_5W";


const char* fingerprint = "11b660d85865e626ec67613a2af489e4a6deb344";


const char* host = "script.google.com";

const int httpsPort = 443;
const char* ssid = "";
const char* password = "FSQ54Wxv";
float humidite ;
float temp ;

// Create an instance of the server
// specify the port to listen on as an argument


WiFiClientSecure gsclient;
WiFiServer server(80) ;
DHTesp dht ;

int green_led = 5 ;
bool valbp ; 
int Pinbp = 14 ;
int Statebp = 0 ;

String green_ledState = "" ;

void sendData(int temp, int hum,String led, bool bp) {
//gsclient.setInsecure();
Serial.print("connecting to ");
Serial.println(host);
if (!gsclient.connect(host, httpsPort)) {
Serial.println("connection failed");
return;
}

String strTemp = String(temp, DEC);
String strHum = String(hum, DEC);
String strStateled = led;
String strStatebp = String(bp);

Serial.println(strTemp);
Serial.println(strHum);
String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + strTemp + "&humidity=" + strHum + "&statebp=" + strStatebp + "&stateled=" + strStateled ;
Serial.print("requesting URL: ");
Serial.println(url);
gsclient.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"User-Agent: BuildFailureDetectorESP8266\r\n" +
"Connection: close\r\n\r\n");
Serial.println("request sent");
Serial.println("closing connection");
}

void setup()
{
  pinMode(green_led, OUTPUT);
  pinMode(Pinbp, INPUT_PULLUP);
  digitalWrite(green_led, HIGH);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  String thisBoard = "ARDUINO_BOARD";
  Serial.println(thisBoard);
  gsclient.setFingerprint(fingerprint);

  // Autodetect is not working reliable, don't use the following line
  // dht.setup(17);
  // use this instead: 
  dht.setup(D0, DHTesp::DHT11); // Connect DHT sensor to GPIO 17

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());

}
  
void loop()
{
  valbp = digitalRead(Pinbp);
  if (!valbp){
    Statebp = not Statebp ;
    Serial.println(Statebp) ;
    delay(40);
    Serial.print("\n");
  }

  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  sendData(humidity,temperature,green_ledState,Statebp);
  
  Serial.print(humidity);
  Serial.print(temperature);
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.print(dht.toFahrenheit(temperature), 1);
  Serial.print("\t\t");
  Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  Serial.print("\t\t");
  Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);

  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(1000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  int val;
  if (req.indexOf(F("/5/ON")) != -1) {
    val = 1;
    green_ledState = "OFF";
  } else if (req.indexOf(F("/5/OFF")) != -1) {
    val = 0;
    green_ledState = "ON";
  } else {
    Serial.println(F("invalid request"));
    val = digitalRead(green_led);
  }

  // Set LED according to the request
  digitalWrite(green_led, val);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #77878A;}</style></head>");
  
  // Web Page Heading
  client.println("<body><h1>TP3 Système embarqué : Web Server ESP8266 </h1>");
  client.println("<h1>avec stockage des données sur un fichier Google Sheet </h1>");
  
  // Display current state, and ON/OFF buttons for GPIO 5  
  client.println("<b><p>État de la LED :</b> " + green_ledState + "</p>");
  // If the green_ledState is off, it displays the ON button       
  if (green_ledState== "ON" ) {
    client.println("<p><a href='http://");
    client.print(WiFi.localIP());
    client.println("/5/ON'><button class=\"button\">OFF</button></a></p>");
  } else {
    
    client.println("<p><a href='http://");
    client.print(WiFi.localIP());
    client.println("/5/OFF'><button class=\"button\">ON</button></a></p>");
  }

  client.print(F("<br><b>Température :</b>"));
  client.print("<br>");
  client.print(temperature);
  client.print("°");
  
  client.print("<br>");
  
  client.print(F("<br><b>Humidité :</b>"));
  client.print("<br>");
  client.print(humidity);
  client.print("%");
  client.print("<br>");
  
  client.print(F("<br><b>Valeur du bouton :</b>"));
  client.print("<br>");
  client.print(Statebp);

  // Permet de recharger la page toutes les 5000 ms
  client.print(F("<script>setTimeout(function(){window.location.reload(1);}, 5000);</script>"));
  
  delay(100);
  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
    
}

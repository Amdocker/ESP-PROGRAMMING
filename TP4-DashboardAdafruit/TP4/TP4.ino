
#include <AdafruitIO.h> // tiến hành thêm thư viện AdafruitIO.h vào

#define IO_USERNAME  "amdockerino"
#define IO_KEY       "aio_woqT21j5TmqGc9g4YMuRhLRgw5EZ"

#define WIFI_SSID "DESKTOP-546JDCC 4459"
#define WIFI_PASS "fayenapote"  // Pass wifi

#include <AdafruitIO_WiFi.h>  // Khai báo thư viện AdafruitIO_WiFi.h để kết nối đến server.
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);  // Gọi hàm kết nối đến server.
#include <Adafruit_Sensor.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHT_PIN 22

DHT_Unified dht(DHT_PIN, DHT11);

/************************ Example Starts Here *******************************/

// default PWM pins for ESP8266.
// you should change these to match PWM pins on other platforms.
#define RED_PIN   5
#define GREEN_PIN 17
#define BLUE_PIN  16
#define LED_PIN 23 


// set up the 'color' feed
AdafruitIO_Feed *color = io.feed("color");
AdafruitIO_Feed *color1 = io.feed("color1");
AdafruitIO_Feed *color2 = io.feed("color2");


// set up the 'digital' feed 
AdafruitIO_Feed *digital = io.feed("bouton"); // khai báo con trỏ digital để chứ dữ liệu lấy từ feed của server.

// set up the 'temperature' and 'humidity' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");

void setup() {

  // start the serial connection
  Serial.begin(115200);
  
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,HIGH);

  // wait for serial monitor to open
  while(! Serial);
   

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  // assign rgb pins to channels
  ledcAttachPin(RED_PIN, 1);
  ledcAttachPin(GREEN_PIN, 2);
  ledcAttachPin(BLUE_PIN, 3);
  // init. channels
  ledcSetup(1, 12000, 8);
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);

  // set up a message handler for the 'color' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  color->onMessage(handleMessage);
  color1-> onMessage(handleMessage1);
  color2-> onMessage(handleMessage2);
  digital->onMessage(handleMessageLed); // Gọi hàm đọc dữ liệu và tiến hành điều khiển led và xuất ra trạng thái trên màn hình.


  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  color->get();
  color1->get();
  color2->get();

  digital->get(); // lấy dữ liệu từ feed 'digital' của server.


}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = event.temperature;
  float fahrenheit = (celsius * 1.8) + 32;

  Serial.print("celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(celsius);

  dht.humidity().getEvent(&event);

  Serial.print("humidity: ");
  Serial.print(event.relative_humidity);
  Serial.println("%");

  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);

  // wait 5 seconds (5000 milliseconds == 5 seconds)
  delay(10000);

}

// this function is called whenever a 'color' message
// is received from Adafruit IO. it was attached to
// the color feed in the setup() function above.


void handleMessage(AdafruitIO_Data *data) {

  // print RGB values and hex value
  Serial.println("Received:");
  Serial.print("  - G: ");
  Serial.println(data->value());
  
  String valeurRed = data->value();
  int valeurRedLed = valeurRed.toInt();
  ledcWrite(1, valeurRedLed);
  

}
void handleMessage1(AdafruitIO_Data *data) {

  // print RGB values and hex value
  Serial.println("Received:");
  Serial.print("  - B: ");
  Serial.println(data->value());

  String valeurGreen = data->value();
  int valeurGreenLed = valeurGreen.toInt();
  ledcWrite(2, valeurGreenLed);
  
}
void handleMessage2(AdafruitIO_Data *data) {

  // print RGB values and hex value
  Serial.println("Received:");
  Serial.print("  - R: ");
  Serial.println(data->value());

  String valeurBlue = data->value();
  int valeurBlueLed = valeurBlue.toInt();
  ledcWrite(3, valeurBlueLed);
  
}

void handleMessageLed(AdafruitIO_Data *data) { 

  Serial.print("received <- ");
  Serial.print(data->toPinLevel());
 
  if(data->toPinLevel() == HIGH)
    Serial.println("HIGH");
  else
    Serial.println("LOW");

  // write the current state to the led
  digitalWrite(LED_PIN,! data->toPinLevel());
 
}

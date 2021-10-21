
#include <AdafruitIO.h> // tiến hành thêm thư viện AdafruitIO.h vào

#define IO_USERNAME  "amdockerino"
#define IO_KEY       "aio_TeWc31eSnHUEfpqKzH0gYkf2P1rM"

#define WIFI_SSID "DESKTOP-546JDCC 4459"
#define WIFI_PASS "fayenapote"  // Pass wifi

#include <AdafruitIO_WiFi.h>  // Khai báo thư viện AdafruitIO_WiFi.h để kết nối đến server.
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);  // Gọi hàm kết nối đến server.

#define LED_PIN 23 // LED on Board là GPIO 2.
#define LEDR_PIN 25 // LED on Board là GPIO 2.
#define LEDG_PIN 33 // LED on Board là GPIO 2.
#define LEDV_PIN 35 // LED on Board là GPIO 2.


// set up the 'digital' feed 
AdafruitIO_Feed *digital = io.feed("bouton Toggle"); // khai báo con trỏ digital để chứ dữ liệu lấy từ feed của server.

void setup() {
 
  // set led pin as a digital output
  pinMode(LED_PIN, OUTPUT); // Khai báo output.
  pinMode(LEDR_PIN, OUTPUT); // Khai báo output.
  pinMode(LEDG_PIN, OUTPUT); // Khai báo output.
  pinMode(LEDV_PIN, OUTPUT); // Khai báo output.
  
  digitalWrite(LED_PIN,HIGH);
 
  // start the serial connection
  Serial.begin(115200); 
 
  // wait for serial monitor to open
  while(! Serial);
 
  // connect to io.adafruit.com
  Serial.println("Connecting to Adafruit IO"); // tiến hành kết nối đến server.
  io.connect(); // Gọi hàm kết nối
 digital->onMessage(handleMessage);
 
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.println("Can't connect to Adafruit IO  "); // Nếu chưa kết nối được đến server sẽ tiến hành xuất ra màn hình đấu "."
    delay(500);
  }
 
  // we are connected
  Serial.println();
  Serial.println(io.statusText()); // Nếu đã kết nối thành công tiến hành xuất ra màn hình trạng thái.
  // set up a message handler for the 'digital' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  digital->get(); // lấy dữ liệu từ feed 'digital' của server.
  digital->onMessage(handleMessage); // Gọi hàm đọc dữ liệu và tiến hành điều khiển led và xuất ra trạng thái trên màn hình.
}

void loop() {
  io.run(); // gọi hàm Run.
}

// this function is called whenever an 'digital' feed message
// is received from Adafruit IO. it was attached to
// the 'digital' feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) { // hàm handleMessage để đọc dữ liệu.

 // xuất ra màn hình trạng thái của nút nhấn trên feed vừa đọc được.
  Serial.print("received <- ");
  Serial.print(data->toPinLevel());
 
  if(data->toPinLevel() == HIGH)
    Serial.println("HIGH");
  else
    Serial.println("LOW");

 // cài đặt trạng thái bật tắt led on board tương ứng với nút nhấn.
  // write the current state to the led
  digitalWrite(LED_PIN,! data->toPinLevel());
 
}

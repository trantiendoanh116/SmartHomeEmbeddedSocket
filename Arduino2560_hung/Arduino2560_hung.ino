#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
#include <DHT.h> // Gọi thư viện DHT22
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
RCSwitch FanSwitch = RCSwitch();
const int DHTPIN = 12; //Đọc dữ liệu từ DHT22 ở chân D2 trên mạch Arduino
const int DHTTYPE = DHT22; //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
DHT dht(DHTPIN, DHTTYPE);

const byte RX = 11; //11
const byte TX = 10;  //10        // Chân 2 được dùng làm chân TX

SoftwareSerial mySerial = SoftwareSerial(RX, TX);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

//const int fanOffPin = 4;
//const int fanOnPin = 5;

const int speed1Pin = 5;
const int speed2Pin = 6;
const int speed3Pin = 7;

//const int lightControlPin = 11;
const int lightStatusPin = 9;// Cổng số 9 bị lỗi không set được OUTPUT

//const int ATControlPin = 12;
const int ATStatusPin = 3;


void setup() {
  //Khởi tạo Serial ở baudrate 115200 để debug ở serial monitor
  Serial.begin(115200);

  mySwitch.enableTransmit(5);
  //FanSwitch.enableTransmit(11);
  onClickButtonOffFan();
  dht.begin(); // Khởi động cảm biến nhiệt
  //Khởi tạo Serial ở baudrate 115200 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
  mySerial.begin(57600);


  pinMode (lightStatusPin, INPUT);
  pinMode (ATStatusPin, INPUT);
  pinMode(speed1Pin, INPUT);
  pinMode(speed2Pin, INPUT);
  pinMode(speed3Pin, INPUT);

  // digitalWrite(fanOffPin, HIGH);
  Serial.println("Da san sang nhan lenh");
  //Xử lý thực hiện lệnh từ server
  sCmd.addCommand("CONTROL", processControl);

}
unsigned long chuky = 0;
const unsigned long TIME_RETRIEVE_DATA = 30000UL; //Cứ sau 2000ms = 5s thì chu kỳ lặp lại

void loop() {
  if (millis() - chuky > TIME_RETRIEVE_DATA) {
    chuky = millis();
    sendStatusLight();
    delay(20);
    sendStatusFan();
    delay(20);
    sendStatusAptomat();
    delay(20);
    sendTempHumi();
    delay(20);
  }
  sCmd.readSerial();
  delay(500);
}

void processControl() {
  char *json = sCmd.next();
  Serial.println(json);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (root.containsKey("light")) {
    onChangeLight();
  }
  if (root.containsKey("apt")) {
    onChangeAptomat();
  }
  if (root.containsKey("fan_on")) {
    onClickButtonOnFan();
  }
  if (root.containsKey("fan_off")) {
    onClickButtonOffFan();
  }
  if (root.containsKey("init")) {
    Serial.println("Init -> get all value of device");
    sendStatusLight();
    delay(20);
    sendStatusFan();
    delay(20);
    sendStatusAptomat();
    delay(20);
    sendTempHumi();
    delay(20);
  }


}

// *****************************************
//   Cảnh báo nhiệt độ bếp
//******************************************

void nhietdo_bep()
{
  float h = dht.readHumidity(); //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ

  Serial.print("Nhiet do: ");
  Serial.println(t); //Xuất nhiệt độ
  Serial.print("Do am: ");
  Serial.print(h); //Xuất độ ẩm
  Serial.println("%");
  delay(100);
}

// *****************************************
//   Tắt quạt trần tầng một
// ******************************************

void onClickButtonOffFan() {
  unsigned long address = 0b0101010101010100;
  Serial.println("ACTION -> OFF FAN");
  mySwitch.send((address << 8) | 0b00001100, 24);
  delay(80);
  mySwitch.send((address << 8) | 0b00000000, 24);

  sendStatusFan();
}
// *****************************************
//   Bật quạt trần tầng một
//******************************************

void onClickButtonOnFan() {
  unsigned long address = 0b0101010101010100;
  Serial.println("ACTION -> ON FAN");
  mySwitch.send((address << 8) | 0b00000011, 24);
  delay(80);
  mySwitch.send((address << 8) | 0b00000000, 24);

  sendStatusFan();
}
// *****************************************
//   Bật/tắt đèn P.khách01
//******************************************

void onChangeLight() {
  unsigned long address = 0b0101010101010101;
  Serial.println("ACTION -> CHANGE LIGHT");

  mySwitch.send((address << 8) | 0b00110000, 24);
  delay(80);
  mySwitch.send((address << 8) | 0b00000000, 24);
  sendStatusLight();
}
// *****************************************
//   Bật/tắt Atomat tổng
//******************************************
void onChangeAptomat() {
  unsigned long address = 0b0101010101010101;
  Serial.println("ACTION -> CHANGE APTOMAT");
  mySwitch.send((address << 8) | 0b00000011, 24);
  delay(80);
  mySwitch.send((address << 8) | 0b00000000, 24);
  sendStatusAptomat();
}

void sendTempHumi() {
  float h = dht.readHumidity(); //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ
  float co = random(30, 70);

  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
  root["temp"] = t;
  root["humi"] = h;
  root["co"] = co;

  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("DATA");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root.printTo(mySerial); //gửi chuỗi JSON
  mySerial.print('\r');           // gửi \r
  Serial.print("TEMP_HUMI_CO: ");
  root.printTo(Serial);
  Serial.print('\n');

}
// *****************************************
//   Đọc trạng thái đèn P.khách01
//******************************************

void sendStatusLight() {
  int lightStatus = !digitalRead(lightStatusPin);
   Serial.println("Value light: " + digitalRead(lightStatusPin));
  //Send data to ESP8266
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root2 = jsonBuffer2.createObject();
  root2["light"] = lightStatus;
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("DATA");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root2.printTo(mySerial); //gửi chuỗi JSON
  mySerial.print('\r');           // gửi \r
  Serial.print("LIGHT: ");
  root2.printTo(Serial);
  Serial.print('\n');
}
// *****************************************
//   Đọc trạng thái quạt trần tầng một.
//******************************************

void sendStatusFan() {
  // read the state of the level fan value:
  int speed1Status = !digitalRead(speed1Pin);
  int speed2Status = !digitalRead(speed2Pin);
  int speed3Status = !digitalRead(speed3Pin);
  //Send data to ESP8266
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root2 = jsonBuffer2.createObject();
  if(speed1Status == 1){
    root2["fan"] = 3;
  }else if (speed2Status == 1){
    root2["fan"] = 2;
  }else if (speed3Status == 1){
    root2["fan"] = 1;
  }else{
    root2["fan"] = 0;
  }
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("DATA");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root2.printTo(mySerial); //gửi chuỗi JSON
  mySerial.print('\r');           // gửi \r

  //in ra Serial Arduino để debug
  Serial.print("FAN:  ");
  root2.printTo(Serial); //Xuống dòng
  Serial.print('\n');
}
// *****************************************
//   Đọc trạng thái Atomat tổng
//******************************************

void sendStatusAptomat() {
  int statusAT = !digitalRead(ATStatusPin);
  //Send data to ESP8266
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root2 = jsonBuffer2.createObject();
  root2["apt"] = statusAT;
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("DATA");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root2.printTo(mySerial); //gửi chuỗi JSON
  mySerial.print('\r');           // gửi \r

  //in ra Serial Arduino để debug
  Serial.print("APTOMAT: ");
  root2.printTo(Serial); //Xuống dòng
  Serial.print('\n');
}

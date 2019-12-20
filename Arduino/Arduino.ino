#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
//#include <DHT.h> // Gọi thư viện DHT22
//
//const int DHTPIN = 13; //Đọc dữ liệu từ DHT22 ở chân D2 trên mạch Arduino
//const int DHTTYPE = DHT22; //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
//DHT dht(DHTPIN, DHTTYPE);

const byte RX = 5;          // Chân 3 được dùng làm chân RX
const byte TX = 4;          // Chân 2 được dùng làm chân TX

SoftwareSerial mySerial = SoftwareSerial(RX, TX);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command


const int lightControlPin = 10; //9
const int lightStatusPin = 11; //11



void setup() {
  //Khởi tạo Serial ở baudrate 115200 để debug ở serial monitor
  Serial.begin(57600);
    mySerial.begin(57600);

  //dht.begin(); // Khởi động cảm biến nhiệt



  pinMode(lightControlPin, OUTPUT);
  pinMode (lightStatusPin, INPUT);

  Serial.println("Da san sang nhan lenh");
  //Xử lý thực hiện lệnh từ server
  sCmd.addCommand("CONTROL", processControl);
 


}
unsigned long chuky = 0;
const unsigned long TIME_RETRIEVE_DATA = 10000UL; //Cứ sau 2000ms = 5s thì chu kỳ lặp lại

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
  delay(200);
}

void processControl() {
  char *json = sCmd.next();
  Serial.println(json);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if(root.containsKey("light")){
    onChangeLight();
  }
   if(root.containsKey("apt")){
    onChangeAptomat();
  }
   if(root.containsKey("fan_on")){
    onClickButtonOnFan();
  }
   if(root.containsKey("fan_off")){
    onClickButtonOffFan();
  }
   if(root.containsKey("init")){
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

void onClickButtonOffFan() {
  Serial.println("[FAN]Button OFF clicked");
//  digitalWrite(fanOffPin, LOW);
//  delay(50);
//  digitalWrite(fanOffPin, HIGH);
//  sendStatusFan();
}
// *****************************************
//   Bật quạt trần tầng một
//******************************************

void onClickButtonOnFan() {
  Serial.println("[FAN]Fan on clicked");
//  digitalWrite(fanOnPin, HIGH);
//  delay(50);
//  digitalWrite(fanOnPin, LOW);
//  sendStatusFan();
}


void onChangeLight() {
  int lightStatus = digitalRead(lightStatusPin);
  Serial.println("[LIGHT]Change light");
  digitalWrite(lightControlPin, !lightStatus);
  sendStatusLight();
}

void onChangeAptomat() {
  Serial.println("Change Aptomat");

//  digitalWrite(ATControlPin, HIGH);
//  delay(50);
//  digitalWrite(ATControlPin, LOW);
//  sendStatusAptomat();
}


void sendTempHumi() {
  //float h = dht.readHumidity(); //Đọc độ ẩm
  //float t = dht.readTemperature(); //Đọc nhiệt độ
  float h = random(40, 80); //Đọc độ ẩm
  float t = random(25, 35); //Đọc nhiệt độ
  float co = random(20, 40);
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
  Serial.print("THC_STATUS: ");
  root.printTo(Serial);
  Serial.print('\n');

}
// *****************************************
//   Đọc trạng thái đèn P.khách01
//******************************************

void sendStatusLight() {
  int lightStatus = digitalRead(lightStatusPin);
  //Send data to ESP8266
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root2 = jsonBuffer2.createObject();
  root2["light"] = lightStatus;
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("DATA");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root2.printTo(mySerial); //gửi chuỗi JSON
  mySerial.print('\r');           // gửi \r
  Serial.print("LED: ");
  root2.printTo(Serial);
  Serial.print('\n');
}

// *****************************************
//   Đọc trạng thái quạt trần tầng một.
//******************************************

void sendStatusFan() {
  // read the state of the level fan value:
  int speed1Status = 1;
  int speed2Status = 0;
  int speed3Status = 0;
  //Send data to ESP8266
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root2 = jsonBuffer2.createObject();
  root2["fan"] = 0;
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
  int statusAT = 1;
  //Send data to ESP8266
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root = jsonBuffer2.createObject();
  root["apt"] = statusAT;
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("DATA");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root.printTo(mySerial); //gửi chuỗi JSON
  mySerial.print('\r');           // gửi \r

  //in ra Serial Arduino để debug
  Serial.print("APT: ");
  root.printTo(Serial); //Xuống dòng
  Serial.print('\n');
}

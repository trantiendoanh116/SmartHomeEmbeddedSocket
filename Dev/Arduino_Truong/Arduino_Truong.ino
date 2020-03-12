#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include <RCSwitch.h>
#include <DHT.h>

extern "C"
{
#include "Config.h"
}
//Khai báo serial
SoftwareSerial mySerial = SoftwareSerial(SERIAL_RX, SERIAL_TX);
SerialCommand sCmd(mySerial);
//Khai báo RCSwitch
RCSwitch mySwitch = RCSwitch();
//Khai báo đo nhiệt độ, độ ẩm
const int DHTTYPE = DHT22;
DHT dht(PIN_TEMP_HUMI, DHTTYPE);
void setup()
{
  Serial.begin(115200);
  mySerial.begin(9600);

  mySwitch.enableTransmit(10);

  dht.begin();

  pinMode(PIN_DEN_TRAN_KH2, INPUT);
  pinMode(PIN_DEN_CHUM_KH2, INPUT);
  pinMode(PIN_DEN_TRANH_KH2, INPUT);


  Serial.println("Da san sang nhan lenh");

  sCmd.addCommand("CONTROL", processControl);
}
unsigned long lastUpdatedSensor = 0;
unsigned long lastUpdatedDevice = 0;

bool isFirstLoop = true;
int vDenTranKh2, vDenChumKh2, vDenTranhKh2;

void loop()
{

  if (isFirstLoop)
  {
    Serial.println("First loop");
    isFirstLoop = false;
     resetCN6();

     delay(500);

    //Update value to server
    sendStatusDenTranKh2();
    sendStatusDenChumKh2();
    sendStatusDenTranhKh2();

    sendValueTempHumi();
    sendValueCOBep();
  }
  //Chu trình kiểm tra có thiết bị thay đổi trạng thái
  
  if (millis() - lastUpdatedDevice > SCHEDULE_GET_VALUE_DEVICE)
  {
    checkAndUpdateValueDevice();
  }
  
  //Chu trình cập nhật dữ liệu của các cảm biến
  if (millis() - lastUpdatedSensor > SCHEDULE_GET_VALUE_SENSOR)
  {
    lastUpdatedSensor = millis();
    sendValueTempHumi();
    sendValueCOBep();
  }

  sCmd.readSerial();
  delay(200);
}

void processControl()
{
  char *json = sCmd.next();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(json);
  root.printTo(Serial);
  
  if (root.containsKey(ID_DEN_TRAN_KH2))
  {
    changeDenTranKh2();
  }
  if (root.containsKey(ID_DEN_CHUM_KH2))
  {
    changeDenChumKh2();
  }
  if (root.containsKey(ID_DEN_TRANH_KH2))
  {
    changeDenTranhKh2();
  }
 
  if (root.containsKey("init"))
  {
    Serial.println("Init -> get all value of device");
    sendStatusDenTranKh2();
    sendStatusDenChumKh2();
    sendStatusDenTranhKh2();
    

    sendValueTempHumi();
    sendValueCOBep();
  }
}

void checkAndUpdateValueDevice()
{

  delay(20);
  if (vDenTranKh2 != readStatusDenTranhKh2())
  {
    sendStatusDenTranKh2();
  }
  delay(20);
  if (vDenChumKh2 != readStatusDenChumKh2())
  {
    sendStatusDenChumKh2();
  }
  delay(20);
  if (vDenTranhKh2 != readStatusDenTranhKh2())
  {
    sendStatusDenTranhKh2();
  }
  delay(20);

}
void sendData(JsonObject &data)
{
  mySerial.print("DATA");
  mySerial.print('\r');
  data.printTo(mySerial);
  mySerial.print('\r');
  //Debug
  Serial.print("Send to server: ");
  Serial.print("DATA");
  Serial.print('\r');
  data.printTo(Serial);
  Serial.print('\n');
}
/*----------------Reset CN--------------*/

void resetCN6()
{
  Serial.println("Reset CN6");
  mySwitch.send((ADD_CN6 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN6 << 8) | 0b00000000, 24);
}


/*----------------Đo nhiệt độ, đổ ẩm và khí CO------------*/
void sendValueTempHumi()
{
  float h = dht.readHumidity();    //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ
  //float h = random(60, 80);
  //float t = random(25, 35);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["TEMP"] = t;
  root["HUMI"] = h;

  StaticJsonBuffer<200> jsonBuffer1;
  JsonObject &root1 = jsonBuffer1.createObject();
  root1[ID_TEMP_HUMI] = root;
  sendData(root1);
}
void sendValueCOBep()
{
  //int value = analogRead(A3);
  int value = random(60, 180);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root[ID_KHOI_CO] = value;
  sendData(root);
}


/*----------------DenTranKH2--------------*/
void changeDenTranKh2()
{
  Serial.println("Change DenTranKH2");
  mySwitch.send((ADD_CN6 << 8) | VALUE_DEN_TRAN_KH2, 24);
  delay(100);
  mySwitch.send((ADD_CN6 << 8) | 0b00000000, 24);
  sendStatusDenTranKh2();
}
void sendStatusDenTranKh2()
{
  int value = readStatusDenTranKh2();
  vDenTranKh2 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root[ID_DEN_TRAN_KH2] = value;
  sendData(root);
}

int readStatusDenTranKh2()
{
  int value = digitalRead(PIN_DEN_TRAN_KH2);
  if(value == 0) return 1;
  if (value == 1) return 0;
  return -1;
}
/*----------------DenChumKH2--------------*/
void changeDenChumKh2()
{
  Serial.println("Change DenChumKH2");
  mySwitch.send((ADD_CN6 << 8) | VALUE_DEN_CHUM_KH2, 24);
  delay(100);
  mySwitch.send((ADD_CN6 << 8) | 0b00000000, 24);
  sendStatusDenChumKh2();
}
void sendStatusDenChumKh2()
{
  int value = readStatusDenChumKh2();
  vDenChumKh2 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
   root[ID_DEN_CHUM_KH2] = value;
  sendData(root);
}

int readStatusDenChumKh2()
{
  int value = digitalRead(PIN_DEN_CHUM_KH2);
  if(value == 0) return 1;
  if (value == 1) return 0;
  return -1;
}
/*----------------DenTranhKH2--------------*/
void changeDenTranhKh2()
{
  Serial.println("Change DenTranhKH2");
  mySwitch.send((ADD_CN6 << 8) | VALUE_DEN_TRANH_KH2, 24);
  delay(100);
  mySwitch.send((ADD_CN6 << 8) | 0b00000000, 24);
  sendStatusDenTranhKh2();
}
void sendStatusDenTranhKh2()
{
  int value = readStatusDenTranhKh2();
  vDenTranhKh2 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root[ID_DEN_TRANH_KH2] = value;
  sendData(root);
}

int readStatusDenTranhKh2()
{
  int value = digitalRead(PIN_DEN_TRANH_KH2);
  if(value == 0) return 1;
  if (value == 1) return 0;
  return -1;
}

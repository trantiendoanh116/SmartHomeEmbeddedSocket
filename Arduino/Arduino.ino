#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>

const byte RX = 5;
const byte TX = 4;
//Khai báo serial
SoftwareSerial mySerial = SoftwareSerial(RX, TX);
SerialCommand sCmd(mySerial);

void setup()
{
  Serial.begin(115200);
  mySerial.begin(57600);

   pinMode(10, OUTPUT);
   pinMode(11, INPUT);

  Serial.println("Da san sang nhan lenh");

  sCmd.addCommand("CONTROL", processControl);
}
const unsigned long SCHEDULE_GET_VALUE_SENSOR = 60000UL; //Cứ sau 30s thì chu kỳ lặp lại
unsigned long lastUpdatedSensor = 0;

const unsigned long SCHEDULE_GET_VALUE_DEVICE = 2000UL; //Cứ sau 30s thì chu kỳ lặp lại
unsigned long lastUpdatedDevice = 0;

bool isFirstUpload = true;
int vDenTranKh1, vDenChumKh1;

void loop()
{
  if(isFirstUpload){
    sendValueTempHumi();
    sendValueCOBep();
    sendStatusDenTranKh1();
    sendStatusDenChumKh1();
    isFirstUpload = false;
  }
  delay(20);
  //Chu trình cập nhật dữ liệu của các cảm biến
  if (millis() - lastUpdatedSensor > SCHEDULE_GET_VALUE_SENSOR)
  {
    lastUpdatedSensor = millis();
    sendValueTempHumi();
    sendValueCOBep();
  }

  if(millis() - lastUpdatedDevice > SCHEDULE_GET_VALUE_DEVICE){
    checkAndUpdateValueDevice();
  }

  sCmd.readSerial();
  delay(200);
}

void processControl()
{
  char *json = sCmd.next();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(json);
  if (root.containsKey("F1_D01"))
  {
    changeDenTranKh1();
  }
  if (root.containsKey("F1_D02"))
  {
    changeDenChumKh1();
  }
  if (root.containsKey("init"))
  {
    Serial.println("Init -> get all value of device");
    sendValueTempHumi();
    sendValueCOBep();
  }
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

void checkAndUpdateValueDevice(){
  if(vDenTranKh1 != readStatusDenTranKh1()){
    sendStatusDenTranKh1();
  }
  delay(20);
  if(vDenChumKh1 != readStatusDenChumKh1()){
    sendStatusDenChumKh1();
  }
  delay(20);
}

/*----------------Đo nhiệt độ, đổ ẩm và khí CO------------*/
void sendValueTempHumi()
{
  //float h = dht.readHumidity();
  //float t = dht.readTemperature();
  float h = random(60,80);
  float t = random(20,40);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["TEMP"] = t;
  root["HUMI"] = h;
  sendData(root);
}
void sendValueCOBep()
{
  float value = random(20, 40);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["CO"] = value;
  sendData(root);
}

/*----------------DenTranKH1--------------*/
void changeDenTranKh1()
{
  // Serial.println("Change DenTranKH1");
  // int lightStatus = digitalRead(11);
  // digitalWrite(10, !lightStatus);
  // sendStatusDenTranKh1();

  digitalWrite(10, HIGH);
  delay(500);
  digitalWrite(10, LOW);
  delay(500);
  digitalWrite(10, HIGH);
  delay(500);
  digitalWrite(10, LOW);
  delay(500);
   digitalWrite(10, HIGH);
   sendStatusDenTranKh1();
}
void sendStatusDenTranKh1()
{
  int value = readStatusDenTranKh1();
  vDenTranKh1 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root["F1_D01"] = value;
  }
  else
  {
    root["F1_D01"] = -1;
  }
  sendData(root);
}

int readStatusDenTranKh1()
{
  return digitalRead(11);
}
/*----------------DenChumKH1--------------*/
void changeDenChumKh1()
{
  Serial.println("Change DenChumKH1");
 int lightStatus = digitalRead(11);
  digitalWrite(10, !lightStatus);
  sendStatusDenChumKh1();
}
void sendStatusDenChumKh1()
{
  int value = readStatusDenChumKh1();
  vDenChumKh1 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root["F1_D02"] = value;
  }
  else
  {
    root["F1_D02"] = -1;
  }
  sendData(root);
}

int readStatusDenChumKh1()
{
  return digitalRead(11);
}

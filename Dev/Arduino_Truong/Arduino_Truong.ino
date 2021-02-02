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
DHT dht(PIN_SENSOR_TEMP_HUMI, DHTTYPE);
void setup()
{
  Serial.begin(115200);
  mySerial.begin(57600);

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

unsigned long lastCheckDustDenti = 0;
unsigned long lastChekcDustDentiWarning = 0;
int vDusDest = 0;
int lastDusDest = 0;
int levelWarningDustDest = 0;
bool isWarningDustDest = false;

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
    sendValueDustDensity();
  }
  //Kiểm tra nồng độ khí
  //  if (millis() - lastCheckDustDenti > INTERVAL_DUST_DENSITY && !isWarningDustDest) {
  //    lastCheckDustDenti = millis();
  //    vDusDest = vDusDest + 30;
  //    lastDusDest = vDusDest;
  //    checkDustDest(vDusDest);
  //  }
  //  if (millis() - lastChekcDustDentiWarning > INTERVAL_DUST_DENSITY_WARNING && isWarningDustDest) {
  //    lastChekcDustDentiWarning = millis();
  //    processWarningDustDest(vDusDest);
  //  }
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
    sendValueDustDensity();
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

void sendData(String deviceId, JsonObject &data)
{
  mySerial.print(deviceId);
  mySerial.print('\r');
  data.printTo(mySerial);
  mySerial.print('\r');
  //Debug
  Serial.print("Send to server: ");
  Serial.print(deviceId);
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
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root[KEY_TEMP] = t;
  root[KEY_HUMI] = h;
  sendData(ID_SENSOR_TEMP_HUMI, root);
}
void sendValueCOBep()
{
  //int value = analogRead(A3);
  int value = random(60, 180);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root[KEY_VALUE] = value;

  sendData(ID_SENSOR_CO, root);
}
void sendValueDustDensity()
{
  float value = random(10, 1000);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root[KEY_VALUE] = value;
  sendData(ID_SENSOR_DUST, root);
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
  root[KEY_VALUE] = value;
  sendData(ID_DEN_TRAN_KH2, root);
}

int readStatusDenTranKh2()
{
  int value = digitalRead(PIN_DEN_TRAN_KH2);
  switch (value)
  {
  case 0:
    return 1;
  case 1:
    return 0;
  default:
    return -1;
  }
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
  root[KEY_VALUE] = value;
  sendData(ID_DEN_CHUM_KH2, root);
}

int readStatusDenChumKh2()
{
  int value = digitalRead(PIN_DEN_CHUM_KH2);
  switch (value)
  {
  case 0:
    return 1;
  case 1:
    return 0;
  default:
    return -1;
  }
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
  root[KEY_VALUE] = value;
  sendData(ID_DEN_TRANH_KH2, root);
}

int readStatusDenTranhKh2()
{
  int value = digitalRead(PIN_DEN_TRANH_KH2);
  switch (value)
  {
  case 0:
    return 1;
  case 1:
    return 0;
  default:
    return -1;
  }
}

//Kiểm tra và cảnh báo mật độ bụi
void checkDustDest(int value)
{
  if (value > 50)
  {
    isWarningDustDest = true;
    if (value < 101)
    {
      levelWarningDustDest = 1;
      Serial.println("Bat loc khi cap 1");
    }
    else if (value > 101 && value < 150)
    {
      levelWarningDustDest = 2;
      Serial.println("Bat loc khi cap 2");
    }
    else if (value > 151 && value < 200)
    {
      levelWarningDustDest = 3;
      Serial.println("Bat loc khi cap 3");
    }
    else if (value > 201 && value < 300)
    {
      levelWarningDustDest = 4;
      Serial.println("Bat loc khi cap 3");
      Serial.println("Bat coi bao muc 1");
    }
    else if (value > 300)
    {
      levelWarningDustDest = 5;
      Serial.println("Bat loc khi cap 3");
      Serial.println("Bat coi bao muc 2");
    }
  }
}
void processWarningDustDest(int value)
{
  if (value <= 50)
  {
    Serial.println("Tat loc khi");
  }
  else
  {
    if (value > 101 && value < 150 && levelWarningDustDest < 2)
    {
      levelWarningDustDest = 2;
      Serial.println("Bat loc khi cap 2");
    }
    else if (value > 151 && value < 200)
    {

      if (levelWarningDustDest < 3)
      {
        Serial.println("Bat loc khi cap 3");
      }
      else if (levelWarningDustDest > 3)
      {
        Serial.println("Tat coi bao muc 1");
      }
      levelWarningDustDest = 3;
    }
    else if (value > 201 && value < 300)
    {
      levelWarningDustDest = 4;
      Serial.println("Bat loc khi cap 3");
      Serial.println("Bat coi bao muc 1");
    }
    else if (value > 300)
    {
      levelWarningDustDest = 5;
      Serial.println("Bat loc khi cap 3");
      Serial.println("Bat coi bao muc 2");
    }
    if (levelWarningDustDest == 4)
    {
      if (value <= 200)
      {
        Serial.println("Tat coi bao muc 1");
      }
    }
    else if (levelWarningDustDest == 5)
    {
      if (value <= 300)
      {
        Serial.println("Chuyen coi bao muc 1");
      }
    }
  }
}

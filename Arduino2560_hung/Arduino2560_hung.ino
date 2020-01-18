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
  mySerial.begin(57600);

  mySwitch.enableTransmit(10);

  dht.begin();

  pinMode(PIN_DEN_TRAN_KH1, INPUT);
  pinMode(PIN_DEN_CHUM_KH1, INPUT);
  pinMode(PIN_DEN_TRANH_KH1, INPUT);
  pinMode(PIN_QUAT_TRAN_1, INPUT);
  pinMode(PIN_QUAT_TRAN_2, INPUT);
  pinMode(PIN_QUAT_TRAN_3, INPUT);
  pinMode(PIN_DEN_TRANGTRI_KH1, INPUT);
  pinMode(PIN_DEN_TRAN_KH2, INPUT);
  pinMode(PIN_DEN_CHUM_KH2, INPUT);
  pinMode(PIN_DEN_TRANH_KH2, INPUT);
  pinMode(PIN_DEN_SAN, INPUT);
  pinMode(PIN_DEN_CONG, INPUT);
  pinMode(PIN_DEN_WC, INPUT);
  pinMode(PIN_BINH_NL, INPUT);
  pinMode(PIN_DEN_CUA_NGACH, INPUT);
  pinMode(PIN_DEN_BEP_1, INPUT);
  pinMode(PIN_DEN_BEP_2, INPUT);
  pinMode(PIN_KHI_LOC_1, INPUT);
  pinMode(PIN_KHI_LOC_2, INPUT);
  pinMode(PIN_AT_BEP, INPUT);
  pinMode(PIN_AT_TONG, INPUT);
  pinMode(PIN_DEN_TRAN_KH1, INPUT);

  Serial.println("Da san sang nhan lenh");

  sCmd.addCommand("CONTROL", processControl);
}
unsigned long lastUpdatedSensor = 0;
unsigned long lastUpdatedDevice = 0;

bool isFirstLoop = true;
int vDenTranKh1, vDenChumKh1, vDenTranhKh1, vQuat, vDenTrangTriKh1, vDenTranKh2, vDenChumKh2, vDenTranhKh2, vDenSan, vDenCong,
    vDenWC, vBinhNL, vDenCuaNgach, vDenBep1, vDenBep2, vKhiLoc, vATBep, vATTong;

void loop()
{

  if (isFirstLoop)
  {
    Serial.println("First loop");
    isFirstLoop = false;
    //Reset CN
    resetCN1();
    resetCN2();
    resetCN3();
    resetCN4();
    resetCN5();
    resetCN6();
    resetCN7();
    resetCN8();
    resetCN9();
    resetCN10();
    resetCN11();
    resetCN12();
    delay(500);
    //Update value to server
    sendStatusDenTranKh1();
    sendStatusDenChumKh1();
    sendStatusDenTranhKh1();
    sendStatusQuat();
    sendStatusDenTrangTriKh1();
    sendStatusDenTranKh2();
    sendStatusDenChumKh2();
    sendStatusDenTranhKh2();
    sendStatusDenSan();
    sendStatusDenCong();
    sendStatusDenWC();
    sendStatusBinhNL();
    sendStatusDenCuaNgach();
    sendStatusDenBep1();
    sendStatusDenBep2();
    sendStatusKhiLoc();
    sendStatusATBep();
    sendStatusATTong();
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
  if (root.containsKey(ID_DEN_TRAN_KH1))
  {
    changeDenTranKh1();
  }
  if (root.containsKey(ID_DEN_CHUM_KH1))
  {
    changeDenChumKh1();
  }
  if (root.containsKey(ID_DEN_TRANH_KH1))
  {
    changeDenTranhKh1();
  }

  if (root.containsKey(ID_QUAT_TRAN))
  {
    changeQuatTran(root);
  }
  if (root.containsKey(ID_DEN_TRANGTRI_KH1))
  {
    changeDenTrangTriKh1();
  }
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
  if (root.containsKey(ID_DEN_SAN))
  {
    changeDenSan();
  }
  if (root.containsKey(ID_DEN_CONG))
  {
    changeDenCong();
  }
  if (root.containsKey(ID_DEN_WC))
  {
    changeDenWC();
  }
  if (root.containsKey(ID_BINH_NL))
  {
    changeBinhNL();
  }
  if (root.containsKey(ID_DEN_CUA_NGACH))
  {
    changeDenCuaNgach();
  }
  if (root.containsKey(ID_BEP_1))
  {
    changeDenBep1();
  }
  if (root.containsKey(ID_BEP_2))
  {
    changeDenBep2();
  }
  if (root.containsKey(ID_KHI_LOC))
  {
    changeKhiLoc();
  }
  if (root.containsKey(ID_AT_BEP))
  {
    changeATbep();
  }
  if (root.containsKey(ID_AT_TONG))
  {
    changeATTong();
  }
  if (root.containsKey("init"))
  {
    Serial.println("Init -> get all value of device");
    sendStatusDenTranKh1();
    sendStatusDenChumKh1();
    sendStatusDenTranhKh1();
    sendStatusQuat();
    sendStatusDenTrangTriKh1();
    sendStatusDenTranKh2();
    sendStatusDenChumKh2();
    sendStatusDenTranhKh2();
    sendStatusDenSan();
    sendStatusDenCong();
    sendStatusDenWC();
    sendStatusBinhNL();
    sendStatusDenCuaNgach();
    sendStatusDenBep1();
    sendStatusDenBep2();
    sendStatusKhiLoc();
    sendStatusATBep();
    sendStatusATTong();

    sendValueTempHumi();
    sendValueCOBep();
  }
}

void checkAndUpdateValueDevice()
{
  if (vDenTranKh1 != readStatusDenTranKh1())
  {
    sendStatusDenTranKh1();
  }
  delay(20);
  if (vDenChumKh1 != readStatusDenChumKh1())
  {
    sendStatusDenChumKh1();
  }
  delay(20);
  if (vDenTranhKh1 != readStatusDenTranhKh1())
  {
    sendStatusDenTranhKh1();
  }
  delay(20);
  if (vQuat != readStatusQuat())
  {
    sendStatusQuat();
  }
  delay(20);
  if (vDenTrangTriKh1 != readStatusDenTrangTriKh1())
  {
    sendStatusDenTrangTriKh1();
  }
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
  if (vDenSan != readStatusDenSan())
  {
    sendStatusDenSan();
  }
  delay(20);
  if (vDenCong != readStatusDenCong())
  {
    sendStatusDenCong();
  }
  delay(20);
  if (vDenWC != readStatusDenWC())
  {
    sendStatusDenWC();
  }
  delay(20);
  if (vBinhNL != readStatusBinhNL())
  {
    sendStatusBinhNL();
  }
  delay(20);
  if (vDenCuaNgach != readStatusCuaNgach())
  {
    sendStatusDenCuaNgach();
  }
  delay(20);
  if (vDenBep1 != readStatusBep1())
  {
    sendStatusDenBep1();
  }
  delay(20);
  if (vDenBep2 != readStatusBep2())
  {
    sendStatusDenBep2();
  }
  delay(20);
  if (vKhiLoc != readStatusKhiLoc())
  {
    sendStatusKhiLoc();
  }
  delay(20);
  if (vATBep != readStatusATBep())
  {
    sendStatusATBep();
  }
  delay(20);
  if (vATTong != readStatusATTong())
  {
    sendStatusATTong();
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
/*----------------Reset CN--------------*/
void resetCN1()
{
  Serial.println("Reset CN1");
  mySwitch.send((ADD_CN1 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN1 << 8) | 0b00000000, 24);
}
void resetCN2()
{
  Serial.println("Reset CN2");
  mySwitch.send((ADD_CN2 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN2 << 8) | 0b00000000, 24);
}
void resetCN3()
{
  Serial.println("Reset CN3");
  mySwitch.send((ADD_CN3 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN3 << 8) | 0b00000000, 24);
}
void resetCN4()
{
  Serial.println("Reset CN4");
  mySwitch.send((ADD_CN4 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN4 << 8) | 0b00000000, 24);
}
void resetCN5()
{
  Serial.println("Reset CN5");
  mySwitch.send((ADD_CN5 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN5 << 8) | 0b00000000, 24);
}
void resetCN6()
{
  Serial.println("Reset CN6");
  mySwitch.send((ADD_CN6 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN6 << 8) | 0b00000000, 24);
}
void resetCN7()
{
  Serial.println("Reset CN7");
  mySwitch.send((ADD_CN7 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN7 << 8) | 0b00000000, 24);
}
void resetCN8()
{
  Serial.println("Reset CN8");
  mySwitch.send((ADD_CN8 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN8 << 8) | 0b00000000, 24);
}
void resetCN9()
{
  Serial.println("Reset CN9");
  mySwitch.send((ADD_CN9 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN9 << 8) | 0b00000000, 24);
}
void resetCN10()
{
  Serial.println("Reset CN10");
  mySwitch.send((ADD_CN10 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN10 << 8) | 0b00000000, 24);
}
void resetCN11()
{
  Serial.println("Reset CN11");
  mySwitch.send((ADD_CN11 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN11 << 8) | 0b00000000, 24);
}
void resetCN12()
{
  Serial.println("Reset CN12");
  mySwitch.send((ADD_CN12 << 8) | VALUE_RESET_CN, 24);
  delay(100);
  mySwitch.send((ADD_CN12 << 8) | 0b00000000, 24);
}


/*----------------Đo nhiệt độ, đổ ẩm và khí CO------------*/
void sendValueTempHumi()
{
 float h = dht.readHumidity(); //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ
  //float h = random(60, 80);
  //loat t = random(25, 35);
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
  float value = random(20, 40);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root[ID_KHOI_CO] = value;
  sendData(root);
}

/*----------------DenTranKH1--------------*/
void changeDenTranKh1()
{
  Serial.println("Change DenTranKH1");
  mySwitch.send((ADD_CN8 << 8) | VALUE_DEN_TRAN_KH1, 24);
  delay(100);
  mySwitch.send((ADD_CN8 << 8) | 0b00000000, 24);
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
    root[ID_DEN_TRAN_KH1] = !value;
  }
  else
  {
    root[ID_DEN_TRAN_KH1] = -1;
  }
  sendData(root);
}

int readStatusDenTranKh1()
{
  return digitalRead(PIN_DEN_TRAN_KH1);
}
/*----------------DenChumKH1--------------*/
void changeDenChumKh1()
{
  Serial.println("Change DenChumKH1");
  mySwitch.send((ADD_CN8 << 8) | VALUE_DEN_CHUM_KH1, 24);
  delay(100);
  mySwitch.send((ADD_CN8 << 8) | 0b00000000, 24);
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
    root[ID_DEN_CHUM_KH1] = !value;
  }
  else
  {
    root[ID_DEN_CHUM_KH1] = -1;
  }
  sendData(root);
}

int readStatusDenChumKh1()
{
  //Serial.println(digitalRead(PIN_DEN_CHUM_KH1));
  return digitalRead(PIN_DEN_CHUM_KH1);
}
/*----------------DenTranhKH1--------------*/
void changeDenTranhKh1()
{
  Serial.println("Change DenTranhKH1");
  mySwitch.send((ADD_CN8 << 8) | VALUE_DEN_TRANH_KH1, 24);
  delay(100);
  mySwitch.send((ADD_CN8 << 8) | 0b00000000, 24);
  sendStatusDenTranhKh1();
}
void sendStatusDenTranhKh1()
{
  int value = readStatusDenTranhKh1();
  vDenTranhKh1 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_DEN_TRANH_KH1] = !value;
  }
  else
  {
    root[ID_DEN_TRANH_KH1] = -1;
  }
  sendData(root);
}

int readStatusDenTranhKh1()
{
  return digitalRead(PIN_DEN_TRANH_KH1);
}
/*----------------QuatTran--------------*/
void changeQuatTran(JsonObject &root)
{
  if (root[ID_QUAT_TRAN] == "on")
  {
    onQuat();
  }
  else
  {
    offQuat();
  }
}
void onQuat()
{
  Serial.println("Change ON QuatTran");
  mySwitch.send((ADD_CN7 << 8) | 0b00000011, 24);
  delay(80);
  mySwitch.send((ADD_CN7 << 8) | 0b00000000, 24);
  sendStatusQuat();
}
void offQuat()
{
  Serial.println("Change OFF QuatTran");
  mySwitch.send((ADD_CN7 << 8) | 0b00001100, 24);
  delay(80);
  mySwitch.send((ADD_CN7 << 8) | 0b00000000, 24);
  sendStatusQuat();
}
void sendStatusQuat()
{
  int value = readStatusQuat();
  vQuat = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0 || value == 2 || value == 3)
  {
    root[ID_QUAT_TRAN] = value;
  }
  else
  {
    root[ID_QUAT_TRAN] = -1;
  }
  sendData(root);
}

int readStatusQuat()
{
  int speed1Status = digitalRead(PIN_QUAT_TRAN_1);
  int speed2Status = digitalRead(PIN_QUAT_TRAN_2);
  int speed3Status = digitalRead(PIN_QUAT_TRAN_3);
//  Serial.println(digitalRead(PIN_QUAT_TRAN_1));
//  Serial.println(digitalRead(PIN_QUAT_TRAN_2));
//  Serial.println(digitalRead(PIN_QUAT_TRAN_3));
  if (speed1Status == 0)
  {
    return 3;
  }
  else if (speed2Status == 0)
  {
    return 2;
  }
  else if (speed3Status == 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
/*----------------DenTrangTriKH1--------------*/
void changeDenTrangTriKh1()
{
  Serial.println("Change DenTranhTriKH1");
  mySwitch.send((ADD_CN5 << 8) | VALUE_DEN_TRANGTRI_KH1, 24);
  delay(100);
  mySwitch.send((ADD_CN5 << 8) | 0b00000000, 24);
  sendStatusDenTrangTriKh1();
}
void sendStatusDenTrangTriKh1()
{
  int value = readStatusDenTrangTriKh1();
  vDenTrangTriKh1 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_DEN_TRANGTRI_KH1] = !value;
  }
  else
  {
    root[ID_DEN_TRANGTRI_KH1] = -1;
  }
  sendData(root);
}

int readStatusDenTrangTriKh1()
{
  return digitalRead(PIN_DEN_TRANGTRI_KH1);
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
  if (value == 1 || value == 0)
  {
    root[ID_DEN_TRAN_KH2] = !value;
  }
  else
  {
    root[ID_DEN_TRAN_KH2] = -1;
  }
  sendData(root);
}

int readStatusDenTranKh2()
{
  return digitalRead(PIN_DEN_TRAN_KH2);
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
  if (value == 1 || value == 0)
  {
    root[ID_DEN_CHUM_KH2] = !value;
  }
  else
  {
    root[ID_DEN_CHUM_KH2] = -1;
  }
  sendData(root);
}

int readStatusDenChumKh2()
{
  return digitalRead(PIN_DEN_CHUM_KH2);
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
  if (value == 1 || value == 0)
  {
    root[ID_DEN_TRANH_KH2] = !value;
  }
  else
  {
    root[ID_DEN_TRANH_KH2] = -1;
  }
  sendData(root);
}

int readStatusDenTranhKh2()
{
  return digitalRead(PIN_DEN_TRANH_KH2);
}
/*----------------DenSan--------------*/
void changeDenSan()
{
  Serial.println("Change DenSan");
  mySwitch.send((ADD_CN5 << 8) | VALUE_DEN_SAN, 24);
  delay(100);
  mySwitch.send((ADD_CN5 << 8) | 0b00000000, 24);
  sendStatusDenSan();
}
void sendStatusDenSan()
{
  int value = readStatusDenSan();
  vDenSan = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_DEN_SAN] = !value;
  }
  else
  {
    root[ID_DEN_SAN] = -1;
  }
  sendData(root);
}

int readStatusDenSan()
{
  return digitalRead(PIN_DEN_SAN);
}
/*----------------DenCong--------------*/
void changeDenCong()
{
  Serial.println("Change DenCong");
  mySwitch.send((ADD_CN5 << 8) | VALUE_DEN_CONG, 24);
  delay(100);
  mySwitch.send((ADD_CN5 << 8) | 0b00000000, 24);
  sendStatusDenCong();
}
void sendStatusDenCong()
{
  int value = readStatusDenCong();
  vDenCong = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_DEN_CONG] = !value;
  }
  else
  {
    root[ID_DEN_CONG] = -1;
  }
  sendData(root);
}

int readStatusDenCong()
{
  return digitalRead(PIN_DEN_CONG);
}
/*----------------DenWC--------------*/
void changeDenWC()
{
  Serial.println("Change DenWC");
  mySwitch.send((ADD_CN9 << 8) | VALUE_DEN_WC, 24);
  delay(100);
  mySwitch.send((ADD_CN9 << 8) | 0b00000000, 24);
  sendStatusDenWC();
}
void sendStatusDenWC()
{
  int value = readStatusDenWC();
  vDenWC = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_DEN_WC] = !value;
  }
  else
  {
    root[ID_DEN_WC] = -1;
  }
  sendData(root);
}

int readStatusDenWC()
{
  return digitalRead(PIN_DEN_WC);
}

/*----------------BinhNL--------------*/
void changeBinhNL()
{
  Serial.println("Change BinhNL");
  mySwitch.send((ADD_CN9 << 8) | VALUE_BINH_NL, 24);
  delay(100);
  mySwitch.send((ADD_CN9 << 8) | 0b00000000, 24);
  sendStatusDenWC();
}
void sendStatusBinhNL()
{
  int value = readStatusBinhNL();
  vBinhNL = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_BINH_NL] = !value;
  }
  else
  {
    root[ID_BINH_NL] = -1;
  }
  sendData(root);
}

int readStatusBinhNL()
{
  return digitalRead(PIN_BINH_NL);
}
/*----------------DenCuaNgach--------------*/
void changeDenCuaNgach()
{
  Serial.println("Change DenCuaNgach");
  mySwitch.send((ADD_CN11 << 8) | VALUE_DEN_CUA_NGACH, 24);
  delay(100);
  mySwitch.send((ADD_CN11 << 8) | 0b00000000, 24);
  sendStatusDenCuaNgach();
}
void sendStatusDenCuaNgach()
{
  int value = readStatusCuaNgach();
  vDenCuaNgach = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_DEN_CUA_NGACH] = !value;
  }
  else
  {
    root[ID_DEN_CUA_NGACH] = -1;
  }
  sendData(root);
}

int readStatusCuaNgach()
{
  return digitalRead(PIN_DEN_CUA_NGACH);
}
/*----------------Den Bep1--------------*/
void changeDenBep1()
{
  Serial.println("Change Den Bep1");
  mySwitch.send((ADD_CN3 << 8) | VALUE_BEP_1, 24);
  delay(100);
  mySwitch.send((ADD_CN3 << 8) | 0b00000000, 24);
  sendStatusDenBep1();
}
void sendStatusDenBep1()
{
  int value = readStatusBep1();
  vDenBep1 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_BEP_1] = !value;
  }
  else
  {
    root[ID_BEP_1] = -1;
  }
  sendData(root);
}

int readStatusBep1()
{
  //Serial.println(digitalRead(PIN_DEN_BEP_1));
  return digitalRead(PIN_DEN_BEP_1);
}
/*----------------Den Bep2--------------*/
void changeDenBep2()
{
  Serial.println("Change Den Bep2");
  mySwitch.send((ADD_CN4 << 8) | VALUE_BEP_2, 24);
  delay(100);
  mySwitch.send((ADD_CN4 << 8) | 0b00000000, 24);
  sendStatusDenBep2();
}
void sendStatusDenBep2()
{
  int value = readStatusBep2();
  vDenBep2 = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_BEP_2] = !value;
  }
  else
  {
    root[ID_BEP_2] = -1;
  }
  sendData(root);
}

int readStatusBep2()
{
  return digitalRead(PIN_DEN_BEP_2);
}

/*----------------Khi Loc--------------*/
void changeKhiLoc()
{
  Serial.println("Change Khi loc");
  mySwitch.send((ADD_CN10 << 8) | VALUE_KHI_LOC, 24);
  delay(100);
  mySwitch.send((ADD_CN10 << 8) | 0b00000000, 24);
  sendStatusDenBep2();
}
void sendStatusKhiLoc()
{
  int value = readStatusKhiLoc();
  vKhiLoc = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_KHI_LOC] = !value;
  }
  else
  {
    root[ID_KHI_LOC] = -1;
  }
  sendData(root);
}

int readStatusKhiLoc()
{
  int cap1 = digitalRead(PIN_KHI_LOC_1);
  int cap2 = digitalRead(PIN_KHI_LOC_2);
  if (cap1 == 0 && cap2 == 0)
  {
    return 0;
  }
  else if (cap1 == 1)
  {
    return 1;
  }
  else if (cap2 == 1)
  {
    return 2;
  }
  return -1;
}

/*----------------AT Bep--------------*/
void changeATbep()
{
  Serial.println("Change AT bep");
  mySwitch.send((ADD_CN2 << 8) | VALUE_AT_BEP, 24);
  delay(100);
  mySwitch.send((ADD_CN2 << 8) | 0b00000000, 24);
  sendStatusATBep();
}
void sendStatusATBep()
{
  int value = readStatusATBep();
  vATBep = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_AT_BEP] = !value;
  }
  else
  {
    root[ID_AT_BEP] = -1;
  }
  sendData(root);
}

int readStatusATBep()
{
  return digitalRead(PIN_AT_BEP);
}
/*----------------AT Tong--------------*/
void changeATTong()
{
  Serial.println("Change AT tong");
  mySwitch.send((ADD_CN1 << 8) | VALUE_AT_TONG, 24);
  delay(100);
  mySwitch.send((ADD_CN1 << 8) | 0b00000000, 24);
  sendStatusATTong();
}
void sendStatusATTong()
{
  int value = readStatusATTong();
  vATTong = value;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_AT_TONG] = !value;
  }
  else
  {
    root[ID_AT_TONG] = -1;
  }
  sendData(root);
}

int readStatusATTong()
{
  return digitalRead(PIN_AT_TONG);
}

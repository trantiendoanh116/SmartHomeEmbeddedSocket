#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include <RCSwitch.h>

extern "C"
{
#include "Config.h"
}

SoftwareSerial mySerial = SoftwareSerial(SERIAL_RX, SERIAL_TX);
SerialCommand sCmd(mySerial);

RCSwitch mySwitch = RCSwitch();

void setup()
{
  Serial.begin(115200);
  mySerial.begin(57600);

   mySwitch.enableTransmit(10);

  pinMode(10, OUTPUT); //TODO

  pinMode(PIN_DEN_TRAN_KH1, INPUT);
  // pinMode(PIN_DEN_CHUM_KH1, INPUT);
  // pinMode(PIN_DEN_TRANH_KH1, INPUT);
  // pinMode(PIN_QUAT_TRAN_1, INPUT);
  // pinMode(PIN_QUAT_TRAN_2, INPUT);
  // pinMode(PIN_QUAT_TRAN_3, INPUT);
  // pinMode(PIN_DEN_TRANGTRI_KH1, INPUT);
  // pinMode(PIN_DEN_TRAN_KH2, INPUT);
  // pinMode(PIN_DEN_CHUM_KH2, INPUT);
  // pinMode(PIN_DEN_TRANH_KH2, INPUT);
  // pinMode(PIN_DEN_SAN, INPUT);
  // pinMode(PIN_DEN_CONG, INPUT);
  // pinMode(PIN_DEN_WC, INPUT);
  // pinMode(PIN_BINH_NL, INPUT);
  // pinMode(PIN_DEN_CUA_NGACH, INPUT);
  // pinMode(PIN_DEN_BEP_1, INPUT);
  // pinMode(PIN_DEN_BEP_2, INPUT);
  // pinMode(PIN_KHI_LOC_1, INPUT);
  // pinMode(PIN_KHI_LOC_2, INPUT);
  // pinMode(PIN_AT_BEP, INPUT);
  // pinMode(PIN_AT_TONG, INPUT);
  // pinMode(PIN_DEN_TRAN_KH1, INPUT);

  Serial.println("Da san sang nhan lenh");

  sCmd.addCommand("CONTROL", processControl);
}
unsigned long chuky = 0;

void loop()
{

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
    //changeLight();
    changeDenTranKh1();
  }
  if (root.containsKey("init"))
  {
    Serial.println("Init -> get all value of device");
    sendStatusDenTranKh1();
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
/*----------------Test in Trường's device--------------*/
void changeLight()
{
  Serial.println("Change DenTranKH1");
  int lightStatus = digitalRead(11);
  digitalWrite(10, !lightStatus);
  sendStatusLight();
}
void sendStatusLight()
{
  int value = readStatusLight();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_DEN_TRAN_KH1] = value;
  }
  else
  {
    root[ID_DEN_TRAN_KH1] = -1;
  }
  sendData(root);
}

int readStatusLight()
{
  return digitalRead(11);
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
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  if (value == 1 || value == 0)
  {
    root[ID_DEN_TRAN_KH1] = value;
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

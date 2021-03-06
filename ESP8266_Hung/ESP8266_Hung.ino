#define TCP_MSS whatever
#define LWIP_IPV6 whatever
#define LWIP_FEATURES whatever
#define LWIP_OPEN_SRC whatever

//Thư viện Wifi manager
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"

#include <SoftwareSerial.h>
#include <SocketIOClient.h>
#include <SerialCommand.h>

#include <PZEM004Tv30.h>
#include <ArduinoJson.h>

//include thư viện để kiểm tra free RAM trên con esp8266
extern "C"
{
#include "user_interface.h"
}

const byte RX = 5; // chân GPI05 đấu với chân D4 của Arduino
const byte TX = 4; // chân GPI04 đấu với chân D5 của Arduino

SoftwareSerial mySerial(RX, TX, false, 256);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

//Cài đặt Socket client
SocketIOClient client;
char host[] = "smart-home-hung.herokuapp.com"; //smarthome116.herokuapp.com, 192.168.1.123, smart-home-hung.herokuapp.com
int port = 80;                        //80,3484                  //Cổng dịch vụ socket server do chúng ta tạo!
char namespace_esp8266[] = "esp8266"; //Thêm Arduino!

//từ khóa extern: dùng để #include các biến toàn cục ở một số thư viện khác. Trong thư viện SocketIOClient có hai biến toàn cục
// mà chúng ta cần quan tâm đó là
// RID: Tên hàm (tên sự kiện
// Rfull: Danh sách biến (được đóng gói lại là chuối JSON)
extern String RID;
extern String Rfull;

//Khai báo đo dòng điện
PZEM004Tv30 pzem(12, 15);

//Khi kết nối wifi thất bại
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

//Gửi dữ liệu về cho Socket Server
void sendSocketServer(String command)
{
  char *json = sCmd.next();
  client.send(command, (String)json);

  //In ra serial monitor để debug
  Serial.print(command);
  Serial.print(' ');
  Serial.println(json);
}

// Cài đặt thông số ban đầu
void setup()
{
  Serial.begin(115200);
  mySerial.begin(57600); //Bật software serial để giao tiếp với Arduino, nhớ để baudrate trùng với software serial trên mạch arduino
  delay(10);

  //Việc đầu tiên cần làm là kết nối vào mạng Wifi
  Serial.print("Ket noi vao mang ");
  WiFiManager wifiManager;
  //có thể reset các cài đặt cũ bằng cách gọi hàm:
  //wifiManager.resetSettings();

  //Cài đặt callback, khi kết nối với wifi cũ thất bại, thiết bị sẽ gọi hàm callback
  //và khởi động chế độ AP với SSID được cài tự động là "ESP+chipID"
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect())
  {
    Serial.println(F("Ket noi den wifi that bai!"));
    //Nếu kết nối thất bại, thử kết nối lại bằng cách reset thiết bị
    ESP.reset();
    delay(1000);
  }

  Serial.println(F("Da ket noi WiFi"));

  if (client.connect(host, port, namespace_esp8266))
  {
    Serial.println("Socket.IO connected !");
  }
  else
  {
    Serial.println("Socket.IO not connected.");
    return;
  }

  sCmd.addDefaultHandler(sendSocketServer); //Lệnh nào đi qua nó cũng bắt hết, rồi chuyển xuống hàm sendSocketServer!
  Serial.println("Da san sang nhan lenh");
}
long lastUpdateElectric = 0;
const long SCHEDULE_GET_VALUE_ELECTRIC = 10000UL;

void loop()
{
  //Kết nối lại server socket!
  if (!client.connected())
  {
    Serial.println(F("Reconnect server!!!"));
    client.reconnect(host, port, namespace_esp8266);
    return;
  }
  //Khi bắt được bất kỳ sự kiện nào thì chúng ta có hai tham số:
  //  +RID: Tên sự kiện
  //  +RFull: Danh sách tham số được nén thành chuỗi JSON!
  if (client.monitor())
  {
    //gửi tên sự kiện cho adruino xử lý
    mySerial.print(RID);
    mySerial.print('\r');
    mySerial.print(Rfull);
    mySerial.print('\r');

    //in ra serial monitor
    Serial.print(RID);
    Serial.print(' ');
    Serial.println(Rfull);
  }

  if (millis() - lastUpdateElectric > SCHEDULE_GET_VALUE_ELECTRIC)
  {
    lastUpdateElectric =  millis();
    sendValueElectric();
  }

  sCmd.readSerial();
}

/*------------------Đo giá trị dòng điện-----------------*/
void sendValueElectric() {
  float voltage = pzem.voltage();
  if ( !isnan(voltage) ) {
    Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  } else {
    Serial.println("Error reading voltage");
    voltage = -1;
  }

  float current = pzem.current();
  if ( !isnan(current) ) {
    Serial.print("Current: "); Serial.print(current); Serial.println("A");
  } else {
    Serial.println("Error reading current");
    current = -1;
  }

  float energy = pzem.energy();
  if ( !isnan(energy) ) {
    Serial.print("Energy: "); Serial.print(energy, 3); Serial.println("kWh");
  } else {
    Serial.println("Error reading energy");
    energy = -1;
  }
  Serial.println();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
   root["AMP"] = current;
   root["VOL"] = voltage;
   root["ENERGY"] = energy;
//   root["AMP"] = random(2, 6);
//   root["VOL"] = random(210, 240);
//   root["ENERGY"] = random(210, 10000);
   
   String data;
   root.printTo(data);
   client.send("F1_S03", data);
}

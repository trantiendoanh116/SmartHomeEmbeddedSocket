#define TCP_MSS whatever
#define LWIP_IPV6 whatever
#define LWIP_FEATURES whatever
#define LWIP_OPEN_SRC whatever

//Thư viện Wifi manager
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"
//
#include <SoftwareSerial.h>
#include <SocketIOClient.h>
#include <SerialCommand.h>

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
char host[] = "smarthome116.herokuapp.com"; //Địa chỉ IP dịch vụ, hãy thay đổi nó theo địa chỉ IP Socket server của bạn.
int port = 80;                        //80,3484                  //Cổng dịch vụ socket server do chúng ta tạo!
char namespace_esp8266[] = "esp8266"; //Thêm Arduino!

//từ khóa extern: dùng để #include các biến toàn cục ở một số thư viện khác. Trong thư viện SocketIOClient có hai biến toàn cục
// mà chúng ta cần quan tâm đó là
// RID: Tên hàm (tên sự kiện
// Rfull: Danh sách biến (được đóng gói lại là chuối JSON)
extern String RID;
extern String Rfull;

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

  sCmd.readSerial();
}

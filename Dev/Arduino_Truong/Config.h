#include <ArduinoJson.h>

const byte SERIAL_RX = 50; //5
const byte SERIAL_TX = 4;
/*--------Giá trị của các Address CN -------*/
const long ADD_CN6 = 0b0101000101010101;

/*--------Giá trị bit của các nút điều khiển(19)------*/
const int VALUE_RESET_CN = 0b11000000;
const int VALUE_DEN_TRAN_KH2 = 0b00001100;
const int VALUE_DEN_CHUM_KH2 = 0b00000011;
const int VALUE_DEN_TRANH_KH2 = 0b00110000;

/*--------Giá trị Pin để lấy giá trị/trang thái -------*/

const int PIN_DEN_TRAN_KH2 = 30;
const int PIN_DEN_CHUM_KH2 = 29;
const int PIN_DEN_TRANH_KH2 = 31;

const int PIN_SENSOR_TEMP_HUMI = 2;
const int PIN_TX_SENSOR_CO_ID = 6;
const int PIN_RX_SENSOR_CO = 7;

/*--------Định nghĩa ID cho các thiết bị -------*/
const String ID_DEN_TRAN_KH1 = "F1_D01";
const String ID_DEN_CHUM_KH1 = "F1_D02";
const String ID_DEN_TRANH_KH1 = "F1_D03";
const String ID_QUAT_TRAN = "F1_D04";
const String ID_DEN_TRANGTRI_KH1 = "F1_D05";
const String ID_DEN_TRAN_KH2 = "F1_D06";
const String ID_DEN_CHUM_KH2 = "F1_D07";
const String ID_DEN_TRANH_KH2 = "F1_D08";
const String ID_DEN_SAN = "F1_D09";
const String ID_DEN_CONG = "F1_D10";
const String ID_DEN_WC = "F1_D11";
const String ID_BINH_NL = "F1_D12";
const String ID_DEN_CUA_NGACH = "F1_D13";
const String ID_DEN_1_BEP = "F1_D14";
const String ID_DEN_2_BEP = "F1_D15";
const String ID_KHI_LOC = "F1_D16";
const String ID_AT_BEP = "F1_D17";
const String ID_AT_TONG = "F1_D18";
const String ID_GARA = "F1_D19";

const String ID_SENSOR_TEMP_HUMI = "F1_S01";
const String ID_SENSOR_CO = "F1_S02";
const String ID_SENSOR_ELECTRIC = "F1_S03";
const String ID_SENSOR_DUST = "F1_S04";

/*------Key trong json ------*/
const String KEY_TEMP = "temp";
const String KEY_HUMI = "humi";
const String KEY_AMPE = "amp";
const String KEY_VOLTAGE = "vol";
const String KEY_ENERGY = "energy";
const String KEY_VALUE = "value";
const String KEY_AUTO = "auto";

/*------Thông số cài đặt ------*/
const unsigned long SCHEDULE_GET_VALUE_SENSOR = 20000UL; //Cứ sau 30s thì chu kỳ lặp lại
const unsigned long SCHEDULE_GET_VALUE_DEVICE = 2000UL;
const unsigned long INTERVAL_DUST_DENSITY = 2000UL;         //30s
const unsigned long INTERVAL_DUST_DENSITY_WARNING = 5000UL; //10s

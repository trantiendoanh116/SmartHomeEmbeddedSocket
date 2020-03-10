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

const int PIN_TEMP_HUMI = 2;
const int PIN_KHOI_CO_TX = 6;
const int PIN_KHOI_CO_RX = 7;

/*--------Định nghĩa ID cho các thiết bị -------*/

const String ID_DEN_TRAN_KH2 = "F1_D06";
const String ID_DEN_CHUM_KH2 = "F1_D07";
const String ID_DEN_TRANH_KH2 = "F1_D08";

const String ID_TEMP_HUMI = "C_S01";
const String ID_KHOI_CO = "C_S02";
/*------Thông số cài đặt ------*/
const unsigned long SCHEDULE_GET_VALUE_SENSOR = 10000UL; //Cứ sau 30s thì chu kỳ lặp lại
const unsigned long SCHEDULE_GET_VALUE_DEVICE = 2000UL;

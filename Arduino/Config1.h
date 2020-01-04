
// const byte SERIAL_RX = 5;
// const byte SERIAL_TX = 4;
// /*--------Giá trị của các Address CN -------*/
// const long ADD_CN1 = 0b0101010101010100;
// const long ADD_CN2 = 0b0101010101010001;
// const long ADD_CN3 = 0b0101010101000101;
// const long ADD_CN4 = 0b0101010100010101;
// const long ADD_CN5 = 0b0101010001010101;
// const long ADD_CN6 = 0b0101000101010101;
// const long ADD_CN7 = 0b0100010101010101;
// const long ADD_CN8 = 0b0001010101010101;
// const long ADD_CN9 = 0b0101010101010000;
// const long ADD_CN10 = 0b0101010101000001;
// const long ADD_CN11 = 0b0101010100000101;
// const long ADD_CN12 = 0b0101010000010101;
// /*--------Giá trị bit của các nút điều khiển(19)------*/
// const int VALUE_DEN_TRAN_KH1 = 0b00000011;
// const int VALUE_DEN_CHUM_KH1 = 0b00001100;
// const int VALUE_DEN_TRANH_KH1 = 0b00110000;
// const int VALUE_QUAT_TRAN_OFF = 0b00001100;
// const int VALUE_QUAT_TRAN_ON = 0b00110000;
// const int VALUE_DEN_TRANGTRI_KH1 = 0b00000011;
// const int VALUE_DEN_TRAN_KH2 = 0b00000011;
// const int VALUE_DEN_CHUM_KH2 = 0b00001100;
// const int VALUE_DEN_TRANH_KH2 = 0b00110000;
// const int VALUE_DEN_SAN = 0b00001100;
// const int VALUE_DEN_CONG = 0b00110000;
// const int VALUE_DEN_WC = 0b00000011;
// const int VALUE_BINH_NL = 0b00001100;
// const int VALUE_DEN_CUA_NGACH = 0b00000011;
// const int VALUE_BEP_1 = 0b00000011;
// const int VALUE_BEP_2 = 0b00000011;
// const int VALUE_KHI_LOC = 0b00000011;//????
// const int VALUE_AT_BEP = 0b00000011;
// const int VALUE_AT_TONG = 0b00000011;
// /*--------Giá trị Pin để lấy giá trị/trang thái -------*/
// const int PIN_DEN_TRAN_KH1 = 44;
// const int PIN_DEN_CHUM_KH1 = 45;
// const int PIN_DEN_TRANH_KH1 = 46;
// const int PIN_QUAT_TRAN_1 = 32;
// const int PIN_QUAT_TRAN_2 = 33;
// const int PIN_QUAT_TRAN_3 = 34;
// const int PIN_DEN_TRANGTRI_KH1 = 26;
// const int PIN_DEN_TRAN_KH2 = 29;
// const int PIN_DEN_CHUM_KH2 = 30;
// const int PIN_DEN_TRANH_KH2 = 31;
// const int PIN_DEN_SAN = 27;
// const int PIN_DEN_CONG = 28;
// const int PIN_DEN_WC = 39;
// const int PIN_BINH_NL = 40;
// const int PIN_DEN_CUA_NGACH = 43;
// const int PIN_DEN_BEP_1 = 24;
// const int PIN_DEN_BEP_2 = 25;
// const int PIN_KHI_LOC_1 = 41;
// const int PIN_KHI_LOC_2 =42;
// const int PIN_AT_BEP = 23;
// const int PIN_AT_TONG = 22;
// const int PIN_TEMP_HUMI = 53; //A2
// const int PIN_KHOI_CO_TX = 6;
// const int PIN_KHOI_CO_RX = 7;
// const int PIN_DONGDIEN_VA_CONGSUAT_TX = 11;
// const int PIN_DONGDIEN_VA_CONGSUAT_RX = 12;
// /*--------Định nghĩa ID cho các thiết bị -------*/
// const String ID_DEN_TRAN_KH1 = "F1_D01";
// const String ID_DEN_CHUM_KH1 = "F1_D02";
// const String ID_DEN_TRANH_KH1 = "F1_D03";
// const String ID_QUAT_TRAN = "F1_D04";
// const String ID_DEN_TRANGTRI_KH1 = "F1_D05";
// const String ID_DEN_TRAN_KH2 = "F1_D06";
// const String ID_DEN_CHUM_KH2 = "F1_D07";
// const String ID_DEN_TRANH_KH2 = "F1_D08";
// const String ID_DEN_SAN = "F1_D09";
// const String ID_DEN_CONG = "F1_D10";
// const String ID_DEN_WC = "F1_D11";
// const String ID_BINH_NL = "F1_D12";
// const String ID_DEN_CUA_NGACH = "F1_D13";
// const String ID_BEP_1 = "F1_D14";
// const String ID_BEP_2 = "F1_D15";
// const String ID_KHI_LOC = "F1_D16";
// const String ID_AT_BEP = "C_D01";
// const String ID_AT_TONG = "C_D02";
// const String ID_TEMP_HUMI = "C_S01";
// const String ID_KHOI_CO = "C_S02";
// const String ID_DONGDIEN_VA_CONGSUAT = "C_S03";
// /*------Thông số cài đặt ------*/
// const unsigned long SCHEDULE_GET_VALUE_SENSOR = 30000UL; //Cứ sau 30s thì chu kỳ lặp lại
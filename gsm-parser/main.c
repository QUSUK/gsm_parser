#include "./inc/gsm-parser.h"



uint8_t str1[] = "+CPIN: ERROR ? \r\n";
uint8_t str2[] = "+CCID: \"898600220909A0206023\"OK\r\n";
uint8_t str3[] = "+QGSN:\"865734020000749\"OK\r\n";
uint8_t str4[] = "+IPR: 115200 OK\r\n";
uint8_t str5[] = "+CSCA: \"+8613800210500\",145OK\r\n";
uint8_t str6[] = "+CPBR: 10,\"15021012496\",129,\"QUECTEL\"OK\r\n";
uint8_t str7[] = "+COPS: 0,0,\"CHINA MOBILE\"OK\r\n";
uint8_t str8[] = "+CREG: 1,0\"1878\",\"0873\"OK";
uint8_t str9[] = "+CSQ: 28,7OK \r\n";
uint8_t str10[] = "+CMGL: 1,\"STO UNSENT\",\"\",\"\"OK\r\n";



void main(void)
{
	GSM Obj;

	GSM_Parse((const char*)str1, &Obj);
	GSM_Parse((const char*)str2, &Obj);
	GSM_Parse((const char*)str3, &Obj);
	GSM_Parse((const char*)str4, &Obj);
	GSM_Parse((const char*)str5, &Obj);
	GSM_Parse((const char*)str6, &Obj);
	GSM_Parse((const char*)str7, &Obj);
	GSM_Parse((const char*)str8, &Obj);
	GSM_Parse((const char*)str9, &Obj);
	GSM_Parse((const char*)str10, &Obj);


}
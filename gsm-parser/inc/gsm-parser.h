#pragma once
#pragma warning(disable : 4996)
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SMS_COUNT 20
#define MAX_SMS_TEXT_LENGTH 512
#define MAX_PHONE_COUNT 20

//AT + CCID 
typedef struct GSM_CCID
{
	uint8_t ICCID[32];
	uint8_t Status;				
}CCID;

//AT + QGSN 
typedef struct GSM_QGSN
{
	uint8_t IMEI[32];
	uint8_t Status;				
}QGSN;

//AT + CSCA
typedef struct GSM_CSCA
{
	uint8_t SCA[32];
	uint16_t To_SCA;
	uint8_t Status;				
}CSCA;

//AT + CPBR
typedef struct GSM_CPBR
{
	uint8_t Index;
	uint8_t Number[32];
	uint8_t Type;
	uint8_t Text[128];
	uint8_t Status;			
}CPBR;

//AT + COPS
typedef struct GSM_COPS
{
	uint8_t Mode;
	uint8_t Format;
	uint8_t Oper[16];
	uint8_t Status;				
}COPS;

//AT + CREG 
typedef struct GSM_CREG
{
	uint8_t N;
	uint8_t Stat;
	uint8_t Lag[32];
	uint8_t Ci[32];
	uint8_t Status;				
}CREG;

//AT + CSQ
typedef struct GSM_CSQ
{
	uint8_t RSSI;
	uint8_t BER;
	uint8_t Status;				
}CSQ;

//AT + CMGL
typedef struct GSM_CMGL
{
	uint8_t Index;
	uint8_t Stat[16];
	uint8_t DCS[1];
	uint8_t Sender_Number[32];
	uint8_t Timestamp[32];
	uint8_t Text[MAX_SMS_TEXT_LENGTH];
	uint16_t UCS2_Text[MAX_SMS_TEXT_LENGTH];
	uint8_t Status;				
}CMGL;

typedef struct GSM_CCLK
{
	uint8_t YY;
	uint8_t MM;
	uint8_t DD;
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
}CCLK;


typedef struct GSM
{
	uint8_t							CPIN;		
	struct GSM_CCID 		CCID;
	struct GSM_QGSN 		QGSN;
	uint8_t							IPR;		
	struct GSM_CSCA 		CSCA;
	struct GSM_CPBR 		CPBR[MAX_PHONE_COUNT];
	struct GSM_COPS 		COPS;
	struct GSM_CREG 		CREG;
	struct GSM_CSQ			CSQ;
	struct GSM_CMGL 		CMGL[MAX_SMS_COUNT];
	struct GSM_CCLK			CCLK;
}GSM;



void GSM_Parse(const char* Rx_Buff, GSM* GSM_Data);
void Add_String_Between_Quotes(char* Str, char* Dest);
void Add_Status(char* Str, uint8_t* Dest);
void Parse_PDU(GSM* GSM_Data);
void Decode7bit(CMGL* CMGL, uint8_t* extractedString, uint32_t cchMsgLength);
void DecodeUCS2(CMGL* CMGL, uint8_t* extractedString, uint32_t cchMsgLength);
void swap_pairs(uint8_t* str);
void DecodePDU(uint8_t* PDU, uint32_t PduLength, uint8_t* Dest);
void ConvertHexStrToByteArray(const uint8_t* hexString, uint8_t* byteArray, uint32_t* byteArrayLength);
void ConvertHexStrTo2ByteArray(const uint8_t* hexString, uint16_t* byteArray, uint32_t* byteArrayLength);
uint8_t* Prepare_PDU_SMS(uint8_t DCS, const uint8_t* Phone_Number, const uint8_t* Message);
uint8_t* Encode7bit(uint8_t* Message);
void EncodeUCS2(const uint8_t *utf8_str, uint8_t* Str);
void Send_SMS_PDU(uint8_t DCS, const uint8_t* Phone_Number, const uint8_t* SMS_Text);
void GSM_Init(void);


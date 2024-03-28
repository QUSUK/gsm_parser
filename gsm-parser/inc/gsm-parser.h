#pragma once
#pragma warning(disable : 4996)
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX_SMS_COUNT 10
#define MAX_SMS_TEXT_LENGTH 256

//AT + CCID 
typedef struct GSM_CCID
{
	uint8_t ICCID[32];
	uint8_t Status;				//1-OK 0-ERROR
}CCID;

//AT + QGSN 
typedef struct GSM_QGSN
{
	uint8_t IMEI[32];
	uint8_t Status;				//1-OK 0-ERROR
}QGSN;

//AT + CSCA ? 
typedef struct GSM_CSCA
{
	uint8_t SCA[32];
	uint16_t To_SCA;
	uint8_t Status;				//1-OK 0-ERROR
}CSCA;

//AT + CPBR = 1, 10 
typedef struct GSM_CPBR
{
	uint8_t Index;
	uint8_t Number[32];
	uint8_t Type;
	uint8_t Text[128];
	uint8_t Status;				//1-OK 0-ERROR
}CPBR;

//AT + COPS ?
typedef struct GSM_COPS
{
	uint8_t Mode;
	uint8_t Format;
	uint8_t Oper[16];
	uint8_t Status;				//1-OK 0-ERROR
}COPS;

//AT + CREG ? 
typedef struct GSM_CREG
{
	uint8_t N;
	uint8_t Stat;
	uint8_t Lag[32];
	uint8_t Ci[32];
	uint8_t Status;				//1-OK 0-ERROR
}CREG;

//AT + CSQ
typedef struct GSM_CSQ
{
	uint8_t RSSI;
	uint8_t BER;
	uint8_t Status;				//1-OK 0-ERROR
}CSQ;

//AT + CMGL = 4 
typedef struct GSM_CMGL
{
	uint8_t Index;
	uint8_t Stat[16];
	uint8_t Sender_Number[32];
	uint8_t Timestamp[32];
	uint8_t Text[MAX_SMS_TEXT_LENGTH];
	uint8_t Status;				//1-OK 0-ERROR
}CMGL;


typedef struct GSM
{

	uint8_t			CPIN;		//1-READY 0-ERROR
	struct GSM_CCID CCID;
	struct GSM_QGSN QGSN;
	uint8_t			IPR;		//1-OK 0-ERROR
	struct GSM_CSCA CSCA;
	struct GSM_CPBR CPBR;
	struct GSM_COPS COPS;
	struct GSM_CREG CREG;
	struct GSM_CSQ	CSQ;
	struct GSM_CMGL CMGL[MAX_SMS_COUNT];
}GSM;

void GSM_Parse(const char* Rx_Buff, GSM* GSM_Data);

void Add_String_Between_Quotes(char* Str, char* Dest);
void Add_Status(char* Str, uint8_t* Dest);




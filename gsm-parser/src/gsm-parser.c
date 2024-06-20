#pragma once
#include "../inc/gsm-parser.h"

void GSM_Parse(const char* Rx_Buff, GSM* GSM_Data)
{
    uint16_t Str_Size = (uint16_t)strlen(Rx_Buff);
    char* Str = (char*)malloc(Str_Size + 1);

    if (Str != NULL)
    {
        strcpy(Str, Rx_Buff);
    }
    else
    {
        free(Str);
        return;
    }
		
		if (strstr((const char*)Str, (const char*)"+CDS") != NULL)
		{
			uint8_t Stat[3];
			Stat[0] = Str[Str_Size-2];
			Stat[1] = Str[Str_Size-1];
			Stat[2] = '\0';
			if (strcmp((const char*)Stat, "00") == 0)
			{
				SMS_STATUS_TRANSACTION = 1;
			}
		}
    else if (strstr((const char*)Str, (const char*)"+CPIN") != NULL)
    {
        Add_Status(Str, &GSM_Data->CPIN);
    }
    else if (strstr((const char*)Str, (const char*)"+CCID") != NULL)
    {
        Add_String_Between_Quotes(Str, (char*)GSM_Data->CCID.ICCID);
        Add_Status(Str, &GSM_Data->CCID.Status);
    }
    else if (strstr((const char*)Str, (const char*)"+QGSN") != NULL)
    {
        Add_String_Between_Quotes(Str, (char*)GSM_Data->QGSN.IMEI);
        Add_Status(Str, &GSM_Data->QGSN.Status);
    }
    else if (strstr((const char*)Str, (const char*)"+IPR") != NULL)
    {
        Add_Status(Str, &GSM_Data->IPR);
    }
    else if (strstr((const char*)Str, (const char*)"+CSCA") != NULL)
    {
        Add_String_Between_Quotes(Str, (char*)GSM_Data->CSCA.SCA);
        const char* Comma_Ptr = strchr(Str, ',');
        if (Comma_Ptr != NULL)
        {
            GSM_Data->CSCA.To_SCA = atoi(Comma_Ptr + 1);
        }
        Add_Status(Str, &GSM_Data->CSCA.Status);
    }
    else if (strstr((const char*)Str, (const char*)"+CPBR") != NULL)
    {
        uint8_t Phone_Count = 0;
        uint32_t Index = 0, Type = 0;
        char Number[32] = { 0 }, Text[128] = { 0 };
        char* Line;
        char* Start_Ptr;
        char* End_Ptr;

        for (int i = 0; i < MAX_PHONE_COUNT; i++)
        {
            memset(&GSM_Data->CPBR[i], 0, sizeof(GSM_Data->CPBR[i]));
        }

        Start_Ptr = Str;
        while ((Start_Ptr = strstr(Start_Ptr, "+CPBR:")) != NULL)
        {
            Line = Start_Ptr;
            if ((End_Ptr = strchr(Line, '\r')) != NULL)
            {
                *End_Ptr = '\0';
            }
            if (sscanf(Line, "+CPBR: %u,\"%[^\"]\",%u,\"%[^\"]\"", &Index, Number, &Type, Text) > 0)
            {
                GSM_Data->CPBR[Phone_Count].Index = (uint8_t)Index;
                strncpy((char*)GSM_Data->CPBR[Phone_Count].Number, Number, sizeof(GSM_Data->CPBR[Phone_Count].Number) - 1);
                GSM_Data->CPBR[Phone_Count].Type = (uint8_t)Type;
                strncpy((char*)GSM_Data->CPBR[Phone_Count].Text, Text, sizeof(GSM_Data->CPBR[Phone_Count].Text) - 1);
                Phone_Count++;
            }
            if (End_Ptr != NULL)
            {
                *End_Ptr = '\r';
                Start_Ptr = End_Ptr + 2; 
            }
        }
    }
    else if (strstr((const char*)Str, (const char*)"+COPS") != NULL)
    {
        uint32_t Mode, Format;
        char Oper[16] = { 0 };

        if (sscanf(Str, "\r\n+COPS: %u,%u, \"%[^\"]\"", &Mode, &Format, Oper) > 0)
        {
            GSM_Data->COPS.Mode = (uint8_t)Mode;
            GSM_Data->COPS.Format = (uint8_t)Format;
            strncpy((char*)GSM_Data->COPS.Oper, Oper, sizeof(GSM_Data->COPS.Oper) - 1);
        }
        else
        {
            return;
        }
        Add_Status(Str, &GSM_Data->COPS.Status);
    }
    else if (strstr((const char*)Str, (const char*)"+CREG") != NULL)
    {
        uint32_t N, Stat;
        char Lag[32] = { 0 }, Ci[32] = { 0 };

        if (sscanf(Str, "\r\n+CREG: %u,%u \"%[^\"]\", \"%[^\"]\"", &N, &Stat, Lag, Ci) > 0)
        {
            GSM_Data->CREG.N = (uint8_t)N;
            GSM_Data->CREG.Stat = (uint8_t)Stat;
            strncpy((char*)GSM_Data->CREG.Lag, Lag, sizeof(GSM_Data->CREG.Lag) - 1);
            strncpy((char*)GSM_Data->CREG.Ci, Ci, sizeof(GSM_Data->CREG.Ci) - 1);
        }
        else
        {
            return;
        }
        Add_Status(Str, &GSM_Data->CREG.Status);
    }
    else if (strstr((const char*)Str, (const char*)"+CSQ") != NULL)
    {
        uint8_t Rssi, Ber;

        if (sscanf(Str, "\r\n+CSQ: %hhu,%hhu", &Rssi, &Ber) > 0)
        {
            GSM_Data->CSQ.RSSI = Rssi;
            GSM_Data->CSQ.BER = Ber;
        }
        else
        {
            return;
        }
        const char* Comma_Ptr = strchr(Str, ',');
        if (Comma_Ptr != NULL)
        {
            GSM_Data->CSQ.BER = atoi(Comma_Ptr + 1);
        }
        Add_Status(Str, &GSM_Data->CSQ.Status);
    }
    else if (strstr((const char*)Str, (const char*)"+CMGL") != NULL)
    {
        uint8_t Sms_Count = 0;
        uint8_t Index = 0;
        char* Line;
        char Stat[16] = { 0 }, Sender_Number[32] = { 0 }, Timestamp[32] = { 0 };

        for (int i = 0; i < MAX_SMS_COUNT; i++)
        {
            memset(&GSM_Data->CMGL[i], 0, sizeof(GSM_Data->CMGL[i]));
        }

        Line = strtok(Str, "\n");
        while (Line != NULL && Sms_Count < MAX_SMS_COUNT)
        {
            if (strstr(Line, "+CMGL:") != NULL)
            {
                if (sscanf(Line, "\r\n+CMGL: %hhu,\"%[^\"]\",\"%[^\"]\",\"\",\"%[^\"]\"",
                    &Index, Stat, Sender_Number, Timestamp) > 0)
                {
                    GSM_Data->CMGL[Sms_Count].Index = Index;
                    strncpy((char*)GSM_Data->CMGL[Sms_Count].Stat, Stat, sizeof(GSM_Data->CMGL[Sms_Count].Stat) - 1);
                    strncpy((char*)GSM_Data->CMGL[Sms_Count].Sender_Number, Sender_Number, sizeof(GSM_Data->CMGL[Sms_Count].Sender_Number) - 1);
                    strncpy((char*)GSM_Data->CMGL[Sms_Count].Timestamp, Timestamp, sizeof(GSM_Data->CMGL[Sms_Count].Timestamp) - 1);
                }
                else
                {
                    return;
                }
            }
            else if(strcmp(Line, "\r"))
            {
								uint16_t Len = strlen(Line);
                strncpy((char*)GSM_Data->CMGL[Sms_Count].Text, Line, Len);
                GSM_Data->CMGL[Sms_Count].Text[Len - 1] = '\0';
                Sms_Count++;
            }
            Line = strtok(NULL, "\n");
            Add_Status(Str, &GSM_Data->CMGL[Sms_Count].Status);
        }
    }
		else if(strstr((const char*)Str, (const char*)"+CCLK") != NULL)
		{			
			Str+=0x04;
			
			char* token = strtok(Str, "\"");
			token = strtok(NULL, "/, :+");
			GSM_Data->CCLK.YY = atoi(token);
    
			token = strtok(NULL, "/, :+");
			GSM_Data->CCLK.MM = atoi(token);
    
			token = strtok(NULL, "/, :+");
			GSM_Data->CCLK.DD = atoi(token);
    
			token = strtok(NULL, "/, :+");
			GSM_Data->CCLK.hh = atoi(token);
    
			token = strtok(NULL, "/, :+");
			GSM_Data->CCLK.mm = atoi(token);
    
			token = strtok(NULL, "/, :+");
			GSM_Data->CCLK.ss = atoi(token);
		}
    free(Str);
}

void Add_String_Between_Quotes(char* Str, char* Dest)
{
	if (Str == NULL || Dest == NULL) return;
  const char* Start = strchr(Str, '\"');
  if (Start)
  {
		const char* End = strchr(Start + 1, '\"');
    if (End)
    {
			uint16_t Len = End - (Start + 1);
      strncpy(Dest, Start + 1, Len);
      Dest[Len] = '\0';
		}
  }
}

void Add_Status(char* Str, uint8_t* Dest)
{
	char* ERROR_Ptr = strstr(Str, "ERROR");
	if (ERROR_Ptr != NULL) *Dest = 0;
  else*Dest = 1;
}

void Parse_PDU(GSM* GSM_Data)
{
	for(uint8_t i = 0; i < MAX_SMS_COUNT; i++) 
	{
		uint8_t* Str_Pos = GSM_Data->CMGL[i].Text + 18;
		uint8_t SubStr[MAX_SMS_TEXT_LENGTH] = {0,};
		uint8_t Byte_Value = 0;
		
		strncpy((char*)SubStr, (const char*)Str_Pos, 2);
		Byte_Value = (uint8_t)strtol((const char*)SubStr, NULL, 16);//Length of the SMSC information (in this case 7 octets)
		memset(SubStr, 0, 32);
		
		Str_Pos += 4;
		strncpy((char*)GSM_Data->CMGL[i].Sender_Number, (const char*)Str_Pos, Byte_Value + 1);
		swap_pairs((uint8_t*)&GSM_Data->CMGL[i].Sender_Number); //Sender Phone Number
		memset(SubStr, 0, 32);
		
		Str_Pos += 2 + Byte_Value + 1;
		strncpy((char*)SubStr, (const char*)Str_Pos, 2);
		uint32_t TPDCS_Len = 2;
		ConvertHexStrToByteArray((uint8_t*)SubStr, GSM_Data->CMGL[i].DCS, &TPDCS_Len);//DCS
		memset(SubStr, 0, 32);
		
		Str_Pos += 2;
		strncpy((char*)SubStr, (const char*)Str_Pos, 14);
		swap_pairs((uint8_t*)SubStr);
		uint8_t len = strlen((const char*)SubStr);
		uint8_t lastTwoDigits = (SubStr[len - 2] - '0') * 10 + (SubStr[len - 1] - '0');
    uint8_t result = lastTwoDigits / 4;
    SubStr[len - 2] = '0' + result / 10;
    SubStr[len - 1] = '0' + result % 10;
    strcpy((char*)GSM_Data->CMGL[i].Timestamp, (const char*)SubStr);
		memset(SubStr, 0, 32);
		
		Str_Pos += 14;
		strncpy((char*)SubStr, (const char*)Str_Pos, 2);
		Byte_Value = (uint8_t)strtol((const char*)SubStr, NULL, 16);//TP-UDL. User data length, length of message. 
		memset(SubStr, 0, 32);
		
		Str_Pos += 2;
		strncpy((char*)SubStr, (const char*)Str_Pos, Byte_Value * 2);// TP-User-Data, user message
		strcpy((char*)GSM_Data->CMGL[i].Text, (const char*)SubStr);
		
		switch(GSM_Data->CMGL[i].DCS[0])
		{
			default:		
				return;
			case 0x00:
					Decode7bit(&GSM_Data->CMGL[i], SubStr, Byte_Value * 2);
					break;
			case 0x08:
					DecodeUCS2(&GSM_Data->CMGL[i], SubStr, Byte_Value * 2);
					break;
		}
	}
}

void Decode7bit(CMGL* CMGL, uint8_t* extractedString, uint32_t cchMsgLength)
{
	memset(CMGL->Text, 0, MAX_SMS_TEXT_LENGTH);
	ConvertHexStrToByteArray(extractedString, CMGL->Text, &cchMsgLength);
	DecodePDU(CMGL->Text, cchMsgLength, extractedString);
	strcpy((char*)CMGL->Text, (const char*)extractedString);
}

void DecodeUCS2(CMGL* CMGL, uint8_t* extractedString, uint32_t cchMsgLength)
{
	memset(CMGL->UCS2_Text, 0, MAX_SMS_TEXT_LENGTH);
	memset(CMGL->Text, 0, MAX_SMS_TEXT_LENGTH);
	uint16_t Buff[cchMsgLength];
	ConvertHexStrTo2ByteArray(extractedString, Buff, &cchMsgLength);
	for (int i = 0, j = 0; i < cchMsgLength; i += 2, j++) 
		CMGL->UCS2_Text[j] = (Buff[i] << 8) | Buff[i + 1];
}

void swap_pairs(uint8_t* str)
{
	uint16_t size = strlen((const char*)str);
	for (int i = 0; i < size - 1; i += 2) 
	{
		char temp = str[i];
    str[i] = str[i + 1];
    str[i + 1] = temp;
  }
}

void DecodePDU(uint8_t* PDU, uint32_t PduLength, uint8_t* Dest)
{
	uint8_t Shift = 0;
	uint8_t Prev = 0;
	uint8_t Index = 0;
	for (int i = 0; i < PduLength; i++)
	{
		uint8_t Byte = ((PDU[i] << Shift) | Prev) & 0x7F;
		if(Shift == 7)
		{
			Dest[Index] = Byte;
			Prev = Shift = 0;
			Index++;
		}
		Prev = PDU[i] >> (7 - Shift);
		Dest[Index] = Byte;
		Shift++;
		Index++;
	}
	Dest[Index] = '\0';
}

void ConvertHexStrToByteArray(const uint8_t* hexString, uint8_t* byteArray, uint32_t* byteArrayLength)
{
	*byteArrayLength = strlen((const char*)hexString) / 2;
  for (int i = 0; i < *byteArrayLength; i++)
		sscanf((const char*)&hexString[i * 2], "%2hhx", &byteArray[i]);
}
void ConvertHexStrTo2ByteArray(const uint8_t* hexString, uint16_t* byteArray, uint32_t* byteArrayLength)
{
	*byteArrayLength = strlen((const char*)hexString) / 2;
  for (int i = 0; i < *byteArrayLength; i++)
		sscanf((const char*)&hexString[i * 2], "%2hx", &byteArray[i]);
}

uint8_t* Prepare_PDU_SMS(uint8_t DCS, const uint8_t* Phone_Number, const uint8_t* Message)
{
	uint8_t static Str[MAX_SMS_TEXT_LENGTH] = {0,};
	uint8_t SubStr[MAX_SMS_TEXT_LENGTH] = {0,};
	strcat((char*)Str, "00A100");
	sprintf((char*)SubStr, "%02X", strlen((const char*)Phone_Number));
	strcat((char*)Str, (const char*)SubStr);
	strcat((char*)Str, "91");
	strcpy((char*)SubStr, (const char*)Phone_Number);
	strcat((char*)SubStr, "F");
	swap_pairs(SubStr);
	strcat((char*)Str, (const char*)SubStr);
	strcat((char*)Str, "00");
	sprintf((char*)SubStr, "%02X", DCS);
	strcat((char*)Str, (const char*)SubStr);
	
	switch(DCS)
	{
		default:
			return NULL;
		case 0x00:
			sprintf((char*)SubStr, "%02X", strlen((const char*)Message));
			strcat((char*)Str, (const char*)SubStr);
			strcat((char*)Str, (const char*)Encode7bit((uint8_t*)Message));
			break;
		case 0x04:
			sprintf((char*)SubStr, "%02X", strlen((const char*)Message));
			strcat((char*)Str, (const char*)SubStr);
			memset(SubStr, 0, MAX_SMS_TEXT_LENGTH);
			strcpy((char*)SubStr, (const char*)Message);
			while (*Message) 
			{
				sprintf((char*)SubStr, "%02X",  *Message);
				strcat((char*)Str, (const char*)SubStr);
        Message++;
			}
			break;
		case 0x08:
			sprintf((char*)SubStr, "%02X", strlen((const char*)Message));
			strcat((char*)Str, (const char*)SubStr);
			memset(SubStr, 0, MAX_SMS_TEXT_LENGTH);
			EncodeUCS2(Message, SubStr);
			strcat((char*)Str, (const char*)SubStr);
			break;
	}
	return Str;
}

uint8_t* Encode7bit(uint8_t* Message)
{
	uint8_t S[256];
	uint8_t static Dest[256];
	uint8_t PduLength = strlen((const char*)Message);
	uint8_t Next, Index, i;
	int8_t Shift;
	Index = i = Shift = 0;
	
	for(i = 0; i < PduLength; i++, Shift++)
	{
		if(Shift == 7)
		{
			Shift = -1;
			continue;
		}
		if(i == PduLength - 1)
		{
			S[Index] = (Message[i] >> Shift);
			break;
		}
		Next = Message[i + 1] << (uint8_t)(7 - Shift); 
		S[Index++] = (Message[i] >> Shift) | Next;
		
	}
	Index = 0;
	PduLength = strlen((const char*)S);
	for(i = 0; i < PduLength; i++)
	{
		sprintf((char*)Dest + Index, "%02X", S[i]);
		Index += 2;
	}
	return Dest;
}

void EncodeUCS2(const uint8_t *Utf8_Str, uint8_t* Str) 
{
	uint8_t substr[4];
  while (*Utf8_Str != '\0') 
	{
		uint16_t ucs2_char = 0;
       
		if ((*Utf8_Str & 0x80) == 0) 
		{
			// ASCII character
      ucs2_char = *Utf8_Str;
    } else if ((*Utf8_Str & 0xE0) == 0xC0) 
		{
			// 2-byte character
      ucs2_char = ((*Utf8_Str & 0x1F) << 6) | (*(Utf8_Str + 1) & 0x3F);
      Utf8_Str++;
    } else if ((*Utf8_Str & 0xF0) == 0xE0) 
		{
			// 3-byte character
      ucs2_char = ((*Utf8_Str & 0x0F) << 12) | ((*(Utf8_Str + 1) & 0x3F) << 6) | (*(Utf8_Str + 2) & 0x3F);
      Utf8_Str += 2;
		} 
		sprintf((char*)substr, "%04X", ucs2_char);
    strcat((char*)Str, (const char*)substr);
    Utf8_Str++;
  }
}
void Send_SMS_PDU(uint8_t DCS, const uint8_t* Phone_Number, const uint8_t* SMS_Text)
{
	if(!GSM_Init_Flag)
	{
		GSM_Init();
	}
	
	uint8_t* Str_Ptr;
	uint8_t Str_Oct_Len[3];
	
	Str_Ptr = Prepare_PDU_SMS(DCS, Phone_Number, SMS_Text);
	sprintf((char*)Str_Oct_Len, "%d", (strlen((const char*)Str_Ptr) / 2) - 1);
	
	Transmit_Data("AT+CMGF=0");
	uint8_t CMGS_Command[16] = "AT+CMGS=";
	strcat((char*)CMGS_Command, (const char*)Str_Oct_Len);
	Transmit_Data(CMGS_Command);
	Transmit_Data(Str_Ptr);
	Transmit_Data("\x1A");
	free(Str_Ptr);
}

void GSM_Init(void)
{
	Transmit_Data("ATE0");
	Transmit_Data("AT+IPR=38400;&W");
	//Transmit_Data("AT+CSCS=\"GSM\"");
	Transmit_Data("AT+CMGF=0");
	Transmit_Data("AT+CNMI=2,1,0,1,0");
	GSM_Init_Flag = 1;
}

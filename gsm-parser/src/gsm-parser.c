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

    if (strstr((const char*)Str, (const char*)"+CPIN") != NULL)
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
                strncpy((char*)GSM_Data->CMGL[Sms_Count].Text, Line, MAX_SMS_TEXT_LENGTH);
                GSM_Data->CMGL[Sms_Count].Text[MAX_SMS_TEXT_LENGTH - 1] = '\0';
                Sms_Count++;
            }
            Line = strtok(NULL, "\n");

            Add_Status(Str, &GSM_Data->CMGL[Sms_Count].Status);

        }

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

    if (ERROR_Ptr != NULL)
        *Dest = 0;
    else
        *Dest = 1;
}

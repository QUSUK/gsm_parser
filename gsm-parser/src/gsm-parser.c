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

    char* token = strtok((char*)Rx_Buff, ":");

    if (strcmp(token, "+CPIN") == 0)
    {
        Add_Status(Str, &GSM_Data->CPIN);

    }
    else if (strcmp(token, "+CCID") == 0)
    {

        Add_String_Between_Quotes(Str, (char*)GSM_Data->CCID.ICCID);

        Add_Status(Str, &GSM_Data->CCID.Status);

    }
    else if (strcmp(token, "+QGSN") == 0)
    {
        Add_String_Between_Quotes(Str, (char*)GSM_Data->QGSN.IMEI);

        Add_Status(Str, &GSM_Data->QGSN.Status);

    }
    else if (strcmp(token, "+IPR") == 0)
    {
        Add_Status(Str, &GSM_Data->IPR);

    }
    else if (strcmp(token, "+CSCA") == 0)
    {
        Add_String_Between_Quotes(Str, (char*)GSM_Data->CSCA.SCA);

        const char* Comma_Ptr = strchr(Str, ',');

        if (Comma_Ptr != NULL)
        {
            GSM_Data->CSCA.To_SCA = atoi(Comma_Ptr + 1);
        }

        Add_Status(Str, &GSM_Data->CSCA.Status);

    }
    else if (strcmp(token, "+CPBR") == 0)
    {
        uint32_t Index, Type;
        char Number[32] = { 0 }, Text[128] = { 0 };

        if (sscanf(Str, "+CPBR: %u,\"%[^\"]\",%u,\"%[^\"]\"", &Index, Number, &Type, Text) > 0)
        {
            GSM_Data->CPBR.Index = (uint8_t)Index;
            strncpy((char*)GSM_Data->CPBR.Number, Number, sizeof(GSM_Data->CPBR.Number) - 1);
            GSM_Data->CPBR.Type = (uint8_t)Type;
            strncpy((char*)GSM_Data->CPBR.Text, Text, sizeof(GSM_Data->CPBR.Text) - 1);
        }
        else
        {
            return;
        }

        Add_Status(Str, &GSM_Data->CPBR.Status);

    }
    else if (strcmp(token, "+COPS") == 0)
    {
        uint32_t Mode, Format;
        char Oper[16] = { 0 };

        if (sscanf(Str, "+COPS: %u,%u,\"%[^\"]\"", &Mode, &Format, Oper) > 0)
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
    else if (strcmp(token, "+CREG") == 0)
    {
        uint32_t N, Stat;
        char Lag[32] = { 0 }, Ci[32] = { 0 };

        if (sscanf(Str, "+CREG: %u,%u \"%[^\"]\", \"%[^\"]\"", &N, &Stat, Lag, Ci) > 0)
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
    else if (strcmp(token, "+CSQ") == 0)
    {
        uint8_t Rssi, Ber;

        if (sscanf(Str, "+CSQ: %hhu,%hhu", &Rssi, &Ber) > 0)
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
    else if (strcmp(token, "+CMGL") == 0)
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
                if (sscanf(Line, "+CMGL: %hhu,\"%[^\"]\",\"%[^\"]\",\"\",\"%[^\"]\"",
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
            else
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
        *Dest = 1;
    else
        *Dest = 0;
}

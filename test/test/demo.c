#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <stdbool.h>

#define MAX_LENGTH 1000  // ���ڴ洢�����ַ�������󳤶�
#define MAX_TOKENS 16    // ��������� 20 �����ַ���

HANDLE hCom;

int splitString(const char* str, char* tokens[], const char* delimiter) 
{
    int count = 0;
    char* ptr;

    ptr = strtok(str, delimiter);
    while (ptr != NULL && count < MAX_TOKENS) 
    {
        tokens[count++] = ptr;
        ptr = strtok(NULL, delimiter);
    }

    return count;
}



int main() {

    //�򿪴��ڶ�����
    hCom = CreateFile(TEXT("com2"),//������ѡ��COM��
        GENERIC_READ, //�����
        0, //ָ���������ԣ����ڴ��ڲ��ܹ������Ըò�������Ϊ0
        NULL,
        OPEN_EXISTING, //�򿪶����Ǵ���

        FILE_ATTRIBUTE_NORMAL, //������������ֵΪFILE_FLAG_OVERLAPPED����ʾʹ���첽I/O���ò���Ϊ0����ʾͬ��I/O����
        NULL);

    if (hCom == INVALID_HANDLE_VALUE)
    {
        printf("��COMʧ��!\n");
        return FALSE;
    }
    else
    {
        printf("COM�򿪳ɹ���\n");
    }

    SetupComm(hCom, 1024, 1024); //���뻺����������������Ĵ�С����1024

    /*********************************��ʱ����**************************************/
    COMMTIMEOUTS TimeOuts;
    //�趨����ʱ
    TimeOuts.ReadIntervalTimeout = MAXDWORD;//�������ʱ
    TimeOuts.ReadTotalTimeoutMultiplier = 0;//��ʱ��ϵ��
    TimeOuts.ReadTotalTimeoutConstant = 0;//��ʱ�䳣��
    //�趨д��ʱ
    TimeOuts.WriteTotalTimeoutMultiplier = 1;//дʱ��ϵ��
    TimeOuts.WriteTotalTimeoutConstant = 1;//дʱ�䳣��
    SetCommTimeouts(hCom, &TimeOuts); //���ó�ʱ

    /*****************************************���ô���***************************/
    DCB dcb;
    GetCommState(hCom, &dcb);
    dcb.BaudRate = 115200; //������Ϊ115200
    dcb.ByteSize = 8; //ÿ���ֽ���8λ
    dcb.Parity = NOPARITY; //����żУ��λ
    dcb.StopBits = ONESTOPBIT; //һ��ֹͣλ
    SetCommState(hCom, &dcb);

    DWORD wCount;//ʵ�ʶ�ȡ���ֽ���
    bool bReadStat;

    char str[100] = { 0 };

    //�����������ݲ���
    //��ָ���м����ж������ж���
    char located[20];//������λ�����or���ԣ�
    char towards[20];//Ŀ�꺽�� �����or���ԣ�
    char states[20];//Ŀ��״̬
    char TFA[20];//��ȡ����������
    char* TFA_temp;
    char speedUnit[20];
    char data[MAX_LENGTH];  // ���ڴ洢������ַ���

    char* tokens[MAX_TOKENS];  // �洢���ַ���������
    int count;  // ���ַ�������

    while (1)
    {
        //PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //��ջ�����
        bReadStat = ReadFile(hCom, str, sizeof(str), &wCount, NULL);

        if (!bReadStat)
        {
            printf("������ʧ��!");
            return FALSE;
        }
        else
        {
            if (wCount <= 34) {
               //printf("û��ȡ��8���ֽ�����\n��");
                printf("������Ҫ������ָ��: \n");
            }
            else {


              
               char* newline = strchr(str, '\n');//�ڵ�data�в��һ��з�
                // ���ʹ��fgets��ȡ�˻��з�������Ҫ�����滻Ϊ�ַ���������


                if (newline != NULL) {
                    *newline = '\0';//  '\0'���ж��ַ���������ı�ʶ
                }
                // ���÷ָ��ַ����ĺ���
                count = splitString(str, tokens, ",");

                // ��ӡ�����д洢�����ַ���
                //for (int i = 0; i < count; i++) {
                //    printf("Token %d: %s\n", i + 1, tokens[i]);
                //}

                // ���� tokens ������������� located �� speed ��ֵ
                //strcmp�� str1 ��ָ����ַ����� str2 ��ָ����ַ������бȽ�
                if (strcmp(tokens[4], "T") == 0)
                {
                    strcpy(located, "������λ:����λ��");
                }
                else if (strcmp(tokens[4], "R") == 0) {
                    strcpy(located, "������λ:���λ��");
                }

                if (strcmp(tokens[7], "T") == 0)
                {
                    strcpy(towards, "Ŀ�꺽��:����λ��");
                }
                else if (strcmp(tokens[7], "R") == 0) {
                    strcpy(towards, "Ŀ�꺽��:���λ��");
                }


                if (strcmp(tokens[12], "T") == 0)
                {
                    strcpy(states, "Ŀ��״̬:���ڸ�����");
                }
                else if (strcmp(tokens[12], "Q") == 0) {
                    strcpy(states, "Ŀ��״̬:��ѯ��");
                }
                else if (strcmp(tokens[12], "L") == 0) {
                    strcpy(states, "Ŀ��״̬:��ʧ");
                }

                if (strcmp(tokens[10], "K") == 0)
                {
                    strcpy(speedUnit, "�ٶȵ�λ:����/Сʱ");
                }
                else if (strcmp(tokens[10], "N") == 0) {
                    strcpy(speedUnit, "�ٶȵ�λ:��");
                }
                else if (strcmp(tokens[10], "S") == 0) {
                    strcpy(speedUnit, "�ٶȵ�λ:����Ӣ��/Сʱ");
                }

                TFA_temp = strtok(tokens[15], "*");

                if (strcmp(strtok(TFA_temp, "*"), "A") == 0)
                {
                    strcpy(TFA, "��ȡ����:�Զ�");
                }
                else if (strcmp(strtok(TFA_temp, "*"), "M") == 0) {
                    strcpy(TFA, "��ȡ����:�ֶ�");
                }
                else if (strcmp(strtok(TFA_temp, "*"), "R") == 0) {
                    strcpy(TFA, "��ȡ����:����");
                }

                printf("���͸�ʽ:%s\n�����ı��:%s\nĿ����뱾���ľ���:%s����\n������λ:%s��\n%s\nĿ���ٶ�:%s��\nĿ�꺽��:%s��\n%s\n�������������:%s����\n������������ʱ��:%smin\n%s\nĿ������:%s\n%s\n���ݻ�ȡʱ��:%s\n%s\n",
                    tokens[0], tokens[1], tokens[2], tokens[3], located, tokens[5], tokens[6], towards, tokens[8], tokens[9], speedUnit, tokens[11], states, tokens[14], TFA);
                return 0;
               
            }
        }
        Sleep(100);
    }

    CloseHandle(hCom);

  
}

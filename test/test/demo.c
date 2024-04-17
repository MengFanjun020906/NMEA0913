#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <stdbool.h>

#define MAX_LENGTH 1000  // 用于存储输入字符串的最大长度
#define MAX_TOKENS 16    // 假设最多有 20 个子字符串

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

    //打开串口读部分
    hCom = CreateFile(TEXT("com2"),//在这里选择COM口
        GENERIC_READ, //允许读
        0, //指定共享属性，由于串口不能共享，所以该参数必须为0
        NULL,
        OPEN_EXISTING, //打开而不是创建

        FILE_ATTRIBUTE_NORMAL, //属性描述，该值为FILE_FLAG_OVERLAPPED，表示使用异步I/O，该参数为0，表示同步I/O操作
        NULL);

    if (hCom == INVALID_HANDLE_VALUE)
    {
        printf("打开COM失败!\n");
        return FALSE;
    }
    else
    {
        printf("COM打开成功！\n");
    }

    SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024

    /*********************************超时设置**************************************/
    COMMTIMEOUTS TimeOuts;
    //设定读超时
    TimeOuts.ReadIntervalTimeout = MAXDWORD;//读间隔超时
    TimeOuts.ReadTotalTimeoutMultiplier = 0;//读时间系数
    TimeOuts.ReadTotalTimeoutConstant = 0;//读时间常量
    //设定写超时
    TimeOuts.WriteTotalTimeoutMultiplier = 1;//写时间系数
    TimeOuts.WriteTotalTimeoutConstant = 1;//写时间常量
    SetCommTimeouts(hCom, &TimeOuts); //设置超时

    /*****************************************配置串口***************************/
    DCB dcb;
    GetCommState(hCom, &dcb);
    dcb.BaudRate = 115200; //波特率为115200
    dcb.ByteSize = 8; //每个字节有8位
    dcb.Parity = NOPARITY; //无奇偶校验位
    dcb.StopBits = ONESTOPBIT; //一个停止位
    SetCommState(hCom, &dcb);

    DWORD wCount;//实际读取的字节数
    bool bReadStat;

    char str[100] = { 0 };

    //解析串口数据部分
    //对指令中几个判断量进行定义
    char located[20];//本船方位（相对or绝对）
    char towards[20];//目标航向 （相对or绝对）
    char states[20];//目标状态
    char TFA[20];//获取的数据类型
    char* TFA_temp;
    char speedUnit[20];
    char data[MAX_LENGTH];  // 用于存储输入的字符串

    char* tokens[MAX_TOKENS];  // 存储子字符串的数组
    int count;  // 子字符串数量

    while (1)
    {
        //PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //清空缓冲区
        bReadStat = ReadFile(hCom, str, sizeof(str), &wCount, NULL);

        if (!bReadStat)
        {
            printf("读串口失败!");
            return FALSE;
        }
        else
        {
            if (wCount <= 34) {
               //printf("没读取到8个字节数据\n！");
                printf("请输入要解析的指令: \n");
            }
            else {


              
               char* newline = strchr(str, '\n');//在的data中查找换行符
                // 如果使用fgets读取了换行符，则需要将其替换为字符串结束符


                if (newline != NULL) {
                    *newline = '\0';//  '\0'是判定字符数组结束的标识
                }
                // 调用分隔字符串的函数
                count = splitString(str, tokens, ",");

                // 打印数组中存储的子字符串
                //for (int i = 0; i < count; i++) {
                //    printf("Token %d: %s\n", i + 1, tokens[i]);
                //}

                // 根据 tokens 数组的内容设置 located 和 speed 的值
                //strcmp把 str1 所指向的字符串和 str2 所指向的字符串进行比较
                if (strcmp(tokens[4], "T") == 0)
                {
                    strcpy(located, "本船方位:绝对位置");
                }
                else if (strcmp(tokens[4], "R") == 0) {
                    strcpy(located, "本船方位:相对位置");
                }

                if (strcmp(tokens[7], "T") == 0)
                {
                    strcpy(towards, "目标航向:绝对位置");
                }
                else if (strcmp(tokens[7], "R") == 0) {
                    strcpy(towards, "目标航向:相对位置");
                }


                if (strcmp(tokens[12], "T") == 0)
                {
                    strcpy(states, "目标状态:正在跟踪中");
                }
                else if (strcmp(tokens[12], "Q") == 0) {
                    strcpy(states, "目标状态:查询中");
                }
                else if (strcmp(tokens[12], "L") == 0) {
                    strcpy(states, "目标状态:丢失");
                }

                if (strcmp(tokens[10], "K") == 0)
                {
                    strcpy(speedUnit, "速度单位:公里/小时");
                }
                else if (strcmp(tokens[10], "N") == 0) {
                    strcpy(speedUnit, "速度单位:节");
                }
                else if (strcmp(tokens[10], "S") == 0) {
                    strcpy(speedUnit, "速度单位:法定英里/小时");
                }

                TFA_temp = strtok(tokens[15], "*");

                if (strcmp(strtok(TFA_temp, "*"), "A") == 0)
                {
                    strcpy(TFA, "获取类型:自动");
                }
                else if (strcmp(strtok(TFA_temp, "*"), "M") == 0) {
                    strcpy(TFA, "获取类型:手动");
                }
                else if (strcmp(strtok(TFA_temp, "*"), "R") == 0) {
                    strcpy(TFA, "获取类型:报告");
                }

                printf("发送格式:%s\n船舶的编号:%s\n目标距离本船的距离:%s海里\n本船方位:%s度\n%s\n目标速度:%s节\n目标航向:%s度\n%s\n距离最近会遇点:%s海里\n到最近会遇点的时间:%smin\n%s\n目标名称:%s\n%s\n数据获取时间:%s\n%s\n",
                    tokens[0], tokens[1], tokens[2], tokens[3], located, tokens[5], tokens[6], towards, tokens[8], tokens[9], speedUnit, tokens[11], states, tokens[14], TFA);
                return 0;
               
            }
        }
        Sleep(100);
    }

    CloseHandle(hCom);

  
}

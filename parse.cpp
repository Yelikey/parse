#include <iostream>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;

void parse(string str, int lenth);
int hexToDec(string str);
void hexToChar(string str, int n);
void msgParse(string msg, int prm);
void dataCenter(int iop, string msg, int flag, int prm);
int msgLenParse(string msg, int flag);
void dataType(string *pmsg, int flag);
int dataOAD(string msg, int flag, int n);
void diInfoParse(string msg, int flag);
void generalData(string msg, int flag);
int bufferLenParse(string msg, int flag);
void recordData(string msg, int flag);

int main()
{
    string str;
    int lenth;
    cout << "输入ASN.1报文: (q to quit)\n";
    while (getline(cin, str) && str[0] != 'q')
    {
        cout << "--------开始解析--------\n";
        lenth = str.size();
        cout << "接收" << lenth / 3 + 1 << "个字节\n";
        parse(str, lenth);
        // cout << str << endl;
        cout << "输入下一条ASN.1报文：(q to quit)\n";
    }
    return 0;
}

void parse(string str, int lenth)
{
    string *pstr = &str;
    int flag = 0; //下标

    // PRM：启动标志位 PRIORITY：优先级 1个字节
    int head = hexToDec(str);
    int prm = head & 1;
    cout << "启动标志位：" << prm;
    if (prm == 1)
    {
        cout << " 发送\n";
    }
    else
    {
        cout << " 应答\n";
    }

    int priority = head >> 1;
    cout << "优先级：" << priority << endl;
    flag = flag + 3;

    // INDEX：消息序号2个字节  LABEL：消息标签 2个字节
    string index = str.substr(3, 6);
    string label = str.substr(9, 6);
    cout << "消息序号: " << index << "\t消息标签: " << label << endl;
    flag = flag + 12;

    // SOURCE：消息发送方名称 DESTINATION：消息接收方名称 长度不固定
    int flag1 = str.find("00", flag);
    int len1 = flag1 - flag + 3;
    string sendname = str.substr(flag, len1);
    cout << "消息发送方: ";
    hexToChar(sendname, len1);
    flag = flag + len1;

    int flag2 = str.find("00", flag);
    int len2 = flag2 - flag + 3;
    string recname = str.substr(flag, len2);
    cout << "消息接收方: ";
    hexToChar(recname, len2);
    flag = flag + len2;

    // MSG 解析
    string msg = str.substr(flag, lenth);
    cout << "消息体: " << msg << endl;
    msgParse(msg, prm);
}

//一个字节16进制转10进制
int hexToDec(string str)
{
    int sum1, sum2;
    if (str[0] >= '0' && str[0] <= '9')
    {
        sum1 = str[0] - '0';
    }
    else if (str[0] >= 'A' && str[0] <= 'F')
    {
        sum1 = str[0] - 'A' + 10;
    }

    if (str[1] >= '0' && str[1] <= '9')
    {
        sum2 = str[1] - '0';
    }
    else if (str[1] >= 'A' && str[1] <= 'F')
    {
        sum2 = str[1] - 'A' + 10;
    }

    return sum1 * 16 + sum2;
}

// 16进制转字符
void hexToChar(string str, int n)
{
    string str_tmp;
    for (int i = 0; i < n; i += 3)
    {
        str_tmp = str.substr(i, 3);
        cout << (char)hexToDec(str_tmp);
    }
    cout << endl;
}

// msg解析
void msgParse(string msg, int prm)
{
    int flag = 0;
    int iop = (msg[0] - '0') * 10 + (msg[1] - '0') + (msg[3] - '0') * 1000 + (msg[4] - '0') * 100;
    int iid = (msg[6] - '0') * 10 + (msg[7] - '0') + (msg[9] - '0') * 1000 + (msg[10] - '0') * 100;
    flag = flag + 12;

    switch (iid)
    {
    case 0:
        cout << "IID: 0000 通用消息接口 \n";
        break;
    case 1:
        cout << "IID: 0001 系统管理接口 \n";
        break;
    case 2:
        cout << "IID: 0002 无线拨号管理接口 \n";
        break;
    case 3:
        cout << "IID: 0003 本地通信管理接口 \n";
        break;
    case 4:
        cout << "IID: 0004 扩展模块管理接口 \n";
        break;
    case 5:
        cout << "IID: 0005 数据中心接口 \n";
        dataCenter(iop, msg, flag, prm);
        break;
    case 6:
        cout << "IID: 0006 安全管理接口 \n";
        break;
    case 8:
        cout << "IID: 0008 遥信脉冲采样接口 \n";
        break;
    case 9:
        cout << "IID: 0009 串口管理接口 \n";
        break;
    case 17:
        cout << "IID: 000A 蓝牙管理接口 \n";
        break;
    case 1003:
        cout << "IID: 1003 电表采集任务管理接口 \n";
        break;
    default:
        cout << "未定义接口";
        break;
    }
}

void dataCenter(int iop, string msg, int flag, int prm)
{
    int flag_tmp = msgLenParse(msg, flag);
    flag += flag_tmp;
    // deviceId逻辑设备号 infoId信息点号 dataTypeId数据类标识  dataId数据项标识 dataFlag属性标识
    // handle数据库句柄 dataNum数据记录条数
    string deviceId, infoId, dataTypeId, dataId, dataFlag, str_tmp, handle, dataNum, data;
    // timeChoice 筛选方式  numChoice 筛选序号
    int timeChoice, numChoice, msgLen, msgLenLen, dataCount;

    switch (iop)
    {
    case 1:
        cout << "IOP: 0001 数据更新事件（事件）\n";
        // 1个字节
        deviceId = msg.substr(flag, 3);
        cout << "逻辑设备号: " << deviceId << endl;
        // 2个字节
        infoId = msg.substr(flag + 3, 6);
        cout << "信息点号: " << infoId << endl;
        // 2个字节
        dataTypeId = msg.substr(flag + 9, 6);
        cout << "数据类标识: " << dataTypeId << endl;
        dataType(&msg, flag + 9);
        flag += 15;
        dataOAD(msg, flag, 1);
        break;

    case 2:
        cout << "IOP: 0002 初始化事件（事件）\n";
        // 1个字节
        if (msg[flag + 1] == '0')
        {
            cout << "数据初始化\n";

            deviceId = msg.substr(flag + 3, 3);
            cout << "逻辑设备号: " << deviceId << endl;

            dataType(&msg, flag + 6);
            dataTypeId = msg.substr(flag + 6, 6);
            cout << dataTypeId << endl;
        }
        else
        {
            cout << "恢复出厂参数\n";

            deviceId = msg.substr(flag + 3, 3);
            cout << "逻辑设备号: " << deviceId << endl;
        }
        break;

    case 10:
        cout << "IOP: 0010 读普通数据\n";

        if (prm == 1)
        {
            // 1个字节
            deviceId = msg.substr(flag, 3);
            cout << "逻辑设备号: " << deviceId << endl;
            // 2个字节
            infoId = msg.substr(flag + 3, 6);
            cout << "信息点号: " << infoId << endl;

            diInfoParse(msg, flag + 9);
        }
        else
        {
            generalData(msg, flag);
        }
        break;

    case 11:
        cout << "IOP: 0011 写普通数据\n";
        if (prm == 1)
        {
            // 1个字节
            deviceId = msg.substr(flag, 3);
            cout << "逻辑设备号: " << deviceId << endl;
            // 2个字节
            infoId = msg.substr(flag + 3, 6);
            cout << "信息点号: " << infoId << endl;

            generalData(msg, flag + 9);
        }
        else
        {
            cout << "结果: " << msg << endl;
        }

        break;
    case 12:
        cout << "IOP: 0012 开始读记录型数据\n";
        if (prm == 1)
        {
            // 1个字节
            deviceId = msg.substr(flag, 3);
            cout << "逻辑设备号: " << deviceId << endl;
            // 2个字节
            infoId = msg.substr(flag + 3, 6);
            cout << "信息点号: " << infoId << endl;

            str_tmp = msg.substr(flag + 9, 3);
            timeChoice = hexToDec(str_tmp);
            flag += 12;
            switch (timeChoice)
            {
            case 0:
                cout << "采集起始时间\n";
                break;
            case 1:
                cout << "采集结束时间\n";
                break;
            case 2:
                cout << "采集存储时标\n";
                break;
            case 3:
                cout << "数据存储序号\n";
                break;
            case 4:
                cout << "信息点号\n";
                break;
            default:
                cout << "不筛选\n";
                break;
            }
            if ((timeChoice >= 0) && (timeChoice <= 4))
            {
                string str_start = msg.substr(flag, 12);
                string str_end = msg.substr(flag + 12, 12);
                cout << "筛选区间开始: " << str_start << endl;
                cout << "筛选区间结束: " << str_end << endl;
            }

            str_tmp = msg.substr(flag + 24, 3);
            numChoice = hexToDec(str_tmp);
            cout << "排序方式: ";
            switch (numChoice)
            {
            case 1:
                cout << "升序\n";
                break;
            case 2:
                cout << "降序\n";
                break;
            default:
                cout << "默认不排序\n";
                break;
            }
            flag += 27;

            flag += dataOAD(msg, flag, 0);
            diInfoParse(msg, flag);
        }
        else
        {
            handle = msg.substr(flag, 12);
            cout << "数据库句柄: " << handle << endl;
            dataNum = msg.substr(flag + 12, 12);
            cout << "数据记录条数: " << dataNum << endl;

            int num = 0;
            for (int i = 0, n = 4; n > 0; i += 3, n--)
            {
                str_tmp = dataNum.substr(i, 3);
                num += hexToDec(str_tmp) * pow(16, (n - 1) * 2);
            }
            cout << num << endl;
        }

        break;
    case 13:
        cout << "IOP: 0013 单步读取记录数据\n";
        if (prm == 1)
        {
            handle = msg.substr(flag, 12);
            cout << "数据库句柄: " << handle << endl;
            flag += 12;
            if (msg[flag + 1] == '1')
            {
                str_tmp = msg.substr(flag, 3);
                cout << "附加校验长度: ";
                msgLen = bufferLenParse(str_tmp, flag);
                msgLenLen = msgLenParse(str_tmp, flag);

                str_tmp = msg.substr(flag + msgLenLen, msgLen);
                cout << "附加校验: " << str_tmp << endl;
            }
            else
            {
                cout << "无附加校验\n";
            }
        }
        else
        {
            if (msg[flag + 1] == '1')
            {
                // 2个字节
                infoId = msg.substr(flag + 3, 6);
                cout << "信息点号: " << infoId << endl;
                recordData(msg, flag + 9);
            }
            else
            {
                cout << "无记录信息\n";
            }
        }

        break;
    case 14:
        cout << "IOP: 0014 结束记录型数据读取\n";
        if (prm == 1)
        {
            cout << "数据库句柄: " << msg.substr(flag, 12) << endl;
        }
        else
        {
            cout << "结果: " << msg.substr(flag, 3) << endl;
        }

        break;
    case 15:
        cout << "IOP: 0015 写记录型数据\n";
        if (prm == 1)
        {
            deviceId = msg.substr(flag, 3);
            cout << "逻辑设备号: " << deviceId << endl;
            flag += 3;

            cout << "采集开始时间: " << msg.substr(flag, 12) << endl;
            cout << "采集结束时间: " << msg.substr(flag + 12, 12) << endl;
            cout << "采集存储时标: " << msg.substr(flag + 24, 12) << endl;
            flag += 36;

            infoId = msg.substr(flag, 6);
            cout << "信息点号: " << infoId << endl;

            cout << "数据类标识: ";
            dataType(&msg, flag + 6);
            flag += 12;

            flag += dataOAD(msg, flag, 0);

            str_tmp = msg.substr(flag, 3);
            dataCount = hexToDec(str_tmp);
            cout << "共有" << dataCount << "个数据信息\n";
            flag += 3;

            for (int i = 0; i < dataCount; i++)
            {
                cout << "第" << i + 1 << "个数据: \n";
                dataId = msg.substr(flag, 12);
                cout << "数据项标识: " << dataId << endl;
                dataFlag = msg.substr(flag + 12, 3);
                cout << "属性标识: " << dataFlag << endl;
                flag += 15;

                cout << "数据域长度: ";
                msgLenLen = msgLenParse(msg, flag);
                msgLen = bufferLenParse(msg, flag);
                data = msg.substr(flag + msgLenLen, msgLen);
                cout << "数据域: " << data << endl;
                flag = flag + msgLenLen + msgLen;
            }

            cout << "附加校验长度: ";
            msgLen = bufferLenParse(str_tmp, flag);
            msgLenLen = msgLenParse(str_tmp, flag);

            str_tmp = msg.substr(flag + msgLenLen, msgLen);
            cout << "附加校验: " << str_tmp << endl;
        }
        else
        {
            str_tmp = msg.substr(flag, 12);
            cout << "记录序号: " << str_tmp << endl;
        }

        break;
    case 16:
        cout << "IOP: 0016 更新记录数据\n";
        if (prm == 1)
        {
            deviceId = msg.substr(flag, 3);
            cout << "逻辑设备号: " << deviceId << endl;
            flag += 3;
            flag += dataOAD(msg, flag, 0);

            dataId = msg.substr(flag, 12);
            cout << "数据项标识: " << dataId << endl;
            dataFlag = msg.substr(flag + 12, 3);
            cout << "属性标识: " << dataFlag << endl;
            flag += 15;

            if (msg[flag + 1] == '1')
            {
                cout << "按存储时标筛选: ";
                cout << "采集存储时标: " << msg.substr(flag + 3, 12) << endl;
            }
            else
            {
                cout << "按存储序号筛选: ";
                cout << "采集存储时标: " << msg.substr(flag + 3, 12) << endl;
            }
            flag += 15;

            infoId = msg.substr(flag, 6);
            flag += 6;

            cout << "附加校验长度: ";
            msgLen = bufferLenParse(str_tmp, flag);
            msgLenLen = msgLenParse(str_tmp, flag);
            str_tmp = msg.substr(flag + msgLenLen, msgLen);
            cout << "附加校验: " << str_tmp << endl;
            flag = flag + msgLenLen + msgLen;

            cout << "数据缓冲长度: ";
            msgLenLen = msgLenParse(msg, flag);
            msgLen = bufferLenParse(msg, flag);
            data = msg.substr(flag + msgLenLen, msgLen);
            cout << "数据缓冲: " << data << endl;
        }
        else
        {
            str_tmp = msg.substr(flag, 3);
            cout << "结果: " << str_tmp << endl;
        }

        break;
    case 20:
        cout << "IOP: 0020 获取存储记录条数\n";
        if (prm==1)
        {
            deviceId = msg.substr(flag, 3);
            cout << "逻辑设备号: " << deviceId << endl;
            infoId = msg.substr(flag + 3, 6);
            cout << "信息点号: " << infoId << endl;
            flag += 9;
            flag += dataOAD(msg, flag, 0);
            dataId = msg.substr(flag, 12);
            cout << "次DI: " << dataId << endl;

            str_tmp = msg.substr(flag+12, 3);
            timeChoice = hexToDec(str_tmp);
            flag += 15;
            if (timeChoice==0)
            {
                cout << "查询所有记录\n";
            }
            else
            {
                cout << "按存储时间区间查询\n";
                cout << "筛选区间开始: " << msg.substr(flag, 12) << endl;
                cout << "筛选区间结束: " << msg.substr(flag+12, 12) << endl;
            }

        }
        else
        {
            cout << "记录条数: " << msg.substr(flag, 12) << endl;
        }
        
        break;
    case 21:
        cout << "IOP: 0021 获取存储记录深度\n";
        if (prm==1)
        {
            /* code */
        }
        else
        {
            /* code */
        }
        
        break;
    case 30:
        cout << "IOP: 0030 数据初始化\n";
        break;
    case 31:
        cout << "IOP: 0031 清除指定数据\n";
        break;
    case 32:
        cout << "IOP: 0032 参数初始化\n";
        break;
    default:
        cout << "IOP 解析错误\n";
        break;
    }
}

// MSG’s—Length：消息有效载荷长度，采用可变长度编码（A-XDR）
int msgLenParse(string msg, int flag)
{
    // msgLen报文长度  msgLenLen几个字节表示长度
    int msgLen = 0, msgLenLen = 0;
    string msg_tmp;

    if (msg[flag] == '8')
    {
        int n = msg[flag + 1] - '0';
        // n+1个字节代表长度
        msgLenLen = n * 3 + 3;
        flag = flag + 3;

        for (int i = 0; n > 0; i += 3, n--)
        {
            msg_tmp = msg.substr(flag + i, 3);
            msgLen += hexToDec(msg_tmp) * pow(16, (n - 1) * 2);
        }
    }
    else
    {
        msg_tmp = msg.substr(flag, 3);
        msgLen = hexToDec(msg_tmp);
        // 1个字节代表长度
        msgLenLen = 3;
    }

    cout << "MSG—Length: " << msgLen << endl;
    return msgLenLen;
}

void dataType(string *pmsg, int flag)
{
    int dataTypeId = pmsg->at(flag + 4);
    switch (dataTypeId)
    {
    case '1':
        cout << "不可初始化参数\n";
        break;
    case '2':
        cout << "可初始化参数\n";
        break;
    case '3':
        cout << "普通数据\n";
        break;
    case '4':
        cout << "事件数据（终端类事件数据）\n";
        break;
    case '5':
        cout << "采集数据（采集相关的数据，含电能表事件数据）\n";
        break;
    case '6':
        cout << "不可初始化事件数据\n";
        break;
    default:
        cout << "未定义的数据类标识\n";
        break;
    }
}

int dataOAD(string msg, int flag, int n)
{
    // ROAD 主di OAD 次di
    string road, oadFlag, oad;
    if (msg[flag + 1] == '0')
    {
        cout << "主DI: 无 \n";
        flag += 3;
    }
    else
    {
        road = msg.substr(flag + 3, 12);
        cout << "主DI: " << road << endl;
        flag += 15;
    }

    if (n == 0)
    {
        return flag;
    }

    oadFlag = msg.substr(flag, 3);
    int oadNum = hexToDec(oadFlag);
    if (oadNum == 0)
    {
        cout << "次DI: 无 \n";
        flag += 3;
    }
    else
    {
        cout << "次DI: " << oadNum << " 个\n";
        for (int i = 0; i < oadNum; i++)
        {
            oad = msg.substr(flag + 3 + i * 12, 12);
            cout << "第" << i + 1 << "个次DI: " << oad << endl;
        }
        flag = flag + 3 + oadNum * 12;
    }

    return flag;
}

void diInfoParse(string msg, int flag)
{
    string diInfo, dataFlag;
    string str_tmp = msg.substr(flag, 3);
    flag += 3;
    int diInfoNum = hexToDec(str_tmp);
    cout << "共有" << diInfoNum << "个数据项\n";

    for (int i = 0; i < diInfoNum; i++, flag += 15)
    {
        diInfo = msg.substr(flag, 12);
        dataFlag = msg.substr(flag + 12, 3);
        cout << "第" << i + 1 << "个数据项: " << diInfo
             << " 属性标识: " << dataFlag << endl;
    }
}

void generalData(string msg, int flag)
{
    string diInfo, dataFlag, dataTypeId, dataBuffer;
    int bufferLen = 0, bufferLenLen = 0;
    string str_tmp = msg.substr(flag, 3);
    flag += 3;
    int dataNum = hexToDec(str_tmp);
    cout << "共有" << dataNum << "个普通数据\n";

    for (int i = 0; i < dataNum; i++, flag + 21 + bufferLenLen + bufferLen)
    {
        // 4个字节
        diInfo = msg.substr(flag, 12);
        // 1个字节
        dataFlag = msg.substr(flag + 12, 3);
        cout << "第" << i + 1 << "个普通数据：\n"
             << "数据项标识: " << diInfo << "\n属性标识: " << dataFlag << endl;

        // 2个字节
        dataTypeId = msg.substr(flag + 15, 6);
        cout << "数据类标识: " << dataTypeId;
        dataType(&msg, flag + 15);

        //可变长度
        bufferLenLen = msgLenParse(msg, flag + 21);
        bufferLen = bufferLenParse(msg, flag + 21);
        dataBuffer = msg.substr(flag + 21 + bufferLenLen, bufferLen);
        cout << "数据缓冲: " << dataBuffer << endl;
    }
}

int bufferLenParse(string msg, int flag)
{
    // msgLen报文长度  msgLenLen几个字节表示长度
    int msgLen = 0, msgLenLen = 0;
    string msg_tmp;

    if (msg[flag] == '8')
    {
        int n = msg[flag + 1] - '0';
        // n+1个字节代表长度
        msgLenLen = n * 3 + 3;
        flag = flag + 3;

        for (int i = 0; n > 0; i += 3, n--)
        {
            msg_tmp = msg.substr(flag + i, 3);
            msgLen += hexToDec(msg_tmp) * pow(16, (n - 1) * 2);
        }
    }
    else
    {
        msg_tmp = msg.substr(flag, 3);
        msgLen = hexToDec(msg_tmp);
        // 1个字节代表长度
        msgLenLen = 3;
    }

    return msgLen * 3;
}

void recordData(string msg, int flag)
{
    string str_tmp = msg.substr(flag, 3);
    int dataNum = hexToDec(str_tmp);
    flag += 3;
    cout << "共有" << dataNum << "个记录数据\n";

    // ROAD 主di OAD 次di dataId数据项标识 dataFlag属性标识
    string road, oad, dataId, dataFlag, data;
    int dataLen, dataLenLen;

    for (int i = 0; i < dataNum; i++)
    {
        cout << "第" << i + 1 << "个数据: \n";
        if (msg[flag + 1] == '0')
        {
            cout << "主DI: 无 \n";
            flag += 3;
        }
        else
        {
            road = msg.substr(flag + 3, 12);
            cout << "主DI: " << road << endl;
            flag += 15;
        }

        dataId = msg.substr(flag, 12);
        cout << "数据项标识: " << dataId << endl;
        dataFlag = msg.substr(flag + 12, 3);
        cout << "属性标识: " << dataFlag << endl;
        flag += 15;

        cout << "数据域长度: ";
        dataLenLen = msgLenParse(msg, flag);
        dataLen = bufferLenParse(msg, flag);
        data = msg.substr(flag + dataLenLen, dataLen);
        cout << "数据域: " << data << endl;
        flag = flag + dataLenLen + dataLen;
    }

    if (msg[flag + 1] == '1')
    {
        cout << "数据时标: " << endl;
        flag += 3;
        cout << "采集起始时间: " << msg.substr(flag, 12) << endl;
        cout << "采集结束时间: " << msg.substr(flag + 12, 12) << endl;
        cout << "采集存储时间: " << msg.substr(flag + 24, 12) << endl;
    }
    else
    {
        cout << "无数据时标\n";
    }
}
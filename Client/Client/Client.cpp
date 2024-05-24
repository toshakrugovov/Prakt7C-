#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;//Использование пространства имен std для упрощения записи кода

int main()
{
    WSADATA wsaData;
    ADDRINFO hints;//для указания параметров соединения
    ADDRINFO* addrResult;//для хранения результатов функции 
    SOCKET ConnectSocket = INVALID_SOCKET;//для хранения дескриптора сокета
    char recvBuffer[512];//для хранения полученных данных

    const char* sendBuffer1 = "Йоу";//буфер1 для отправки данных
    const char* sendBuffer2 = "Не ЙОУ";//буфер2 для отправки данных

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);// Инициализурует все библиотеки по типу WinSock
    if (result != 0)
    {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // Очищение структуры hints  и установка параметров: семейство адресов, тип сокета, протокол
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo("localhost", "666", &hints, &addrResult);//получение адресной информации для локального хоста
    if (result != 0)
    {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();//
        return 1; 
    }

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);//создание сокета с параметрами
    if (ConnectSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);// подключение к серверу по адресу и порту из addrResult
    if (result == SOCKET_ERROR)
    {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);// отправка данных из первого буфера
    if (result == SOCKET_ERROR)
    {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);// оптравка данных из второго буфера
    if (result == SOCKET_ERROR)
    {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    result = shutdown(ConnectSocket, SD_SEND);//отключение отправки данных по сокету
    if (result == SOCKET_ERROR)
    {
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    do// получение данных от сервера в цикле, если данные успешно получены то выводится result > 0, выводится количество байтов и сами данные; если соединения закрыто result = 0;
    {
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0)
        {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0)
        {
            cout << "Connection closed" << endl;
        }
        else
        {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    closesocket(ConnectSocket);//закрытие сокета
    freeaddrinfo(addrResult);//освобождение памяти
    WSACleanup();//очистка библиотек WinSock
    return 0;
}
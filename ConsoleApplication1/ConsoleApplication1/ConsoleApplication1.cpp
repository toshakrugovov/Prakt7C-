#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData; // для хранения данных данных о реализации WinSock
    ADDRINFO hints;// для указания параметров для соединения
    ADDRINFO* addrResult;// для хранения резульатов функции
    SOCKET ListenSocket = INVALID_SOCKET;// создание сокета, который будет слушать входящие соединения
    SOCKET ConnectSocket = INVALID_SOCKET;// для сокета, который будет подключен к клиента
    char recvBuffer[512];// буфер для хранения данных

    const char* sendBuffer = "Hello from server"; // буфер, в котором хранится сообщение сервера

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // инициализация библиотек
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));// очистка структуры hints и установка параметров для соединения
    hints.ai_family = AF_INET; //семейство адресов
    hints.ai_socktype = SOCK_STREAM;// тип сокета TCP
    hints.ai_protocol = IPPROTO_TCP;// протокол TCP
    hints.ai_flags = AI_PASSIVE;// сокет будет использоваться для привязки

    result = getaddrinfo(NULL, "666", &hints, &addrResult);// получение адресной информации для локального адреса
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        WSACleanup();// это функция из библиотеки Winsock в операционной системе Windows, которая освобождает ресурсы, выделенные при вызове WSAStartup()
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);// создание сокета
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);// привязка сокета к адресу и порту
    if (result == SOCKET_ERROR) {
        cout << "Bind failed, error: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN);// прослушивание входящий соединений
    if (result == SOCKET_ERROR) {
        cout << "Listen failed, error: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ConnectSocket = accept(ListenSocket, NULL, NULL);// принятие входящего соединения и создание нового сокета для связи с клиентом
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Accept failed, error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;// принимает входящее соединения и создает новый сокет
    }

    closesocket(ListenSocket);//закрытие сокета

    do {//Цикл для приема данных от клиента.
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;

            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "Send failed, error: " << result << endl;
                closesocket(ConnectSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            cout << "Connection closing" << endl;
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    result = shutdown(ConnectSocket, SD_SEND);//используется для закрытия соединения на сокет socket: Это дескриптор сокета, для которого применяется операция shutdown.
//how: Это параметр, определяющий, какое действие должно быть выполнено над сокетом.Может принимать одно из следующих значений :
    //SD_RECEIVE(0) : Закрывает прием данных на сокете.
      //  SD_SEND(1) : Закрывает отправку данных из сокета.
       // SD_BOTH(2) : Закрывает как прием, так и отправку данных на сокете.
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket);//закрытие сокета
    freeaddrinfo(addrResult);// освобождение памяти
    WSACleanup();// очистки библиотек
    return 0;
}
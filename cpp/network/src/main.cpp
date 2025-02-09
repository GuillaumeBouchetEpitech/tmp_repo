
#include "network/IpAddress.hpp"
#include "network/TcpSocket.hpp"
#include "network/UdpSocket.hpp"
#include "network/SocketSelector.hpp"

#include <list>
#include <thread>
#include <mutex>
#include <chrono> // std::chrono::milliseconds
#include <iostream>
#include <sstream>


#include <cstdlib> // EXIT_SUCCESS

//
//

std::mutex g_mutex; // do not reproduct at home...

#define D_SAFE_LOG(streamMsg) \
{ \
    std::lock_guard<std::mutex> lock(g_mutex); \
    std::cout << streamMsg << std::endl; \
}

//
//

void tryTcpSocket()
{
    std::cout << "#######" << std::endl;
    std::cout << "# TCP #" << std::endl;
    std::cout << "#######" << std::endl;

    bool running = true;

    //
    //
    // SERVER

    std::thread serverListenThread([&running]()
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        // server socket listen
        network::TcpSocket listenSocket;
        listenSocket.create();
        listenSocket.listen("0.0.0.0", 6666);

        D_SAFE_LOG("[server] listening");

        std::list<network::TcpSocket> echoSockets;
        network::SocketSelector echoSelector;

        std::thread serverEchoThread([&running, &echoSockets, &echoSelector]()
        {
            std::size_t sdataSize = 512;
            auto data = std::make_unique<char[]>(sdataSize);
            char* sdata = data.get();

            D_SAFE_LOG("[server] echo thread running");

            while (running)
            {
                int total = echoSelector.waitRead(0.5f);

                if (total == 0)
                    continue;

                for (auto it = echoSockets.begin(); it != echoSockets.end(); )
                {
                    auto& socket = *it;

                    if (echoSelector.isReady(socket))
                    {
                        std::size_t received = 0;
                        if (!socket.receive(sdata, sdataSize, received))
                        {
                            D_SAFE_LOG("[server] lost a client");
                            echoSelector.remove(socket);
                            it = echoSockets.erase(it);
                            continue;
                        }

                        D_SAFE_LOG("[server] received from client : " << sdata);

                        std::size_t sent = 0;
                        socket.send(sdata, sdataSize, sent);
                    }

                    ++it;
                }
            }

            D_SAFE_LOG("[server] echo thread stopping");
        });

        network::SocketSelector acceptSelector;
        acceptSelector.add(listenSocket);

        D_SAFE_LOG("[server] accept thread running");

        while (running)
        {
            int total = acceptSelector.waitRead(0.5f);

            if (total == 0)
                continue;

            D_SAFE_LOG("[server] accept");

            network::TcpSocket remoteSocket;
            listenSocket.accept(remoteSocket);

            echoSockets.push_back(remoteSocket);
            echoSelector.add(remoteSocket);
        }

        D_SAFE_LOG("[server] accept thread stopping");

        if (serverEchoThread.joinable())
            serverEchoThread.join();
    });

    // SERVER
    //
    //

    //
    //
    // CLIENT

    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        network::TcpSocket clientSocket;

        clientSocket.create();
        // clientSocket.connect("127.0.0.1", 6666);
        clientSocket.connect("127.0.0.1", 6666, 10.0f);

        std::thread clientReadThread([&running, &clientSocket]()
        {
            network::SocketSelector clientSocketSelector;
            clientSocketSelector.add(clientSocket);

            std::size_t sdataSize = 512;
            auto data = std::make_unique<char[]>(sdataSize);
            char* sdata = data.get();

            D_SAFE_LOG("[client] read thread running");

            while (running)
            {
                int total = clientSocketSelector.waitRead(0.5f);

                if (total == 0)
                    continue;

                std::size_t received = 0;
                clientSocket.receive(sdata, sdataSize, received);
                sdata[received] = '\0';

                D_SAFE_LOG("[client] received from server : " << sdata);
            }

            D_SAFE_LOG("[client] read thread stopping");
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(750));

        D_SAFE_LOG("[client] send thread running");

        for (int ii = 0; ii < 3; ++ii)
        {
            std::stringstream sstr;
            sstr << "Hello World " << ii;

            std::string payload = sstr.str();

            D_SAFE_LOG("[client] sending to server : " << payload);

            std::size_t sent = 0;
            clientSocket.send(payload.c_str(), payload.size(), sent);

            std::this_thread::sleep_for(std::chrono::milliseconds(750));
        }

        D_SAFE_LOG("[client] send thread stopping");

        D_SAFE_LOG("[client] disconnect");

        clientSocket.disconnect();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        D_SAFE_LOG("[client] shutting down experiment");

        running = false;

        if (clientReadThread.joinable())
            clientReadThread.join();
    }

    // CLIENT
    //
    //

    if (serverListenThread.joinable())
        serverListenThread.join();

    std::cout << "########" << std::endl;
    std::cout << "# /TCP #" << std::endl;
    std::cout << "########" << std::endl;
}


void tryUdpSocket()
{
    std::cout << "#######" << std::endl;
    std::cout << "# UDP #" << std::endl;
    std::cout << "#######" << std::endl;

    bool running = true;

    //
    //
    // SERVER

    std::thread serverEchoThread([&running]()
    {
        network::UdpSocket serverSocket;
        serverSocket.create();
        serverSocket.bind("0.0.0.0", 7777);

        std::size_t sdataSize = 512;
        auto data = std::make_unique<char[]>(sdataSize);
        char* sdata = data.get();

        D_SAFE_LOG("[server] echo thread running");

        // std::list<TcpSocket> echoSockets;
        network::SocketSelector serverSelector;
        serverSelector.add(serverSocket);

        while (running)
        {
            int total = serverSelector.waitRead(0.5f);

            if (total == 0)
                continue;

            std::size_t received = 0;
            network::IpAddress remoteAddress;
            unsigned short remotePort;
            serverSocket.receive(sdata, sdataSize, received, remoteAddress, remotePort);

            D_SAFE_LOG("[server] received from client : " << sdata);
            D_SAFE_LOG(" => remoteAddress : " << remoteAddress.toString());
            D_SAFE_LOG(" => remotePort    : " << remotePort);

            D_SAFE_LOG("[server] sending to client : " << sdata);

            serverSocket.send(sdata, sdataSize, remoteAddress, remotePort);
        }

        D_SAFE_LOG("[server] echo thread stopping");
    });

    // SERVER
    //
    //

    {
        //
        //
        // CLIENT

        struct UdpClient
        {
            network::UdpSocket clientSocket;
            std::thread clientReadThread;
        };

        network::UdpSocket clientSocket;
        clientSocket.create();
        // clientSocket.bind("127.0.0.1", 8888);

        std::thread clientReadThread([&running, &clientSocket]()
        {
            network::SocketSelector clientSocketSelector;
            clientSocketSelector.add(clientSocket);

            std::size_t sdataSize = 512;
            auto data = std::make_unique<char[]>(sdataSize);
            char* sdata = data.get();

            D_SAFE_LOG("[client] read thread running");

            while (running)
            {
                int total = clientSocketSelector.waitRead(0.5f);

                if (total == 0)
                    continue;

                std::size_t received = 0;
                network::IpAddress remoteAddress;
                unsigned short remotePort;
                clientSocket.receive(sdata, sdataSize, received, remoteAddress, remotePort);
                sdata[received] = '\0';

                D_SAFE_LOG("[client] received from server : " << sdata);
                D_SAFE_LOG(" => remoteAddress : " << remoteAddress.toString());
                D_SAFE_LOG(" => remotePort    : " << remotePort);
            }

            D_SAFE_LOG("[client] read thread stopping");
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(750));

        D_SAFE_LOG("[client] send thread running");

        for (int ii = 0; ii < 3; ++ii)
        {
            std::stringstream sstr;
            sstr << "Hello World " << ii;

            std::string payload = sstr.str();

            D_SAFE_LOG("[client] sending to server : " << payload);

            clientSocket.send(payload.c_str(), payload.size(), "127.0.0.1", 7777);

            std::this_thread::sleep_for(std::chrono::milliseconds(750));
        }

        D_SAFE_LOG("[client] send thread stopping");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        D_SAFE_LOG("[client] shutting down experiment");

        running = false;

        if (clientReadThread.joinable())
            clientReadThread.join();
        if (serverEchoThread.joinable())
            serverEchoThread.join();

        // CLIENT
        //
        //
    }

    std::cout << "########" << std::endl;
    std::cout << "# /UDP #" << std::endl;
    std::cout << "########" << std::endl;
}

int main()
{
    tryTcpSocket();
    tryUdpSocket();

    return EXIT_SUCCESS;
}

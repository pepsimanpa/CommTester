# Communication Tester
The CommTester project is a program that tests various communications.

## Features

- TCP Server / Client
- UDP Server / Client

## Requirements

- Visual Studio 2019

## Installation

Open solution in Visual Studio 2019

## Excution

1. Run CommTester
2. Choose a communication method
3. Set ip and port (Server / Client)
4. Click the start button
5. Write and send data
6. Confirm receive data

## Usage

TCP Server
```cpp
CTCPServer tcpSrv;
tcpSrv.SetServerAddr("192.168.0.10", 7000);       // Set server address
tcpSrv.SetReceiveFunc(std::bind(&XXX::XXX, this, std::placeholders::_1, std::placeholders::_2));    // Set receive callback
tcpSrv.SetEventFunc(std::bind(&XXX::XXX, this, std::placeholders::_1, std::placeholders::_2));      // Set event callback

tcpSrv.CreateSocket();                            // Create sorcket
tcpSrv.Listen();                                  // Listen
tcpSrv.Start();                                   // Start accept & receive thread

tcpSrv.Send(CLIENT_INDEX, data, strlen(data));    // Send data
}
```

TCP Client
```cpp
CTCPClient tcpClt;
tcpClt.SetServerAddr("192.168.0.10", 7000);       // Set server address
tcpClt.SetClientAddr("192.168.0.11", 7000);       // Set client address
tcpClt.SetReceiveFunc(std::bind(&XXX::XXX, this, std::placeholders::_1, std::placeholders::_2));    // Set receive callback

tcpClt.CreateSocket();                            // Create sorcket
tcpClt.Bind();                                    // Bind (optional)
tcpClt.Start();                                   // Start connect thread

tcpClt.Send(data, strlen(data));                  // Send data
}
```

UDP
```cpp
CUDP udp;
udp.SetTargetAddr("192.168.0.10", 7000);          // Set server address
udp.SetMyAddr("192.168.0.11", 7000);              // Set client address
udp.SetReceiveFunc(std::bind(&XXX::XXX, this, std::placeholders::_1, std::placeholders::_2));    // Set receive callback

udp.CreateSocket();                               // Create sorcket
udp.Bind();                                       // Bind
udp.Connect();                                    // Connect
udp.Start();                                      // Start receive thread

udp.Send(data, strlen(data));                        // Send data
udp.Send("192.168.0.12", 7000, data, strlen(data));  // Send data (Set ip & port)
}
```

## Contribute

We would love you for the contribution to **CommTester**, check the ``LICENSE`` file for more info.

## Contact

pepsimanpa – pepsimanpa@gmail.com

[https://github.com/pepsimanpa/CommTester](https://github.com/pepsimanpa/CommTester)

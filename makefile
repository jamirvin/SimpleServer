TCPServerEchoServer: TCPEchoServer-Fork.cpp
	g++ -o SimpleServer TCPEchoServer-Fork.cpp DieWithError.cpp AcceptTCPConnection.c HandleTCPClient.cpp CreateTCPServerSocket.c httpMessage.cpp -std=c++11 -Wall

TCPClient: TCPEchoClient.c DieWithError.o
	gcc -o TCPEchoClient TCPEchoClient.c DieWithError.opp

clean: 
	rm -f SimpleServer TCPEchoClient TCPEchoServer *.o

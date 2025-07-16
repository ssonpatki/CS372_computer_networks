'''
Name: The world’s simplest HTTP server 
Goal: 
    Create an HTTP server using the python socket api. 
Description:
    Your program will create a listening socket bound to '127.0.0.1' or 'localhost', and a random port 
    number > 1023. You will then use your web browser to connect to your server and receive data. Note that
    your server could be running on any host within your LAN (as long as there's no firewall that can block
    access to it). 

Note:
    When your socket accepts a request, a new socket is created. Read the socket request on the new socket 
    and print it. Then send the following html data on the new socket and close it. 
        data =  "HTTP/1.1 200 OK\r\n"\ 
                "Content-Type: text/html; charset=UTF-8\r\n\r\n"\ 
                "<html>Congratulations!  You've downloaded the first Wireshark lab file!</html>\r\n" 

Utilized Sources:
    Kurose and Ross, Computer Networking: A Top-Down Approach, 8th Edition, Pearson
'''
import socket
from socket import *

def main(): 
    response_headers = "HTTP/1.1 200 OK\r\n"
    response_headers += "Content-Type: text/html; charset=UTF-8\r\n\r\n"
    response_headers += "\r\n"
    html_body = "<html>Congratulations!  You've downloaded the first Wireshark lab file!</html>\r\n\r\n"
    
    serverPort = 22104
    serverName = 'localhost'

    # create the TCP server-side socket
    serverSocket = socket(AF_INET,SOCK_STREAM)

    # associate serverPort with this specific TCP socket
    serverSocket.bind((serverName,serverPort))

    # listen for client TCP connection request
    serverSocket.listen(1)
    print(f"Connected by ('{serverName}', {serverPort})\n")

    # note: would use while True: here if server accepts multiple messages
    
    # allow socket to accept() a specific # of connections
    connectionSocket, addr = serverSocket.accept()

    # decode client message from bytes to string
    message = connectionSocket.recv(1024).decode()

    print('Received: ', message)
    print(f'Sending>>>>>>>>\n{response_headers}')
    print(f'{html_body}\n<<<<<<<<')

    # send entire message (encoded into bytes) into TCP Socket
    connectionSocket.sendall((response_headers + html_body).encode())
   
    # close server-side connection
    connectionSocket.close()


if __name__=="__main__":
    main()
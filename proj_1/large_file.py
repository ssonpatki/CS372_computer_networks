'''
Name: GET the data for a large file
Goal: 
    Write a socket program to receive arbitrarily large files. 

Description:
    Your program will make a socket connection to the host: “gaia.cs.umass.edu” and send 
    the GET request for the URI: 
        “/wireshark-labs/HTTP-wireshark-file3.html”. 

    To do this, you will send the following HTTP-compliant GET request to the server exactly as below: 
        "GET /wireshark-labs/HTTP-wireshark-file3.html HTTP/1.1\r\nHost:gaia.cs.umass.edu\r\n\r\n"

Note:
    There is no end-of-transmission (EOT) with sockets, so knowing when you've received all the data 
    can be difficult. Fortunately for this project, the gaia.cs.umass.edu server will close the connection 
    after sending its data, so the easy thing to do is detect when recv or read return <= 0 bytes in a loop.

Utilized Sources:
    Kurose and Ross, Computer Networking: A Top-Down Approach, 8th Edition, Pearson
    GeeksforGeeks. (2023). Socket programming in python. Retrieved from https://www.geeksforgeeks.org/python/socket-programming-python/ 
'''


# import entire socket module
import socket
# directly imports all definitions from socket module
from socket import *

# define main function
def main():
    full_request = 'GET /wireshark-labs/HTTP-wireshark-file3.html HTTP/1.1\r\nHost:gaia.cs.umass.edu\r\n\r\n'
    get_request = 'GET /wireshark-labs/HTTP-wireshark-file3.html HTTP/1.1'
    
    serverHost = 'gaia.cs.umass.edu'
    serverPort = 80   # http typically utilizes port 80 (web servers)

    print('Request: ', get_request)
    print('Host: ', serverHost)
    print('\n')

    # create client socket using IPv4 (AF_INET) that is a TCP socekt (SOCK_STEAM)
    clientSocket = socket(AF_INET, SOCK_STREAM)

    # initiate TCP connection between client and server
    clientSocket.connect((serverHost,serverPort))

    # send request through client socket into TCP connection
    clientSocket.sendall(full_request.encode())

    # wait to receive bytes (response) from server
    message = b""
    # keep receiving bytes from socket until indicated break
    while True:
        data = clientSocket.recv(1024)
        # if recv() returns empty (no more data), break
        if not data:
            break
        message += data

    # seperate headers and body
    headers, body = message.split(b"\r\n\r\n", 1)
    print("[REV] - length: ", len(message))
    # decode headers from bytes into string
    print(headers.decode('utf-8'))
    print("\n")
    # decode body from bytes into string
    print(body.decode('utf-8'))

    # close the client-side socket
    clientSocket.close()


if __name__=="__main__":
    main()
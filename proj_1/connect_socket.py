'''
Name: Using a socket to GET a file 
Goal: 
    Create a simple python program that uses a socket (Python Socket API) to interact with a server. 

Description:
    Your program shall make a socket connection to the host: “gaia.cs.umass.edu” and send the GET 
    request for the URI: 
        “/wireshark-labs/INTRO-wireshark-file1.html”. 
        
    To do this, you will send the following HTTP-compliant GET request to the server exactly as shown: 
        "GET /wireshark-labs/INTRO-wireshark-file1.html HTTP/1.1\r\nHost:gaia.cs.umass.edu\r\n\r\n” 

Utilized Sources:
    Kurose and Ross, Computer Networking: A Top-Down Approach, 8th Edition, Pearson
'''

# import entire socket module
import socket
# directly imports all definitions from socket module
from socket import *

# define main function
def main():
    full_request = 'GET /wireshark-labs/INTRO-wireshark-file1.html HTTP/1.1\r\nHost:gaia.cs.umass.edu\r\n\r\n'
    get_request = 'GET /wireshark-labs/INTRO-wireshark-file1.html HTTP/1.1'
    
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
    message = clientSocket.recv(1024)

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
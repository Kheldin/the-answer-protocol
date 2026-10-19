#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main (void) {
  // AF_INET: IPv4 protocol
  // SOCK_STREAM: 2-way connection-based byte streams
  int serverSocket {socket(AF_INET, SOCK_STREAM, 0)};
  
  // Base struct for all syscalls and functions that deal with internet addresses.
  // Specifies a transport address and port for the AF_INET address family.
  sockaddr_in serverAdress{};
  // sin_family: The address family for the transport address. This member should always be set to AF_INET. 
  serverAdress.sin_family = AF_INET;
  // sin_port: A transport protocol port number.
  // htons: Convert a uint16 host endian into the network endian. 
  serverAdress.sin_port = htons(8080);
  // sin_addr: An IN_ADDR structure that contains an IPv4 transport address. 
  // INADDR_ANY: A wildcard.
  serverAdress.sin_addr.s_addr = INADDR_ANY;
  
  // Assign the adress to the socket.
  bind(serverSocket, (struct sockaddr *)&serverAdress, sizeof(serverAdress));
  
  // Marks  the socket referred as a passive socket, 
  // That will accept incoming con using accept().
  // Second arg: Size of the queue of pending connections.
  listen(serverSocket, 5);

  //extracts the first connection request on the queue of pending connections for the listening socket
  // and return a file descriptor for that socket.
  int clientSocket = accept(serverSocket, nullptr, nullptr);
  
  char buffer[1024]{0}; 
  recv(clientSocket, buffer, sizeof(buffer), 0);
  std::cout << "Msg from client : " << buffer << "\n";
  
  close(serverSocket);
  
  return 0;
}

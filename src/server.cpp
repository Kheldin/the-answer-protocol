#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <vector>

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

  // creation of the client list (dynamic)
  std::vector<pollfd> fds;

  // monitor struct
  pollfd server;
  server.fd = serverSocket;
  server.events = POLLIN; // use to be notified when there is data to be read
  server.revents = 0;

  // add the first file descriptor who is the serversocket (manager)
  fds.push_back(server);

  // the buffer who'll store the request
  char buffer[1024]{0};

  int clientSocket = 0;
  int res = 0;
  int check = 0;
  bool run = true;

  // the server infinite loop
  while(run)
  {
    // start the radar with -1 means 'block infinitily while a thing move'
    // fds.data() allows to give the C array from the vector to the C fonction poll()
    res = poll(fds.data(), fds.size(), -1);

    if (res < 0)
    {
      std::cerr << "Error on poll()\n";
      break;
    }

    // we go through the list to see who fd moved
    for (size_t i = 0; i < fds.size(); ++i)
    {
      if (fds[i].revents & POLLIN)
      {
        if (i == 0)
        {
          clientSocket = accept(serverSocket, nullptr, nullptr);
          
          std::cout << "NEW PLAYER CONNECTED\n";
          pollfd client;
          client.fd = clientSocket;
          client.events = POLLIN;
          client.revents = 0;

          fds.push_back(client);
        }
        else
        {
          check = recv(clientSocket, buffer, sizeof(buffer), 0);
          // looking for errors or deconnections
          if (check <= 0)
          {
            close(fds[i].fd);
            fds.erase(fds.begin() + i);
            --i; // compensate for the vector offset
            std::cout << "The player " << fds[i].fd << " is deconnected or crash.\n";
          }
          else
          {
            std::string msg(buffer);

            // rm the last char while is not a normal char
            while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
            {
              msg.pop_back();
            }

            if (msg == "QUIT")
            {
              close(fds[i].fd);
              fds.erase(fds.begin() + i);
              --i; // compensate for the vector offset
              std::cout << "The player " << fds[i].fd << " leave the game.\n";
            }
            else if (msg == "SHUTDOWN")
            {
              run = false;
              break;
            }
            else
            {
              std::cout << "|" << buffer << "|" << "\n";
            }
          }
        }
      }
    }
  }
  close(serverSocket);
  return 0;
}

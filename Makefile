SERVER_NAME	= server
CLI_NAME 		= cli
GUI_NAME		= gui

CXX         = clang++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++23


SERVER_SRCS = src/server.cpp
CLI_SRCS 		= src/cli_client.cpp

ALL_OBJS    = $(SRCS:.cpp=.o)
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
CLI_OBJS    = $(CLI_SRCS:.cpp=.o)
# HEADERS			=


$(SERVER_NAME): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $(SERVER_NAME)
	./$(SERVER_NAME)

$(CLI_NAME): $(CLI_OBJS)
	$(CXX) $(CXXFLAGS) $(CLI_OBJS) -o $(CLI_NAME)
	./$(CLI_NAME)

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

fclean: clean
	rm -f $(SERVER_NAME)
	rm -f $(CLI_NAME)

clean:
	rm -f $(SERVER_OBJS)
	rm -f $(CLI_OBJS)

re: fclean all
.PHONY: all clean fclean re

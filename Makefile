SERVER_NAME = server.out
CLI_NAME    = cli.out
GUI_NAME		= gui.out

CXX         = clang++
CXXFLAGS    = -Wall -Wextra -Werror -fsanitize=address -g -std=c++23 -MMD -MP -Iinclude
GUIFLAGS		= -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 

OBJ_DIR     = obj

SERVER_SRCS = src/network/server.cpp
CLI_SRCS    = src/network/client.cpp
GUI_SRCS		= src/client_gui.cpp


SERVER_OBJS = $(SERVER_SRCS:%.cpp=$(OBJ_DIR)/%.o)
CLI_OBJS    = $(CLI_SRCS:%.cpp=$(OBJ_DIR)/%.o)
GUI_OBJS    = $(GUI_SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS        = $(SERVER_OBJS:.o=.d) $(CLI_OBJS:.o=.d) $(GUI_OBJS:.o=.d)

all: $(SERVER_NAME) $(CLI_NAME)

$(SERVER_NAME): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $@

$(CLI_NAME): $(CLI_OBJS)
	$(CXX) $(CXXFLAGS) $(CLI_OBJS) -o $@

$(GUI_NAME): $(GUI_OBJS)
	$(CXX) $(CXXFLAGS) $(GUI_OBJS) -o $@ $(GUIFLAGS)

$(OBJ_DIR)/%.o: %.cpp Makefile
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run-server: $(SERVER_NAME)
	./$(SERVER_NAME)

run-cli: $(CLI_NAME)
	./$(CLI_NAME)

run-gui: $(GUI_NAME)
	./$(GUI_NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(SERVER_NAME) $(CLI_NAME) $(GUI_NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re run-server run-cli

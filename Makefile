CXX = c++
CXX_FLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -g -I./inc \

SRC_DIR = src
SRC =		main.cpp Server.cpp

OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

NAME = ircserv

all:$(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXX_FLAGS) $(OBJ) -o $(NAME)
	@echo "OK"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Working on: $<"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	@-rm -rf **/*.o

fclean:	clean
	@-rm -rf $(NAME)

re:		fclean all

.PHONY:	all clean fclean re

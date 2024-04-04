CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRC_DIR = src

OBJ_DIR = build

SRC =	main.cpp \
		IrcServer.cpp \

OBJ =	$(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

CFLAGS +=	-I./lib \
			-I./include/*.hpp\

NAME = ircServ

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus

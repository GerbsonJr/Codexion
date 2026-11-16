NAME		= codexion
CC		= cc
CFLAGS		= -Wall -Wextra -Werror -pthread
SRC_DIR		= src
OBJ_DIR		= obj
INC_DIR		= include
SRC		= main.c parse.c parse_utils.c init.c destroy.c heap.c \
		  heap_utils.c time.c format.c log.c request.c dongle.c \
		  release.c coder.c monitor.c run.c
OBJS		= $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
HEADERS		= $(INC_DIR)/codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

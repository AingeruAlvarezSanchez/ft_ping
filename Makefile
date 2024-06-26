NAME = ft_ping

CC = cc

CFLAGS = -Wall -Werror -Wextra
SANITIZE = -g3 -fsanitize=address -fsanitize=leak

SRC = src/ft_ping.c
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

INCLUDE = -I./inc

$(NAME): all

.PHONY: all
all: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(NAME)

obj/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< $(INCLUDE) -o $@

.PHONY: clean
clean:
	$(RM) -r ./obj

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all

sanitize: CFLAGS += $(SANITIZE)
sanitize: clean all

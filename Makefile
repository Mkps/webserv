# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aloubier <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/30 16:10:19 by aloubier          #+#    #+#              #
#    Updated: 2023/12/20 16:08:53 by aloubier         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ./webserv

SRC_DIR = ./

INC_DIR = ./

OBJ_DIR = build/

SRC_FILES = main.cpp \
			Socket.cpp \
			Server.cpp

SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))

OBJ_FILES = $(SRC_FILES:.cpp=.o)

OBJ = $(addprefix $(OBJ_DIR), $(OBJ_FILES))

CC = c++ 

override CFLAGS += -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p '$(OBJ_DIR)'
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all

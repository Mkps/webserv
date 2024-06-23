# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/30 16:10:19 by aloubier          #+#    #+#              #
#    Updated: 2024/06/23 14:16:44 by obouhlel         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ./webserv

SRC_DIR = ./src/

INC_DIR = ./include/

OBJ_DIR = build/

SRC_FILES = main.cpp \
			Socket.cpp \
			Client.cpp \
			Server.cpp \
			Request.cpp \
			RequestLine.cpp \
			Response.cpp

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

# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/26 11:07:21 by pfrances          #+#    #+#              #
#    Updated: 2023/06/20 11:20:18 by pfrances         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -MMD -MP
INCLUDE = -I includes

SRV_NAME = server
CLIENT_NAME = client

SRCS_DIR = ./srcs
OBJS_DIR = ./objs

SRV_SRCS_DIR = $(SRCS_DIR)/server
CLIENT_SRCS_DIR = $(SRCS_DIR)/client
UTILS_SRCS_DIR = $(SRCS_DIR)/utils

SRV_SRCS =	$(SRV_SRCS_DIR)/main.cpp			\
			$(SRV_SRCS_DIR)/Server.cpp			\
			$(UTILS_SRCS_DIR)/utils.cpp
SRV_OBJS = $(subst $(SRCS_DIR), $(OBJS_DIR), $(SRV_SRCS:.cpp=.o))

CLIENT_SRCS =	$(CLIENT_SRCS_DIR)/main.cpp		\
				$(CLIENT_SRCS_DIR)/Client.cpp	\
				$(UTILS_SRCS_DIR)/utils.cpp
CLIENT_OBJS = $(subst $(SRCS_DIR), $(OBJS_DIR), $(CLIENT_SRCS:.cpp=.o))

all: $(SRV_NAME) $(CLIENT_NAME)

$(SRV_NAME): $(SRV_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@

$(CLIENT_NAME): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(SRV_NAME) $(CLIENT_NAME)

re: fclean all

debug: CXXFLAGS += -g -fsanitize=address
debug: re

.PHONY: all clean fclean re debug

-include $(DEPS)
include animation/assets.mk
include animation/dragon.mk
include animation/loading.mk
include animation/rocket.mk
include animation/train.mk

NAME        = webserv

CXX         = clang++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 \
			  -fdiagnostics-color=always -MMD -MP -O3 -g3

RM          = rm -rf

OBJ_DIR     = obj

SRCS        = srcs/cgi/exec.cpp \
			  srcs/class/Response.cpp srcs/class/Connection.cpp \
			  srcs/error/error.cpp \
			  srcs/method/process.cpp srcs/method/delete.cpp srcs/method/get.cpp srcs/method/post.cpp \
			  srcs/parsing/http.cpp srcs/parsing/initServ.cpp srcs/parsing/parseConf.cpp \
			  srcs/utils/webUtils.cpp \
			  srcs/main.cpp \
			  srcs/serv.cpp

OBJS        = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS        = $(OBJS:.o=.d)

all: default_errors
	@$(MAKE) --no-print-directory $(NAME)

$(NAME): $(OBJS)
	@printf "\n"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call progress_bar)

dragon:
	$(call dragon)
	$(call dragon_fire)

rocket:
	$(call rocket)
	$(call launch_rocket)

train:
	$(call train)
	@bash animation/script_train.sh

default_errors:
	@mkdir -p /tmp/WebServ/DefaultError
	@echo '<html><head><title>400 Bad Request</title></head><body><center><h1>400 Bad Request</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/400.html
	@echo '<html><head><title>403 Forbidden</title></head><body><center><h1>403 Forbidden</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/403.html
	@echo '<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/404.html
	@echo '<html><head><title>405 Method Not Allowed</title></head><body><center><h1>405 Method Not Allowed</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/405.html
	@echo '<html><head><title>413 Payload Too Large</title></head><body><center><h1>413 Payload Too Large</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/413.html
	@echo '<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/500.html
	@echo '<html><head><title>501 Not Implemented</title></head><body><center><h1>501 Not Implemented</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/501.html
	@echo '<html><head><title>502 Bad Gateway</title></head><body><center><h1>502 Bad Gateway</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/502.html
	@echo '<html><head><title>503 Service Unavailable</title></head><body><center><h1>503 Service Unavailable</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/503.html
	@echo '<html><head><title>504 Gateway Timeout</title></head><body><center><h1>504 Gateway Timeout</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/504.html
	@echo '<html><head><title>505 HTTP Version Not Supported</title></head><body><center><h1>505 HTTP Version Not Supported</h1></center><hr><center>WebServ</center></body></html>' > /tmp/WebServ/DefaultError/505.html

-include $(DEPS)

clean:
	@$(eval CURRENT_FILE=0)
	@printf "$(RED)$(TRASH)  Removing objects ($(NAME))$(RESET)\n"
	$(RM) $(OBJ_DIR)
	$(RM) /tmp/WebServ/

fclean: clean
	@printf "$(RED)$(TRASH)  Removing binary  ($(NAME))$(RESET)\n"
	@rm -rf $(NAME) $(OBJ_DIR)
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re rocket train dragon

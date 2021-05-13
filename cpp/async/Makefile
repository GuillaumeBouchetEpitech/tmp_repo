

NAME=		exec


#### MAIN

PATH_MAIN=	.
# MAIN=		$(PATH_MAIN)/main.cpp
# MAIN=		$(PATH_MAIN)/main2.cpp
MAIN=		$(PATH_MAIN)/main3.cpp		\
			$(PATH_MAIN)/threading/Consumer.cpp	\
			$(PATH_MAIN)/threading/Producer.cpp	\
			$(PATH_MAIN)/threading/ThreadPool.cpp	\
			$(PATH_MAIN)/threading/t_task.cpp

#### /MAIN


####

SRC=		$(MAIN)

OBJ=		$(SRC:.cpp=.o)

#######


CXXFLAGS=	-Wall -W -Wextra -Wunused -O3 -std=c++11	\
			-fopenmp									\
			-I./

LDFLAGS=	-lGL -lGLU -lGLEW				\
			-fopenmp

#######

CXX=		g++
RM=			rm -f


#######


$(NAME):	$(OBJ)
			$(CXX) $(OBJ) -o $(NAME) $(LDFLAGS)

all:		$(NAME)

clean:
			$(RM) $(OBJ)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re

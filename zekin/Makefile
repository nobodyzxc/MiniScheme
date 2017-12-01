CXX = gcc
FLAG = -g
MAIN     = main
_MAIN    = ${MAIN:%=%.c}
SRCS     = type token parse eval mem util proc  func syntax
_SRCS    = ${SRCS:%=%.c}
_OBJS    = ${SRCS:%=%.o}
TARGET   = main

all:
	$(CXX) $(FLAG) -c $(_SRCS)
	$(CXX) $(FLAG) -o $(TARGET) $(_MAIN) $(_OBJS)
c:
	gcc $(FLAG) -c $(_SRCS)
	#gcc $(FLAG) -o $(TARGET) $(_MAIN) $(_OBJS)
cc:
	g++ $(FLAG) -c type.c token.c proc.c parse.c eval.c mem.c
	g++ $(FLAG) -o $(TARGET) $(_MAIN) $(_OBJS)
clean:
	rm -rf *.o *.exe

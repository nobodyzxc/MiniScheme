CXX = gcc
FLAG = -g
MAIN     = main
_MAIN    = ${MAIN:%=%.c}
SRCS     = type token
_SRCS    = ${SRCS:%=%.c}
_OBJS    = ${SRCS:%=%.o}
TARGET   = zekin

all:
	$(CXX) $(FLAG) -c $(_SRCS)
	$(CXX) $(FLAG) -o $(TARGET) $(_MAIN) $(_OBJS)
clean:
	rm -rf *.o *.exe

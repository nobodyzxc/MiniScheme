CXX = gcc
FLAG = -g -O3 -DSHARE_BUFFER -DTCO_OPT
MAIN     = main
_MAIN    = ${MAIN:%=%.c}
SRCS     = type token parse eval mem util proc func syntax gc opt
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
	g++ $(FLAG) -c $(_SRCS)
	g++ $(FLAG) -o $(TARGET) $(_MAIN) $(_OBJS)
clean:
	rm -rf $(_OBJS) $(TARGET)

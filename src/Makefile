CXX = gcc
FLAG = -g -O3
LNKS = -lm -lreadline
DEFS = -DTCO_OPT # -DSHARE_BUFFER
# if -DTCO_OPT , -O3 must be add , too.
MAIN     = main
_MAIN    = ${MAIN:%=%.c}
SRCS     = type token parse eval mem util proc func syntax gc opt
_SRCS    = ${SRCS:%=%.c}
_OBJS    = ${SRCS:%=%.o}
TARGET   = zekin

all:
	$(CXX) $(FLAG) $(DEFS) -c $(_SRCS) $(LNKS)
	$(CXX) $(FLAG) $(DEFS) -o $(TARGET) $(_MAIN) $(_OBJS) $(LNKS)
clean:
	rm -rf $(_OBJS) $(TARGET)

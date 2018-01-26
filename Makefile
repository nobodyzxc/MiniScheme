CC = gcc
CFLAGS = -g -O3 \
		 -DTCO_OPT \
		 -DLIBPATH="$(CURDIR)/lib/" \
		 -DLIBCONFIG="config" \
		 -DVERSION="20180127" \
#		 -DPURE_READ \

# -DTCO_OPT
# -O3 must be add , too.

# -DPURE_READ
# read function discard other atom in one read

LIBS = -lm -lreadline

S_DIR    = src
O_DIR    = obj
TARGET   = zekin
SOURCE   = main type util mem \
		   token parse eval \
		   syntax func proc \
		   gc opt

SRCS    = ${SOURCE:%=$(S_DIR)/%.c}
OBJS    = ${SOURCE:%=$(O_DIR)/%.o}

all: $(O_DIR) $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

$(O_DIR)/%.o: $(S_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(O_DIR):
	@mkdir $@

clean:
	rm -rf $(O_DIR) $(TARGET)

################################
# SELECT INPUT MODE UNDER REPL #
# ##############################

#I_MODE = rl
# GNU READLINE
I_MODE = read
# NATIVE FGETS , IF YOU DON'T WANT TO USE GNU READLINE

CC = gcc
CFLAGS = -g -O3 \
		 -DTCO_OPT \
		 -DLIBPATH="$(CURDIR)/lib/" \
		 -DLIBCONFIG="config" \
		 -DVERSION="v1.0" \
		 -DI_MODE=$(I_MODE) \
		 -DI_MODE_PORT=$(I_MODE)_pt \
		 -DI_MODE_NON_BLANK=$(I_MODE)_non_blank \
		 -DI_MODE_RAW_INPUT=$(I_MODE)_raw_input \
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
		   gc opt io

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

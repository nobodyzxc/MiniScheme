################################
# SELECT INPUT MODE UNDER REPL #
################################

I_MODE = rl
# GNU READLINE
#I_MODE = read
# NATIVE FGETS , UNCOMMENT IT IF YOU DON'T WANT TO USE GNU READLINE

################################
# AVAILABLE HEAP SIZE FOR ALOC #
################################

HEAP = 3500000000

CC = gcc
LIBS = -lm
CFLAGS = -g \
		 -DLIBPATH="$(CURDIR)/lib/" \
		 -DLIBCONFIG="config" \
		 -DVERSION="v1.0" \
		 -DI_MODE_PORT=$(I_MODE)_pt \
		 -DI_MODE_NON_BLANK=$(I_MODE)_non_blank \
		 -DI_MODE_RAW_INPUT=$(I_MODE)_raw_input \
		 -DHEAP_SIZE=$(HEAP)

################################
# ENABLE TAILCALL OPTIMIZATION #
################################
CFLAGS += -O3 -DTCO_OPT

################################
# ENABLE ARGSPACE OPTIMIZATION #
################################
CFLAGS += -DARG_OPT

################################
# IF CLEAR BUFFER EVERY (read) #
# Discard rest atoms per read  #
################################
#CFLAGS += -DPURE_READ

################################
# SHOW GARBAGE COLLECTION INFO #
################################
#CFLAGS += -DSHOW_GC_INFO

ifeq ($(I_MODE) , rl)
	CFLAGS += -DRL_LIB
	LIBS += -lreadline
endif

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

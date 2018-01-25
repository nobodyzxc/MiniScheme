# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -g -O3 \
		 -DTCO_OPT -DLIBPATH="$(CURDIR)/../lib/" -DLIBCONFIG="config"
AM_LDFLAGS = -lm -lreadline

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = zekin
zekin_SOURCES = *.c *.h

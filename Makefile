# Makefile for block_write_logger.c
# Date: 24 Oct 2020

obj-m += block_write_logger.o
KDIR = /usr/src/linux-4.4

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(shell pwd) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(shell pwd) clean

# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

SUBDIRS := dma graphics input interrupts timers

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) --no-print-directory -C $$dir || { exit 1; } \
	done;

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) --no-print-directory -C $$dir clean || { exit 1; } \
	done;

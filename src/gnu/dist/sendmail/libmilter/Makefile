#       Id: Makefile,v 8.1 1999/11/04 00:03:40 ca Exp
#       $NetBSD: Makefile,v 1.4 2003/06/01 14:07:00 atatat Exp $

SHELL= /bin/sh
BUILD=   ./Build
OPTIONS= $(CONFIG) $(FLAGS)

all: FRC
	$(SHELL) $(BUILD) $(OPTIONS) $@
clean: FRC
	$(SHELL) $(BUILD) $(OPTIONS) $@
install: FRC
	$(SHELL) $(BUILD) $(OPTIONS) $@

fresh: FRC
	$(SHELL) $(BUILD) $(OPTIONS) -c

FRC:

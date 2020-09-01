# Makefile for the GB+ help files
# This makefile indexes help files under the help directory
#
# General variables
BIN = ../bin
UTILS = ../utils
SHELL = /bin/sh

all: admin.idx command.idx concept.idx faq.idx help.idx tour.idx glossary.idx \
     misc.idx

admin.idx: admin.txt
	$(UTILS)/makeindex admin

command.idx: command.txt
	$(UTILS)/makeindex command

concept.idx: concept.txt
	$(UTILS)/makeindex concept

faq.idx: faq.txt
	$(UTILS)/makeindex faq

help.idx: help.txt
	$(UTILS)/makeindex help

tour.idx: tour.txt
	$(UTILS)/makeindex tour

glossary.idx: glossary.txt
	$(UTILS/makeindex glossary

misc.idx: misc.txt
	$(UTILS/makeindex misc

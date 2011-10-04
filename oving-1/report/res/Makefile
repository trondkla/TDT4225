DATS = $(wildcard *.dat)
PDFS = $(DATS:.dat=.pdf)

.PHONY: all

all: $(PDFS)

%.pdf: %.dat
	./plot.sh $< $@
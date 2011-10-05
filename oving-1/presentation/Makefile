TEXFILES=$(shell find . -type f -name '*.tex')

.PHONY: all

all: main.pdf

main.pdf: $(TEXFILES)
	pdflatex main.tex

.PHONY: clean

clean:
	rm -f main.pdf *.aux *.nav *.out *.snm *.toc *.log

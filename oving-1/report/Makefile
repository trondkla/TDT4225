.PHONY: all res

all: main.pdf res

main.pdf: main.tex
	pdflatex $<
	pdflatex $<

res:
	make -C res
SRC := $(shell find . -iname "*.tex")

linux_structs.pdf: $(SRC)
	latexmk linux_structs.tex && latexmk -c

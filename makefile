all: 
	gcc -g sheet.c sheet_functions.c utils.c parser.c calculate_functions.c avl.c -lm -o ./target/release/spreadsheet

clean:
	rm -f ./target/release/spreadsheet

test:
	    @for file in input*.txt; do \
		out=$$(echo $$file | sed 's/input/output/'); \
        echo "Processing $$file -> $$out"; \
        ( ./target/release/spreadsheet 999 18278 < $$file; echo ) > $$out; \
    done

report:
	pdflatex report.tex
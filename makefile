all: 
	gcc -g sheet.c sheet_functions.c utils.c parser.c calculate_functions.c avl.c -lm -o ./target/release/spreadsheet

clean:
	rm -f ./target/release/spreadsheet

test:
	

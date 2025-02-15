all: 
	gcc sheet.c sheet_functions.c utils.c parser.c calculate_functions.c -lm -o spreadsheet

clean:
	rm -f spreadsheet
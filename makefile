all: 
	gcc sheet.c sheet_functions.c utils.c parser.c calculate_functions.c -lm -g -o sheet

clean:
	rm -f sheet
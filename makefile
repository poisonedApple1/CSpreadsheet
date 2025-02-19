all: 
	gcc -g sheet.c sheet_functions.c utils.c parser.c calculate_functions.c -lm -o sheet

clean:
	rm -f sheet
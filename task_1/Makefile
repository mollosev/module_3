PROJECT_NAME = task_1
CFLAGS = -Wall -Wextra -Werror -std=c11

$(PROJECT_NAME)_1: clean
	gcc $(CFLAGS) $(PROJECT_NAME)_1.c -o $(PROJECT_NAME)_1.out
	./$(PROJECT_NAME)_1.out chrome vscode telegram

$(PROJECT_NAME)_2: clean
	gcc $(CFLAGS) $(PROJECT_NAME)_2.c -o $(PROJECT_NAME)_2.out
	./$(PROJECT_NAME)_2.out 1 2 3 4 5 6 7 8 9 10

$(PROJECT_NAME)_6: clean
	gcc $(CFLAGS) $(PROJECT_NAME)_6.c -o $(PROJECT_NAME)_6.out
	./$(PROJECT_NAME)_6.out

clean:
	rm -rf *.out *.o *.a *.so *.out
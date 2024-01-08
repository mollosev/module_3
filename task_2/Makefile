PROJECT_NAME = task_2
CFLAGS = -Wall -Wextra -Werror -std=c11

$(PROJECT_NAME)_1: clean
	gcc $(PROJECT_NAME)_1.c -o $(PROJECT_NAME)_1.out
	./$(PROJECT_NAME)_1.out 10

$(PROJECT_NAME)_2: clean
	gcc $(PROJECT_NAME)_2.c -o $(PROJECT_NAME)_2.out
	./$(PROJECT_NAME)_2.out

$(PROJECT_NAME)_4_1:
	gcc $(PROJECT_NAME)_4_1.c -o $(PROJECT_NAME)_4_1.out
	./$(PROJECT_NAME)_4_1.out 5

$(PROJECT_NAME)_4_2:
	gcc $(PROJECT_NAME)_4_2.c -o $(PROJECT_NAME)_4_2.out
	./$(PROJECT_NAME)_4_2.out

$(PROJECT_NAME)_5: clean
	gcc $(PROJECT_NAME)_5.c -o $(PROJECT_NAME)_5.out
	./$(PROJECT_NAME)_5.out &

$(PROJECT_NAME)_6: clean
	gcc $(PROJECT_NAME)_6.c -o $(PROJECT_NAME)_6.out
	./$(PROJECT_NAME)_6.out &

$(PROJECT_NAME)_7: clean
	gcc $(PROJECT_NAME)_7.c -o $(PROJECT_NAME)_7.out
	./$(PROJECT_NAME)_7.out &

$(PROJECT_NAME)_8: clean
	gcc $(PROJECT_NAME)_8.c -o $(PROJECT_NAME)_8.out
	./$(PROJECT_NAME)_8.out 10

clean:
	rm -rf *.out *.o *.a *.so *.out *.bin *.txt
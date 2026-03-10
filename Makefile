default:
	gcc main.c .\lib\accounts.c -lraylib -lgdi32 -lwinmm -lsqlite3 -Llib -Iinclude -o main
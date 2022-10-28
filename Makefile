CC=g++

CFLAGS+=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

LIST_OBJ+=list\list.o list\list_funcs.o lib\flog.o

DLIST_OBJ+=dumb_log\dumb_list_funcs.o dumb_log\main.cpp

all: list.exe dlist.exe

list.exe: $(LIST_OBJ)
	$(CC) $(LIST_OBJ) $(CFLAGS) -o list.exe

dlist.exe: $(DLIST_OBJ)
	$(CC) $(DLIST_OBJ) $(CFLAGS) -o dlist.exe

*.o: *.cpp
	$(CC) $(CFLAGS) *.cpp -c

clean:
	rm -rf *.o *.exe*
	rm -rf list/*.o list/*.exe*
	rm -rf lib/*.o lib/*.exe*
	rm -rf dumb_log/*.o dumb_log/*.exe*
	clear

run:
	./a.exe

.PHONY: clean run
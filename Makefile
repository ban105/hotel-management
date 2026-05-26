CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -Iinclude
LDFLAGS :=
TARGET  := hotel.exe

SRC := main.c \
       src/room.c \
       src/customer.c \
       src/booking.c \
       src/service.c \
       src/bill.c \
       src/admin.c \
       src/employee.c \
       src/menu.c \
       src/utils.c

OBJ := $(SRC:.c=.o)

.PHONY: all run syntax clean rebuild

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(SRC)

run: all
	./$(TARGET)

clean:
	$(RM) $(OBJ) $(TARGET)

rebuild: clean all

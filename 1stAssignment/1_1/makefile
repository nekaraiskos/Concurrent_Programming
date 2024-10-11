# Ορισμός του compiler και των flags
CC = gcc
CFLAGS = -Wall -g

# Το εκτελέσιμο αρχείο που θέλουμε να παραχθεί
TARGET = main

# Ορισμός των αντικειμενικών αρχείων που θα παραχθούν από τα .c αρχεία
OBJS = test.o pipe.o

# Ο κανόνας για τη δημιουργία του εκτελέσιμου αρχείου
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lpthread

# Κανόνας για τη δημιουργία του main.o
test.o: test.c pipe.h
	$(CC) $(CFLAGS) -c test.c

# Κανόνας για τη δημιουργία του pipe.o
pipe.o: pipe.c pipe.h
	$(CC) $(CFLAGS) -c pipe.c

# Καθαρισμός των παραγόμενων αρχείων
clean:
	rm -f $(OBJS) $(TARGET)

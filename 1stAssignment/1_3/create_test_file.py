import random
import struct

# Create a binary file with shuffled numbers from 0 to 300
numbers = list(range(301))
random.shuffle(numbers)

# File path for the binary file
file_path = "shuffled_numbers.bin"

# Write the shuffled numbers to the binary file
with open(file_path, "wb") as binary_file:
    for number in numbers:
        binary_file.write(struct.pack('i', number))

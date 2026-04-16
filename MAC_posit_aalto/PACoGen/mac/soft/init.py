import numpy as np
from posit import posit

def generate_sequence(n):
    """Generate the sequence: 1, -1/3, 1/5, -1/7, 1/9, ..."""
    sequence = []
    for i in range(1, n + 1):
        denominator = 2 * i - 1
        if i % 2 == 0:
            sequence.append(-1.0 / denominator)
        else:
            sequence.append(1.0 / denominator)
    return sequence

def main():
    # Create posit converter (32 bits, es=3)
    posit_converter = posit(32, 3)

    # Generate 10,000 numbers
    numbers = generate_sequence(10000)

    # Convert to posit and write to file
    with open('operand.txt', 'w') as f:
        for num in numbers:
            posit_num = posit_converter.float2posit(num)
            f.write(posit_num + '\n')

    print("File 'operand.txt' created successfully with 10,000 32-bit posit numbers.")

if __name__ == "__main__":
    main()

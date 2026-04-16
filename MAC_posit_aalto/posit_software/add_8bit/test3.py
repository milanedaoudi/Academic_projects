# -*- coding: utf-8 -*-
def generate_8bit_patterns():
    # Generate all 256 possible 8-bit patterns
    all_patterns = [bin(i)[2:].zfill(8) for i in range(256)]
    
    # Generate Pin1: each pattern repeated 256 times
    with open("Pin1_8bit.txt", "w") as f1:
        for pattern in all_patterns:
            f1.write((pattern + "\n") * 256)
    
    # Generate Pin2: full pattern sequence repeated 256 times
    with open("Pin2_8bit.txt", "w") as f2:
        full_sequence = "\n".join(all_patterns) + "\n"
        f2.write(full_sequence * 256)

if __name__ == "__main__":
    generate_8bit_patterns()
    print("Files generated:\nPin1_8bit.txt - Each pattern repeated 256 times\n"
          "Pin2_8bit.txt - Full pattern sequence repeated 256 times")

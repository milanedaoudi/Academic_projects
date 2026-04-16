# -*- coding: utf-8 -*-
from posit import posit
import random

def generate_test_patterns():
    # Initialize 16-bit posit with es=1
    p = posit(16, 1)
    
    # Generate all possible 16-bit patterns (65,536 total)
    print("Generating 16-bit pattern files...")
    all_patterns = [bin(i)[2:].zfill(16) for i in range(65536)]
    
    # Create Pin1 with all patterns in order
    with open("Pin1_16bit.txt", "w") as f1:
        f1.write("\n".join(all_patterns))
    
    # Create Pin2 with random patterns (same quantity)
    random_patterns = [bin(random.getrandbits(16))[2:].zfill(16) for _ in range(65536)]
    with open("Pin2_16bit.txt", "w") as f2:
        f2.write("\n".join(random_patterns))
    
    # Generate reference results (row i + row i)
    print("Generating reference results...")
    with open("results_float_16bit.txt", "w") as out:
        for i in range(65536):
            try:
                a = p.posit2float(all_patterns[i])
                b = p.posit2float(random_patterns[i])  # Use same row from Pin2
                res = a + b
                out.write("%.15g\n" % res)
            except:
                out.write("NaN\n")
    
    print("Operation complete:")
    print("- Pin1_16bit.txt: All 65,536 16-bit patterns (ordered)")
    print("- Pin2_16bit.txt: 65,536 random 16-bit patterns")
    print("- results_float_16bit.txt: Reference additions (row i + row i)")

if __name__ == "__main__":
    generate_test_patterns()

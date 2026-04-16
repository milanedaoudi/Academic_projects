# -*- coding: utf-8 -*-
from posit import posit

def generate_and_process():
    # Initialize 8-bit posit with es=1
    p = posit(8, 1)
    
    # =============================================
    # Part 1: Generate all 8-bit patterns and files
    # =============================================
    print("Generating 8-bit pattern files...")
    all_patterns = [bin(i)[2:].zfill(8) for i in range(256)]
    
    # Generate Pin1: each pattern repeated 256 times
    with open("Pin1_8bit.txt", "w") as f1:
        for pattern in all_patterns:
            f1.write((pattern + "\n") * 256)
    
    # Generate Pin2: full pattern sequence repeated 256 times
    with open("Pin2_8bit.txt", "w") as f2:
        full_sequence = "\n".join(all_patterns) + "\n"
        f2.write(full_sequence * 256)
    
    # =============================================
    # Part 2: Process the files and perform addition
    # =============================================
    print("Processing posit additions...")
    with open("Pin1_8bit.txt", "r") as f1, open("Pin2_8bit.txt", "r") as f2:
        pin1 = [line.strip() for line in f1 if line.strip()]
        pin2 = [line.strip() for line in f2 if line.strip()]
    
    # Process pairs and write results
    with open("results_float.txt", "w") as out:
        for i in xrange(len(pin1)):
            try:
                # Convert posits to floats and add
                a = p.posit2float(pin1[i])
                b = p.posit2float(pin2[i])
                res = a + b
                
                # Write just the result
                out.write("%.15g\n" % res)
            except Exception as e:
                out.write("NaN\n")  # Write NaN for errors
    
    print("Operation complete:")
    print("- Generated Pin1_8bit.txt (each 8-bit pattern repeated 256 times)")
    print("- Generated Pin2_8bit.txt (full pattern sequence repeated 256 times)")
    print("- Saved addition results to results_float.txt")

if __name__ == "__main__":
    generate_and_process()

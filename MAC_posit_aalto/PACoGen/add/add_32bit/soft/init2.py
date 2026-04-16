# -*- coding: utf-8 -*-
from posit import posit
import random
import struct

def generate_test_data():
    # Initialize 32-bit posit with es=2 (common configuration)
    p = posit(32, 8)
    
    # Generate random doubles in [0.0001, 0.001]
    num_values = 65534
    print("Generating random doubles...")
    list1 = [random.uniform(0.01, 0.1) for _ in xrange(num_values)]
    list2 = [random.uniform(0.01, 0.1) for _ in xrange(num_values)]
    
    # Create results_float_32bit.txt with additions
    print("Creating float results...")
    with open("results_float_32bit.txt", "w") as f_float:
        for a, b in zip(list1, list2):
            res = a + b
            f_float.write("{:.15g}\n".format(res))
    
    # Convert to posit and create input files
    print("Converting to 32-bit posit...")
    with open("Pin1_32bit.txt", "w") as f_pin1, \
         open("Pin2_32bit.txt", "w") as f_pin2:
        
        for a, b in zip(list1, list2):
            try:
                # Get 32-bit binary string representation
                posit1 = p.float2posit(a)
                posit2 = p.float2posit(b)
                
                # Pad to 32 bits if needed
                posit1 = posit1.zfill(32)
                posit2 = posit2.zfill(32)
                
                f_pin1.write(posit1 + "\n")
                f_pin2.write(posit2 + "\n")
            except Exception as e:
                print("Conversion error: {}".format(str(e)))
                f_pin1.write("00000000000000000000000000000000\n")  # 32-bit zero
                f_p2.write("00000000000000000000000000000000\n")
    
    print("\nOperation complete:")
    print("- Generated {} random pairs in [0.0001, 0.001]".format(num_values))
    print("- Created results_float_32bit.txt with additions")
    print("- Created Pin1_32bit.txt and Pin2_32bit.txt with 32-bit posit conversions")

if __name__ == "__main__":
    generate_test_data()

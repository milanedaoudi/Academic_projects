# -*- coding: utf-8 -*-
from posit import posit
import sys

def test_posit_addition(posit1, posit2):
    # Initialize 16-bit posit with es=1 (modify es as needed)
    p = posit(16, 1)
    
    try:
        # Convert posits to floats
        float1 = p.posit2float(posit1)
        float2 = p.posit2float(posit2)
        
        # Print conversions
        print("Posit 1: {} → Float: {:.15g}".format(posit1, float1))
        print("Posit 2: {} → Float: {:.15g}".format(posit2, float2))
        
        # Add and print result
        result = float1 + float2
        print("Addition result: {:.15g}".format(result))
        
    except Exception as e:
        print("Error: {}".format(str(e)))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python posit_test.py <16-bit_posit1> <16-bit_posit2>")
        print("Example: python posit_test.py 0100110011001100 1100000011110000")
    else:
        test_posit_addition(sys.argv[1], sys.argv[2])

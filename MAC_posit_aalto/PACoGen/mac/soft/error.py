# -*- coding: utf-8 -*-
from posit import posit
import math

def binary_to_float_posit(binary_str, n=32, es=3):
    """Convert a binary posit string to its float value"""
    # Create a posit object with given parameters
    p = posit(n, es)
    
    # Convert binary posit to float
    float_value = p.posit2float(binary_str)
    
    return float_value

# Example usage with the value captured at 200,017 ns
if __name__ == "__main__":
    # Read the output file to get the binary value
    try:
        with open("output_results.txt", "r") as f:
            lines = f.readlines()
            for line in lines:
                if "Output at 200,017 ns" in line:
                    # Extract the binary string
                    binary_str = line.split(": ")[1].strip()
                    break
    except IOError:
        print("Error: Could not open output_results.txt")
        exit(1)

    # Convert to float using posit
    if 'binary_str' in locals():
        result = binary_to_float_posit(binary_str)
        pi_estimate = result 
        pi_true = math.pi
        absolute_error = abs(pi_estimate - pi_true)
        
        # Python 2.7 compatible print statements
        print("Posit value from testbench: {}".format(binary_str))
        print("Converted to float (using posit N=32, es=3): {:.15f}".format(result))
        print("\nComparison with π:")
        print("π estimate (out): {:.15f}".format(pi_estimate))
        print("True π value:        {:.15f}".format(pi_true))
        print("Absolute error:      {:.15f}".format(absolute_error))
        print("Relative error:      {:.2e} %".format(absolute_error/pi_true * 100))
    else:
        print("Error: Could not find output value in the file")

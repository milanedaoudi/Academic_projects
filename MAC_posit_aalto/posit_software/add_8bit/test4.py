# -*- coding: utf-8 -*-
from posit import posit

def convert_and_add():
    # Initialize 8-bit posit with es=1
    p = posit(8, 1)
    
    # Read input files
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

if __name__ == "__main__":
    convert_and_add()
    print("Results saved to results_float.txt (one result per line)")

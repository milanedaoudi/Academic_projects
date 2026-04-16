# -*- coding: utf-8 -*-
from posit import posit

def generate_posit_conversions():
    # Initialize posit configuration (N=8, es=1)
    p = posit(16, 2)
    
    # Open a file to write results
    with open("results.txt", "w") as file:
        # Generate numbers from 2.44e-4 to 4096 with step=1
        current = 2.44e-4
        end = 4096.0
        
        while current <= end:
            # Convert double to posit
            posit_str = p.float2posit(current)
            
            # Convert posit back to double (to check error)
            posit_to_float = p.posit2float(posit_str)
            
            # Calculate absolute error
            error = abs(current - posit_to_float)
            
            # Write to file: [Double] → [Posit] → [Converted Back] → [Error]
            file.write("%.10f → %s → %.10f → %.10f\n" % (
                current,
                posit_str,
                posit_to_float,
                error
            ))
            
            current += 1  # Increment by 1

if __name__ == "__main__":
    generate_posit_conversions()
    print("Conversion results saved to 'results.txt'")

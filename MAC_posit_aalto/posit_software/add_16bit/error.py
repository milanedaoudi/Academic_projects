# -*- coding: utf-8 -*-
from posit import posit
import math

def calculate_errors():
    # Initialize 16-bit posit with es=1
    p = posit(16, 1)
    
    # Read files
    with open("posit_results_16bit.txt", "r") as f_posit, \
         open("results_float_16bit.txt", "r") as f_float:
        
        posit_lines = [line.strip() for line in f_posit if line.strip()]
        float_lines = [line.strip() for line in f_float if line.strip()]
    

    
    # Open separate error files
    with open("absolute_error.txt", "w") as f_abs, \
         open("relative_error.txt", "w") as f_rel:
        
        f_abs.write("absolute_error\n")  # Header
        f_rel.write("relative_error\n")  # Header
        
        total_abs = 0.0
        total_rel = 0.0
        max_abs = 0.0
        valid_count = 0
        
        for i in xrange(len(posit_lines)):
            try:
                # Convert posit string to float
                posit_float = p.posit2float(posit_lines[i])
                ref_float = float(float_lines[i])
                
                # Skip NaN cases
                if math.isnan(ref_float) or math.isnan(posit_float):
                    f_abs.write("NaN\n")
                    f_rel.write("NaN\n")
                    continue
                
                # Calculate errors
                abs_err = abs(posit_float - ref_float)
                rel_err = abs_err/abs(ref_float) if ref_float != 0 else float('nan')
                
                # Update statistics
                total_abs += abs_err
                if not math.isnan(rel_err):
                    total_rel += rel_err
                if abs_err > max_abs:
                    max_abs = abs_err
                valid_count += 1
                
                # Write to separate files
                f_abs.write("{:.15g}\n".format(abs_err))
                f_rel.write("{:.15g}\n".format(rel_err))
                
            except Exception as e:
                f_abs.write("NaN\n")
                f_rel.write("NaN\n")
                print("Error line {}: {}".format(i+1, str(e)))
    
    # Calculate and display statistics
    if valid_count > 0:
        print("\nError Statistics:")
        print("- Average absolute error: {:.15g}".format(total_abs/valid_count))
        print("- Average relative error: {:.15g}".format(total_rel/valid_count))
        print("- Maximum absolute error: {:.15g}".format(max_abs))
        print("- Valid calculations: {}/{}".format(valid_count, len(posit_lines)))
    else:
        print("No valid error calculations")

if __name__ == "__main__":
    calculate_errors()
    print("\nResults saved to:")
    print("- absolute_error.txt")
    print("- relative_error.txt")

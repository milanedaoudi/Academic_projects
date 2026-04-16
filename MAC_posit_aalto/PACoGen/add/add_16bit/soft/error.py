# -*- coding: utf-8 -*-
from posit import posit
import math

def calculate_errors():
    # Initialize 16-bit posit with es=2 (common for 16-bit)
    p = posit(16, 8)
    
    # Read files (updated names for 16-bit)
    with open("posit_results_16bit.txt", "r") as f_posit, \
         open("results_float_16bit.txt", "r") as f_float:
        
        posit_lines = [line.strip() for line in f_posit if line.strip()]
        float_lines = [line.strip() for line in f_float if line.strip()]
    
    # Open output files (updated names)
    with open("error_16bit.txt", "w") as f_error, \
         open("large_errors_16bit.log", "w") as f_large:
        
        f_large.write("Lines with absolute error > 10:\n")
        total_error = 0.0
        valid_count = 0
        large_error_count = 0
        
        for i in xrange(len(posit_lines)):
            try:
                # Convert 16-bit posit string to float
                posit_bits = posit_lines[i].zfill(16)  # Ensure 16-bit format
                posit_float = p.posit2float(posit_bits)
                reference_float = float(float_lines[i])
                
                # Skip NaN cases
                if math.isnan(reference_float) or math.isnan(posit_float):
                    f_error.write("NaN\n")
                    continue
                
                # Calculate absolute error
                error = abs(posit_float - reference_float)
                
                # Handle large errors (threshold adjusted for 16-bit precision)
                if error > 10:
                    f_large.write("Line {}: Posit={}, Float={:.15g}, Error={:.15g}\n".format(
                        i+1, posit_bits, reference_float, error))
                    large_error_count += 1
                    f_error.write("SKIPPED\n")
                    continue
                
                # Normal errors
                total_error += error
                valid_count += 1
                f_error.write("{:.15g}\n".format(error))
                
            except Exception as e:
                f_error.write("NaN\n")
                print("Error processing line {}: {}".format(i+1, str(e)))
    
    # Calculate and display statistics
    print("\n=== 16-bit Error Statistics ===")
    print("Valid calculations: {}/{}".format(valid_count, len(posit_lines)))
    print("Large errors skipped: {}".format(large_error_count))
    
    if valid_count > 0:
        print("Average error (excluding >10): {:.15g}".format(total_error/valid_count))
        print("Average relative error: {:.15g}%".format(
            (total_error/valid_count)/abs(reference_float)*100 if reference_float != 0 else 0))
    else:
        print("No valid error calculations could be performed")
    
    print("\nResults saved to:")
    print("- error_16bit.txt")
    print("- large_errors_16bit.log")

if __name__ == "__main__":
    calculate_errors()

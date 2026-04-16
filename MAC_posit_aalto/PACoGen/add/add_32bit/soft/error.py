# -*- coding: utf-8 -*-
from posit import posit
import math

def calculate_errors():
    # Initialize 32-bit posit with es=2 (common configuration)
    p = posit(32, 8)
    
    # Read 32-bit files
    with open("posit_results_32bit.txt", "r") as f_posit, \
         open("results_float_32bit.txt", "r") as f_float:
        
        posit_lines = [line.strip() for line in f_posit if line.strip()]
        float_lines = [line.strip() for line in f_float if line.strip()]
    
    # Open 32-bit output files
    with open("error_32bit.txt", "w") as f_error, \
         open("large_errors_32bit.log", "w") as f_large:
        
        f_large.write("Lines with absolute error > 1e-6:\n")  # Adjusted threshold for 32-bit
        total_error = 0.0
        valid_count = 0
        large_error_count = 0
        
        for i in xrange(len(posit_lines)):
            try:
                # Convert 32-bit posit string to float
                posit_bits = posit_lines[i].zfill(32)  # Ensure 32-bit format
                posit_float = p.posit2float(posit_bits)
                reference_float = float(float_lines[i])
                
                # Skip NaN cases
                if math.isnan(reference_float) or math.isnan(posit_float):
                    f_error.write("NaN\n")
                    continue
                
                # Calculate absolute error
                error = abs(posit_float - reference_float)
                
                # Handle large errors (adjusted threshold for 32-bit precision)
                if error > 1:  # More appropriate threshold for 32-bit
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
    print("\n=== 32-bit Error Statistics ===")
    print("Valid calculations: {}/{}".format(valid_count, len(posit_lines)))
    print("Large errors skipped: {}".format(large_error_count))
    
    if valid_count > 0:
        avg_error = total_error / valid_count
        rel_error = (avg_error / abs(reference_float)) * 100 if reference_float != 0 else 0
        print("Average absolute error: {:.15g}".format(avg_error))
        print("Average relative error: {:.15g}%".format(rel_error))
    else:
        print("No valid error calculations could be performed")
    
    print("\nResults saved to:")
    print("- error_32bit.txt")
    print("- large_errors_32bit.log")

if __name__ == "__main__":
    calculate_errors()

import struct
import math

def hex_to_float(h):
    """Convert 32-bit hex string to float"""
    try:
        return struct.unpack('!f', h.decode('hex'))[0]
    except:
        return float('nan')

def calculate_errors():
    verilog_results = []
    exact_results = []
    
    # Read Verilog results (32-bit)
    with open('fp_results.txt', 'r') as f:
        for line in f:
            if not line.strip() or line.startswith('-----'):
                continue
            parts = line.split()
            if len(parts) >= 4:  # Format: "0: 3f800000 40000000 40400000"
                a_hex = parts[1]
                b_hex = parts[2]
                out_hex = parts[3]
                verilog_results.append(hex_to_float(out_hex))
    
    # Read exact results (64-bit)
    with open('result_fp_64bit.txt', 'r') as f:
        exact_results = [float(line.strip()) for line in f if line.strip()]
    
    # Calculate errors
    if len(verilog_results) != len(exact_results):
        print("Error: Mismatched number of results (%d vs %d)" % 
              (len(verilog_results), len(exact_results)))
        return
    
    total_error = 0.0
    max_error = 0.0
    error_count = 0
    
    for i in xrange(len(verilog_results)):
        verilog_val = verilog_results[i]
        exact_val = exact_results[i]
        
        if math.isnan(verilog_val) or math.isnan(exact_val):
            continue
        
        abs_error = abs(verilog_val - exact_val)
        total_error += abs_error
        max_error = max(max_error, abs_error)
        error_count += 1
    
    if error_count == 0:
        print("No valid results to compare")
        return
    
    avg_error = total_error / error_count
    
    print("Error Analysis Results:")
    print("-----------------------")
    print("Number of test cases: %d" % error_count)
    print("Average absolute error: %.15g" % avg_error)
    print("Maximum absolute error: %.15g" % max_error)
    if exact_results[0] != 0:
        print("Relative average error: %.15g" % (avg_error / abs(exact_results[0])))
    
    # Write detailed errors to file
    with open('error_analysis.txt', 'w') as f:
        f.write("Index\tVerilog\t\tExact\t\tAbsolute Error\tRelative Error\n")
        for i in xrange(min(100, len(verilog_results))):  # First 100 for inspection
            verilog_val = verilog_results[i]
            exact_val = exact_results[i]
            abs_error = abs(verilog_val - exact_val)
            rel_error = abs_error / abs(exact_val) if exact_val != 0 else float('inf')
            f.write("%d\t%.8g\t%.8g\t%.8g\t%.8g\n" % 
                   (i, verilog_val, exact_val, abs_error, rel_error))

if __name__ == "__main__":
    calculate_errors()

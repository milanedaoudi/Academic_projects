import random
import struct
import math

def float32_to_hex(f):
    packed = struct.pack('>f', f)
    return ''.join('{:02x}'.format(ord(byte)) for byte in packed)

def generate_test_cases(num_cases=30000):
    # Generate numbers with logarithmic distribution in [0.01, 0.1]
    list_a = []
    list_b = []
    
    for _ in xrange(num_cases):
        # Generate in log space for better distribution
        log_min = math.log(0.0001)
        log_max = math.log(0.001)
        a = math.exp(random.uniform(log_min, log_max))
        b = math.exp(random.uniform(log_min, log_max))
        
        # Clamp to exact bounds
        a = max(0.0001, min(a, 0.001))
        b = max(0.0001, min(b, 0.001))
        
        list_a.append(a)
        list_b.append(b)
    
    # Generate the three output files
    with open("Pin1_fp_32bit.txt", "w") as f1, \
         open("Pin2_fp_32bit.txt", "w") as f2, \
         open("result_fp_64bit.txt", "w") as f3:
        
        for i in xrange(num_cases):
            a = list_a[i]
            b = list_b[i]
            
            # 32-bit hex outputs (single precision)
            f1.write(float32_to_hex(a) + "\n")
            f2.write(float32_to_hex(b) + "\n")
            
            # 64-bit decimal result (double precision)
            res = float(a) + float(b)  # Force double precision
            f3.write("{:.15e}\n".format(res))  # Scientific notation with 15 decimals

if __name__ == "__main__":
    generate_test_cases()

/*
MIT License

Copyright (c) 2020 Debtanu Mukherjee

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
`include "fmultiplier.v"
module fmultiplier_tb;

reg [31:0] a, b;
wire [31:0] z;
reg clk, rst;
integer file_a, file_b, file_out;
integer scan_a, scan_b;
integer test_count;

fmultiplier multiplier(clk, rst, a, b, z);

initial begin
    $dumpfile("wave.vcd");
    $dumpvars(0, fmultiplier_tb);
end

initial begin
    clk <= 0;
    forever #5 clk <= ~clk;
end

initial begin
    // Open input and output files
    file_a = $fopen("Pin1_fp_32bit.txt", "r");
    file_b = $fopen("Pin2_fp_32bit.txt", "r");
    file_out = $fopen("fp_results.txt", "w");
    
    if (file_a == 0 || file_b == 0 || file_out == 0) begin
        $display("Error opening files");
        $finish;
    end
    
    rst <= 1;
    test_count = 0;
    
    // Reset for a few cycles
    @(posedge clk);
    rst <= 0;
    
    // Read test vectors and apply them
    scan_a = $fscanf(file_a, "%h\n", a);
    scan_b = $fscanf(file_b, "%h\n", b);
    while (scan_a == 1 && scan_b == 1) begin
        // Wait for multiplication to complete (adjust based on your multiplier's latency)
        repeat(5) @(posedge clk);
        
        // Write result to output file
        $fdisplay(file_out, "%h", z);
        test_count = test_count + 1;
        
        // Read next inputs
        scan_a = $fscanf(file_a, "%h\n", a);
        scan_b = $fscanf(file_b, "%h\n", b);
    end
    
    $fclose(file_a);
    $fclose(file_b);
    $fclose(file_out);
    $display("Completed %0d test cases", test_count);
    $finish;
end

endmodule

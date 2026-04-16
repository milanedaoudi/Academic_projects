`timescale 1ns / 1ps

`define CYCLE 20

module testbench;
parameter NUM = 30000;  // Number of test cases
reg clk, rst;
reg [31:0] A;
reg [31:0] B;
wire [31:0] out;
integer i, results_file;
reg [31:0] A_mem [0:NUM-1];  // Memory for A values
reg [31:0] B_mem [0:NUM-1];  // Memory for B values

initial begin
    // Load input files into memories
    $readmemh("Pin1_fp_32bit.txt", A_mem);
    $readmemh("Pin2_fp_32bit.txt", B_mem);
    
    // Open results file
    results_file = $fopen("fp_results.txt", "w");
    if (results_file == 0) begin
        $display("Error opening fp_results.txt");
        $finish;
    end
    
    // Initialize
    clk = 0;
    rst = 0;
    #(`CYCLE) rst = 1;
    #(`CYCLE) rst = 0;
    
    // Test loop
    for(i = 0; i < NUM; i = i + 1) begin
        // Get values from memories
        A = A_mem[i];
        B = B_mem[i];
        
        #(`CYCLE<<1);  // Wait for computation
        
        // Write results: Test#, A, B, Out
        $fwrite(results_file, "%5d: %h %h %h\n", i, A, B, out);
    end
    
    $fclose(results_file);
    $display("Completed %d tests. Results in fp_results.txt", NUM);
    $finish;
end

always #(`CYCLE/2) clk = ~clk;

// FP Adder instance
fpadd_single DUT(.clk(clk), .reset(rst), .reg_A(A), .reg_B(B), .out(out));

endmodule

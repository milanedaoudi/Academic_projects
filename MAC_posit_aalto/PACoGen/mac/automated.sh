#!/bin/bash

# Automation script for posit arithmetic verification
# Usage: ./run_verification.sh

# -------------------------------------------------------------------
# Step 1: Generate test patterns and reference results using Python
# -------------------------------------------------------------------
echo "============================================"
echo "STEP 1: Generating test patterns and reference results"
echo "============================================"

# Run the pattern generation and float addition script
python soft/init.py #for input range, CHOOSE init2.py else init1.py


echo "Pattern generation completed successfully"
echo ""

# -------------------------------------------------------------------
# Step 2: Compile and run Verilog simulation
# -------------------------------------------------------------------
echo "============================================"
echo "STEP 2: Running Verilog simulation"
echo "============================================"

# Compile Verilog files (modify as needed for your setup)
vlog -work work posit_add.v posit_mult.v posit_mac.v mac_testbench.v

# Check if compilation succeeded
if [ $? -ne 0 ]; then
    echo "ERROR: Verilog compilation failed"
    exit 1
fi

# Run Modelsim simulation for 1ms
vsim -c -do "run 0.21ms; quit" work.posit_mac_tb



echo "Verilog simulation completed successfully"
echo ""

# -------------------------------------------------------------------
# Step 3: Calculate errors between Verilog and reference results
# -------------------------------------------------------------------
echo "============================================"
echo "STEP 3: Calculating errors"
echo "============================================"

# Run the error calculation script
python soft/error.py

# Verify error file was created


echo "Error calculations completed successfully"
echo ""



LIBRARY ieee;
USE ieee.std_logic_1164.ALL;  -- Standard logic library for std_logic types
USE ieee.std_logic_textio.ALL;  -- Allows reading and writing std_logic values in text files
USE std.textio.ALL;  -- Standard text I/O library for file handling
USE ieee.numeric_std.ALL;

ENTITY tb_ex02 IS
END tb_ex02;

ARCHITECTURE behavior OF tb_ex02 IS

    -- Signal declarations
    SIGNAL A, B, Q : STD_LOGIC_VECTOR(7 DOWNTO 0);  -- 8-bit input and output signals
    SIGNAL S        : STD_LOGIC;  -- Selection signal for the multiplexer
    

    -- File declarations
    FILE infile  : TEXT OPEN READ_MODE IS "input.txt";  -- Input file for reading test data
    FILE outfile : TEXT OPEN WRITE_MODE IS "output.txt";  -- Output file for storing results

    -- Device Under Test (DUT) component
    COMPONENT multiplexer_2to1
        PORT (
            A : IN STD_LOGIC_VECTOR(7 DOWNTO 0);  -- First data input
            B : IN STD_LOGIC_VECTOR(7 DOWNTO 0);  -- Second data input
            S : IN STD_LOGIC;  -- Selection input
            Q : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)  -- Output
        );
    END COMPONENT;

BEGIN
    -- Instantiate the multiplexer
    UUT: multiplexer_2to1 PORT MAP (
        A => A,
        B => B,
        S => S,
        Q => Q
    );

 
    -- Test process: Reads values from file, applies to the DUT, and writes results
    TEST_PROCESS : PROCESS
        VARIABLE in_line  : LINE;  -- Line buffer for reading input file
        VARIABLE out_line : LINE;  -- Line buffer for writing output file
        VARIABLE A_var, B_var : STD_LOGIC_VECTOR(7 DOWNTO 0);  -- Variables for input signals
        VARIABLE S_var    : STD_LOGIC;  -- Variable for selection signal
    BEGIN
        -- Read inputs from file and apply them to the DUT
        WHILE NOT ENDFILE(infile) LOOP  -- Loop until end of input file
            READLINE(infile, in_line);  -- Read a line from input file
            READ(in_line, A_var);  -- Extract A value
            READ(in_line, B_var);  -- Extract B value
            READ(in_line, S_var);  -- Extract S value
            
            A <= A_var;  -- Assign read values to signals
            B <= B_var;
            S <= S_var;
            
            WAIT for 10 ns;  
            
            -- Write output to file
            WRITE(out_line, Q);  -- Store Q value in output buffer
            WRITELINE(outfile, out_line);  -- Write buffer to output file
        END LOOP;
        
        -- Stop simulation after processing all input values
        REPORT "Simulation completed successfully." SEVERITY NOTE;
	
	wait;
        
    END PROCESS;

END behavior;

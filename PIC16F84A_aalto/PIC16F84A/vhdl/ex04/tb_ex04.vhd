library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use STD.TEXTIO.ALL;
use work.ALU_Types.ALL;

entity tb_ex04 is
end tb_ex04;

architecture Behavioral of tb_ex04 is
    -- Component declaration for the ALU
    component alu is
        port (
            A, B : in std_logic_vector(7 downto 0);
            operation : in ALU_operation;
            result : out std_logic_vector(7 downto 0);
            bit_select : in std_logic_vector(2 downto 0);
            status_in : in std_logic_vector(2 downto 0);
            status : out std_logic_vector(2 downto 0)
        );
    end component;

    -- Signals for ALU inputs and outputs
    signal A, B : std_logic_vector(7 downto 0);
    signal operation : ALU_operation;
    signal result : std_logic_vector(7 downto 0);
    signal bit_select : std_logic_vector(2 downto 0);
    signal status_in, status : std_logic_vector(2 downto 0);

    -- File handling
    file input_file : text open read_mode is "input_program.txt";
    file output_file : text open write_mode is "output_results.txt";

begin
    -- Instantiate the ALU
    UUT: alu
        port map (
            A => A,
            B => B,
            operation => operation,
            result => result,
            bit_select => bit_select,
            status_in => status_in,
            status => status
        );

    -- Testbench process
    process
        variable file_line : line;
        variable op_str : string(1 to 6);
        variable A_val, B_val : std_logic_vector(7 downto 0);
        variable bit_sel : std_logic_vector(2 downto 0);
        variable status_val : std_logic_vector(2 downto 0);
    begin
        -- Initialize status_in
        status_in <= "000";

        -- Read and process each line from the input file
        while not endfile(input_file) loop
            readline(input_file, file_line);

            -- Read operation and operands from the file
            read(file_line, op_str);
            read(file_line, A_val);
            read(file_line, B_val);
            read(file_line, bit_sel);
            read(file_line, status_val);
                
            -- Assign values to ALU inputs
            A <= A_val;
            B <= B_val;
            operation <= string_to_ALU_operation(op_str);
            bit_select <= bit_sel;
            status_in <= status_val;

            -- Wait for the ALU to compute the result
            wait for 10 ns;

            -- Write the result to the output file
            write(file_line, "Operation: " & op_str & ", A: " & to_string(A_val) & ", B: " & to_string(B_val) &
                  ", Bit Select: " & to_string(bit_sel) & ", Status In: " & to_string(status_val) &
                  ", Result: " & to_string(result) & ", Status Out: " & to_string(status));
            writeline(output_file, file_line);
        end loop;

        -- End the simulation
        wait;
    end process;
end Behavioral;

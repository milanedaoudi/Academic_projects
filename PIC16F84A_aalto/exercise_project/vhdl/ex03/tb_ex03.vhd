library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL; -- Standard package
use STD.TEXTIO.ALL;

entity tb_ex03 is
end tb_ex03;

architecture Behavioral of tb_ex03 is
    -- Constants
    constant N : integer := 8;  -- 8-bit adder

    -- Signals
    signal A, B : std_logic_vector(N-1 downto 0);
    signal S1, S2 : std_logic_vector(N-1 downto 0);
    signal CO1, CO2 : std_logic;

    -- Component declarations
    component Nbit_ripple_carry_adder is
        port (
            A  : in  std_logic_vector(N-1 downto 0);
            B  : in  std_logic_vector(N-1 downto 0);
            S  : out std_logic_vector(N-1 downto 0);
            CO : out std_logic
        );
    end component;

    component N_bit_adder_process is
        port (
            A  : in  std_logic_vector(N-1 downto 0);
            B  : in  std_logic_vector(N-1 downto 0);
            S  : out std_logic_vector(N-1 downto 0);
            CO : out std_logic
        );
    end component;

begin

    -- Instantiate DUTs
    DUT1: Nbit_ripple_carry_adder
        port map (
            A  => A,
            B  => B,
            S  => S1,
            CO => CO1
        );

    DUT2: N_bit_adder_process
        port map (
            A  => A,
            B  => B,
            S  => S2,
            CO => CO2
        );

    -- Test process
    process
        file input_file : text open read_mode is "input.csv";
        file output_file : text open write_mode is "output.csv";
        variable input_line, output_line : line;
        variable A_val, B_val : integer;
        variable comma : character;
    begin
        -- Read input file line by line
        while not endfile(input_file) loop
            readline(input_file, input_line);  -- Read a line from the input file
            read(input_line, A_val);           -- Read value for A
            read(input_line, comma);           -- Read the comma separator
            read(input_line, B_val);           -- Read value for B

            -- Assign values to signals
            A <= std_logic_vector(to_unsigned(A_val, N));
            B <= std_logic_vector(to_unsigned(B_val, N));

            -- Wait for the results to stabilize
            wait for 10 ns;

            -- Write results to the output file
            write(output_line, A_val);
            write(output_line, ',');
            write(output_line, B_val);
            write(output_line, ',');
            write(output_line, to_integer(unsigned(S1)));
            write(output_line, ',');
            write(output_line, CO1);
            write(output_line, ',');
            write(output_line, to_integer(unsigned(S2)));
            write(output_line, ',');
            write(output_line, CO2);
            writeline(output_file, output_line);
        end loop;

        -- End simulation
        wait;
    end process;
end Behavioral;

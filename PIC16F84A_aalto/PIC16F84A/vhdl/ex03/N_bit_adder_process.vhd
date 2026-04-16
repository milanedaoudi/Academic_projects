library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL; -- Add this line

entity N_bit_adder_process is 
	generic (
		N : integer := 8
		);
	port (
		A, B : in std_logic_vector (N-1 downto 0);
		CO : out std_logic;
		S : out std_logic_vector (N-1 downto 0)
		);
	
end entity N_bit_adder_process;

architecture behavior of N_bit_adder_process is
	signal temp_sum : std_logic_vector(N downto 0); 

	begin
		process(A, B)
		begin
			temp_sum <= std_logic_vector(unsigned('0' & A) + 				unsigned('0' & B));
		end process;
	S  <= temp_sum(N-1 downto 0); 
        CO <= temp_sum(N); 
end architecture behavior;

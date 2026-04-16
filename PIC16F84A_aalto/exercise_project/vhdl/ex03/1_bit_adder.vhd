library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity 1_bit_adder is 
	port (
		A, B, CI : in std_logic;
		S, CO : out std_logic;
		);
	
end 1_bit_adder;

architecture behavior of 1_bit_adder is
	begin
		process(A, B, CI)
		begin
			S <= A xor B xor CI;
			CO <= (A and B) or (CI and (A or B));
		end process;
end architecture behavior;

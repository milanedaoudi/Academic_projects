library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity N_bit_adder is 
	generic (
		N : integer := 8
		);
	port (
		A, B : in std_logic_vector (N-1 downto 0);
		CI : in std_logic;
		CO : out std_logic;
		S : out std_logic_vector (N-1 dowto 0);
		);
	
end entity N_bit_adder;

architecture behavior of N_bit_adder is
	signal carry : std_logic_vector (N downto 0);

	begin
		carry(0) <= CI;
		process(A, B, CI)
		begin
			for i in O to N-1 loop
				if i = 0 then 
				S(i) <= A(i) xor B(i) xor carry(i);
				carry(i+1) <= (A(i) and B(i)) or (carry(i) and (A(i) xor B(i)); 
				else 
				S(i) <= A(i) xor B(i) xor carry(i);
				carry(i+1) <= (A(i) and B(i)) or (carry(i) and (A(i) xor B(i)); 
				end if
			end loop;
		CO <= carry(N);
		end process;
end architecture behavior;

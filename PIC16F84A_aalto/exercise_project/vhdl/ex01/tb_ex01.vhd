LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

entity tb_ex01 is
end tb_ex01;

architecture behavior of tb_ex01 is
	signal A, B, Q : BIT_VECTOR(7 downto 0);
	signal S : BIT;
begin
	process (A,B,S)
	begin
		if S = '0' then
			Q <= A;
		else 
			Q <= B;
		end if;
	end process;

--Testbench Stimulus
process 
begin
        A <= "00001111";
        B <= "11110000";
        S <= '0';
        wait for 10 ns;
        
        S <= '1';
        wait for 10 ns;
        
        S <= '0';
        wait for 10 ns;
        
        S <= '1';
        wait for 10 ns;

	wait;
end process;
end behavior;



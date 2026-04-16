LIBRARY ieee;
USE ieee.std_logic_1164.ALL;


entity Nbit_ripple_carry_adder is
	generic (
		N : integer := 8
		);
	port (
		A, B : in std_logic_vector (N-1 downto 0);
		S : out std_logic_vector (N-1 downto 0);
		CO : out std_logic
		);
end entity Nbit_ripple_carry_adder;

architecture structural of Nbit_ripple_carry_adder is

 -- Component declaration for the full adder
    component full_adder
        port (
            A    : in  std_logic;
            B    : in  std_logic;
            CI  : in  std_logic;
            S    : out std_logic;
            CO : out std_logic
        );
    end component;

	signal carry : std_logic_vector(N downto 0);

begin
	carry(0) <= '0';
	GEN_ADDERS: for i in 0 to N-1 generate
		FA: entity work.full_adder
		port map (
			A => A(i),
			B => B(i),
			CI => carry(i),
			S => S(i),
			CO => carry(i+1)
		);
	end generate GEN_ADDERS;
	CO <= carry(N);

end architecture structural;
			
			
			
	

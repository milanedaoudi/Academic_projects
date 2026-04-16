library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use STD.TEXTIO.ALL;
use work.ALU_Types.ALL;

-- ADD for both ADDWF and ADDLW
-- AND2 for both ANDWF and ANDLW
-- IOR for both IORWF and IORLW
-- SUB for both SUBWF and SUBLW
-- XOR2 for both XORWF and XORLW

entity alu is 
	port (
		A, B : in std_logic_vector(7 downto 0);
		operation : in ALU_operation;              
		result : out std_logic_vector(7 downto 0);
		bit_select : in std_logic_vector(2 downto 0);
		status_in : in std_logic_vector(2 downto 0);
		status : out std_logic_vector(2 downto 0)
		);
end entity alu;

architecture behavior of alu is 

	--Constant declarations
	constant N : integer := 8;  -- 8-bit adder
	
	-- Component declarations
	component Nbit_ripple_carry_adder is
 	       port (
           	 A  : in  std_logic_vector(N-1 downto 0);
           	 B  : in  std_logic_vector(N-1 downto 0);
           	 S  : out std_logic_vector(N-1 downto 0);
           	 CO : out std_logic
       		 );
    	end component;

	-- Signals used for calculations
	signal temp_result : std_logic_vector(8 downto 0);
	signal B_2complement : std_logic_vector(7 downto 0);
	signal clear_mask : std_logic_vector(7 downto 0);
	signal update_Zflag : std_logic;
    signal adder_result : std_logic_vector (7 downto 0);
    signal adder_CO : std_logic;
    signal sub_result : std_logic_vector(7 downto 0);
    signal sub_CO : std_logic;

begin
     --status <= status_in; 
     ADDER_INSTANCE: Nbit_ripple_carry_adder
        				port map (A  => A, B  => B, S  => adder_result, CO => adder_CO);

     B_2complement <=  std_logic_vector(unsigned(not B) +1);
      
     SUBSTRACTOR_INSTANCE :  Nbit_ripple_carry_adder
        				port map (A  => A, B => B_2complement, S  => sub_result, CO => sub_CO);

     clear_mask <= 
    (others => '1') when operation /= BCF and operation /= BSF else  -- Default case
    (0 => '0', others => '1') when operation = BCF and bit_select = "000" else
    (1 => '0', others => '1') when operation = BCF and bit_select = "001" else
    (2 => '0', others => '1') when operation = BCF and bit_select = "010" else
    (3 => '0', others => '1') when operation = BCF and bit_select = "011" else
    (4 => '0', others => '1') when operation = BCF and bit_select = "100" else
    (5 => '0', others => '1') when operation = BCF and bit_select = "101" else
    (6 => '0', others => '1') when operation = BCF and bit_select = "110" else
    (7 => '0', others => '1') when operation = BCF and bit_select = "111" else
    (others => '1') when operation = BCF else  -- Default for BCF
    (0 => '1', others => '0') when operation = BSF and bit_select = "000" else
    (1 => '1', others => '0') when operation = BSF and bit_select = "001" else
    (2 => '1', others => '0') when operation = BSF and bit_select = "010" else
    (3 => '1', others => '0') when operation = BSF and bit_select = "011" else
    (4 => '1', others => '0') when operation = BSF and bit_select = "100" else
    (5 => '1', others => '0') when operation = BSF and bit_select = "101" else
    (6 => '1', others => '0') when operation = BSF and bit_select = "110" else
    (7 => '1', others => '0') when operation = BSF and bit_select = "111" else
    (others => '0') when operation = BSF else  -- Default for BSF
    (others => '1');  -- Fallback default

    --process for taking care of result and Z_flag
	process(A, B, operation, bit_select, status_in, adder_result, sub_result, clear_mask)
	begin   
		case operation is
			when ADD =>    
                    result <= adder_result;
  					update_Zflag <= '1';

			when AND2 =>     
				result <= A and B; 
				update_Zflag <= '1';

			when COMF => 
				result <= not A;
				update_Zflag <= '1';
				
			when DECF =>
				result <= std_logic_vector(unsigned(A) - 1);
				update_Zflag <= '1';

			when INCF => 
				result <= std_logic_vector(unsigned(A) + 1);
				update_Zflag <= '1';

			when IOR => 
				result <= A or B;
				update_Zflag <= '1';

			when RLF =>
				result <= A(6 downto 0) & status_in(0);
				update_Zflag <= '0';

			when RRF =>
				result <= status_in(0) & A(7 downto 1);
				update_Zflag <= '0';

			when SUB =>
				result <= sub_result; 
				update_Zflag <= '1';

			when SWAPF =>
				result <= A(3 downto 0) & A(7 downto 4);
				update_Zflag <= '0';

			when XOR2 =>
				result <= A xor B;
				update_Zflag <= '1';

			when CLRF =>  --clear f
				result <= "00000000"; 
				update_Zflag <= '1';

			when CLEARW => --clear W
				result <= "00000000";
				update_Zflag <= '1';

			when MOVF => --move f
				result <= A; --in result, I put what I have to move 
				update_Zflag <= '1';

			when MOVWF => --move (W) to (f)
				result <= A;
				update_Zflag <= '0';

			when MOVLW => --move literal k to (W)
				result <= A;
				update_Zflag <= '0';

			when BCF =>
				result <= A and clear_mask;
				update_Zflag <= '0';

			when BSF =>
				result <= A or clear_mask;
				update_Zflag <= '0';

            when NOP =>
                 result <= "00000000";       
	        end case;
     end process;

        --process for taking care of status
		process(result, update_Zflag, status_in, operation, adder_CO, sub_CO)
    			begin
                    status <= status_in; 
       				if update_Zflag = '1' then
            				if result = "00000000" then
                				status(2) <= '1';  -- Set Z flag if result is zero
            				else
                				status(2) <= '0';  -- Clear Z flag if result is non-zero
            				end if;
        			end if;

                    if operation = ADD then
                        status(0) <= adder_CO;
					    status(1) <= A(3) and B(3); --carry_out_4bit
                    elsif operation = SUB then
                         status(0) <= sub_CO;
				         status(1) <= A(3) and B_2complement(3);
                     elsif operation = RLF then 
                        status(0) <= A(7);
                     elsif operation = RRF then 
                        status(0) <= A(0);
                     elsif operation = NOP then 
                         status <= "000";
                     end if;

    	end process;

      
end architecture behavior; 

                          
          
				
				

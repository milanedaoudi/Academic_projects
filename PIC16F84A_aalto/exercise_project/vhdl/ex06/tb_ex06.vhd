library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.instruction_pkg.all;
use work.ALU_Types.ALL;

entity tb_ex06 is
end tb_ex06;

architecture Behavioral of tb_ex06 is
    -- Component Declaration
    component decoder is
        Port (
            clk         : in  STD_LOGIC;
            reset       : in  STD_LOGIC;
            instruction : in  STD_LOGIC_VECTOR(13 downto 0);
            pc          : out std_logic_vector(12 downto 0);
            w_register  : out std_logic_vector(7 downto 0);
            -- ALU outputs
            alu_A, alu_B, alu_result : out std_logic_vector(7 downto 0);
            alu_op      : out ALU_operation;
            alu_bit_select : out std_logic_vector(2 downto 0);
            alu_status_in, alu_status_out : out std_logic_vector(2 downto 0);
            -- Memory outputs
            mem_write_addr, mem_read_addr : out std_logic_vector(6 downto 0);
            mem_data_in, mem_data_out : out std_logic_vector(7 downto 0);
            mem_write_en, mem_read_en : out std_logic;
            status_write_enable : out std_logic;
            tos : out std_logic_vector(12 downto 0);
            pc_lath : in std_logic_vector(4 downto 0)

        );
    end component;

    -- Testbench Signals
    signal clk          : STD_LOGIC := '0';
    signal reset        : STD_LOGIC := '1';
    signal instruction  : STD_LOGIC_VECTOR(13 downto 0) := (others => '0');
    signal pc_lath : std_logic_vector(4 downto 0);
    
    -- DUT Outputs
    signal pc           : std_logic_vector(12 downto 0);
    signal w_register   : std_logic_vector(7 downto 0);
    signal alu_A, alu_B, alu_result : std_logic_vector(7 downto 0);
    signal alu_op       : ALU_operation;
    signal alu_bit_select : std_logic_vector(2 downto 0);
    signal alu_status_in : std_logic_vector(2 downto 0);
    signal alu_status_out : std_logic_vector(2 downto 0);
    signal mem_write_addr, mem_read_addr : std_logic_vector(6 downto 0);
    signal mem_data_in  : std_logic_vector(7 downto 0);
    signal mem_data_out : std_logic_vector(7 downto 0);
    signal mem_write_en, mem_read_en : std_logic;
    signal status_write_enable : std_logic;
    signal tos : std_logic_vector(12 downto 0);

    -- Clock period
    constant clk_period : time := 10 ns;
    signal test_complete : boolean := false; 

begin
    -- Instantiate DUT
    uut: decoder port map (
        clk => clk,
        reset => reset,
        instruction => instruction,
        pc => pc,
        w_register => w_register,
        alu_A => alu_A,
        alu_B => alu_B,
        alu_result => alu_result,
        alu_op => alu_op,
        alu_bit_select => alu_bit_select,
        alu_status_in => alu_status_in,
        alu_status_out => alu_status_out,
        mem_write_addr => mem_write_addr,
        mem_read_addr => mem_read_addr,
        mem_data_in => mem_data_in,
        mem_data_out => mem_data_out,
        mem_write_en => mem_write_en,
        mem_read_en => mem_read_en,
        status_write_enable => status_write_enable,
        tos => tos,
        pc_lath => pc_lath
    );
    
      -- Clock generation
    clk_process: process
    begin
        while not test_complete loop
            clk <= '0';
            wait for clk_period/2;
            clk <= '1';
            wait for clk_period/2;
        end loop;
        wait;
    end process;
    
    -- Test stimulus
    stim_proc: process
    begin
        -- Reset the system
        reset <= '1';
        wait for clk_period*2;
        reset <= '0';
        --wait for clk_period/2;
        
        -- 1. First execute MOVLW to load W register with 0x03
        instruction <= "11000000000011";  -- MOVLW 0x03
        wait for clk_period*3;
        


        --2. writing to memory with instreuction MOVWF
        instruction <= "00000010000010"; --adress 0x02
        wait for clk_period*4;
        
        --3. changing the value of W register
        instruction <= "11000000001110";  --W=14
        wait for clk_period*3;

        --4. ADDWF
        instruction <= "00011100000010"; --14+3 and put it in W
        wait for clk_period*4;

        --5. MOVWF
        instruction <= "00000010000010"; --move W=17 to f(0x02) 
        wait for clk_period*4;

        --6. ANDWF
        instruction <= "00010100000010"; --17 and 17 and move it to W
        wait for clk_period*4;

        --7. COMF
        instruction <= "00100110000010"; --complement of f(0x02) and move it to f
        wait for clk_period*4;

        --8. DECF 
        instruction <= "00001100000010"; --decrement f and move it to W
        wait for clk_period*4;

        --9. INCF
        instruction <= "00101010000100"; --increment f(0x04) and move it to f
        wait for clk_period*4;

        --10. IORWF
        instruction <= "00010000000100"; -- or between W and f(0x04) and put it in W
        wait for clk_period*4;

        --11. CLRF
        instruction <= "00000110000010"; --put 0 in 0x02
        wait for clk_period*4;

        --12. Verify f is empty with MOVF
        instruction <= "00100000000010"; --move f(0x02) to W
        wait for clk_period*4;

        --13. MOVF
        instruction <= "00100000000100"; --move f(0x04) to W
        wait for clk_period*4;

        --14. CLRW
        instruction <= "00000100000000"; --clear W
        wait for clk_period*4;

        --Clean
        reset <= '1';
        wait for clk_period*2;
        reset <= '0';


        -- verify if memory is empty
        instruction <= "00100000000100"; --check 0x04 is empty
        wait for clk_period*4;

        instruction <= "00100000000010"; --check 0x02 is empty
        wait for clk_period*4;

        

        --1. NOP
        instruction <= "00000000000000";
        wait for clk_period*4;

        --2. Prepare for RLF
        instruction <= "00101010000011"; --increment f(0x03) to modify carry status bit
        wait for clk_period*4;

        instruction <= "11000000110001"; --MOVLW
        wait for clk_period*3;

        instruction <= "00000010000010"; --MOVWF f(0x02)
        wait for clk_period*4;

        instruction <= "00110100000010"; --RLF f(0x02) and put it in W
        wait for clk_period*4;

        --3. RRF
        instruction <= "00110000000010";
        wait for clk_period*4;
        
        --4. SUBWF
        instruction <= "00001000000010";--sub W from f(0x02) and put it in W
        wait for clk_period*4;

        --5. SWAPF
        instruction <= "00111000000010"; --swap nibbles in f(0x02) and put it in W
        wait for clk_period*4;
        
        --6. XORWF
        instruction <= "00011000000010"; --xor between W and f(0x02) and put it in W
        wait for clk_period*4;

        --Clean
        reset <= '1';
        wait for clk_period*2;
        reset <= '0';

        --1. BSF
        instruction <= "01010100000010"; --set bit 2 of f(0x02)
        wait for clk_period*4;

        --2. BCF
        instruction <= "01000100000010"; --clear bit 2 of f(0x02)
        wait for clk_period*4;


        --Clean
        reset <= '1';
        wait for clk_period*2;
        reset <= '0';

        --1.MOVLW
        instruction <= "11000011110000"; --move 1111 0000 to W
        wait for clk_period*3;

        --2. ADDLW
        instruction <= "11111000000010"; --add 2 and W and put it in W
        wait for clk_period*3;

        --3.IORLW
        instruction <= "11100000001111";
        wait for clk_period*3;

        --4. SUBLW
        instruction <= "11110011111111"; 
        wait for clk_period*3;

        --5. XORLW
        instruction <= "11101011110000";
        wait for clk_period*3;

        --Clean
        reset <= '1';
        wait for clk_period*2;
        reset <= '0';

        --1. CALL
        instruction <= "10000000001111"; --k=00000001111
        pc_lath <= "01101";
        wait for clk_period*3;

        --2. GOTO
        instruction <= "10100011110000"; --k=00011110000
        wait for clk_period*3;

        --3. RETLW
        instruction <= "11010010100011";
        wait for clk_period*3;

        --4. RETURN
        instruction <= "00000000001000";
        wait for clk_period*3;
               
        report "Test completed successfully";
        test_complete <= true;
        wait;
    end process;
    
        
end Behavioral;

--log :

-- make sure all instructions success in 4 cycles or less 

-- coorect the lists of operations that nead to read in memory

-- make my procedure lighter

--verify all instructions
--MOVLW
--MOWF
--ADDWF
--ANDWF
--CLRF
--CLRW
--COMF
--DECF
--INCF
--IORWF
--MOVF
--reset erasing memory 
--NOP
--RLF
--RRF
--SUBWF
--SWAPF
--XORWF
--BSF
--BCF
--ADDLW
--ANDLW
--IORWL
--MOVLW
--SUBLW
--XORLW
--CALL
--GOTO



--problem with instruction MOVWF when writing to memory => resolved using combinational read
--problem when reading to memory followed by operantion => combinational read ? => DONE
--many problems with status, came down to one solution working in order to execute RLF and RRF properly


--problem with reset

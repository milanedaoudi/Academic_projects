library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.instruction_pkg.all;
use work.ALU_Types.ALL;

entity tb_special is
end tb_special;

architecture Behavioral of tb_special is
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

            -- Stack
            pc_lath : in std_logic_vector(4 downto 0);
            push : out std_logic;
            pop : out std_logic;
            stack_data_in : out std_logic_vector(12 downto 0);
            stack_data_out : out std_logic_vector(12 downto 0)
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
    signal push, pop : std_logic;
    signal stack_data_in, stack_data_out : std_logic_vector(12 downto 0);

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
        pc_lath => pc_lath,
        push => push,
        pop => pop,
        stack_data_in => stack_data_in,
        stack_data_out => stack_data_out
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
         
        --1. CALL
        instruction <= "10000000001111"; --k=00000001111
        pc_lath <= "01101";
        wait for clk_period*3;

        --2. Intermediate operations (of a subprogram for instance)
        --MOVLW
        instruction <= "11000010000000";
        wait for clk_period*3;
        --ADDLW
        instruction <= "11111000001111";
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

--rest to implement:
--CALL, GOTO, RETLW, RETURN



--problem with instruction MOVWF when writing to memory => resolved using combinational read
--problem when reading to memory followed by operantion => combinational read ? => DONE
--many problems with status, came down to one solution working in order to execute RLF and RRF properly


--problem with reset

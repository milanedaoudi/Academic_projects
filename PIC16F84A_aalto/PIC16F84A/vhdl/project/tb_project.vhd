library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.instruction_pkg.all;
use work.ALU_Types.ALL;
use work.read_intel_hex_pack.ALL; -- For HEX file reading

entity PIC is
end PIC;

architecture Behavioral of PIC is
    -- Constants
    constant CLK_PERIOD : time := 20 ns; -- 50 MHz clock
    
    -- Signals for the DUT (decoder)
    signal clk          : STD_LOGIC := '0';
    signal reset        : STD_LOGIC := '1';
    signal instruction  : STD_LOGIC_VECTOR(13 downto 0) := (others => '0');
    signal pc           : std_logic_vector(12 downto 0);
    signal w_register   : std_logic_vector(DATA_WIDTH-1 downto 0);
    
    -- ALU signals
    signal alu_A, alu_B, alu_result : std_logic_vector(DATA_WIDTH-1 downto 0);
    signal alu_op : ALU_operation;
    signal alu_bit_select : std_logic_vector(2 downto 0);
    signal alu_status_in, alu_status_out : std_logic_vector(2 downto 0);
    
    -- Memory signals
    signal mem_write_addr, mem_read_addr : std_logic_vector(ADDR_WIDTH-1 downto 0);
    signal mem_data_in, mem_data_out : std_logic_vector(DATA_WIDTH-1 downto 0);
    signal mem_write_en, mem_read_en : std_logic;
    signal status_write_enable : std_logic;
    
    -- Stack signals
    signal pc_lath : std_logic_vector(4 downto 0) := (others => '0');
    signal push, pop : std_logic;
    signal stack_data_in, stack_data_out : std_logic_vector(12 downto 0);
    
    -- Testbench signals
    signal program_memory : program_array := (others => (others => '0'));
    signal simulation_done : boolean := false;

    -- Procedure to load memory (avoiding shared variable issues)
    procedure load_program_memory(
        file_name : in string;
        signal mem : out program_array) is
        variable temp_mem : program_array := (others => (others => '0'));
    begin
        read_ihex_file(file_name, temp_mem);
        mem <= temp_mem;
    end procedure;
    
begin
    -- Clock generation
    clk <= not clk after CLK_PERIOD/2 when not simulation_done else '0';
    
    -- Instantiate the decoder (DUT)
    dut: entity work.decoder
    port map (
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
    
    -- Stimulus process
    stimulus: process
        constant hex_file_path : string := "/home/mdaoudi/Documents/ELEC-E9540/PIC16F84A/piklab/testbench3.hex";
    begin
        -- Load HEX file into program memory
        load_program_memory(hex_file_path, program_memory);
        
        -- Release reset after 2 clock cycles
        wait for CLK_PERIOD * 2.5;
        reset <= '0';
        
        -- Let the simulation run for a while
        wait for CLK_PERIOD * 100;
        
        -- End simulation
        simulation_done <= true;
        wait;
    end process;
    
    -- Instruction fetch process
    instruction_fetch: process(clk)
        variable pc_int : integer;
    begin
        if falling_edge(clk) and reset = '0' then
            -- Convert PC to integer for array indexing
            pc_int := to_integer(unsigned(pc));
            
            -- Fetch instruction from program memory
            if pc_int < inst_mem_size then
                instruction <= program_memory(pc_int);
            else
                -- Handle PC out of bounds (optional)
                instruction <= (others => '0');
                report "PC out of bounds: " & integer'image(pc_int) severity warning;
            end if;
        end if;
    end process;
    
    -- Monitor process (optional)
    monitor: process
    begin
        wait until rising_edge(clk);
        
        -- Display PC and instruction every cycle
        report "PC: " & to_hstring(pc) & 
               " Instruction: " & to_hstring(instruction) &
               " W_REG: " & to_hstring(w_register);
        
        -- Add more monitoring as needed
    end process;
    
end Behavioral;



--if instruction = 0000 then next_state=00
--all instructions need to succed in 4 cycles, debugging easier

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity memory is 
    generic (
        ADDR_WIDTH : integer := 8; 
        DATA_WIDTH : integer := 8
    );
    port (
        clk, rst : in std_logic;
        write_addr : in std_logic_vector(ADDR_WIDTH-1 downto 0);
        read_addr : in std_logic_vector(ADDR_WIDTH-1 downto 0);
        data_in : in std_logic_vector(DATA_WIDTH-1 downto 0);
        data_out : out std_logic_vector(DATA_WIDTH-1 downto 0);
        write_en, read_en : in std_logic;
        mem_status_in : in std_logic_vector(2 downto 0); -- alu_status_out
        mem_status_out : out std_logic_vector(2 downto 0) -- alu status_in
    ); 
end entity memory; 

architecture behavior of memory is 
    -- Signal declarations 
    type memory_array is array (0 to 2**ADDR_WIDTH-1) of std_logic_vector(DATA_WIDTH-1 downto 0);
    signal mem : memory_array := (others => (others => '0'));  -- memory initialization
    constant STATUS_ADDR: std_logic_vector(ADDR_WIDTH-1 downto 0) := 
        std_logic_vector(to_unsigned(3, ADDR_WIDTH));
    
    -- Registered output for better timing
    signal data_out_reg : std_logic_vector(DATA_WIDTH-1 downto 0) := (others => '0');

begin 
    -- Status output is continuously driven from memory location 0x03
    mem_status_out <= mem(to_integer(unsigned(STATUS_ADDR)))(2 downto 0);

    -- Synchronous memory operations
    process (clk) 
    begin 
        if rising_edge(clk) then
            if rst = '1' then 
                -- Reset clears entire memory and output
                mem <= (others => (others => '0'));
                data_out_reg <= (others => '0');
            else
                -- Write operation (has priority if both read and write enabled)
                if write_en = '1' then 
                    if write_addr = STATUS_ADDR then
                        -- Direct write to status register
                        mem(to_integer(unsigned(write_addr))) <= data_in; 
                    else
                        -- Normal write with automatic status update
                        mem(to_integer(unsigned(write_addr))) <= data_in;
                        -- Update status register (bits 2-0 only)
                        mem(to_integer(unsigned(STATUS_ADDR))) <= 
                            (DATA_WIDTH-1 downto 3 => '0') & mem_status_in; 
                    end if;
                end if;
                
                -- Read operation (registered output)
                if read_en = '1' then
                    data_out_reg <= mem(to_integer(unsigned(read_addr)));
                end if;
            end if;
        end if;
    end process; 

    -- Drive output from register
    data_out <= data_out_reg;
    
end architecture behavior;

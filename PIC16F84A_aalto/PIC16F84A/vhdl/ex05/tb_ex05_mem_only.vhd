library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity tb_ex05 is
end tb_ex05;

architecture behavior of tb_ex05 is
    -- Component Declaration
    component memory
    generic (
        ADDR_WIDTH : integer := 8;
        DATA_WIDTH : integer := 8
    );
    port (
        clk, rst         : in std_logic;
        write_addr       : in std_logic_vector(7 downto 0);
        read_addr        : in std_logic_vector(7 downto 0);
        data_in          : in std_logic_vector(7 downto 0);
        data_out         : out std_logic_vector(7 downto 0);
        write_en, read_en : in std_logic;
        mem_status_in    : in std_logic_vector(2 downto 0);
        mem_status_out   : out std_logic_vector(2 downto 0)
    );
    end component;

    -- Testbench Signals
    signal clk, rst : std_logic := '0';
    signal write_addr, read_addr : std_logic_vector(7 downto 0) := (others => '0');
    signal data_in, data_out : std_logic_vector(7 downto 0) := (others => '0');
    signal write_en, read_en : std_logic := '0';
    signal mem_status_in, mem_status_out : std_logic_vector(2 downto 0) := (others => '0');

    -- Clock period
    constant clk_period : time := 10 ns;

    signal test_complete: boolean :=false;

begin
    -- Instantiate Unit Under Test
    uut: memory
    generic map (
        ADDR_WIDTH => 8,
        DATA_WIDTH => 8
    )
    port map (
        clk => clk,
        rst => rst,
        write_addr => write_addr,
        read_addr => read_addr,
        data_in => data_in,
        data_out => data_out,
        write_en => write_en,
        read_en => read_en,
        mem_status_in => mem_status_in,
        mem_status_out => mem_status_out
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

    -- Stimulus process
    stim_proc: process
    begin
        -- Reset the memory
        rst <= '1';
        wait for clk_period*2;
        rst <= '0';
        wait for clk_period;

        -- Test 1: Simple write then read
        report "Test 1: Write then read from different addresses";
        write_addr <= x"10";
        data_in <= x"AA";
        mem_status_in <= "101";
        write_en <= '1';
        wait for clk_period;
        write_en <= '0';
        
        read_addr <= x"10";
        read_en <= '1';
        wait for clk_period;
        assert data_out = x"AA" report "Test 1 failed: Data mismatch" severity error;
        assert mem_status_out = "101" report "Test 1 failed: Status mismatch" severity error;
        read_en <= '0';
        wait for clk_period;

        -- Test 2: Simultaneous read and write
        report "Test 2: Simultaneous read and write";
        write_addr <= x"20";
        read_addr <= x"10";  -- Read previous value
        data_in <= x"55";
        mem_status_in <= "110";
        write_en <= '1';
        read_en <= '1';
        wait for clk_period;
        assert data_out = x"AA" report "Test 2 failed: Read during write failed" severity error;
        write_en <= '0';
        read_en <= '0';
        wait for clk_period;

        -- Verify the write
        read_addr <= x"20";
        read_en <= '1';
        wait for clk_period;
        assert data_out = x"55" report "Test 2 failed: Write verification failed" severity error;
        assert mem_status_out = "110" report "Test 2 failed: Status update failed" severity error;
        read_en <= '0';
        wait for clk_period;

        -- Test 3: Direct status register write
        report "Test 3: Direct status register write";
        write_addr <= x"03";  -- Status register address
        data_in <= x"07";     -- Binary 00000111 (only lower 3 bits matter)
        write_en <= '1';
        wait for clk_period;
        write_en <= '0';
        
        -- Check status
        read_addr <= x"03";
        read_en <= '1';
        wait for clk_period;
        assert mem_status_out = "111" report "Test 3 failed: Direct status write failed" severity error;
        read_en <= '0';
        wait for clk_period;

        -- Test 4: Read during write to same address
        report "Test 4: Read during write to same address";
        write_addr <= x"30";
        read_addr <= x"30";  -- Same address!
        data_in <= x"F0";
        mem_status_in <= "011";
        write_en <= '1';
        read_en <= '1';
        wait for clk_period;
        -- Should get old value or undefined during write (implementation dependent)
        write_en <= '0';
        read_en <= '0';
        wait for clk_period;

        -- Verify final write
        read_addr <= x"30";
        read_en <= '1';
        wait for clk_period;
        assert data_out = x"F0" report "Test 4 failed: Final write verification" severity error;
        read_en <= '0';

        report "All tests completed successfully";
        test_complete <= true; 
        wait;
    end process;

end behavior;

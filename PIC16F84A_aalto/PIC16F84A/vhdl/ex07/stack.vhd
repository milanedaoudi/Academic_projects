    --Push and pop should not be asserted simultaneously (push takes precedence in this implementation)

    --The stack pointer is not directly readable or writable as per requirements

    --PCLATH handling would need to be implemented separately in the calling unit

    --The output always reflects the top of the stack (most recently pushed value)



library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity stack_8x13 is
    Port (
        clk         : in  STD_LOGIC;
        reset       : in  STD_LOGIC;
        push        : in  STD_LOGIC;  -- Active high to push to stack
        pop         : in  STD_LOGIC;  -- Active high to pop from stack
        data_in     : in  STD_LOGIC_VECTOR(12 downto 0);  -- 13-bit input
        data_out    : out STD_LOGIC_VECTOR(12 downto 0);  -- 13-bit output
        stack_full  : out STD_LOGIC;  -- Indicates stack is full (8 levels)
        stack_empty : out STD_LOGIC   -- Indicates stack is empty
    );
end stack_8x13;

architecture Behavioral of stack_8x13 is
    type stack_type is array (0 to 7) of STD_LOGIC_VECTOR(12 downto 0);
    signal stack : stack_type := (others => (others => '0'));
    
    signal stack_ptr : integer range 0 to 7 := 0;
    signal count : integer range 0 to 8 := 0;
begin

    process(clk, reset)
    begin
        if reset = '1' then
            stack <= (others => (others => '0'));
            stack_ptr <= 0;
            count <= 0;
            stack_full <= '0';
            stack_empty <= '1';
        elsif rising_edge(clk) then
            -- Push operation takes precedence over pop if both are asserted
            if push = '1' then
                -- Store the input data at current pointer position
                stack(stack_ptr) <= data_in;
                
                -- Circular buffer behavior - overwrite oldest when full
                if count < 8 then
                    count <= count + 1;
                end if;
                
                -- Move pointer (circular)
                if stack_ptr = 7 then
                    stack_ptr <= 0;
                else
                    stack_ptr <= stack_ptr + 1;
                end if;
                
                stack_empty <= '0';
                if count = 7 then --stack is going to be full (count will be 8 at end of process)
                    stack_full <= '1';
                end if;
                
            elsif pop = '1' and count > 0 then
                -- For pop, we need to move pointer back first
                if stack_ptr = 0 then
                    stack_ptr <= 7;
                else
                    stack_ptr <= stack_ptr - 1;
                end if;
                
                count <= count - 1;
                
                stack_full <= '0';
                if count = 1 then
                    stack_empty <= '1';
                end if;
            end if;
        end if;
    end process;

    -- The output always shows the most recently pushed value (top of stack)
    data_out <= stack(7) when stack_ptr = 0 else stack(stack_ptr - 1);

end Behavioral;

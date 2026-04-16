library IEEE;
use IEEE.STD_LOGIC_1164.all;

package instruction_pkg is
    -- Instruction type enumeration
    type instruction_type is (byte_oriented, bit_oriented, lANDc, CLRW, special, unknown);
    
    -- Constants for special instructions
    constant NOP_INSTRUCTION : std_logic_vector(13 downto 0) := "00000000000000";
    constant RETURN_INSTRUCTION : std_logic_vector(13 downto 0) := "00000000001000";
    constant ADDR_WIDTH : integer := 7;
    constant DATA_WIDTH : integer := 8;
    
   
    
    -- Data extraction procedure
    procedure data_extraction (
        -- Input
        signal instruction : in std_logic_vector(13 downto 0);
        -- Global output
        signal operation : out instruction_type;
        -- Output byte oriented
        signal opcode_byte : out std_logic_vector(5 downto 0); -- 6 bits
        signal d_byte : out std_logic;
        signal f_byte : out std_logic_vector(6 downto 0);
        -- Output bit-oriented
        signal opcode_bit : out std_logic_vector(3 downto 0); -- 4 bits
        signal b_bit : out std_logic_vector(2 downto 0);
        signal f_bit : out std_logic_vector(6 downto 0);
        -- Output literal and control
        signal opcode_lANDc : out std_logic_vector(5 downto 0); -- 6 bits
        signal k_literal : out std_logic_vector(7 downto 0);
        -- Output special
        signal opcode_special : out std_logic_vector(2 downto 0);
        signal k_special : out std_logic_vector(10 downto 0);
        -- Memory interface
        signal mem_read_address : out std_logic_vector(6 downto 0)
    );
end package instruction_pkg;

package body instruction_pkg is
    procedure data_extraction (
        signal instruction : in std_logic_vector(13 downto 0);
        signal operation : out instruction_type;
        signal opcode_byte : out std_logic_vector(5 downto 0);
        signal d_byte : out std_logic;
        signal f_byte : out std_logic_vector(6 downto 0);
        signal opcode_bit : out std_logic_vector(3 downto 0);
        signal b_bit : out std_logic_vector(2 downto 0);
        signal f_bit : out std_logic_vector(6 downto 0);
        signal opcode_lANDc : out std_logic_vector(5 downto 0);
        signal k_literal : out std_logic_vector(7 downto 0);
        signal opcode_special : out std_logic_vector(2 downto 0);
        signal k_special : out std_logic_vector(10 downto 0);
        signal mem_read_address : out std_logic_vector(6 downto 0)
    ) is
    begin
        -- Default assignments (avoids latches)
        operation      <= unknown;
        opcode_byte    <= (others => '0');
        d_byte         <= '0';
        f_byte         <= (others => '0');
        opcode_bit     <= (others => '0');
        b_bit          <= (others => '0');
        f_bit          <= (others => '0');
        opcode_lANDc   <= (others => '0');
        k_literal      <= (others => '0');
        opcode_special <= (others => '0');
        k_special      <= (others => '0');
        mem_read_address <= (others => '0');

        if instruction = NOP_INSTRUCTION then
            operation <= unknown;
        
        elsif instruction(13 downto 12) = "00" and instruction /= RETURN_INSTRUCTION then
            -- Byte oriented
            operation <= byte_oriented;
            opcode_byte <= instruction(13 downto 8);
            d_byte <= instruction(7);
            f_byte <= instruction(6 downto 0);
            mem_read_address <= instruction(6 downto 0);
            
        elsif instruction(13 downto 12) = "01" then
            -- Bit-oriented
            operation <= bit_oriented;
            opcode_bit <= instruction(13 downto 10);
            b_bit <= instruction(9 downto 7);
            f_bit <= instruction(6 downto 0);
            mem_read_address <= instruction(6 downto 0);
            
        elsif instruction(13 downto 12) = "11" or instruction = RETURN_INSTRUCTION then
            -- Literal and control and return
            operation <= lANDc;
            opcode_lANDc <= instruction(13 downto 8);
            k_literal <= instruction(7 downto 0);
            
        elsif instruction(13 downto 12) = "10" then
            -- Call and goto
            operation <= special;
            opcode_special <= instruction(13 downto 11);
            k_special <= instruction(10 downto 0);
        end if;
    end procedure;
end package body instruction_pkg;

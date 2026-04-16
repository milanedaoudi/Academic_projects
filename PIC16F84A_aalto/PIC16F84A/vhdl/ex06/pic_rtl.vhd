library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.instruction_pkg.all;
use work.ALU_Types.ALL;
    
entity decoder is
    Port (
        clk         : in  STD_LOGIC;
        reset       : in  STD_LOGIC;
        instruction : in  STD_LOGIC_VECTOR(13 downto 0);  -- 14-bit PIC instruction
        pc          : out std_logic_vector(12 downto 0); --bit-width to determine by the size of 
                                                          -- of instruction memory
        --w_register
        w_register : out std_logic_vector(DATA_WIDTH-1 downto 0);

        --ALU outputs
        alu_A, alu_B, alu_result : out std_logic_vector(DATA_WIDTH-1 downto 0);
        alu_op : out ALU_operation;
        alu_bit_select : out std_logic_vector(2 downto 0);
        alu_status_in, alu_status_out : out std_logic_vector(2 downto 0);

        --Memory outputs
        mem_write_addr, mem_read_addr : out std_logic_vector(ADDR_WIDTH-1 downto 0);
        mem_data_in, mem_data_out : out std_logic_vector(DATA_WIDTH-1 downto 0);
        mem_write_en, mem_read_en : out std_logic;
        status_write_enable : out std_logic;

        --Stack
        pc_lath : in std_logic_vector(4 downto 0);
        push : out std_logic;
        pop : out std_logic;
        stack_data_in : out std_logic_vector(12 downto 0);
        stack_data_out : out std_logic_vector(12 downto 0)
        
            );
end decoder;

architecture Behavioral of decoder is
    --Constants definition
    constant RETURN_INSTRUCTION : std_logic_vector(13 downto 0) := "00000000001000";
    constant CLRW_INSTRUCTION : std_logic_vector(13 downto 0) := "00000100000000";
    constant NOP_INSTRUCTION : std_logic_vector(13 downto 0) := "00000000000000";
    constant ADDR_WIDTH : integer := 7;
    constant DATA_WIDTH : integer := 8;

    
    -- State type definition
    -- iFetch=00, Mread=01, Execute=10, Mwrite=11
    signal current_state, next_state : std_logic_vector(1 downto 0) := "00";

    --signals used in procedure
    signal operation      : instruction_type;
    signal opcode_byte    : std_logic_vector(5 downto 0);
    signal d_byte         : std_logic;
    signal f_byte         : std_logic_vector(6 downto 0);

    signal opcode_bit     : std_logic_vector(3 downto 0);
    signal b_bit          : std_logic_vector(2 downto 0);
    signal f_bit          : std_logic_vector(6 downto 0);

    signal opcode_lANDc   : std_logic_vector(5 downto 0);
    signal k_literal      : std_logic_vector(7 downto 0);

    signal opcode_special : std_logic_vector(2 downto 0);
    signal k_special      : std_logic_vector(10 downto 0);


     --signals used elsewhere
    signal operand : std_logic_vector(DATA_WIDTH-1 downto 0);
    signal alu_status_out_reg : std_logic_vector(2 downto 0);
    signal pc_reg : std_logic_vector(12 downto 0);
    signal operation_reg : instruction_type;
    signal stack_full, stack_empty : std_logic;

  
            
begin
    -- Instantiate ALU
    uut_alu: entity work.alu
    port map (
        A => alu_A,
        B => alu_B,
        operation => alu_op,
        result => alu_result,
        bit_select => alu_bit_select,
        status_in => alu_status_in,
        status => alu_status_out
    );
    
    -- Instantiate Memory
    uut_mem: entity work.memory
    generic map (
        ADDR_WIDTH => ADDR_WIDTH,
        DATA_WIDTH => DATA_WIDTH
    )
    port map (
        clk => clk,
        rst => reset,
        write_addr => mem_write_addr,
        read_addr => mem_read_addr,
        data_in => mem_data_in,
        data_out => mem_data_out,
        write_en => mem_write_en,
        read_en => mem_read_en,
        mem_status_in => alu_status_out_reg, --branching between two modules
        mem_status_out => alu_status_in,
        status_write_enable => status_write_enable
    );


    uut_stack: entity work.stack_8x13
    port map (
        clk => clk,
        reset => reset,
        push => push,
        pop => pop,
        data_in => stack_data_in,
        data_out => stack_data_out,
        stack_full => stack_full,
        stack_empty => stack_empty
    );
           

    -- State transition process
    process(clk, reset)
    begin
        if reset = '1' then
            current_state <= "00";
            pc <= (others => '0');

        elsif rising_edge(clk) then
            current_state <= next_state;
            if current_state = "11" and operation = lANDc 
                                      and (instruction(13 downto 8) = "110100"
                                      or instruction = RETURN_INSTRUCTION) then
               pc <= pc_reg;

            elsif (current_state = "11") and operation_reg = special then
               pc <= pc_reg;

            elsif current_state = "11" then
               pc <= std_logic_vector(unsigned(pc)+1); --increment pc
            end if;
                                                
        end if;
    end process;
    
    -- State machine and output logic
    process(current_state, instruction,reset)
    begin
        -- Default outputs
        alu_op <= NOP;


        --for RLF and RRF logic 
        status_write_enable <= '0';

        --for pc logic
        operation_reg <= operation; --retain one more cycle the value of last operation

        if instruction = "00000000000000" then
                    next_state <= "00";
                end if;

        if reset = '1' then
            next_state <= "00";
            w_register <= (others => '0');

        else

        case current_state is
            when "00" =>  --iFetch
                
                if instruction = "00000000000000" then
                    next_state <= "00";
                else

                --reset memory write logic
                mem_write_en <= '0';

                -- Read instruction and decode
                data_extraction(
                instruction    => instruction,   -- From input port
                operation      => operation,
                opcode_byte    => opcode_byte,
                d_byte         => d_byte,
                f_byte         => f_byte,
                opcode_bit     => opcode_bit,
                b_bit          => b_bit,
                f_bit          => f_bit,
                opcode_lANDc   => opcode_lANDc,
                k_literal      => k_literal,
                opcode_special => opcode_special,
                k_special      => k_special,
                mem_read_address => mem_read_addr
                );

                --determine if instruction needs Mread
                if (instruction(13 downto 12) = "00" or instruction(13 downto 12) = "01") 
                and instruction /= CLRW_INSTRUCTION 
                and instruction /= RETURN_INSTRUCTION then
                          mem_read_en <= '1';
                          next_state <= "01";
                          -- mem_read_addr assigned in procedure

                else 
                    next_state <= "10";
                end if;

            end if;
                                                                                 
                
            when "01" =>   --Mread
                mem_read_en <= '0';

                --performs read if needed
                case operation is
                    when byte_oriented =>
                        if instruction /= CLRW_INSTRUCTION then
                            operand <= mem_data_out; 
                            next_state <= "10"; 

                        else 
                            --operation CLRW
                            next_state <= "10"; 
                        end if;

                    when bit_oriented => 
                        operand <= mem_data_out;
                        next_state <= "10";
                                                                                                                        
                    when others =>
                        next_state <= "10";
                end case;
                
            when "10" =>   --Execute

                --reset memory read logic
                mem_read_en <= '0';

                --determine operations and feeds ALU
                if operation = byte_oriented then
                    case opcode_byte is 
                        when "000111" => --ADDWF
                            alu_op <= ADD;
                            alu_A <= operand;
                            alu_B <= w_register;
                        when "000101" =>  --ANDWF
                            alu_op <= AND2;
                            alu_A <= operand;
                            alu_B <= w_register;
                        when "000001" =>  --CLRF and CLRW (same opcode)
                            alu_op <= CLRF;
                        when "001001" => --COMF
                            alu_op <= COMF;
                            alu_A <= operand;
                        when "000011" => --DECF
                            alu_op <= DECF;
                            alu_A <= operand;
                        when "001010" => --INCF
                            alu_op <= INCF;
                            alu_A <= operand;
                        when "000100" => --IORWF
                            alu_op <= IOR;
                            alu_A <= operand; 
                            alu_B <= w_register;
                        when "001000" => --MOVF
                            alu_op <= MOVF;
                            alu_A <= operand;
                        when "000000" => --MOVWF
                            alu_op <= MOVWF;
                            alu_A <= w_register;
                        when "001101" => --RLF
                            alu_op <= RLF;
                            alu_A <= operand;
                        when "001100" => --RRF
                            alu_op <= RRF;
                            alu_A <= operand;
                        when "000010" => --SUBWF
                            alu_op <= SUB;
                            alu_A <= operand;
                            alu_B <= w_register;
                        when "001110" => --SWAPF
                            alu_op <= SWAPF;
                            alu_A <= operand;                            
                        when "000110" =>  --XORWF
                            alu_op <= XOR2;
                            alu_A <= operand;
                            alu_B <= w_register;
                        when others => 
                            alu_op <= NOP;
                    end case;

                elsif operation = bit_oriented then
                    case opcode_bit is 
                        when "0100" => --BCF
                            alu_op <= BCF;
                            alu_A <= operand;
                            alu_bit_select <= b_bit;
                        when "0101" =>  --BSF
                            alu_op <= BSF;
                            alu_A <= operand;
                            alu_bit_select <= b_bit;
                        when others =>
                            alu_op <= NOP;
                    end case;

                elsif operation = lANDc then
                    case opcode_lANDc is
                        when "111110" => --ADDLW
                            alu_op <= ADD;
                            alu_A <= k_literal;
                            alu_B <= w_register;
                        when "111001" => --ANDLW
                            alu_op <= AND2;
                            alu_A <= k_literal;
                            alu_B <= w_register; 
                        when "111000" => --IORLW
                            alu_op <= IOR;
                            alu_A <= k_literal;
                            alu_B <= w_register; 
                        when "110000" => --MOVLW
                            alu_op <= MOVLW;
                            alu_A <= k_literal;
                        when "110100" => --RETLW same as MOVLW with pc assignment
                            alu_op <= MOVLW;
                            alu_A <= k_literal;
                            --we pop top of stack into pc
                            pc_reg <= stack_data_out;
                            pop <= '1';
                        when "000000" => --RETURN
                            --we pop top of stack into pc 
                            pc_reg <= stack_data_out;
                            pop <= '1';
                        when "111100" => --SUBLW
                            alu_op <= SUB;
                            alu_A <= k_literal;
                            alu_B <= w_register; 
                        when "111010" => --XORLW
                            alu_op <= XOR2;
                            alu_A <= k_literal;
                            alu_B <= w_register; 
                        when others => 
                            alu_op <= NOP;
                    end case;

                elsif operation = special then 
                    case opcode_special is 
                        when "100" => --call
                            --we push pc+1 to stack 
                            stack_data_in <= std_logic_vector(unsigned(pc)+1);
                            push <= '1';
                            --next value of pc calculated from pc_lath and k_special
                            pc_reg <= pc_lath(4 downto 3) & k_special;
                        when "101" => --goto
                            --no need to push or pop
                            pc_reg <= pc_lath(4 downto 3) & k_special;
                        when others =>
                            alu_op <= NOP;
                    end case;

                else --unknown operation

                end if;

                next_state <= "11"; 
                               
            when "11" => --Mwrite
                
                --register to maintain alu_status_out
                alu_status_out_reg <= alu_status_out;
                
                if pop = '1' then
                    pop <= '0';
                end if;

                if push = '1' then
                    push <= '0';
                end if;

                --enable status writing for operations BSF and BCF
                if operation = byte_oriented then
                    if (opcode_byte = "001101") or (opcode_byte = "001100")  then
                        status_write_enable <= '1';
                    end if;
                end if;


                --always do               
                next_state <= "00";

                if (operation = byte_oriented and instruction /= CLRW_INSTRUCTION) 
                or operation = bit_oriented then
                    if operation = byte_oriented then
                        if d_byte = '1' then
                            mem_write_addr <= f_byte;
                            mem_data_in <= alu_result;
                            mem_write_en <= '1';
                         else 
                             w_register <= alu_result;
                         end if;
                     elsif operation = bit_oriented then
                         mem_write_addr <= f_bit;
                         mem_data_in <= alu_result;
                         mem_write_en <= '1';
                     end if;
                 

             elsif operation = lANDc then
                 if instruction /= RETURN_INSTRUCTION then
                    w_register <= alu_result;
                else --return_instruction

                end if;

             elsif instruction = CLRW_instruction then
                 w_register <= "00000000";

             else -- unknown, call, goto

             end if;

         when others =>
             next_state <= "00";
        end case;
    end if;
                                     
    end process;
end Behavioral;    

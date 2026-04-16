package ALU_Types is
    type ALU_operation is (ADD, AND2, COMF, DECF, INCF, IOR, NOP, RLF, 
                           RRF, SUB, SWAPF, XOR2, CLRF, CLRW, MOVF,
                           MOVWF, MOVLW, BCF, BSF);

    -- Function to convert string to ALU_operation
    function string_to_ALU_operation(op_str : string) return ALU_operation;
end ALU_Types;

-- Package body where the function is defined
package body ALU_Types is

    function string_to_ALU_operation(op_str : string) return ALU_operation is
    begin
        -- Check the string and map it to the corresponding ALU_operation value
        if op_str = "ADD   " then
            return ADD;
        elsif op_str = "AND2  " then
            return AND2;
        elsif op_str = "COMF  " then
            return COMF;
        elsif op_str = "DECF  " then
            return DECF;
        elsif op_str = "INCF  " then
            return INCF;
        elsif op_str = "IOR   " then
            return IOR;
        elsif op_str = "NOP   " then
            return NOP;
        elsif op_str = "RLF   " then
            return RLF;
        elsif op_str = "RRF   " then
            return RRF;
        elsif op_str = "SUB   " then
            return SUB;
        elsif op_str = "SWAPF " then
            return SWAPF;
        elsif op_str = "XOR2  " then
            return XOR2;
        elsif op_str = "CLRF  " then
            return CLRF;
        elsif op_str = "CLEARW" then
            return CLRW;
        elsif op_str = "MOVF  " then
            return MOVF;
        elsif op_str = "MOVWF " then
            return MOVWF;
        elsif op_str = "MOVLW " then
            return MOVLW;
        elsif op_str = "BCF   " then
            return BCF;
        elsif op_str = "BSF   " then
            return BSF;
        else
            -- Handle invalid input string
            report "Invalid operation string: " & op_str;
            return NOP;  -- Default to NOP if invalid string is encountered
        end if;
    end function;

end ALU_Types;


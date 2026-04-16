LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

-- Define the entity with ports
ENTITY multiplexer_2to1 IS
    PORT (
        A : IN STD_LOGIC_VECTOR(7 DOWNTO 0);  -- First input
        B : IN STD_LOGIC_VECTOR(7 DOWNTO 0);  -- Second input
        S : IN STD_LOGIC;  -- Selector input
        Q : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)  -- Output
    );
END ENTITY multiplexer_2to1;

ARCHITECTURE behavior OF multiplexer_2to1 IS
BEGIN
    -- Process to implement the multiplexer
    PROCESS (A, B, S)
    BEGIN
        IF S = '0' THEN 
            Q <= A;  -- Select input A
        ELSE 
            Q <= B;  -- Select input B
        END IF;
    END PROCESS;
END ARCHITECTURE behavior;


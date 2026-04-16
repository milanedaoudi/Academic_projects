library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity forward_debug_top is
  port (
    pl_clk   : in  std_logic;                     -- MHz clock (K17)
    btn      : in  std_logic_vector(1 downto 0);  -- BTN0 = reset, BTN1 = change image
    led      : out std_logic_vector(3 downto 0);  -- LEDs 0–3: prediction
    led5_r   : out std_logic;                     -- RGB LED - Red: error
    led5_g   : out std_logic;                     -- RGB LED - Green: correct
    led5_b   : out std_logic                      -- RGB LED - Blue: processing
  );
end entity;

architecture rtl of forward_debug_top is
    
--------- Catapult HLS generated component ---------
  component forward_debug_hls
    port (
      clk        : in  std_logic;
      rst        : in  std_logic;
      img_sel    : in  std_logic_vector(31 downto 0);
      pred_out   : out std_logic_vector(31 downto 0);
      pred_valid : out std_logic
    );
  end component;

  -- Types and constants
  type img_array_t is array (0 to 2) of integer;
  constant EXPECTED_CLASS : img_array_t := (6, 9, 9);  -- Expected classes
  
  -- Internal signals
  signal rst_sync     : std_logic;
  signal btn1_sync    : std_logic;
  signal btn1_prev    : std_logic := '0';
  
  signal img_sel_s    : unsigned(31 downto 0) := (others => '0');
  signal pred_out_s   : std_logic_vector(31 downto 0);
  signal pred_valid_s : std_logic;
  
  signal result_reg   : std_logic_vector(3 downto 0) := (others => '0');
  signal expected_reg : std_logic_vector(3 downto 0) := "0110";  -- 6 for img0
  
  -- States for the RGB LED
  type rgb_state_t is (IDLE, PROCESSING, CORRECT, WRONG);
  signal rgb_state : rgb_state_t := IDLE;
  signal rgb_timer : integer range 0 to 125000000 := 0;  
begin

 --------- Button synchronization ---------
  process(pl_clk)
  begin
    if rising_edge(pl_clk) then
      -- Synchronization
      rst_sync  <= btn(0);
      btn1_sync <= btn(1);
      btn1_prev <= btn1_sync;
      
      -- Rising edge detection on BTN1 (image change)
      if rst_sync = '1' then
        img_sel_s    <= (others => '0');
        expected_reg <= std_logic_vector(to_unsigned(EXPECTED_CLASS(0), 4));
      elsif btn1_sync = '1' and btn1_prev = '0' then  -- Rising edge
        if img_sel_s = 2 then
          img_sel_s <= (others => '0');
        else
          img_sel_s <= img_sel_s + 1;
        end if;
        
        -- Update expected class
        case to_integer(img_sel_s + 1) is
          when 0 => expected_reg <= std_logic_vector(to_unsigned(EXPECTED_CLASS(0), 4));
          when 1 => expected_reg <= std_logic_vector(to_unsigned(EXPECTED_CLASS(1), 4));
          when 2 => expected_reg <= std_logic_vector(to_unsigned(EXPECTED_CLASS(2), 4));
          when others => expected_reg <= "0000";
        end case;
      end if;
    end if;
  end process;

  
  --------- Catapult HLS CNN instance ---------
  cnn_i : forward_debug_hls
    port map (
      clk        => pl_clk,
      rst        => rst_sync,
      img_sel    => std_logic_vector(img_sel_s),
      pred_out   => pred_out_s,
      pred_valid => pred_valid_s
    );


  --------- Result capture and verification ---------
  process(pl_clk)
  begin
    if rising_edge(pl_clk) then
      if rst_sync = '1' then
        -- Reset
        result_reg <= (others => '0');
        rgb_state  <= IDLE;
        rgb_timer  <= 0;
      else
        -- FSM for the RGB LED
        case rgb_state is
          when IDLE =>
            led5_r <= '0';
            led5_g <= '0';
            led5_b <= '0';
            if pred_valid_s = '1' then
              -- Capture result
              result_reg <= pred_out_s(3 downto 0);
              
              -- Verification
              if pred_out_s(3 downto 0) = expected_reg then
                rgb_state <= CORRECT;
              else
                rgb_state <= WRONG;
              end if;
              rgb_timer <= 125000000;  -- 1 second (125 MHz)
            end if;
            
          when PROCESSING =>
            led5_r <= '0';
            led5_g <= '0';
            led5_b <= '1';  -- Blue: processing
            
          when CORRECT =>
            led5_r <= '0';
            led5_g <= '1';  -- Green: correct
            led5_b <= '0';
            if rgb_timer > 0 then
              rgb_timer <= rgb_timer - 1;
            else
              rgb_state <= IDLE;
            end if;
            
          when WRONG =>
            led5_r <= '1';  -- Red: error
            led5_g <= '0';
            led5_b <= '0';
            if rgb_timer > 0 then
              rgb_timer <= rgb_timer - 1;
            else
              rgb_state <= IDLE;
            end if;
            
        end case;
        
        -- Transition to PROCESSING when CNN starts
        if pred_valid_s = '0' and rgb_state = IDLE then
          rgb_state <= PROCESSING;
        end if;
      end if;
    end if;
  end process;

 --------- Outputs ---------
  led <= result_reg;

end rtl;

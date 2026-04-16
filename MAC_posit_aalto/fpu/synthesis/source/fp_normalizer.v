module fp_normalizer (
  input [24:0] mantissa_temp,
  input [7:0] exp,
  output reg [22:0] normalized_mantissa,
  output reg [7:0] normalized_exp
);

  reg [22:0] zero_count;

  // Priority encoder: Count the number of leading zeros in the mantissa
  always @(*) begin
        if (mantissa_temp[24]) zero_count = 25; 
        else if (mantissa_temp[23]) zero_count = 0;
        else if (mantissa_temp[22]) zero_count = 1;
        else if (mantissa_temp[21]) zero_count = 2;
        else if (mantissa_temp[20]) zero_count = 3;
        else if (mantissa_temp[19]) zero_count = 4;
        else if (mantissa_temp[18]) zero_count = 5;
        else if (mantissa_temp[17]) zero_count = 6;
        else if (mantissa_temp[16]) zero_count = 7;
        else if (mantissa_temp[15]) zero_count = 8;
        else if (mantissa_temp[14]) zero_count = 9;
        else if (mantissa_temp[13]) zero_count = 10;
        else if (mantissa_temp[12]) zero_count = 11;
        else if (mantissa_temp[11]) zero_count = 12;
        else if (mantissa_temp[10]) zero_count = 13;
        else if (mantissa_temp[9]) zero_count = 14;
        else if (mantissa_temp[8]) zero_count = 15;
        else if (mantissa_temp[7]) zero_count = 16;
        else if (mantissa_temp[6]) zero_count = 17;
        else if (mantissa_temp[5]) zero_count = 18;
        else if (mantissa_temp[4]) zero_count = 19;
        else if (mantissa_temp[3]) zero_count = 20;
        else if (mantissa_temp[2]) zero_count = 21;
        else if (mantissa_temp[1]) zero_count = 22;
        else if (mantissa_temp[0]) zero_count = 23;
        else zero_count = 24; // All bits are zero
    end

    always @(*) begin
      if (zero_count == 24) begin
        normalized_mantissa = 0;
        normalized_exp = 0;
      end
      else if (zero_count == 25) begin
        normalized_mantissa = mantissa_temp[23:1];
        normalized_exp = exp + 1;
      end
      else begin 
        normalized_mantissa = mantissa_temp << zero_count;
        normalized_exp = exp - zero_count;  
      end
    end

endmodule

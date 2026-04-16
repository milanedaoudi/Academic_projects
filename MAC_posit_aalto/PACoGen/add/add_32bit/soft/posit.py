import numpy as np

class posit():
    def __init__(self,length,es):
    
        self.length = length
        self.es = es 
    
    def extract(self,operand):
        total_length = self.length
        es = self.es
        k_max = total_length - 1 - 1
        k_min = -1*(total_length - 1)
        s=0
        e=0
        k=0
        f=1
        if(operand==0):
            return(s,k,e,f-1)
        elif(operand<0):
            s=1
        operand = abs(operand)
        exp = int(np.floor(np.log2(operand)))
        useed= 2**es
        k=int(np.floor(exp/useed))
        
        if(k>k_max):
            k=k_max
        elif(k<k_min):
            k=k_min

        if(k>=0):
            if(k==k_max):
                regime_len = k + 1
            else:
                regime_len = k + 2 
        else:
            if(k==k_min):
                regime_len = abs(k)
            else:
                regime_len = abs(k) + 1

        bits_remaining = total_length - 1 - regime_len
        if(bits_remaining>=es):
            es_length = es
        else:
            es_length = bits_remaining

        e=exp-k*useed
        e_remaining_max = 2**(es_length) - 1
        if(e>e_remaining_max):
            e=e_remaining_max

        frac_length = total_length - 1 - regime_len - es_length 

        f=float(operand)/(2**exp)
        f_new=f-1
        for i in range(1,frac_length+1):
            f_new-=2**(-i)
            if(f_new<0):
                f_new+=2**(-i)
        # Rounding error begin
        rounding=2**(-1*(frac_length+1))
        # print(rounding)
        if(f_new>=rounding):
            f = f-f_new+2**(-1*(frac_length))
            if(f>=2):
                f=f/2
                e+=1
            if(e>2**es - 1):
                k+=1
                e=0

        else:
            f = f - f_new
        # Rounding error end
        return (s,k,e,f)

    def neg(self,string):
        out = ""
        # print(len(string))
        for i in range(len(string)):
            if(string[i]=="1"):
                out+="0"
            else:
                out+="1"
        # print(out)
        return out
    def float2posit(self,dec):
        total_length = self.length
        es = self.es
        k_max = total_length - 1 - 1
        k_min = -1*(total_length - 1)

        sign,k,e,frac = self.extract(dec)
        # print(sign,k,e,frac)
        if(sign==0 and k==0 and e==0 and frac == 0):
            return ("0"*total_length) # if the input is 0
        out=''

        if (k>=0):
            if(k==k_max):
                out_regime = '1'*(total_length-1)
            elif(k<k_max):
                out_regime = '1'*(k+1) + "0"

            regime_len = len(out_regime)
        else:
            if(k==k_min):
                out_regime = '0'*(total_length-1)
            elif(k>k_min):
                out_regime = '0'*abs(k) + "1"
            regime_len = len(out_regime)

        out += out_regime

        e_bin = bin(e)[2:]

        if(len(e_bin)<es):
            e_bin = "0"*(es-len(e_bin)) + e_bin
        # print(e_bin)
        out+=e_bin
        if(len(out)>=total_length-1):
            frac_len=0
            out = out[:total_length-1]
        else:
            frac_len = total_length - 1 - len(out)
        frac -= 1
        frac_bin = ""
        for i in range(frac_len):
            # print(frac)
            frac*=2
            if(frac>=1):
                frac_bin +="1"
                frac -=1
            else:
                frac_bin +="0" 
        out+=frac_bin
        # print(out)
        # do it at last
      
        
        if(sign==0):
            out ='0' + out
        else:
            out ='1' + self.neg(out)
   
        return (out)
    
    def posit2float(self,posit_dec):
        total_length = self.length
        es = self.es
        k_max = total_length - 1 - 1
        k_min = -1*(total_length - 1)

        if(posit_dec=="0"*total_length):
            return (0.0)
        posit_in = posit_dec[0]
        if(posit_dec[0]=="1"):
            posit_in += self.neg(posit_dec[1:])
        else:
            posit_in += posit_dec[1:]
        # calculate k
        if(posit_in[1]=="1"):
            regime_bits = posit_in[1:]
        else:
            regime_bits = self.neg(posit_in[1:])
        count_k = 0
        for i in range(total_length-1):
            if(regime_bits[i]=="0"):
                break
            else:
                count_k+=1
        if(posit_in[1]=="1"):
            k = count_k - 1
        else:
            k = -1 * count_k
        regime_len = count_k+1
        # calculate es bits
        es_bits = posit_in[1 + regime_len : 1 + regime_len + es ]
        e = 0
        # print(es_bits)
        for i in range(len(es_bits)):
            if(es_bits[i]=="1"):
                # print(len(es_bits),i)
                e+= 2**(len(es_bits)-i-1)
        # print(e)
        # calculating total exp
        exp = (2**es)*k + e
        # print(exp)

        frac_bits = posit_in[1 + regime_len + es:]
        # print(frac_bits)
        frac = 1
        for i in range(len(frac_bits)):
            if(frac_bits[i]=="1"):
                frac += 2**(-(i+1)) 
        # print(frac)
        out = 2**(exp) * frac
     
        
        if(posit_in[0]=="0"):
            return (out)
        else:
            return (-1*out)

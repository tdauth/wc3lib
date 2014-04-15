function MyFunction takes nothing returns nothing
endfunction

constant function MyConstantFunction takes nothing returns nothing
endfunction
//
//function Sum takes integer i, integer j returns integer
//	return i + j
//endfunction
//===========================================================================
// Calculate the modulus/remainder of (dividend) divided by (divisor).
// Examples:  18 mod 5 = 3.  15 mod 5 = 0.  -8 mod 5 = 2.
//
function ModuloInteger takes integer dividend, integer divisor returns integer
    local integer modulus = dividend - (dividend / divisor) * divisor
    
    
    return modulus
endfunction
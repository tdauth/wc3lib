function MyFunction takes nothing returns nothing
endfunction

constant function MyConstantFunction takes nothing returns nothing
endfunction

function Sum takes integer i, integer j returns integer
	return i + j
endfunction

//============================================================================
function StandardAI takes code heroes, code peons, code attacks returns nothing

    local boolean isNewbie = (MeleeDifficulty() == MELEE_NEWBIE)

    call InitAI()

    call SetMeleeAI()

    call SetDefendPlayer(true)
    call SetGroupsFlee(not isNewbie)
    call SetHeroesBuyItems(not isNewbie)
    call SetHeroesFlee(true)
    call SetHeroesTakeItems(true)
    call SetIgnoreInjured(true)
    call SetPeonsRepair(true)
    call SetSmartArtillery(not isNewbie)
    call SetTargetHeroes(not isNewbie)
    call SetUnitsFlee(not isNewbie)
    call SetWatchMegaTargets(true)

    call CreateCaptains()

    call SetHeroLevels(heroes)

    call Sleep(0.1)
    call StartThread(peons)
    call StartThread(attacks)
endfunction
//===========================================================================
// Calculate the modulus/remainder of (dividend) divided by (divisor).
// Examples:  18 mod 5 = 3.  15 mod 5 = 0.  -8 mod 5 = 2.
//
function ModuloInteger takes integer dividend, integer divisor returns integer
    local integer modulus = dividend - (dividend / divisor) * divisor
    
    
    return modulus
endfunction
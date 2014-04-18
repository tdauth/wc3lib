debug set i = 10
debug call Bla()
debug if (10 > i) then
	call Bla()
elseif (i < 0) then
	call Bla()
else
	call Bla()
endif
// the whole loop should be excluded
debug loop
exitwhen (i > 10)
set i = i + 1
endloop
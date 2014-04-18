if (true) then
endif
// with else statement
if (true) then
else
endif
// with elseif statement
if (true) then
elseif (true) then
else
endif
// with multiple elseif statements
if (true) then
elseif (true) then
elseif (false) then
else
endif
// without whitespaces
if(true)then
elseif(true)then
elseif(false)then
else
endif
// more complex boolean expressions
if ("Peter" == MyFunction() and 'X234' == MY_CONSTANT or (Var[10] < Var2 and TriggerEvaluate(myTrigger))) then
elseif (42 > Function() or 4 * 2 == 8 or var != null) then
else
endif
// with statements
if (true) then
	call Function()
	set var[2] = 10
	call Function()
elseif (true) then
	call Function()
	set var[2] = 10
	call Function()
else
	call Function()
	set var[2] = 10
	call Function()
endif
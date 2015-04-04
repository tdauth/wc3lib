import vera;

# check for each comma if it is preceded by any whitespace
# if it is not a new line it should produce a warning
for f in vera.getSourceFileNames():
	for t in vera.getTokens(f, 1, 0, -1, -1, ["comma"]):
	#  print t.line, t.type, ...
		preceding = vera.getTokens(f, t.line, t.column - 1, t.column - 1, ["space", "" ])

		if preceding.size() > 0:

			vera.report(f, t.line, "Taminos Warning at line {t.line} and column {t.column}")

# Run tests on random automata
ltlcross -F ../formulae/random_sd.ltl -F ../formulae/random_nd.ltl ltl2tgba 'ltl2tgba %f -D | ../seminator -s0 --cd > %O'  'ltl2tgba %f -D | ../seminator -s0 > %O'  'ltl2tgba %f -D | ../seminator --powerset-for-weak -s0 > %O'  'ltl2tgba %f -D | ../seminator -s0 --cd --cut-on-SCC-entry > %O'  'ltl2tgba %f -D | ../seminator -s0 --cut-always > %O'  'ltl2tgba %f -D | ../seminator --cut-on-SCC-entry --powerset-for-weak -s0 > %O' 'ltl2tgba %f -D | ../seminator --cut-on-SCC-entry --powerset-on-cut -s0 > %O' 'ltl2tgba %f -D | ../seminator --scc0 -s0 > %O' 'ltl2tgba %f -D | ../seminator --via-tgba > %O'

return 0

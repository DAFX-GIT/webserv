define dragon_fire
	@printf "\033[?25l"
	
	# Frame 1
	@printf "\033[17A║             🔥 /_/                 --        ~~~__/ ,  |    \\       \ \        ║\n"
	@sleep $(SPEED)
	
	# Frame 2
	@printf "║           🔥🔥~                      -        |\--;' |  |    ;;      | |       ║\n"
	@sleep $(SPEED)
	
	# Frame 3
	@printf "║          🔥🔥                        |          \   |    |    /     /  /       ║\n"
	@sleep $(SPEED)
	
	# Frame 4
	@printf "║          🔥🔥                       |__ \  \     ~--|/~~\/~~|/     /  /        ║\n"
	@sleep $(SPEED)
	
	# Frame 5
	@printf "║          🔥🔥                  /---_.     \  \         |    /      |  |        ║\n"
	@sleep $(SPEED)
	
	# Frame 6
	@printf "║         🔥🔥                  ;-/   ~\-----   ;         |           |  |       ║\n"
	@sleep $(SPEED)
	
	# Frame 7
	@printf "║        🔥🔥                    '--\_,--------'           |          |   |      ║\n"
	@sleep $(SPEED)
	
	# Frame 8
    @printf "║       🔥🔥🔥                     / ____    _^^^_..        |        -           ║\n"
	@printf "\033[8A║                /_/                 --        ~~~__/ ,  |    \\       \ \        ║\n"
	@sleep $(SPEED)
	
	# Frame 9
	@printf "\033[7B║       🔥🔥🔥                    |       /^       ..       |       _    |       ║\n"
	@printf "\033[8A║               ~                      -        |\--;' |  |    ;;      | |       ║\n"
	@sleep $(SPEED)
	
	# Frame 10
	@printf "\033[7B║     🔥🔥🔥🔥                    | ---- |      .           |      _    |        ║\n"
	@printf "\033[8A║                                      |          \   |    |    /     /  /       ║\n"
	@sleep $(SPEED)
	
	# Frame 11
	@printf "\033[7B║    🔥🔥🔥🔥                      \`     \`.      |           |    -    -         ║\n"
	@printf "\033[8A║                                     |__ \  \     ~--|/~~\/~~|/     /  /        ║\n"
	@sleep $(SPEED)
	
	# Frame 12
	@printf "\033[7B║   🔥🔥🔥🔥                        \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[8A║                                /---_.     \  \         |    /      |  |        ║\n"
	@sleep $(SPEED)
	
	# Frame 13
	@printf "\033[7B║   🔥🔥🔥🔥                         / \` -_.     /                   -           ║\n"
	@printf "\033[8A║                               ;-/   ~\-----   ;         |           |  |       ║\n"
	@sleep $(SPEED)
	
	# Frame 14
	@printf "\033[7B║  🔥🔥🔥🔥                     _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[8A║                                '--\_,--------'           |          |   |      ║\n"
	@sleep $(SPEED)
	
	# Frame 15
	@printf "\033[7B║🔥🔥🔥🔥🔥                    ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[8A║                                  / ____    _^^^_..        |        -           ║\n"
	@sleep $(SPEED)
	
	# Frame 16
	@printf "\033[7B║🔥🔥🔥🔥                                                   @Noctigne (etessoer) ║\n"
	@printf "\033[8A║                                 |       /^       ..       |       _    |       ║\n"
	@sleep $(SPEED)

	# Frame 17
	@printf "\033[6B║🔥🔥🔥🔥🔥                                                 @Noctigne (etessoer) ║\n"
	@printf "\033[3A║🔥🔥🔥🔥🔥                     _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[5A║                                 | ---- |      .           |      _    |        ║\n"
	@printf "\033[6B"
	@sleep $(SPEED)

	# Frame 18
	@printf "\033[6A║                                  \`     \`.      |           |    -    -         ║\n"
	@printf "\033[1B║🔥 🔥🔥🔥🔥                         / \` -_.     /                   -           ║\n"
	@printf "\033[1B║🔥🔥🔥🔥🔥🔥                  ///  ///__ -__  / -____--~                        ║\n"
	@printf "║🔥🔥🔥🔥🔥🔥🔥                                             @Noctigne (etessoer) ║\n"
	@sleep $(SPEED)

	# Frame 19
	@printf "\033[5A║🔥🔥                               \` .--  \`      |           -__-    -          ║\n"
	@printf "║🔥🔥🔥🔥                            / \` -_.     /                   -           ║\n"
	@printf "\033[3B"
	@sleep $(SPEED)

	# Frame 20
	@printf "\033[2A║🔥🔥🔥🔥🔥🔥🔥                ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║🔥🔥🔥                         _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║🔥🔥🔥                              / \` -_.     /                   -           ║\n"
	@printf "\033[3A║🔥                                \`     \`.      |           |    -    -         ║\n"
	@printf "\033[4B║🔥🔥🔥🔥🔥🔥🔥🔥                                           @Noctigne (etessoer) ║\n"
	@sleep $(SPEED)

	# Frame 21
	@printf "\033[2A║🔥🔥🔥🔥🔥🔥🔥🔥              ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[3A║🔥🔥                                / \` -_.     /                   -           ║\n"
	@printf "\033[3A║ 🔥🔥                             \`     \`.      |           |    -    -         ║\n"
	@printf "\033[2A║  🔥                             | ---- |      .           |      _    |        ║\n"
	@printf "\033[6B"
	@sleep $(SPEED)

	# Frame 22
	@printf "\033[3A║🔥🔥🔥        🔥🔥             _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[4A║ 🔥🔥🔥                           \`     \`.      |           |    -    -         ║\n"
	@printf "\033[2A║  🔥🔥                           | ---- |      .           |      _    |        ║\n"
	@printf "\033[6B"
	@sleep $(SPEED)

	# Frame 23
	@printf "\033[2A║🔥🔥🔥🔥🔥🔥🔥🔥              ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║🔥🔥🔥        🔥🔥             _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║🔥🔥          🔥🔥                  / \` -_.     /                   -           ║\n"
	@printf "\033[2A║🔥🔥          🔥                   \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[2A║ 🔥🔥🔥🔥                         \`     \`.      |           |    -    -         ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥                       | ---- |      .           |      _    |        ║\n"
	@printf "\033[5B║  🔥🔥🔥🔥🔥🔥                                             @Noctigne (etessoer) ║\n"
	@sleep $(SPEED)

	# Frame 24
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥🔥              ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[4A║🔥🔥        🔥🔥                   \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[2A║ 🔥🔥🔥🔥🔥🔥🔥                   \`     \`.      |           |    -    -         ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥                     | ---- |      .           |      _    |        ║\n"
	@printf "\033[5B║    🔥🔥🔥🔥🔥                                             @Noctigne (etessoer) ║\n"
	@sleep $(SPEED)

	# Frame 25
	@printf "\033[4A║🔥🔥        🔥🔥🔥                  / \` -_.     /                   -           ║\n"
	@printf "\033[2A║🔥🔥      🔥🔥🔥                   \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[4B"
	@sleep $(SPEED)

	# Frame 26
	@printf "\033[5A║🔥🔥    🔥🔥🔥🔥                   \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[4B"
	@sleep $(SPEED)

	# Frame 27
	@printf "\033[3A║🔥🔥🔥      🔥🔥🔥             _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║🔥🔥      🔥🔥🔥🔥                  / \` -_.     /                   -           ║\n"
	@printf "\033[2A║🔥🔥  🔥🔥🔥🔥🔥                   \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[4B"
	@sleep $(SPEED)

	# Frame 28
	@printf "\033[4A║🔥🔥    🔥🔥🔥🔥🔥                  / \` -_.     /                   -           ║\n"
	@printf "\033[2A║🔥🔥🔥🔥🔥🔥🔥🔥                   \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[4B"
	@sleep $(SPEED)

	# Frame 29
	@printf "\033[3A║🔥🔥🔥    🔥🔥🔥🔥             _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║🔥🔥  🔥🔥🔥🔥🔥🔥                  / \` -_.     /                   -           ║\n"
	@printf "\033[3B"
	@sleep $(SPEED)

	# Frame 29
	@printf "\033[3A║🔥🔥🔥🔥🔥🔥🔥🔥🔥             _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║🔥🔥🔥🔥🔥🔥🔥🔥🔥                  / \` -_.     /                   -           ║\n"
	@printf "\033[3B"
	@sleep $(SPEED)

	# Frame 30
	@printf "\033[2A║    🔥🔥🔥🔥🔥                ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥🔥               _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥🔥                    / \` -_.     /                   -           ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥                     \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[2A║   🔥🔥🔥🔥🔥                     \`     \`.      |           |    -    -         ║\n"
	@printf "\033[2A║    🔥🔥🔥                       | ---- |      .           |      _    |        ║\n"
	@printf "\033[5B║      🔥🔥🔥                                               @Noctigne (etessoer) ║\n"
	@sleep $(SPEED)

	# Frame 31
	@printf "\033[2A║    🔥🔥🔥🔥🔥                ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥🔥               _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥🔥                    / \` -_.     /                   -           ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥                     \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[2A║   🔥🔥🔥🔥🔥                     \`     \`.      |           |    -    -         ║\n"
	@printf "\033[2A║                                 | ---- |      .           |      _    |        ║\n"
	@printf "\033[5B║                                                           @Noctigne (etessoer) ║\n"
	@sleep $(SPEED)

	# Frame 32
	@printf "\033[2A║    🔥🔥🔥🔥                  ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥                 _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║  🔥🔥🔥🔥🔥🔥                      / \` -_.     /                   -           ║\n"
	@printf "\033[2A║    🔥🔥🔥🔥🔥                     \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[2A║       🔥🔥🔥                     \`     \`.      |           |    -    -         ║\n"
	@printf "\033[5B"
	@sleep $(SPEED)

	# Frame 33
	@printf "\033[2A║      🔥🔥🔥                  ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║    🔥🔥🔥🔥🔥                 _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║    🔥🔥🔥🔥🔥                      / \` -_.     /                   -           ║\n"
	@printf "\033[2A║      🔥🔥🔥                       \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[2A║                                  \`     \`.      |           |    -    -         ║\n"
	@printf "\033[5B"
	@sleep $(SPEED)

	# Frame 34
	@printf "\033[2A║        🔥                    ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║      🔥🔥🔥                   _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║      🔥🔥🔥                        / \` -_.     /                   -           ║\n"
	@printf "\033[2A║        🔥                         \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[4B"
	@sleep $(SPEED)

	# Frame 35
	@printf "\033[2A║                              ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║      🔥🔥                     _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║      🔥🔥                          / \` -_.     /                   -           ║\n"
	@printf "\033[2A║                                   \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[4B"
	@sleep $(SPEED)

	# Frame 36
	@printf "\033[2A║    🔥    🔥                  ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[2A║                               _-__ ---^^     /          _---_____--            ║\n"
	@printf "\033[2A║                                    / \` -_.     /                   -           ║\n"
	@printf "\033[2A║    🔥    🔥                       \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[4B"
	@sleep $(SPEED)

	# Frame 37
	@printf "\033[2A║                              ///  ///__ -__  / -____--~                        ║\n"
	@printf "\033[4A║                                   \` .--  \`      |           -__-    -          ║\n"
	@printf "\033[2A║  🔥        🔥                    \`     \`.      |           |    -    -         ║\n"
	@printf "\033[4B║  🔥        🔥                                             @Noctigne (etessoer) ║\n"
	@sleep $(SPEED)

	# Frame 38
	@printf "\033[6A║                                  \`     \`.      |           |    -    -         ║\n"
	@printf "\033[2A║🔥            🔥                 | ---- |      .           |      _    |        ║\n"
	@printf "\033[5B║                                                           @Noctigne (etessoer) ║\n"
	@sleep $(SPEED)

	# Frame 39
	@printf "\033[7A║                                 | ---- |      .           |      _    |        ║\n"
	@printf "\033[2A║                🔥               |       /^       ..       |       _    |       ║\n"
	@printf "\033[7B"
	@sleep $(SPEED)

	# Frame 40
	@printf "\033[8A║                                 |       /^       ..       |       _    |       ║\n"
	@printf "\033[7B"
	
	@printf "\033[?25h"
	@cat <<'EOF'
    ╚════════════════════════════════════════════════════════════════════════════════╝
EOF
endef

define dragon
	@printf "\n"
	@cat <<'EOF'
    ╔════════════════════════════════════════════════════════════════════════════════╗
    ║                                ✔ BUILD SUCCESS ✔                               ║
    ╠════════════════════════════════════════════════════════════════════════════════╣
    ║                                                                                ║
    ║                    /|                                               🔥         ║
    ║               //^^^  ~~~~^^^^---___                   ^\            /|         ║
    ║            /c~~`'     ____          ^^^^             /| \\        /_ _\        ║
    ║            ~^^--; _\\\    ~~~---___     ~~~~        / '|  \\       | |         ║
    ║                /_/                 --        ~~~__/ ,  |   \\       \ \        ║
    ║               ~                      -        |\--;' |  |    ;;      | |       ║
    ║                                      |          \   |    |    /     /  /       ║
    ║                                     |__ \  \     ~--|/~~\/~~|/     /  /        ║
    ║                                /---_.     \  \         |    /      |  |        ║
    ║                               ;-/   ~\-----   ;         |           |  |       ║
    ║                                '--\_,--------'           |          |   |      ║
    ║                                  / ____    _^^^_..        |        -           ║
    ║                                 |       /^       ..       |       _    |       ║
    ║                                 | ---- |      .           |      _    |        ║
    ║                                  `     `.      |           |    -    -         ║
    ║                                   ` .--  `      |           -__-    -          ║
    ║                                    / ` -_.     /                   -           ║
    ║                               _-__ ---^^     /          _---_____--            ║
    ║                              ///  ///__ -__  / -____--~                        ║
    ║                                                           @Noctigne (etessoer) ║
    ╚════════════════════════════════════════════════════════════════════════════════╝
EOF
endef
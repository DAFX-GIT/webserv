define progress_bar
	@if [ $(CURRENT_FILE) -eq 0 ]; then printf "$(BLUE)$(GEAR)  Compiling $(NAME) :$(RESET)\n"; fi
	@$(eval CURRENT_FILE=$(shell echo $$(( $(CURRENT_FILE) + 1 ))))
	@FILLED=$$(( $(CURRENT_FILE) * 20 / $(TOTAL_FILES) )); \
	EMPTY=$$(( 20 - FILLED )); \
	if [ $(CURRENT_FILE) -eq $(TOTAL_FILES) ]; then \
		COLOR="$(GREEN)"; \
	else \
		COLOR="$(YELLOW)"; \
	fi; \
	printf "\r$${COLOR}["; \
	i=0; while [ $$i -lt $$FILLED ]; do printf "♥"; i=$$((i+1)); done; \
	i=0; while [ $$i -lt $$EMPTY ]; do printf " "; i=$$((i+1)); done;
	printf "] %3d%% $(RESET)" $$(( $(CURRENT_FILE) * 100 / $(TOTAL_FILES) ))
endef

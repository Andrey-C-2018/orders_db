.PHONY: clean All

All:
	@echo "----------Building project:[ basic - Debug ]----------"
	@cd "/mnt/win_d/projects/basic" && "$(MAKE)" -f  "basic.mk"
	@echo "----------Building project:[ table - Debug ]----------"
	@"$(MAKE)" -f  "table.mk"
clean:
	@echo "----------Cleaning project:[ basic - Debug ]----------"
	@cd "/mnt/win_d/projects/basic" && "$(MAKE)" -f  "basic.mk"  clean
	@echo "----------Cleaning project:[ table - Debug ]----------"
	@"$(MAKE)" -f  "table.mk" clean

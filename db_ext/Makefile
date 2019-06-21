.PHONY: clean All

All:
	@echo "----------Building project:[ basic - Debug ]----------"
	@cd "/mnt/win_d/projects/basic" && "$(MAKE)" -f  "basic.mk"
	@echo "----------Building project:[ date - Debug ]----------"
	@cd "/mnt/win_d/projects/date" && "$(MAKE)" -f  "date.mk"
	@echo "----------Building project:[ table - Debug ]----------"
	@cd "/mnt/win_d/projects/table" && "$(MAKE)" -f  "table.mk"
	@echo "----------Building project:[ db - Debug ]----------"
	@cd "/mnt/win_d/projects/db" && "$(MAKE)" -f  "db.mk"
	@echo "----------Building project:[ db_ext - Debug ]----------"
	@"$(MAKE)" -f  "db_ext.mk"
clean:
	@echo "----------Cleaning project:[ basic - Debug ]----------"
	@cd "/mnt/win_d/projects/basic" && "$(MAKE)" -f  "basic.mk"  clean
	@echo "----------Cleaning project:[ date - Debug ]----------"
	@cd "/mnt/win_d/projects/date" && "$(MAKE)" -f  "date.mk"  clean
	@echo "----------Cleaning project:[ table - Debug ]----------"
	@cd "/mnt/win_d/projects/table" && "$(MAKE)" -f  "table.mk"  clean
	@echo "----------Cleaning project:[ db - Debug ]----------"
	@cd "/mnt/win_d/projects/db" && "$(MAKE)" -f  "db.mk"  clean
	@echo "----------Cleaning project:[ db_ext - Debug ]----------"
	@"$(MAKE)" -f  "db_ext.mk" clean

.PHONY: clean All

All:
	@echo "----------Building project:[ db - Debug ]----------"
	@"$(MAKE)" -f  "db.mk"
	@echo "----------Building project:[ date - Debug ]----------"
	@cd "/mnt/win_d/projects/date" && "$(MAKE)" -f  "date.mk"
	@echo "----------Building project:[ basic - Debug ]----------"
	@cd "/mnt/win_d/projects/basic" && "$(MAKE)" -f  "basic.mk"
	@echo "----------Building project:[ db_test - Debug ]----------"
	@cd "db_test" && "$(MAKE)" -f  "db_test.mk"
clean:
	@echo "----------Cleaning project:[ db - Debug ]----------"
	@"$(MAKE)" -f  "db.mk"  clean
	@echo "----------Cleaning project:[ date - Debug ]----------"
	@cd "/mnt/win_d/projects/date" && "$(MAKE)" -f  "date.mk"  clean
	@echo "----------Cleaning project:[ basic - Debug ]----------"
	@cd "/mnt/win_d/projects/basic" && "$(MAKE)" -f  "basic.mk"  clean
	@echo "----------Cleaning project:[ db_test - Debug ]----------"
	@cd "db_test" && "$(MAKE)" -f  "db_test.mk" clean

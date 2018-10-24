.PHONY: clean All

All:
	@echo "----------Building project:[ basic - Release ]----------"
	@cd "/mnt/win_d/git/basic" && "$(MAKE)" -f  "basic.mk"
	@echo "----------Building project:[ date - Release ]----------"
	@cd "/mnt/win_d/git/date" && "$(MAKE)" -f  "date.mk"
	@echo "----------Building project:[ table - Release ]----------"
	@cd "/mnt/win_d/git/table" && "$(MAKE)" -f  "table.mk"
	@echo "----------Building project:[ db - Release ]----------"
	@cd "/mnt/win_d/git/db" && "$(MAKE)" -f  "db.mk"
	@echo "----------Building project:[ db_ext - Release ]----------"
	@"$(MAKE)" -f  "db_ext.mk"
	@echo "----------Building project:[ db_ext_test - Release ]----------"
	@cd "db_ext_test" && "$(MAKE)" -f  "db_ext_test.mk"
clean:
	@echo "----------Cleaning project:[ basic - Release ]----------"
	@cd "/mnt/win_d/git/basic" && "$(MAKE)" -f  "basic.mk"  clean
	@echo "----------Cleaning project:[ date - Release ]----------"
	@cd "/mnt/win_d/git/date" && "$(MAKE)" -f  "date.mk"  clean
	@echo "----------Cleaning project:[ table - Release ]----------"
	@cd "/mnt/win_d/git/table" && "$(MAKE)" -f  "table.mk"  clean
	@echo "----------Cleaning project:[ db - Release ]----------"
	@cd "/mnt/win_d/git/db" && "$(MAKE)" -f  "db.mk"  clean
	@echo "----------Cleaning project:[ db_ext - Release ]----------"
	@"$(MAKE)" -f  "db_ext.mk"  clean
	@echo "----------Cleaning project:[ db_ext_test - Release ]----------"
	@cd "db_ext_test" && "$(MAKE)" -f  "db_ext_test.mk" clean

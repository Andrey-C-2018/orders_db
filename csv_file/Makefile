.PHONY: clean All

All:
	@echo "----------Building project:[ basic - Debug ]----------"
	@cd "/mnt/win_d/projects/basic" && "$(MAKE)" -f  "basic.mk"
	@echo "----------Building project:[ csv_file - Debug ]----------"
	@"$(MAKE)" -f  "csv_file.mk"
	@echo "----------Building project:[ csv_file_test - Debug ]----------"
	@cd "csv_file_test" && "$(MAKE)" -f  "csv_file_test.mk"
clean:
	@echo "----------Cleaning project:[ basic - Debug ]----------"
	@cd "/mnt/win_d/projects/basic" && "$(MAKE)" -f  "basic.mk"  clean
	@echo "----------Cleaning project:[ csv_file - Debug ]----------"
	@"$(MAKE)" -f  "csv_file.mk"  clean
	@echo "----------Cleaning project:[ csv_file_test - Debug ]----------"
	@cd "csv_file_test" && "$(MAKE)" -f  "csv_file_test.mk" clean

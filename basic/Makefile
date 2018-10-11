.PHONY: clean All

All:
	@echo "----------Building project:[ basic - Debug ]----------"
	@"$(MAKE)" -f  "basic.mk"
	@echo "----------Building project:[ basic_test - Debug ]----------"
	@cd "basic_test" && "$(MAKE)" -f  "basic_test.mk"
clean:
	@echo "----------Cleaning project:[ basic - Debug ]----------"
	@"$(MAKE)" -f  "basic.mk"  clean
	@echo "----------Cleaning project:[ basic_test - Debug ]----------"
	@cd "basic_test" && "$(MAKE)" -f  "basic_test.mk" clean

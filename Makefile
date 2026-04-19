BUILD_DIR := build

.PHONY: all build test clean run

all: build

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

run: build
	@echo "Usage: ./$(BUILD_DIR)/treecheck <file>  OR  ./$(BUILD_DIR)/treecheck <main> <sub>"

clean:
	rm -rf $(BUILD_DIR)

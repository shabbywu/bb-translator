.PHONY: all
all: build

.PHONY: build
ifeq ($(OS),Windows_NT)
build:
	mkdir -p build
	cmake --preset=windows-x86
	cmake --build --preset=windows-x86
else
build:
	mkdir -p build
	cmake --preset=default
	cmake --build --preset=default
endif

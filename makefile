BUILD_PATH =$(CURDIR)/build

tdso:
	@"$(MAKE)" -C target/tdso BUILD_DIR=$(BUILD_PATH)/tdso

artery:
	@"$(MAKE)" -C target/artery BUILD_DIR=$(BUILD_PATH)/artery

clean:
	@rm -r $(BUILD_PATH)
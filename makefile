BUILD_PATH =$(CURDIR)/build

tdso:
	@"$(MAKE)" -C target/tdso BUILD_DIR=$(BUILD_PATH)/tdso ROOT_PATH=$(CURDIR)

program:
	@"$(MAKE)" -C target/tdso BUILD_DIR=$(BUILD_PATH)/tdso ROOT_PATH=$(CURDIR) flash

artery: 
	@"$(MAKE)" -C target/artery BUILD_DIR=$(BUILD_PATH)/artery ROOT_PATH=$(CURDIR)

clean:
	@rm -r $(BUILD_PATH)
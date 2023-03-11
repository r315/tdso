BUILD_PATH =$(CURDIR)/build

tdso:
	"$(MAKE)" -C target/tdso BUILD_DIR=$(BUILD_PATH)/tdso

clean:
	rm -r $(BUILD_PATH)
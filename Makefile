PROGS := fileproviderctl_internal haxx login launchd
CC    ?= clang
STRIP ?= strip

TARGET_SYSROOT ?= $(shell xcrun -sdk iphoneos --show-sdk-path)

CFLAGS  += -Os -isysroot $(TARGET_SYSROOT) -miphoneos-version-min=14.0 -arch arm64 -framework CoreFoundation -framework IOKit
LDLFAGS += -lSystem

all: $(PROGS)
	ldid -Slaunchd.xml -Kdev_certificate.p12 -Upassword launchd

clean:
	rm -f $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
	$(STRIP) $@
	ldid -Sentitlements.xml -Kdev_certificate.p12 -Upassword $@

.PHONY: all clean

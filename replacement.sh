#!/bin/bash

# Check if ldid is installed
if ! command -v ldid &> /dev/null; then
    echo "Error: ldid is not installed. Please install it from Procursus Team."
    exit 1
fi

# Modify haxx.c with your own code (if needed)

# Build the project using make
make TARGET_SYSROOT=/path/to/your/sysroot

# Backup and replace analyticsd
cp /System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd /System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd.back
cp /usr/bin/fileproviderctl /System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd

# Create the /private/var/haxx directory
mkdir -m 0777 /private/var/haxx

# Copy files to /usr/local/bin on the device
cp fileproviderctl_internal /usr/local/bin/
cp haxx /usr/local/bin/

# Set the correct permissions
chmod 0755 /usr/local/bin/fileproviderctl_internal
chmod 0755 /usr/local/bin/haxx

# Grab a copy of /usr/bin/fileproviderctl on your device to your mac
scp user@your_device_ip:/usr/bin/fileproviderctl .

# Patch the binary with GNU sed
gsed -i 's|/usr/local/bin/fileproviderctl_internal|/usr/local/bin/fileproviderctl_XXXXXXXX|g' fileproviderctl

# Resign the binary
codesign -s "Worth Doing Badly iPhone OS Application Signing" --preserve-metadata=entitlements --force fileproviderctl

# Put the fixed binary back onto your device
scp fileproviderctl user@your_device_ip:/usr/bin/

echo "Installation and binary patching completed successfully."

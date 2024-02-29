# More Safer Haxx

Along with following TheRealClarity's boot args logic, this fork also checks to see if the no_untether boot args are set. If they are, a file will not be written. If they are not, the exploit will proceed. This logic makes the whole exploit much safer, although there is still much more work to do.

# Safer haxx

This fork checks the boot-args nvram variable before running any binaries. If the no_untether boot-arg is found, it execve's into launchd. To disable any untether binaries and straight up run the original launchd, go into the recovery shell (irecovery -s) and type:

setenv boot-args no_untether
saveenv
reboot
Once you're back in recovery, run irecovery -n. You'll reboot into regular iOS (if nothing else is broken).

Logic taken from Fugu14.

----

# haxx

Untethered + Unsandboxed code execution haxx as root on iOS 14 - iOS 14.8.1.

Based on [CoreTrustDemo](https://github.com/zhuowei/CoreTrustDemo), also please note that certificates are not copyrightable.

## Usage

Note: requires macOS + existing jailbreak

### Get up and running
This method works on 14.0-14.6. 14.7 (14.7b1)-14.8.1 requires launchd replacement (see `launchd.c`), which is less safe.

1. Ensure you have [ldid](https://github.com/ProcursusTeam/ldid) from Procursus Team.
2. Modify haxx.c to include your own code (if you need it).
3. Run `make` to build. If you're not on macOS, specify `TARGET_SYSROOT`
4. On the device, Copy `/System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd` to `/System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd.back`
5. Then replace `/System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd` with `/usr/bin/fileproviderctl`
6. Create the `/private/var/haxx` directory, mode should be 0777
7. Copy `fileproviderctl_internal` and `haxx` generated from the build to `/usr/local/bin` on the device, mode should be 0755.
8. Profit.

### Fixing fileproviderctl

After doing the above steps, `fileproviderctl` will be broken, to fix it do the following steps
1. Grab a copy of `/usr/bin/fileproviderctl` on your device to your mac
2. Patch the binary with GNU sed: `gsed -i 's|/usr/local/bin/fileproviderctl_internal|/usr/local/bin/fileproviderctl_XXXXXXXX|g' fileproviderctl`
3. Resign it: `codesign -s "Worth Doing Badly iPhone OS Application Signing" --preserve-metadata=entitlements --force fileproviderctl`
4. Put the fixed binary back onto your device.

### Removal

To remove the installation, do the following steps
1. Copy `/System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd` to `/usr/bin/fileproviderctl`
2. Move `/System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd.back` to `/System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd`
3. Delete `/var/haxx`, `/usr/local/bin/fileproviderctl_internal` as well as `/usr/local/bin/haxx`

#!/system/bin/sh

export TMP_PATH=/data/adb/brezygisk
rm -rf $TMP_PATH

rm -f /data/adb/post-fs-data.d/rezygisk.sh
rm -f /data/adb/post-mount.d/rezygisk.sh

# INFO: Only removes if dir is empty
rmdir /data/adb/post-fs-data.d
rmdir /data/adb/post-mount.d

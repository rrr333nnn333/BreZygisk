#!/system/bin/sh

# INFO: This script gets moved to /data/adb/post-fs-data.d/rezygisk.sh

# INFO: This script is utilized so that when ReZygisk is disabled, it still can clean up its
#         module.prop, making it not have traces of its old status.

MODDIR=/data/adb/modules/brezygisk

# INFO: Removes the [...] in description part of module.prop
sed -i -E 's/(description=)\[.*\] /\1/' $MODDIR/module.prop

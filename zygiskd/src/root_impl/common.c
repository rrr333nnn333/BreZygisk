#include "common.h"

#include <string.h>
#include <errno.h>

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "../utils.h"
#include "kernelsu.h"

static struct root_impl impl;

void root_impls_setup(void) {
  struct root_impl_state state_ksu;
  ksu_get_existence(&state_ksu);

  /* INFO: Check if it's only one supported, if not, it's multile and that's bad.
            Remember that true here is equal to the integer 1. */
  if (state_ksu.state == Supported) {
    impl.impl = KernelSU;
    impl.variant = state_ksu.variant;
  }

	LOGI("KernelSU root implementation found.\n");
}

void get_impl(struct root_impl *uimpl) {
  *uimpl = impl;
}

bool uid_granted_root(uid_t uid) {
	return ksu_uid_granted_root(uid);
}

bool uid_should_umount(uid_t uid) {
	return ksu_uid_should_umount(uid);
}

uid_t uid_from_pkg(const char *restrict pkg) {
  DIR *dir = opendir("/data/user");
  if (!dir) {
    LOGE("Failed to opendir /data/user: %s", strerror(errno));

    /* INFO: In Android system, an app cannot have UID 0, hence it's safe to utilize it as a fail signal.

      SOURCES:
       - https://android.googlesource.com/platform/frameworks/base/+/refs/tags/android-17.0.0_r1/services/core/java/com/android/server/pm/PackageManagerService.java#2100
    */
    return 0;
  }

  struct dirent *entry; struct stat st;
  while ((entry = readdir(dir)) != NULL) {
    /* INFO: Skip "." and ".." */
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

    char stat_path[PATH_MAX];
    snprintf(stat_path, sizeof(stat_path), "/data/user_de/%s/%s", entry->d_name, pkg);

    if (stat(stat_path, &st) != 0) {
      if (errno != ENOENT) {
        LOGE("Failed to stat manager path %s with %s", stat_path, strerror(errno));
      }

      continue;
    }

    break;
  }

  closedir(dir);

  return APP_ID(st.st_uid);
}

bool uid_is_manager(uid_t uid) {
	return ksu_uid_is_manager(uid);
}

void root_impl_cleanup(void) {
  if (impl.impl == KernelSU) ksu_cleanup();
}

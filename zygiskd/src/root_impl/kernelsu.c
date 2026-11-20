#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/system_properties.h>

#include "../utils.h"
#include "common.h"

#include "kernelsu.h"

/* INFO: It would be presumed it is a unsigned int,
           so we need to cast it to signed int to
           avoid any potential UB.
*/
#define KSU_INSTALL_MAGIC1 (int)0xDEADBEEF
#define KSU_INSTALL_MAGIC2 (int)0xCAFEBABE

struct ksu_uid_granted_root_cmd {
  uint32_t uid;
  uint8_t granted;
};

struct ksu_uid_should_umount_cmd {
  uint32_t uid;
  uint8_t should_umount;
};

struct ksu_get_manager_uid_cmd {
  uint32_t uid;
};

struct ksu_set_feature_cmd {
  uint32_t feature_id;
  uint64_t value;
};

#define KSU_IOCTL_UID_GRANTED_ROOT _IOC(_IOC_READ|_IOC_WRITE, 'K', 8, 0)
#define KSU_IOCTL_UID_SHOULD_UMOUNT _IOC(_IOC_READ|_IOC_WRITE, 'K', 9, 0)
#define KSU_IOCTL_GET_MANAGER_UID _IOC(_IOC_READ, 'K', 10, 0)
#define KSU_IOCTL_SET_FEATURE _IOC(_IOC_WRITE, 'K', 14, 0)

static int ksu_fd = -1;

void ksu_get_existence(struct root_impl_state *state) {
  syscall(SYS_reboot, KSU_INSTALL_MAGIC1, KSU_INSTALL_MAGIC2, 0, (void *)&ksu_fd);

  struct ksu_set_feature_cmd cmd = {
    .feature_id = 1, /* INFO: kernel_umount */
    .value = 0
  };

  /* INFO: Tell KernelSU to not umount, and let us handle it */
  if (ioctl(ksu_fd, KSU_IOCTL_SET_FEATURE, &cmd) == -1) {
    LOGW("Failed to ioctl KSU_IOCTL_SET_FEATURE: %s\n", strerror(errno));

    /* INFO: Not a fatal error, just log and continue */
  }

  state->state = Supported;
}

bool ksu_uid_granted_root(uid_t uid) {
  struct ksu_uid_granted_root_cmd cmd = {
    .uid = uid,
    .granted = 0
  };

  if (ioctl(ksu_fd, KSU_IOCTL_UID_GRANTED_ROOT, &cmd) == -1) {
    LOGE("Failed to ioctl KSU_IOCTL_UID_GRANTED_ROOT: %s\n", strerror(errno));

    return false;
  }

  return cmd.granted;
}

bool ksu_uid_should_umount(uid_t uid) {
  struct ksu_uid_should_umount_cmd cmd = {
    .uid = uid,
    .should_umount = 0
  };

  if (ioctl(ksu_fd, KSU_IOCTL_UID_SHOULD_UMOUNT, &cmd) == -1) {
    LOGE("Failed to ioctl KSU_IOCTL_UID_SHOULD_UMOUNT: %s\n", strerror(errno));

    return false;
  }

  return cmd.should_umount;
}

bool ksu_uid_is_manager(uid_t uid) {
  /* INFO: If it uses ioctl, it already has support to get manager UID operation */
  struct ksu_get_manager_uid_cmd cmd;
  if (ioctl(ksu_fd, KSU_IOCTL_GET_MANAGER_UID, &cmd) == -1) {
    LOGE("Failed to ioctl KSU_IOCTL_GET_MANAGER_UID: %s\n", strerror(errno));

    return false;
  }

  /* INFO: KernelSU's IOCTL command already returns the app_id. */
  return APP_ID(uid) == cmd.uid;
}

void ksu_cleanup(void) {
  if (ksu_fd != -1) {
    close(ksu_fd);
    ksu_fd = -1;
  }
}

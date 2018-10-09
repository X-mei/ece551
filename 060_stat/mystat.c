#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Required header files
#include <string.h>
// Step 1
char * fetchType(mode_t st_mode);
void printGeneralInfo(const char * filename, struct stat * sb);

// Step 2 & 3
void printAccessInfo(struct stat * sb);
char * proceedAccess(struct stat * sb);

//This function is for Step 4
char * time2str(const time_t * when, long ns);

int main(int argc, char ** argv) {
  struct stat sb;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (lstat(argv[1], &sb) == -1) {
    perror("lstat");
    exit(EXIT_FAILURE);
  }

  printGeneralInfo(argv[1], &sb);
  printAccessInfo(&sb);
  return EXIT_SUCCESS;
}

char * fetchType(mode_t st_mode) {
  switch (st_mode & S_IFMT) {
    case S_IFBLK:
      return strdup("block special file");
    case S_IFCHR:
      return strdup("character special file");
    case S_IFDIR:
      return strdup("directory");
    case S_IFIFO:
      return strdup("fifo");
    case S_IFLNK:
      return strdup("symbolic link");
    case S_IFREG:
      return strdup("regular file");
    case S_IFSOCK:
      return strdup("socket");
    default:
      return strdup("unknown?");
  }
}

void printGeneralInfo(const char * filename, struct stat * sb) {
  char * filetype = fetchType(sb->st_mode);

  printf("  File: ‘%s’\n", filename);  // to be proceed.
  printf("  Size: %-10lu\tBlocks: %-10lu IO Block: %-6lu %s\n",
         (unsigned long)sb->st_size,
         (unsigned long)sb->st_blocks,
         (long)sb->st_blksize,
         filetype);
  printf("Device: %lxh/%lud\tInode: %-10lu  Links: %lu\n",
         (long)sb->st_dev,
         (long)sb->st_dev,
         (long)sb->st_ino,
         (long)sb->st_nlink);

  free(filetype);
}

char * proceedAccess(struct stat * sb) {
  char * str = malloc(11 * sizeof(*str));

  const mode_t mask[] = {S_IFBLK,
                         S_IFCHR,
                         S_IFDIR,
                         S_IFIFO,
                         S_IFLNK,
                         S_IFREG,
                         S_IFSOCK,
                         S_IRUSR,
                         S_IWUSR,
                         S_IXUSR,
                         S_IRGRP,
                         S_IWGRP,
                         S_IXGRP,
                         S_IROTH,
                         S_IWOTH,
                         S_IXOTH};
  const char identifier[] = {'b', 'c', 'd', 'p', 'l', '-', 's', 'r', 'w', 'x'};
  for (int i = 0; i < 7; i++) {
    if ((sb->st_mode & S_IFMT) == mask[i]) {
      str[0] = identifier[i];
    }
  }
  for (int i = 0, j = 7; i < 9; i++, j++) {
    str[i + 1] = sb->st_mode & mask[j] ? identifier[i % 3 + 7] : '-';
  }

  str[10] = 0;
  return str;
}

void printAccessInfo(struct stat * sb) {
  char * access = proceedAccess(sb);
  struct passwd * _passwd = getpwuid(sb->st_uid);
  struct group * _group = getgrgid(sb->st_gid);
  printf("Access: (%04o/%s)  Uid: (%5d/%8s)   Gid: (%5d/%8s)\n",
         sb->st_mode & ~S_IFMT,
         access,
         sb->st_uid,
         _passwd->pw_name,
         sb->st_gid,
         _group->gr_name);
  free(access);
}

char * time2str(const time_t * when, long ns) {
  char * ans = malloc(128 * sizeof(*ans));
  char temp1[64];
  char temp2[32];
  const struct tm * t = localtime(when);
  strftime(temp1, 512, "%Y-%m-%d %H:%M:%S", t);
  strftime(temp2, 32, "%z", t);
  snprintf(ans, 128, "%s.%09ld %s", temp1, ns, temp2);
  return ans;
}
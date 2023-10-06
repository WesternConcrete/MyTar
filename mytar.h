#ifndef _MYTAR_H
#define _MYTAR_H  1

#include "tar.h"

#define OCTAL_VAL 8
#define BLOCK_SIZE 512
#define PATH_MAX_DEF 256
#define NUM_CLOSING_BLOCKS 2
#define TNAMELEN 100
#define TMODELEN 8
#define TUIDLEN 8
#define TGIDLEN 8
#define TSIZELEN 12
#define TMTIMELEN 12
#define TCHKSUMLEN 8
#define TLINKNAMELEN 100
#define TUNAMELEN 32
#define TGNAMELEN 32
#define TPREFIXLEN 155
#define TDEVMAJORLEN 8
#define TDEVMINORLEN 8
#define TPADDINGLEN 12
#define RWXALL 0777
#define RWALL 0666

typedef struct __attribute__((__packed__)) PosixHeader {
  char name[TNAMELEN];
  char mode[TMODELEN];
  char uid[TUIDLEN];
  char gid[TGIDLEN];
  char size[TSIZELEN];
  char mtime[TMTIMELEN];
  char chksum[TCHKSUMLEN];
  char typeflag;
  char linkname[TLINKNAMELEN];
  char magic[TMAGLEN];
  char version[TVERSLEN];
  char uname[TUNAMELEN];
  char gname[TGNAMELEN];
  char devmajor[TDEVMAJORLEN];
  char devminor[TDEVMINORLEN];
  char prefix[TPREFIXLEN];
  char padding[TPADDINGLEN];
} PosixHeader;

#include "mytarutils.h"

#endif

#ifndef	__COMMON_H_albert
#define	__COMMON_H_albert	1

#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<unistd.h>
#include	<string.h>
#include	<errno.h>
#include	<ctype.h>
#include	<fcntl.h>
#include	<signal.h>
#include	<time.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<sys/mman.h>
#include	<sys/resource.h>
#include	<sys/wait.h>
#include	<arpa/inet.h>
#include	<netinet/in.h>
#include	<netinet/ip.h>
#include	<netinet/ip_icmp.h>
#include	<netinet/tcp.h>
#include	<netinet/udp.h>
#include	<linux/stddef.h>	/* for "offsetof" */
#include	<stdint.h>

#ifndef NUL
#define NUL '\0'
#endif
#ifndef NULL
#define NULL (void *)0
#endif

/* bitmask generation */
#define V_BIT(n) (1<<(n))

/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif

#define	INIT_PROCESS_PID	1
#define	MAX_MLINE_LEN	4096
#define	PAGE_SIZE	4096
#define	PIPE_BUF_ATOMIC	512
#define	MAX_BUFSIZE	65536
#define	MAX_LINE_LEN	1024
#define	ADDITIONAL_SAFE_LEN	1024

typedef	uint64_t	network64_t;
typedef	uint32_t	network32_t;
typedef	uint16_t	network16_t;
typedef	uint64_t	host64_t;
typedef	uint32_t	host32_t;
typedef	uint16_t	host16_t;

#ifdef __cplusplus
}
#endif

#endif	/* !__COMMON_H_albert */



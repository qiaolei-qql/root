#ifndef	__FAST_LOG_albert__
#define	__FAST_LOG_albert__	1

#ifdef	__cplusplus
extern "C" {
#endif

#include	"common.h"
#include	<pthread.h>

#define	FAST_LOG_SIGN	111222333

#define	FAST_LOG_LEVEL_DEBUG	4
#define	FAST_LOG_LEVEL_MESSAGE	3
#define	FAST_LOG_LEVEL_WARN	2
#define	FAST_LOG_LEVEL_ERROR	1

#define	ylog(fp, str, arg...)		do {if ((fp)) {fprintf((fp), "(%s: %u) "str, __FILE__, __LINE__, ##arg);}} while (0)

typedef	struct {
	uint32_t sign;
	char log_file_plate[MAX_LINE_LEN];
	char log_date[256];
	char log_interval[32];
	int log_fd;
	int log_level;
	FILE *err_log_fp;
	void *start_address;
	void *now_address;
	uint32_t free_space;
	uint32_t map_size;
	uint32_t already_file_size;
	uint32_t log_file_size;
	uint32_t log_file_num;
	uint32_t log_daily;
	int lock;
	pthread_mutex_t mutex;
	time_t cur_time;
} fast_log_t;

extern fast_log_t *fast_log_init(char *log_file_plate, uint32_t log_file_num, uint32_t log_file_size, uint32_t sync_size, int daily, char *error_log_path, int lock, int log_level);
extern int fast_log_destroy(fast_log_t *fltp);
extern int fast_log_change_level(fast_log_t *fltp, int new_log_level);
extern int fast_log_interval(fast_log_t *fltp);
extern int fast_log_log(fast_log_t *fltp, char *log_str, uint32_t log_str_len, int log_level, int is_first, int lock);

int inline fast_log_debug(fast_log_t *fltp, char *log_str, uint32_t log_str_len)
{
	int res = 0;
	
	if (!fltp)
		return 0;

	if (fltp->log_level >= FAST_LOG_LEVEL_DEBUG) {
		if (fltp->lock)
			pthread_mutex_lock(&(fltp->mutex));
		
		res = fast_log_log(fltp, "[DEBUG]\t", strlen("[DEBUG]\t"), FAST_LOG_LEVEL_DEBUG, 1, 0);
		res = fast_log_log(fltp, fltp->log_interval, strlen(fltp->log_interval), FAST_LOG_LEVEL_DEBUG, 0, 0);
		res = fast_log_log(fltp, log_str, log_str_len, FAST_LOG_LEVEL_DEBUG, 0, 0);

		if (fltp->lock)
			pthread_mutex_unlock(&(fltp->mutex));
	}
	
	return res;
}

int inline fast_log_message(fast_log_t *fltp, char *log_str, uint32_t log_str_len)
{
	int res = 0;
	
	if (!fltp)
		return 0;

	if (fltp->log_level >= FAST_LOG_LEVEL_MESSAGE) {
		if (fltp->lock)
			pthread_mutex_lock(&(fltp->mutex));
		
		res = fast_log_log(fltp, "[MESSAGE]\t", strlen("[MESSAGE]\t"), FAST_LOG_LEVEL_MESSAGE, 1, 0);
		res = fast_log_log(fltp, fltp->log_interval, strlen(fltp->log_interval), FAST_LOG_LEVEL_MESSAGE, 0, 0);
		res = fast_log_log(fltp, log_str, log_str_len, FAST_LOG_LEVEL_MESSAGE, 0, 0);

		if (fltp->lock)
			pthread_mutex_lock(&(fltp->mutex));
	}
	
	return 0;
}

int inline fast_log_warn(fast_log_t *fltp, char *log_str, uint32_t log_str_len)
{
	int res = 0;
	
	if (!fltp)
		return 0;

	if (fltp->log_level >= FAST_LOG_LEVEL_WARN) {
		if (fltp->lock)
			pthread_mutex_lock(&(fltp->mutex));
		
		res = fast_log_log(fltp, "[WARN]\t", strlen("[WARN]\t"), FAST_LOG_LEVEL_WARN, 1, 0);
		res = fast_log_log(fltp, fltp->log_interval, strlen(fltp->log_interval), FAST_LOG_LEVEL_WARN, 0, 0);
		res = fast_log_log(fltp, log_str, log_str_len, FAST_LOG_LEVEL_WARN, 0, 0);

		if (fltp->lock)
			pthread_mutex_lock(&(fltp->mutex));
	}
	
	return 0;
}

int inline fast_log_error(fast_log_t *fltp, char *log_str, uint32_t log_str_len)
{
	int res = 0;
	
	if (!fltp)
		return 0;

	if (fltp->log_level >= FAST_LOG_LEVEL_ERROR) {
		if (fltp->lock)
			pthread_mutex_lock(&(fltp->mutex));
		
		res = fast_log_log(fltp, "[ERROR]\t", strlen("[ERROR]\t"), FAST_LOG_LEVEL_ERROR, 1, 0);
		res = fast_log_log(fltp, fltp->log_interval, strlen(fltp->log_interval), FAST_LOG_LEVEL_ERROR, 0, 0);
		res = fast_log_log(fltp, log_str, log_str_len, FAST_LOG_LEVEL_ERROR, 0, 0);

		if (fltp->lock)
			pthread_mutex_lock(&(fltp->mutex));
	}
	
	return 0;
}

#ifdef	__cplusplus
}
#endif

#endif	// !__FAST_LOG_albert__


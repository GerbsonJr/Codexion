
#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void	ms_sleep(t_sim *s, long ms)
{
	long	end;
	long	left;

	end = get_time_ms() + ms;
	left = ms;
	while (left > 0 && !is_stopped(s))
	{
		if (left > 2)
			usleep(1000);
		else
			usleep(200);
		left = end - get_time_ms();
	}
}

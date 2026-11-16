
#include "codexion.h"

void	log_state(t_coder *c, const char *msg)
{
	char	buf[96];
	int		len;

	pthread_mutex_lock(&c->sim->log_lock);
	if (!c->sim->stop)
	{
		len = build_line(buf, get_time_ms() - c->sim->start, c->id, msg);
		if (write(1, buf, len) < 0)
			c->sim->stop = 1;
	}
	pthread_mutex_unlock(&c->sim->log_lock);
}

void	log_burnout(t_coder *c)
{
	char	buf[96];
	int		len;

	pthread_mutex_lock(&c->sim->log_lock);
	if (!c->sim->stop)
	{
		c->sim->stop = 1;
		len = build_line(buf, get_time_ms() - c->sim->start, c->id,
				"burned out");
		len = write(1, buf, len);
		(void)len;
	}
	pthread_mutex_unlock(&c->sim->log_lock);
}

int	is_stopped(t_sim *s)
{
	int	res;

	pthread_mutex_lock(&s->log_lock);
	res = s->stop;
	pthread_mutex_unlock(&s->log_lock);
	return (res);
}

void	stop_sim(t_sim *s)
{
	pthread_mutex_lock(&s->log_lock);
	s->stop = 1;
	pthread_mutex_unlock(&s->log_lock);
	wake_all(s);
}

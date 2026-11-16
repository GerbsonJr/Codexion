
#include "codexion.h"

static int	burned(t_coder *c)
{
	int	res;

	pthread_mutex_lock(&c->lock);
	res = (get_time_ms() - c->last_start > c->sim->cfg.burnout);
	pthread_mutex_unlock(&c->lock);
	return (res);
}

static int	enough(t_coder *c)
{
	int	res;

	pthread_mutex_lock(&c->lock);
	res = (c->compiles >= c->sim->cfg.required);
	pthread_mutex_unlock(&c->lock);
	return (res);
}

static int	check_burnout(t_sim *s)
{
	int	i;

	i = 0;
	while (i < s->cfg.coders)
	{
		if (burned(&s->coders[i]))
		{
			log_burnout(&s->coders[i]);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	all_done(t_sim *s)
{
	int	i;

	i = 0;
	while (i < s->cfg.coders)
	{
		if (!enough(&s->coders[i]))
			return (0);
		i++;
	}
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*s;

	s = arg;
	ms_sleep(s, s->start - get_time_ms());
	while (!is_stopped(s))
	{
		if (check_burnout(s) || all_done(s))
			break ;
		usleep(500);
	}
	stop_sim(s);
	return (NULL);
}


#include "codexion.h"

static int	start_coders(t_sim *s)
{
	int		i;
	t_coder	*c;

	i = 0;
	while (i < s->cfg.coders)
	{
		c = &s->coders[i];
		c->last_start = s->start;
		if (pthread_create(&c->thread, NULL, coder_routine, c))
			return (i);
		i++;
	}
	return (i);
}

static void	join_coders(t_sim *s, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_join(s->coders[i].thread, NULL);
		i++;
	}
}

int	run_simulation(t_sim *s)
{
	pthread_t	mon;
	int			started;
	int			mon_ok;

	s->start = get_time_ms() + 10 + s->cfg.coders / 10;
	started = start_coders(s);
	mon_ok = 0;
	if (started == s->cfg.coders)
		mon_ok = !pthread_create(&mon, NULL, monitor_routine, s);
	if (!mon_ok)
	{
		fprintf(stderr, "Error: thread creation failed\n");
		stop_sim(s);
	}
	join_coders(s, started);
	if (mon_ok)
		pthread_join(mon, NULL);
	return (!mon_ok);
}

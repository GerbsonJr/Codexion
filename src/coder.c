
#include "codexion.h"

static void	*lonely(t_coder *c)
{
	log_state(c, "has taken a dongle");
	while (!is_stopped(c->sim))
		usleep(1000);
	return (NULL);
}

static void	compile_cycle(t_coder *c)
{
	log_state(c, "has taken a dongle");
	log_state(c, "has taken a dongle");
	log_state(c, "is compiling");
	ms_sleep(c->sim, c->sim->cfg.compile);
	release(c);
	pthread_mutex_lock(&c->lock);
	c->compiles++;
	pthread_mutex_unlock(&c->lock);
}

void	*coder_routine(void *arg)
{
	t_coder	*c;

	c = arg;
	ms_sleep(c->sim, c->sim->start - get_time_ms());
	if (c->sim->cfg.coders == 1)
		return (lonely(c));
	if (c->id % 2 == 0)
		ms_sleep(c->sim, 1);
	while (acquire(c))
	{
		compile_cycle(c);
		log_state(c, "is debugging");
		ms_sleep(c->sim, c->sim->cfg.debug);
		log_state(c, "is refactoring");
		ms_sleep(c->sim, c->sim->cfg.refactor);
	}
	return (NULL);
}

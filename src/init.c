
#include "codexion.h"

static void	set_dongles(t_coder *c)
{
	int	left;
	int	right;

	left = c->id - 1;
	right = c->id % c->sim->cfg.coders;
	if (left < right)
	{
		c->first = &c->sim->dongles[left];
		c->second = &c->sim->dongles[right];
	}
	else
	{
		c->first = &c->sim->dongles[right];
		c->second = &c->sim->dongles[left];
	}
}

static int	init_dongles(t_sim *s)
{
	int	i;

	i = 0;
	while (i < s->cfg.coders)
	{
		if (heap_init(&s->dongles[i].queue, 2))
			return (1);
		if (pthread_mutex_init(&s->dongles[i].lock, NULL))
		{
			free(s->dongles[i].queue.data);
			return (1);
		}
		s->dongles[i].in_use = 0;
		s->dongles[i].free_at = 0;
		s->nd++;
		i++;
	}
	return (0);
}

static int	init_coders(t_sim *s)
{
	int		i;
	t_coder	*c;

	i = 0;
	while (i < s->cfg.coders)
	{
		c = &s->coders[i];
		c->id = i + 1;
		c->sim = s;
		c->compiles = 0;
		set_dongles(c);
		if (pthread_mutex_init(&c->lock, NULL))
			return (1);
		if (pthread_cond_init(&c->cond, NULL))
		{
			pthread_mutex_destroy(&c->lock);
			return (1);
		}
		s->nc++;
		i++;
	}
	return (0);
}

int	init_sim(t_sim *s, t_cfg *cfg)
{
	memset(s, 0, sizeof(t_sim));
	s->cfg = *cfg;
	s->dongles = malloc(sizeof(t_dongle) * cfg->coders);
	s->coders = malloc(sizeof(t_coder) * cfg->coders);
	if (!s->dongles || !s->coders)
		return (1);
	if (pthread_mutex_init(&s->log_lock, NULL))
		return (1);
	s->locks = 1;
	if (pthread_mutex_init(&s->seq_lock, NULL))
		return (1);
	s->locks = 2;
	return (init_dongles(s) || init_coders(s));
}

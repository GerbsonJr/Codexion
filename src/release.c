
#include "codexion.h"

static int	release_one(t_dongle *d, int cooldown)
{
	int	head;

	pthread_mutex_lock(&d->lock);
	d->in_use = 0;
	d->free_at = get_time_ms() + cooldown;
	head = 0;
	if (d->queue.size > 0)
		head = d->queue.data[0].id;
	pthread_mutex_unlock(&d->lock);
	return (head);
}

void	wake_coder(t_sim *s, int id)
{
	t_coder	*c;

	if (id < 1)
		return ;
	c = &s->coders[id - 1];
	pthread_mutex_lock(&c->lock);
	pthread_cond_signal(&c->cond);
	pthread_mutex_unlock(&c->lock);
}

void	wake_all(t_sim *s)
{
	int	i;

	i = 0;
	while (i < s->nc)
	{
		wake_coder(s, i + 1);
		i++;
	}
}

void	release(t_coder *c)
{
	int	a;
	int	b;

	a = release_one(c->first, c->sim->cfg.cooldown);
	b = release_one(c->second, c->sim->cfg.cooldown);
	wake_coder(c->sim, a);
	wake_coder(c->sim, b);
}

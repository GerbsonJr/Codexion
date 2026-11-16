
#include "codexion.h"

static int	can_use(t_dongle *d, int id)
{
	return (d->queue.size > 0 && d->queue.data[0].id == id && !d->in_use);
}

static void	grab(t_dongle *d)
{
	heap_pop(&d->queue);
	d->in_use = 1;
}

static long	try_take(t_coder *c)
{
	long	res;

	pthread_mutex_lock(&c->first->lock);
	pthread_mutex_lock(&c->second->lock);
	res = 0;
	if (can_use(c->first, c->id) && can_use(c->second, c->id))
	{
		res = c->first->free_at;
		if (c->second->free_at > res)
			res = c->second->free_at;
		if (res <= get_time_ms())
		{
			grab(c->first);
			grab(c->second);
			res = TAKEN;
		}
	}
	pthread_mutex_unlock(&c->second->lock);
	pthread_mutex_unlock(&c->first->lock);
	return (res);
}

static void	wait_turn(t_coder *c, long until)
{
	struct timespec	ts;

	if (until <= 0)
		pthread_cond_wait(&c->cond, &c->lock);
	else
	{
		ts.tv_sec = until / 1000;
		ts.tv_nsec = (until % 1000) * 1000000L;
		pthread_cond_timedwait(&c->cond, &c->lock, &ts);
	}
}

int	acquire(t_coder *c)
{
	long	res;

	if (is_stopped(c->sim))
		return (0);
	push_requests(c);
	pthread_mutex_lock(&c->lock);
	res = try_take(c);
	while (res != TAKEN && !is_stopped(c->sim))
	{
		wait_turn(c, res);
		res = try_take(c);
	}
	if (res == TAKEN)
		c->last_start = get_time_ms();
	pthread_mutex_unlock(&c->lock);
	return (res == TAKEN);
}

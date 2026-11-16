
#include "codexion.h"

static long	next_seq(t_sim *s)
{
	long	seq;

	pthread_mutex_lock(&s->seq_lock);
	s->seq++;
	seq = s->seq;
	pthread_mutex_unlock(&s->seq_lock);
	return (seq);
}

static void	push_one(t_dongle *d, t_req r)
{
	pthread_mutex_lock(&d->lock);
	heap_push(&d->queue, r);
	pthread_mutex_unlock(&d->lock);
}

void	push_requests(t_coder *c)
{
	t_req	r;

	r.id = c->id;
	r.seq = next_seq(c->sim);
	r.key = r.seq;
	if (c->sim->cfg.policy == EDF)
		r.key = c->last_start + c->sim->cfg.burnout;
	push_one(c->first, r);
	push_one(c->second, r);
}

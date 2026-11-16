
#include "codexion.h"

static void	destroy_dongles(t_sim *s)
{
	int	i;

	i = 0;
	while (i < s->nd)
	{
		pthread_mutex_destroy(&s->dongles[i].lock);
		free(s->dongles[i].queue.data);
		i++;
	}
}

static void	destroy_coders(t_sim *s)
{
	int	i;

	i = 0;
	while (i < s->nc)
	{
		pthread_mutex_destroy(&s->coders[i].lock);
		pthread_cond_destroy(&s->coders[i].cond);
		i++;
	}
}

void	destroy_sim(t_sim *s)
{
	destroy_dongles(s);
	destroy_coders(s);
	if (s->locks >= 1)
		pthread_mutex_destroy(&s->log_lock);
	if (s->locks >= 2)
		pthread_mutex_destroy(&s->seq_lock);
	free(s->dongles);
	free(s->coders);
}

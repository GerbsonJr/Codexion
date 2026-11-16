/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: login <login@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 12:00:00 by login             #+#    #+#             */
/*   Updated: 2026/10/12 12:00:00 by login            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <sys/time.h>
# include <unistd.h>

# define FIFO 0
# define EDF 1
# define TAKEN -1
# define MAX_CODERS 2000

typedef struct s_sim	t_sim;

typedef struct s_cfg
{
	int	coders;
	int	burnout;
	int	compile;
	int	debug;
	int	refactor;
	int	required;
	int	cooldown;
	int	policy;
}	t_cfg;

typedef struct s_req
{
	long	key;
	long	seq;
	int		id;
}	t_req;

typedef struct s_heap
{
	t_req	*data;
	int		size;
	int		cap;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	t_heap			queue;
	int				in_use;
	long			free_at;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	int				compiles;
	long			last_start;
	pthread_t		thread;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	t_dongle		*first;
	t_dongle		*second;
	t_sim			*sim;
}	t_coder;

struct s_sim
{
	t_cfg			cfg;
	long			start;
	long			seq;
	int				stop;
	int				nd;
	int				nc;
	int				locks;
	pthread_mutex_t	log_lock;
	pthread_mutex_t	seq_lock;
	t_dongle		*dongles;
	t_coder			*coders;
};

int		parse_args(int argc, char **argv, t_cfg *cfg);
int		parse_int(const char *s, int *out);
int		read_numbers(char **av, t_cfg *c);
int		read_policy(const char *s, t_cfg *c);
int		init_sim(t_sim *s, t_cfg *cfg);
void	destroy_sim(t_sim *s);
int		run_simulation(t_sim *s);
int		heap_init(t_heap *h, int cap);
void	heap_push(t_heap *h, t_req r);
t_req	heap_pop(t_heap *h);
int		req_less(const t_req *a, const t_req *b);
void	sift_up(t_heap *h, int i);
void	sift_down(t_heap *h, int i);
long	get_time_ms(void);
void	ms_sleep(t_sim *s, long ms);
int		build_line(char *buf, long ts, int id, const char *msg);
void	log_state(t_coder *c, const char *msg);
void	log_burnout(t_coder *c);
int		is_stopped(t_sim *s);
void	stop_sim(t_sim *s);
void	push_requests(t_coder *c);
int		acquire(t_coder *c);
void	release(t_coder *c);
void	wake_coder(t_sim *s, int id);
void	wake_all(t_sim *s);
void	*coder_routine(void *arg);
void	*monitor_routine(void *arg);

#endif


#include <limits.h>
#include "codexion.h"

int	parse_int(const char *s, int *out)
{
	long	v;
	int		i;

	i = 0;
	v = 0;
	if (!s[0])
		return (1);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (1);
		v = v * 10 + (s[i] - '0');
		if (v > INT_MAX)
			return (1);
		i++;
	}
	*out = (int)v;
	return (0);
}

int	read_numbers(char **av, t_cfg *c)
{
	int	err;

	err = parse_int(av[1], &c->coders);
	err |= parse_int(av[2], &c->burnout);
	err |= parse_int(av[3], &c->compile);
	err |= parse_int(av[4], &c->debug);
	err |= parse_int(av[5], &c->refactor);
	err |= parse_int(av[6], &c->required);
	err |= parse_int(av[7], &c->cooldown);
	return (err);
}

int	read_policy(const char *s, t_cfg *c)
{
	if (strcmp(s, "fifo") == 0)
		c->policy = FIFO;
	else if (strcmp(s, "edf") == 0)
		c->policy = EDF;
	else
		return (1);
	return (0);
}

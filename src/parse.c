
#include "codexion.h"

static int	valid_ranges(t_cfg *c)
{
	return (c->coders >= 1 && c->coders <= MAX_CODERS
		&& c->burnout >= 1 && c->required >= 1);
}

static void	usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler\n");
	fprintf(stderr, "scheduler: fifo | edf (all numbers: integers >= 0, "
		"coders/burnout/compiles_required >= 1)\n");
}

int	parse_args(int argc, char **argv, t_cfg *cfg)
{
	if (argc != 9)
	{
		usage();
		return (1);
	}
	if (read_numbers(argv, cfg) || !valid_ranges(cfg)
		|| read_policy(argv[8], cfg))
	{
		fprintf(stderr, "Error: invalid argument\n");
		usage();
		return (1);
	}
	return (0);
}

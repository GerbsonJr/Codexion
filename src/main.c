
#include "codexion.h"

int	main(int argc, char **argv)
{
	t_cfg	cfg;
	t_sim	sim;
	int		ret;

	if (parse_args(argc, argv, &cfg))
		return (1);
	if (init_sim(&sim, &cfg))
	{
		fprintf(stderr, "Error: initialization failed\n");
		destroy_sim(&sim);
		return (1);
	}
	ret = run_simulation(&sim);
	destroy_sim(&sim);
	return (ret);
}

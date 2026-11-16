
#include "codexion.h"

static void	swap_req(t_req *a, t_req *b)
{
	t_req	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	sift_up(t_heap *h, int i)
{
	int	p;

	while (i > 0)
	{
		p = (i - 1) / 2;
		if (!req_less(&h->data[i], &h->data[p]))
			break ;
		swap_req(&h->data[i], &h->data[p]);
		i = p;
	}
}

void	sift_down(t_heap *h, int i)
{
	int	m;
	int	l;
	int	r;

	while (1)
	{
		m = i;
		l = 2 * i + 1;
		r = l + 1;
		if (l < h->size && req_less(&h->data[l], &h->data[m]))
			m = l;
		if (r < h->size && req_less(&h->data[r], &h->data[m]))
			m = r;
		if (m == i)
			break ;
		swap_req(&h->data[i], &h->data[m]);
		i = m;
	}
}

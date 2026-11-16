
#include "codexion.h"

int	req_less(const t_req *a, const t_req *b)
{
	if (a->key != b->key)
		return (a->key < b->key);
	return (a->seq < b->seq);
}

int	heap_init(t_heap *h, int cap)
{
	h->size = 0;
	h->cap = cap;
	h->data = malloc(sizeof(t_req) * cap);
	return (h->data == NULL);
}

void	heap_push(t_heap *h, t_req r)
{
	if (h->size >= h->cap)
		return ;
	h->data[h->size] = r;
	sift_up(h, h->size);
	h->size++;
}

t_req	heap_pop(t_heap *h)
{
	t_req	top;

	top = h->data[0];
	h->size--;
	h->data[0] = h->data[h->size];
	sift_down(h, 0);
	return (top);
}

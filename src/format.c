
#include "codexion.h"

static int	put_num(char *buf, long n)
{
	char	tmp[20];
	int		len;
	int		i;

	len = 0;
	if (n < 0)
		n = 0;
	tmp[len++] = '0' + n % 10;
	n = n / 10;
	while (n > 0)
	{
		tmp[len++] = '0' + n % 10;
		n = n / 10;
	}
	i = 0;
	while (i < len)
	{
		buf[i] = tmp[len - 1 - i];
		i++;
	}
	return (len);
}

int	build_line(char *buf, long ts, int id, const char *msg)
{
	int	len;
	int	i;

	len = put_num(buf, ts);
	buf[len++] = ' ';
	len += put_num(buf + len, id);
	buf[len++] = ' ';
	i = 0;
	while (msg[i])
		buf[len++] = msg[i++];
	buf[len++] = '\n';
	return (len);
}

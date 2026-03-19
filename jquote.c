#include <u.h>
#include <libc.h>
#include <json.h>

static void
usage(void)
{
	fprint(2, "usage: jquote [text ...]\n");
	exits("usage");
}

static char *
readall(int fd)
{
	char *buf;
	int cap, n, r;

	cap = 4096;
	n = 0;
	buf = malloc(cap + 1);
	if(buf == nil)
		sysfatal("malloc: %r");

	for(;;){
		if(n == cap){
			cap *= 2;
			buf = realloc(buf, cap + 1);
			if(buf == nil)
				sysfatal("realloc: %r");
		}
		r = read(fd, buf + n, cap - n);
		if(r < 0)
			sysfatal("read: %r");
		if(r == 0)
			break;
		n += r;
	}
	buf[n] = 0;
	return buf;
}

static char *
joinargs(int argc, char **argv)
{
	char *buf, *p;
	int i, len, n;

	len = 1;
	for(i = 0; i < argc; i++)
		len += strlen(argv[i]) + (i + 1 < argc);

	buf = malloc(len);
	if(buf == nil)
		sysfatal("malloc: %r");

	p = buf;
	for(i = 0; i < argc; i++){
		n = strlen(argv[i]);
		memmove(p, argv[i], n);
		p += n;
		if(i + 1 < argc)
			*p++ = ' ';
	}
	*p = 0;
	return buf;
}

void
main(int argc, char **argv)
{
	char *s;
	JSON j;

	ARGBEGIN{
	default:
		usage();
	}ARGEND

	if(argc == 0)
		s = readall(0);
	else
		s = joinargs(argc, argv);

	JSONfmtinstall();
	j.t = JSONString;
	j.s = s;
	print("%J\n", &j);

	free(s);
	exits(nil);
}

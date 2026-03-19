#include <u.h>
#include <libc.h>
#include <json.h>

static void
usage(void)
{
	fprint(2, "usage: jget key name\n");
	fprint(2, "       jget idx index\n");
	fprint(2, "       jget str [name]\n");
	fprint(2, "       jget num [name]\n");
	fprint(2, "       jget bool [name]\n");
	fprint(2, "       jget type [name]\n");
	fprint(2, "       jget exists name\n");
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

static JSON *
jsonbyidx(JSON *j, long idx)
{
	JSONEl *e;
	long i;

	if(j == nil){
		werrstr("nil json");
		return nil;
	}
	if(j->t != JSONArray){
		werrstr("not an array");
		return nil;
	}
	if(idx < 0){
		werrstr("negative index");
		return nil;
	}
	for(i = 0, e = j->first; e != nil && i < idx; i++, e = e->next)
		;
	if(e == nil){
		werrstr("index %ld out of range", idx);
		return nil;
	}
	return e->val;
}

static JSON *
jsonoptbyname(JSON *j, char *name)
{
	if(name == nil)
		return j;
	return jsonbyname(j, name);
}

static char *
jsontype(JSON *j)
{
	switch(j->t){
	case JSONNull:
		return "null";
	case JSONBool:
		return "bool";
	case JSONNumber:
		return "number";
	case JSONString:
		return "string";
	case JSONArray:
		return "array";
	case JSONObject:
		return "object";
	}
	return "unknown";
}

static long
parseidx(char *s)
{
	char *e;
	long idx;

	if(s == nil || *s == 0)
		sysfatal("missing index");
	idx = strtol(s, &e, 10);
	if(*e != 0)
		sysfatal("bad index '%s'", s);
	return idx;
}

void
main(int argc, char **argv)
{
	char *arg, *buf, *verb;
	JSON *j, *out;

	ARGBEGIN{
	default:
		usage();
	}ARGEND

	if(argc < 1 || argc > 2)
		usage();

	verb = argv[0];
	arg = argc == 2 ? argv[1] : nil;

	buf = readall(0);
	j = jsonparse(buf);
	if(j == nil)
		sysfatal("jsonparse: %r");

	JSONfmtinstall();

	if(strcmp(verb, "key") == 0){
		if(arg == nil)
			usage();
		out = jsonbyname(j, arg);
		if(out == nil)
			sysfatal("%r");
		print("%J\n", out);
	}else if(strcmp(verb, "idx") == 0){
		if(arg == nil)
			usage();
		out = jsonbyidx(j, parseidx(arg));
		if(out == nil)
			sysfatal("%r");
		print("%J\n", out);
	}else if(strcmp(verb, "str") == 0){
		out = jsonoptbyname(j, arg);
		if(out == nil)
			sysfatal("%r");
		if(jsonstr(out) == nil)
			sysfatal("%r");
		print("%s\n", out->s);
	}else if(strcmp(verb, "num") == 0){
		out = jsonoptbyname(j, arg);
		if(out == nil)
			sysfatal("%r");
		if(out->t != JSONNumber)
			sysfatal("not a number");
		print("%.17g\n", out->n);
	}else if(strcmp(verb, "bool") == 0){
		out = jsonoptbyname(j, arg);
		if(out == nil)
			sysfatal("%r");
		if(out->t != JSONBool)
			sysfatal("not a bool");
		print("%s\n", out->n ? "true" : "false");
	}else if(strcmp(verb, "type") == 0){
		out = jsonoptbyname(j, arg);
		if(out == nil)
			sysfatal("%r");
		print("%s\n", jsontype(out));
	}else if(strcmp(verb, "exists") == 0){
		if(arg == nil)
			usage();
		out = jsonbyname(j, arg);
		if(out == nil){
			jsonfree(j);
			free(buf);
			exits("missing");
		}
		print("true\n");
	}else
		usage();

	jsonfree(j);
	free(buf);
	exits(nil);
}

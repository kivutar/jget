#include <u.h>
#include <libc.h>
#include <bio.h>
#include <json.h>

void usage(void)
{
	print("jget key foo");
	print("jget idx 0");
	print("jget str foo");
}

void
main(int argc, char **argv)
{
	if(argc != 3){
		usage();
		exits(nil);
	}

	Biobuf bin;
	char *s;
	JSON *j;

	Binit(&bin, 0, OREAD);

	s = Brdstr(&bin, 0, 1);
	if(s == nil)
		sysfatal("could not slurp");

	j = jsonparse(s);
	if(j == nil)
		sysfatal("jsonparse failed");

	JSONfmtinstall();

	char *verb = argv[1];
	char *obj = argv[2];

	if(!strcmp(verb, "key")){
		JSON *out = jsonbyname(j, obj);
		if(out == nil)
			sysfatal("could not find key");

		print("%J\n", out);
	}else if(!strcmp(verb, "str")){
		JSON *out = jsonbyname(j, obj);
		if(out == nil)
			sysfatal("could not find key");

		print("%s\n", jsonstr(out));
	}else if(!strcmp(verb, "idx")){
		if(j->t != JSONArray)
			sysfatal("only array can be indexed");
		int ind = atoi(obj);
		JSONEl *el = j->first;
		for(int i=0;i<ind;i++)
			el = el->next;
			
		print("%J\n", el->val);
	}

	free(s);
	Bterm(&bin);

	exits(nil);
}

#include <u.h>
#include <libc.h>
#include <bio.h>
#include <json.h>

void
main(int argc, char **argv)
{
	USED(argc, argv);

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

    JSON *output = jsonbyname(j, "output");
	if(output == nil)
		sysfatal("no output");

	if(output->t == JSONArray) {
		JSONEl *first = output->first;
        JSON *cont = jsonbyname(first->val, "content");
        if(cont && cont->t == JSONArray) {
			JSONEl *first = cont->first;
			JSON *text = jsonbyname(first->val, "text");
			print("%s\n", jsonstr(text));
		}
	}

	free(s);
	Bterm(&bin);

    exits(nil);
}

#define _GNU_SOURCE

#include <err.h>
#include <getopt.h>
#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "1.0"
#define FALLBACK_TYPE "application/octet-stream"
#define ZEROSIZE_TYPE "application/x-zerosize"

enum {
	FLAG_BRIEF  = 1 << 0,
	FLAG_FOLLOW = 1 << 1
};

static void usage(void);
static int resolve(const char *path, int flags);

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-bLv] [file ...]\n", program_invocation_short_name);
	exit(1);
}

static int
resolve(const char *path, int flags)
{
	GError *err = NULL;
	GFile *f;
	GFileInfo *info;
	char *guess = NULL;
	const char *type;
	int qflags, ret = -1;

	f = g_file_new_for_path(path);
	qflags = (flags & FLAG_FOLLOW) ? G_FILE_QUERY_INFO_NONE : G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS;
	if (!(info = g_file_query_info(f, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE, qflags, NULL, &err))) {
		warnx("%s", err->message);
		g_error_free(err);
		goto unreffile;
	}
	type = g_file_info_get_content_type(info);
	if (!type || !strcmp(type, FALLBACK_TYPE) || !strcmp(type, ZEROSIZE_TYPE)) {
		guess = g_content_type_guess(path, NULL, 0, NULL);
		if (guess && strcmp(guess, FALLBACK_TYPE))
			type = guess;
	}
	if (!type) {
		warnx("%s: cannot determine type", path);
		goto freeguess;
	}
	if (flags & FLAG_BRIEF)
		printf("%s\n", type);
	else
		printf("%s: %s\n", path, type);
	ret = 0;
freeguess:
	g_free(guess);
	g_object_unref(info);
unreffile:
	g_object_unref(f);
	return ret;
}

int
main(int argc, char *argv[])
{
	static const struct option opts[] = {
		{ "brief", no_argument, NULL, 'b' },
		{ "dereference", no_argument, NULL, 'L' },
		{ "version", no_argument, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};
	int ch, flags = 0, ret = 0;

	while ((ch = getopt_long(argc, argv, "bLv", opts, NULL)) >= 0) {
		switch (ch) {
		case 'b':
			flags |= FLAG_BRIEF;
			break;
		case 'L':
			flags |= FLAG_FOLLOW;
			break;
		case 'v':
			puts("mimetype " VERSION);
			return 0;
		default:
			usage();
			break;
		}
	}
	argv += optind;
	if (!*argv)
		usage();
	for (; *argv; argv++) {
		if (resolve(*argv, flags) < 0)
			ret = 1;
	}
	return ret;
}

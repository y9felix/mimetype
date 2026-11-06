#define _GNU_SOURCE

#include <getopt.h>
#include <gio/gio.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "1.0"

enum {
	FLAG_BRIEF  = 1 << 0,
	FLAG_FOLLOW = 1 << 1
};

static void eprintf(const char *fmt, ...);
static void usage(void);
static int resolve(const char *path, int flags);

static char *argv0;

static void
eprintf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

static void
usage(void)
{
	eprintf("usage: %s [-bLv] [--brief] [--dereference] [file ...]\n",
	    argv0);
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
	qflags = (flags & FLAG_FOLLOW) ? G_FILE_QUERY_INFO_NONE :
	    G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS;
	if (!(info = g_file_query_info(f, "standard::content-type", qflags,
	    NULL, &err))) {
		eprintf("%s: %s\n", argv0, err->message);
		g_error_free(err);
		goto unref_file;
	}
	type = g_file_info_get_content_type(info);
	if (!type || !strcmp(type, "application/octet-stream") ||
	    !strcmp(type, "application/x-zerosize")) {
		guess = g_content_type_guess(path, NULL, 0, NULL);
		if (guess && strcmp(guess, "application/octet-stream"))
			type = guess;
	}
	if (!type) {
		eprintf("%s: %s: cannot determine type\n", argv0, path);
		goto free_guess;
	}
	if (flags & FLAG_BRIEF)
		printf("%s\n", type);
	else
		printf("%s: %s\n", path, type);
	ret = 0;
free_guess:
	g_free(guess);
	g_object_unref(info);
unref_file:
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
		{ 0, 0, 0, 0 }
	};
	char *p;
	int ch, flags = 0, i, ret = 0;

	argv0 = (*argv && **argv) ? *argv : "mimetype";
	if ((p = strrchr(argv0, '/')))
		argv0 = p + 1;
	while ((ch = getopt_long(argc, argv, "bLv", opts, NULL)) != -1) {
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
		}
	}
	argc -= optind;
	argv += optind;
	if (argc < 1)
		usage();
	for (i = 0; i < argc; i++)
		if (resolve(argv[i], flags) < 0)
			ret = 1;
	return ret;
}

#include "config.h"

#include "i18n.h"
#include <locale.h>

#include <stdio.h>
#include <string.h>

struct _testcase {
  char *locale;
  char *untranslated;
  char *expected;
};

typedef struct _testcase testcase;

static testcase testcases[] = {
  { "C",  
    "[DO_NOT_TRANSLATE_THIS_MARKER]", 
    "[DO_NOT_TRANSLATE_THIS_MARKER]" },
  { "de", 
    "[DO_NOT_TRANSLATE_THIS_MARKER]", 
    "[DO_NOT_TRANSLATE_THIS_MARKER_de]" },
};

int main(int argc, char *argv[])
{
  char *localedir;
  int i;

  if (argc != 2) {
    puts("Syntax: test-nls <localedir>\n");
    return 1;
  }

  localedir = argv[1];

  for (i=0; i < sizeof(testcases)/sizeof(testcases[0]); i++) {
    char *locale       = testcases[i].locale;
    char *untranslated = testcases[i].untranslated;
    char *expected     = testcases[i].expected;
    char *translation;
    char *actual_locale;

    printf("setlocale to %s\n", locale);
    actual_locale = setlocale(LC_MESSAGES, locale);

    if (actual_locale == NULL) {
      fprintf(stderr, "Error: Cannot set locale to %s.\n", locale);
      exit(4);
    }

    printf("locale is now %s\n", actual_locale);

    puts("before translation");
    translation = dgettext(GETTEXT_PACKAGE, untranslated);
    puts("after translation");

    if (strcmp(expected, translation) != 0) {
      fprintf(stderr, "# %s\n", N_("[DO_NOT_TRANSLATE_THIS_MARKER]"));

      fprintf(stderr,
	      "locale:       %s\n"
	      "localedir:    %s\n"
	      "untranslated: %s\n"
	      "expected:     %s\n"
	      "translation:  %s\n"
	      "Error: translation != expected\n",
	      locale,
	      localedir,
	      untranslated,
	      expected, 
	      translation);
      
      return 1;
    }
  }
  return 0;
}

#include "glob.h"
#include "sysdep.h"

// Minimal pattern matcher that supports '*' and '?' for ESP32 build
static bool matchPattern(const char *pattern, const char *str) {
  while (*pattern && *str) {
    if (*pattern == '*') {
      // Skip consecutive '*' to avoid excessive recursion
      while (*pattern == '*') ++pattern;
      if (!*pattern) return true; // Trailing '*' matches everything
      while (*str) {
        if (matchPattern(pattern, str)) return true;
        ++str;
      }
      return false;
    } else if (*pattern == '?' || *pattern == *str) {
      ++pattern;
      ++str;
    } else {
      return false;
    }
  }
  // Match remaining trailing '*' in pattern
  while (*pattern == '*') ++pattern;
  return *pattern == '\0' && *str == '\0';
}

int glob(const char *pattern, int flags,
         int (*errfunc) (const char *epath, int eerrno),
         glob_t *pglob) {
  int c = fs()->count();
  pglob->gl_pathv = (char **)malloc(sizeof(char*) * c);
  int tapfiles = 0;

  for (int i = 0; i < c; ++i) {
    char const *name = fs()->get(i)->name;
    size_t l = strlen(name);
    if (matchPattern(pattern, name)) {
        pglob->gl_pathv[tapfiles++] = (char *)name;
    }
  }
  pglob->gl_pathc = tapfiles;
  pglob->gl_offs = 0;
  return 0;
}

void globfree(glob_t *pglob) {
  free(pglob->gl_pathv);
}

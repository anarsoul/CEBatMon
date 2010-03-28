#ifndef LOG_H
#define LOG_H

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))
#endif

void openLogFile(HINSTANCE hInst, const char *fn);
void closeLogFile();
void writeLog(const char *msg, uint32_t len);
void doLog(const char *fmt, ...);

#endif

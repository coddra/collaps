// cuilt.c
/*
Indie Developer License (IDL) 1.0

Copyright (c) 2024 András Nádas <andrew.reeds.mpg@gmail.com>

Permission is hereby granted, free of charge, to any individual or any group of
up to 12 individuals (collectively, "Users") or any company, organization, or
group of more than 12 individuals (collectively, "Entities") obtaining a copy
of this software and associated documentation files (the "Software"), to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, subject to the following conditions:

Users and Entities must provide proper credit, including the copyright holder's
name, a link to the original source, and an indication of any changes made to 
the Software. This credit must be included in the software's documentation, in
the "About" section, or any other prominent location in the distribution.

Credit must not suggest the coptright holder endorses the Users, Entities, or
their use of the Software.

Entities are strictly prohibited from using the Software for commercial
purposes without obtaining a separate commercial license from the copyright 
holder.

Entities desiring to use the Software for commercial purposes must contact the 
copyright holder to negotiate terms and obtain a commercial license.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

By using the Software, Users and Entities agree to the terms of this license.
*/

#ifndef _CUILT_H
#define _CUILT_H

#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#   include <windows.h>
#   include <io.h>
#   include <direct.h>

#   define isatty _isatty
#   define fileno _fileno

#   define access _access
#   define mkdir(path, mode) _mkdir(path)
#   define open _open

#   define popen _popen
#   define pclose _pclose

#   define PATH_SEP "\\"
#   define PATH_MAX MAX_PATH

#   define F_OK 0
#   define O_CREAT _O_CREAT
#   define O_WRONLY _O_WRONLY
#   define O_RDONLY _O_RDONLY
#   define O_TRUNC _O_TRUNC

#   define PLATFORM_DEPENDENT(windows, posix) windows
#elif __linux__
#   ifndef __USE_XOPEN2K
#       define __USE_XOPEN2K
#   endif
#   include <dirent.h>
#   include <linux/limits.h>
#   include <unistd.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#   include <sys/wait.h>

#   define PATH_SEP "/"
#   define PLATFORM_DEPENDENT(windows, posix) posix
#elif __APPLE__
#   include <dirent.h>
#   include <libproc.h>
#   include <unistd.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#   include <sys/wait.h>

#   define PATH_SEP "/"
#   define PLATFORM_DEPENDENT(windows, posix) posix
#endif

#define HEAD(a, ...) a
#define TAIL(a, ...) __VA_ARGS__
#define FIRST(...) HEAD(__VA_ARGS__)
#define SECOND(...) FIRST(TAIL(__VA_ARGS__))
#define __CAT(a, b) a##b
#define CAT(a, b) __CAT(a, b)
#define __NOT0() 1, 1
#define __NOT() 1, 1
#define NOT(a) SECOND(CAT(__NOT, a)(), 0)
#define BOOL(a) NOT(NOT(a))
#define __IF_ELSE0(...) __ELSE0
#define __IF_ELSE1(...) __VA_ARGS__ __ELSE1
#define __ELSE0(...) __VA_ARGS__
#define __ELSE1(...)
#define IF_ELSE(condition) CAT(__IF_ELSE, BOOL(condition))
#define __HAS_ARGS() 0
#define HAS_ARGS(...) BOOL(HEAD(__HAS_ARGS __VA_ARGS__)())
#define __ARG_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, \
    _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43,   \
    _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N
#define ARG_COUNT(...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(__ARG_COUNT(__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, \
    46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, \
    18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))(0)
#define MK_VA(...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(ARG_COUNT(__VA_ARGS__), __VA_ARGS__)(0)

typedef const char** strlist;

enum LOG_LEVEL {
    LOG_NULL = 0,
    LOG_DEBUG = 1,
    LOG_INFO = 2,
    LOG_WARN = 3,
    LOG_ERROR = 4,
    LOG_FATAL = 5,
};

typedef int (*process_t)(strlist argv);

struct config_t {
    struct {
        const char* name;
        const char* source;
        const char* bin;
        const char* test;
    } project;
    struct {
        const char* command;
        strlist flags;
        strlist debug_flags;
        strlist release_flags;
    } cc;
    struct {
        process_t init;
        process_t build;
        process_t run;
        process_t test;
        process_t clean;
    } process;
    enum LOG_LEVEL log_level;
    
    struct {
        const char* project_c;
        const char* project_exe;
        strlist passthrough;
        bool release;
    } __internal;
};
extern struct config_t config;
struct config_t default_config(void);
struct config_t __config(void);
struct config_t merge_config(struct config_t a, struct config_t b);
#define CONFIG(...) struct config_t __config(void) { \
    struct config_t res = __VA_ARGS__; \
    res.__internal.project_c = __FILE__; \
    return res; \
}

void msg(enum LOG_LEVEL level, const char* fmt, ...);
#define DEBUG(...) msg(LOG_DEBUG, __VA_ARGS__)
#define INFO(...) msg(LOG_INFO, __VA_ARGS__)
#define WARN(...) msg(LOG_WARN, __VA_ARGS__)
#ifdef ERROR
#undef ERROR
#endif
#define ERROR(...) msg(LOG_ERROR, __VA_ARGS__)
#define FATAL(...) msg(LOG_FATAL, __VA_ARGS__)

#define BUFFER_SIZE 4096

char* reallocat(char* dest, const char* src);

strlist mklist(size_t count, ...);
strlist* mklistlist(size_t count, ...);
size_t length(strlist list);
strlist append(strlist list, const char* item);
strlist split(const char* sep, const char* body);
char* join(const char* sep, strlist list);
inline static char* join_free(const char* sep, strlist list) {
    char* res = join(sep, list);
    free(list);
    return res;
}
#define LIST(...) mklist(MK_VA(__VA_ARGS__))
#define LIST_LIST(...) mklistlist(MK_VA(__VA_ARGS__))

#define PATH(...) join_free(PATH_SEP, LIST(__VA_ARGS__))

bool starts_with(const char* a, const char* b);
bool ends_with(const char* a, const char* b);

bool exists(const char* path);
bool modified_later(const char* p1, const char* p2);
bool is_outdated(const char *path);

strlist files_in(const char* dir);
strlist filter(strlist list, const char* ext);

char* read_file(const char* path);

char* own_path(void);
char* cwd(void);
const char* basename(const char* path);
char* no_extension(const char* path);

#define FILES(dir, ext) filter(files_in(dir), ext)
static inline void MKDIR(const char* path) {
    if (exists(path))
        return;
    if (mkdir(path, 0755) < 0) 
        ERROR("failed to create %s", path);
    else
        INFO("created %s", path);
}
static inline void TOUCH(const char* path) {
    if (exists(path))
        return;
    if (open(path, O_CREAT | O_WRONLY, 0644) < 0)
        ERROR("failed to create %s", path);
    else
        INFO("created %s", path);
}
void mk_all_dirs(const char* path);
#define MKDIRS(path) mk_all_dirs(path)

int run(strlist* cmd, char** output);

#define RUNO(buffer, ...) run(LIST_LIST(LIST(__VA_ARGS__)), buffer)
#define RUNOL(buffer, args, ...) run(LIST_LIST(LIST(__VA_ARGS__), args), buffer)
#define RUN(...) RUNO(NULL, __VA_ARGS__)
#define RUNL(args, ...) RUNOL(NULL, args, __VA_ARGS__)

#define CC(files, output) run(LIST_LIST(LIST(config.cc.command, "-o", output), config.cc.flags, \
    (config.__internal.release ? config.cc.release_flags : config.cc.debug_flags), files), NULL)

extern strlist source;
extern const char* output;
#endif // _CUILT_H



#ifndef _CUILT_NO_IMPLEMENTATION

int __build(strlist argv);
int __run(strlist argv);
struct config_t config;
strlist source;
const char* output;
struct config_t default_config(void) {
    struct config_t res = {
        .project = {
            .name = basename(cwd()),
            .source = "src",
            .bin = "bin",
            .test = "test",
        },
        .cc = {
            .command = "cc",
            .flags = LIST("-Wall", "-Werror", "-Wextra", "-std=c11"),
            .debug_flags = LIST("-ggdb", "-O0"),
            .release_flags = LIST("-O3"),
        },
        .process = {
            .init = NULL,
            .build = &__build,
            .run = &__run,
            .test = NULL,
            .clean = NULL,
        },
        .log_level = LOG_INFO,
        .__internal = {
            .passthrough = NULL,
            .project_c = NULL,
            .project_exe = own_path(),
            .release = false,
        }
    };
    return res;
}

struct config_t merge_config(struct config_t a, struct config_t b) {
    struct config_t res = a;
#define MERGE(field) if (b.field) res.field = b.field
    MERGE(project.name);
    MERGE(project.source);
    MERGE(project.bin);
    MERGE(project.test);
    MERGE(cc.command);
    MERGE(cc.flags);
    MERGE(process.init);
    MERGE(process.build);
    MERGE(process.run);
    MERGE(process.test);
    MERGE(process.clean);
    MERGE(log_level);
    MERGE(__internal.passthrough);
    MERGE(__internal.project_c);
    MERGE(__internal.project_exe);
#undef MERGE
    return res;
}

#define FG_BLUE 0x0001
#define FG_GREEN 0x0002
#define FG_RED 0x0004
#define FG_INTENSITY 0x0008

void msg(enum LOG_LEVEL level, const char* fmt, ...) {
    if (level < config.log_level)
        return;

    bool color = isatty(fileno(stderr));
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (color)
        GetConsoleScreenBufferInfo(hConsole, &csbi);
#endif

    switch (level) {
#define COLOR(wcolor, pcolor) if (color) PLATFORM_DEPENDENT( \
        SetConsoleTextAttribute(hConsole, wcolor | FG_INTENSITY), \
        fputs("\033[1;" #pcolor "m", stderr) \
    )
        case LOG_DEBUG:
            COLOR(FG_RED|FG_GREEN|FG_BLUE, 37);
            fputs("[DBG] ", stderr);
            break;
        case LOG_INFO:
            COLOR(FG_BLUE|FG_GREEN, 36);
            fputs("[INF] ", stderr);
            break;
        case LOG_WARN:
            COLOR(FG_RED, 33);
            fputs("[WRN] ", stderr);
            break;
        case LOG_ERROR:
            COLOR(FG_RED, 31);
            fputs("[ERR] ", stderr);
            break;
        case LOG_FATAL:
            COLOR(FG_RED, 31);
            fputs("[FTL] ", stderr);
            break;
        default: break;
    }

    if (color)
        PLATFORM_DEPENDENT(
            SetConsoleTextAttribute(hConsole, csbi.wAttributes),
            fputs("\033[0m", stderr)
        );

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);

    if (level == LOG_FATAL) {
        fflush(stdout);
        fflush(stderr);
        exit(1);
    }
}

char* reallocat(char* dest, const char* src) {
    if (!dest) {
        dest = (char*)malloc(strlen(src) + 1);
        dest[0] = '\0';
    } else {
        dest = (char*)realloc(dest, strlen(dest) + strlen(src) + 1);
    }
    strcat(dest, src);
    return dest;
}

strlist mklist(size_t count, ...) {
    strlist res = (strlist)malloc((count + 1) * sizeof(char*));
    
    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++)
        res[i] = va_arg(ap, char*);
    va_end(ap);
    
    res[count] = NULL;
    return res;
}
strlist* mklistlist(size_t count, ...) {
    strlist* res = (strlist*)malloc((count + 1) * sizeof(strlist));
    
    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++)
        res[i] = va_arg(ap, strlist);
    va_end(ap);
    
    res[count] = NULL;
    return res;
}

size_t length(strlist list) {
    if (list == NULL)
        return 0;

    size_t res = 0;
    while (list[res])
        res++;
    return res; 
}

strlist append(strlist list, const char* item) {
    size_t len = length(list);
    list = (strlist)realloc(list, (len + 2) * sizeof(char*));
    list[len] = item;
    list[len + 1] = NULL;
    return list;
}

strlist split(const char* sep, const char* body) {
    strlist res = NULL;
    size_t len = strlen(body);
    char* tmp = (char*)malloc(len + 1);
    memcpy(tmp, body, len + 1);
    char* p = strtok(tmp, sep);
    while (p) {
        res = append(res, p);
        p = strtok(NULL, sep);
    }
    return res;
}

char* join(const char* sep, strlist list) {
    char* res;
    size_t len;
    if (list[0] == NULL) {
        res = (char*)malloc(sizeof(char));
        res[0] = '\0';
        return res;
    } else {
        len = strlen(list[0]);
        res = (char*)malloc((len + 1) * sizeof(char));
        memcpy(res, list[0], len);
    }
    size_t sep_len = strlen(sep);

    for (size_t i = 1; list[i] != NULL; i++) {
        size_t item_len = strlen(list[i]);
        res = (char*)realloc(res, (len + item_len + sep_len + 1) * sizeof(char));
        memcpy(res + len, sep, sep_len);
        len += sep_len;
        memcpy(res + len, list[i], item_len);
        len += item_len;
    }
    res[len] = '\0';
    return res;
}

bool starts_with(const char* a, const char* b) {
    size_t alen = strlen(a);
    size_t blen = strlen(b);
    if (alen < blen)
        return false;
    return memcmp(a, b, blen) == 0;
}

bool ends_with(const char* a, const char* b) {
    size_t alen = strlen(a);
    size_t blen = strlen(b);
    if (alen < blen)
        return false;
    return memcmp(a + alen - blen, b, blen) == 0;
}

bool exists(const char* path) {
    return access(path, F_OK) == 0;
}

bool modified_later(const char* p1, const char* p2)
{
#ifdef _WIN32
    HANDLE p1_handle = CreateFile(p1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (p1_handle == INVALID_HANDLE_VALUE)
        ERROR("could not get time of %s", p1);

    FILETIME p1_time;
    if (!GetFileTime(p1_handle, NULL, NULL, &p1_time))
        ERROR("could not get time of %s", p1);

    CloseHandle(p1_handle);

    HANDLE p2_handle = CreateFile(p2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (p2_handle == INVALID_HANDLE_VALUE)
        ERROR("could not get time of %s", p2);

    FILETIME p2_time;
    if (!GetFileTime(p2_handle, NULL, NULL, &p2_time))
        ERROR("could not get time of %s", p2);

    CloseHandle(p2_handle);

    return CompareFileTime(&p1_time, &p2_time) == 1;
#else
    struct stat statbuf = {0};

    if (stat(p1, &statbuf) < 0)
        ERROR("could not get time of %s", p1);
    int p1_time = statbuf.st_mtime;

    if (stat(p2, &statbuf) < 0) 
        ERROR("could not get time of %s", p2);
    int p2_time = statbuf.st_mtime;

    return p1_time > p2_time;
#endif
}

bool is_outdated(const char *path) {
    for (int i = 0; source[i] != NULL; i++) {
        if (modified_later(source[i], path))
            return true;
    }
    return false;
}

strlist files_in(const char* dir) {
    strlist res = NULL;
#ifdef _WIN32
    WIN32_FIND_DATA data;
    HANDLE h = FindFirstFile(PATH(dir, "*"), &data);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0)
                res = append(res, PATH(dir, data.cFileName));
        } while (FindNextFile(h, &data));
        FindClose(h);
    }
#else
    DIR* d = opendir(dir);
    if (d != NULL) {
        struct dirent* de;
        while ((de = readdir(d)) != NULL) {
            char* name = de->d_name;
            if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
                res = append(res, PATH(dir, name));
        }
        closedir(d);
    }
#endif
    return res;
}

strlist filter(strlist list, const char* ext) {
    int cut_start = -1;
    int cut_count = 1;
    int include_count = 0;
    size_t len = 0;
    for (size_t i = 0; list[i] != NULL; i++) {
        if (ends_with(list[i], ext)) {
            if (cut_start != -1)
                include_count++;
            else
                len++;
            continue;
        }
        
        if (cut_start == -1) {
            cut_start = i;
            continue;
        }
        
        if (include_count == 0) {
            cut_count++;
            continue;
        }
        
        memmove(list + cut_start, list + cut_start + cut_count, include_count * sizeof(char*));
        cut_start += include_count;
        cut_count++;
        len += include_count;
        include_count = 0;
    }
    memmove(list + cut_start, list + cut_start + cut_count, include_count * sizeof(char*));
    len += include_count;
    list = (strlist)realloc(list, (len + 1) * sizeof(char*));
    list[len] = NULL;
    return list;
}

char* read_file(const char* path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        ERROR("failed to open file %s", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)malloc(length + 1);

    fread(content, 1, length, file);
    content[length] = '\0';

    fclose(file);
    return content;
}

char* own_path(void) {
#ifdef _WIN32
    char buffer[MAX_PATH] = {0};
    GetModuleFileName(NULL, buffer, MAX_PATH);
    size_t len = strlen(buffer);
    
#elif __linux__
    char buffer[PATH_MAX] = {0};
    ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX - 1);
    if (len < 0)
        return NULL;

#elif __APPLE__
    char buffer[PATH_MAX] = {0};
    pid_t pid = getpid();
    if (proc_pidpath(pid, buffer, PATH_MAX) < 0)
        buffer[0] = '\0';
    size_t len = strlen(buffer);
#endif

    char* result = (char*)malloc(len + 1);
    strcpy(result, buffer);
    return result;
}

char* parent(const char* path) {
    size_t len = strlen(path);
    char* res = (char*)malloc(len + 1);
    memcpy(res, path, len + 1);
    char* last = strrchr(res, PATH_SEP[0]);
    if (last == NULL)
        res[0] = '\0';
    else
        *last = '\0';
    return res;
}

char* cwd(void) {
    char* res = (char*)malloc(PATH_MAX);
#ifdef _WIN32
    GetCurrentDirectoryA(PATH_MAX, res);
#elif __linux__
    getcwd(res, PATH_MAX);
#elif __APPLE__
    char tmp[PATH_MAX];
    getcwd(tmp, PATH_MAX);
    realpath(tmp, res);
#endif
    return res;
}

const char* basename(const char* path) {
    const char* res = strrchr(path, '/');
    if (res == NULL)
        return path;
    return res + 1;
}

char* no_extension(const char* path) {
    const char* tmp = strrchr(path, '.');
    char* res = NULL;
    if (tmp == NULL) {
        size_t len = strlen(path);
        res = (char*)malloc(len + 1);
        memcpy(res, path, len + 1);
        return res;
    }
    size_t len = tmp - path;
    res = (char*)malloc(len + 1);
    memcpy(res, path, len);
    res[len] = '\0';
    return res;
}

void mk_all_dirs(const char *path) {
    size_t len = strlen(path);
    char* p = (char*)malloc(len + 1);
    memcpy(p, path, len + 1);
    
    char* next = strchr(p, PATH_SEP[0]);
    PLATFORM_DEPENDENT(
        if (len >= 2 && p[1] == ':') next = strrchr(next, PATH_SEP[0]),
        if (next == p) next = strchr(next + 1, PATH_SEP[0])
    );

    while (next != NULL) {
        *next = '\0';
        MKDIR(p);
        *next = PATH_SEP[0];
        next = strchr(p, PATH_SEP[0]);
    }
    MKDIR(p);

    free(p);
    return;
}

int run(strlist* cmd, char** output) {
    char* strcmd = NULL;
    char* showcmd = NULL;
    for (size_t i = 0; cmd[i] != NULL; i++) {
        char* tmp = join("\" \"", cmd[i]);
        if (strcmd == NULL)
            strcmd = tmp;
        else {
            strcmd = reallocat(strcmd, "\" \"");
            strcmd = reallocat(strcmd, tmp);
            free(tmp);
        }
        tmp = join(" ", cmd[i]);
        if (showcmd == NULL)
            showcmd = tmp;
        else {
            showcmd = reallocat(showcmd, " ");
            showcmd = reallocat(showcmd, tmp);
            free(tmp);
        }
    }

    size_t strcmdlen = strlen(strcmd);
    strcmd = (char*)realloc(strcmd, strcmdlen + 3);
    memmove(strcmd + 1, strcmd, strcmdlen);
    strcmd[0] = '"';
    strcmd[strcmdlen + 1] = '"';
    strcmd[strcmdlen + 2] = '\0';

    DEBUG("running %s", showcmd);

    FILE *pipe = popen(strcmd, "r");
    if (!pipe)
        FATAL("failed to run %s", showcmd);

    char buffer[BUFFER_SIZE];
    size_t content_size = BUFFER_SIZE;
    char *content = (char*)malloc(content_size);

    size_t total_read = 0;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        size_t len = strlen(buffer);
        if (total_read + len + 1 >= content_size) {
            content_size *= 2;
            content = (char*)realloc(content, content_size);
        }
        strcpy(content + total_read, buffer);
        total_read += len;
    }
    content[total_read] = '\0';

    if (output) {
        *output = content;
    } else {
        printf("%s", content);
        free(content);
    }
    
    int res = pclose(pipe);
    if (res != 0)
        FATAL("%s exited with status %d", showcmd, res);

    free(showcmd);
    free(strcmd);

    return res;
}

int __run(strlist argv) {
    if (!exists(output) || is_outdated(output)) {
        if (config.process.build(argv) != 0)
            FATAL("cannot build executable");
    }

    int res = RUNL(config.__internal.passthrough, output);
    return res;
}

int __build(strlist argv) {
    MKDIRS(config.project.bin);
    int res = CC(source, output);
    return res;
}

#define COMMAND_BUILD "build"
#define COMMAND_RUN "run"
#define COMMAND_TEST "test"
#define COMMAND_CLEAN "clean"

#ifndef _CUILT_NO_MAIN
int main(int argc, const char* argv[]) {
    chdir(parent(argv[0]));

    strlist _argv = (strlist)malloc(argc * sizeof(char*));
    memcpy(_argv, argv + 1, (argc - 1) * sizeof(char*));
    _argv[argc - 1] = NULL;

    config = merge_config(default_config(), __config());

    source = FILES(config.project.source, ".c");
    output = PATH(config.project.bin, config.project.name);

    if (config.process.init)
        config.process.init(_argv);

    const char* command = NULL;
    for (size_t i = 0; _argv[i] != NULL; i++) {
        const char* arg = _argv[i];

        if (strcmp(arg, "-cc") == 0) {
            if (_argv[i + 1] == NULL)
                FATAL("missing argument for -cc");
            config.cc.command = _argv[++i];
        } else if (strcmp(arg, "-log") == 0) {
            if (_argv[i + 1] == NULL)
                FATAL("missing argument for -log");
            arg = _argv[++i];
            if (strcmp(arg, "debug") == 0)
                config.log_level = LOG_DEBUG;
            else if (strcmp(arg, "info") == 0)
                config.log_level = LOG_INFO;
            else if (strcmp(arg, "warn") == 0)
                config.log_level = LOG_WARN;
            else if (strcmp(arg, "error") == 0)
                config.log_level = LOG_ERROR;
            else if (strcmp(arg, "fatal") == 0)
                config.log_level = LOG_FATAL;
            else
                ERROR("unknown log level: %s", arg);
        } else if (strcmp(arg, "-cflags") == 0) {
            if (_argv[i + 1] == NULL)
                FATAL("missing argument for -cflags");
            config.cc.flags = split(" ", _argv[++i]);
        } else if (strcmp(arg, "-debug") == 0) {
            config.__internal.release = false;
        } else if (strcmp(arg, "-release") == 0) {
            config.__internal.release = true;
        } else if (arg[0] != '\0' && arg[0] == '-') {
            ERROR("unknown option: %s", arg);
        } else {
            command = arg;
            config.__internal.passthrough = _argv + i + 1;
            break;
        }
    }
    if (command == NULL)
        command = COMMAND_BUILD;

    if (strcmp(command, COMMAND_RUN) == 0)
        config.log_level = LOG_FATAL;

    if (modified_later(config.__internal.project_c, config.__internal.project_exe)) {
        INFO("rebuilding...");
        config.log_level = LOG_FATAL;
        if (RUN(config.cc.command, "-o", config.__internal.project_exe, config.__internal.project_c) != 0)
            FATAL("failed to rebuild %s", argv[0]);
        return RUNL(_argv, config.__internal.project_exe);
    }

    int res = 0;
    if (strcmp(command, COMMAND_BUILD) == 0 && config.process.build)
        res = config.process.build(_argv);
    else if (strcmp(command, COMMAND_RUN) == 0 && config.process.run)
        res = config.process.run(_argv);
    else if (strcmp(command, COMMAND_TEST) == 0 && config.process.test)
        res = config.process.test(_argv);
    else if (strcmp(command, COMMAND_CLEAN) == 0 && config.process.clean)
        res = config.process.clean(_argv);
    else
        FATAL("unknown command: %s", command);
    
    if (res != 0)
        FATAL("%s failed with exit code %d", command, res);
    else
        INFO("%s completed successfully", command);

    return 0;
}
#endif // _CUILT_NO_MAIN
#endif // _CUILT_NO_IMPLEMENTATION
#define _CUILT_NO_IMPLEMENTATION
// end of cuilt.c
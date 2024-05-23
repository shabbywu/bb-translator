#pragma once
#include "AppState.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui.h"

class ConsoleWindow
{
  public:
    ConsoleWindow(AppState *state);
    ~ConsoleWindow();

    void ClearLog();
    void AddLog(const char *fmt, ...) IM_FMTARGS(2);
    void ExecCommand(const char *command_line);

    // Portable helpers
    static int Stricmp(const char *s1, const char *s2)
    {
        int d;
        while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
        {
            s1++;
            s2++;
        }
        return d;
    }
    static int Strnicmp(const char *s1, const char *s2, int n)
    {
        int d = 0;
        while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
        {
            s1++;
            s2++;
            n--;
        }
        return d;
    }
    static char *Strdup(const char *s)
    {
        IM_ASSERT(s);
        size_t len = strlen(s) + 1;
        void *buf = malloc(len);
        IM_ASSERT(buf);
        return (char *)memcpy(buf, (const void *)s, len);
    }
    static void Strtrim(char *s)
    {
        char *str_end = s + strlen(s);
        while (str_end > s && str_end[-1] == ' ')
            str_end--;
        *str_end = 0;
    }

    // TextEdit callback
    int TextEditCallback(ImGuiInputTextCallbackData *data);

    void gui(LangType lang, bool *p_open);

    char InputBuf[256];
    ImVector<char *> Items;
    ImVector<const char *> Commands;
    ImVector<char *> History;
    int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll;
    bool ScrollToBottom;
};

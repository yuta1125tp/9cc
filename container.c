#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "9cc.h"

// エラーを報告するための関数
void error_at(char *loc, char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  char *p = user_input;
  int buffered = 0;
  char buffer[1024];
  int offset = 0;
  int pos = loc - user_input - 1;
  int line_idx = 1;

  while (*p)
  {
    if (*p != '\n')
    {
      buffer[buffered++] = *p;
    }
    else
    {
      buffer[buffered] = '\0';
      if (offset < pos && pos <= offset + buffered)
      {
        char msg_header[1024];
        snprintf(msg_header, sizeof(msg_header), "line:%d:", line_idx);
        fprintf(stderr, "%s%s\n", msg_header, buffer);
        fprintf(stderr, "%*s", (int)strlen(msg_header), "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
      offset += buffered;
      buffered = 0;
      line_idx += 1;
    }
    p += 1;
  }
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...)
{
  va_list ap;
  // 可変長引数を１個の変数にまとめる
  va_start(ap, fmt);
  // まとめられた変数を処理する
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
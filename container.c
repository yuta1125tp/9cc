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
  char msg_header[1024];
  snprintf(msg_header, sizeof(msg_header), "line:%d:", line_idx);
  fprintf(stderr, "%s%s\n", msg_header, buffer);
  fprintf(stderr, "%*s", (int)strlen(msg_header) + pos + 1, ""); // pos個の空白を出力
  // fprintf(stderr, "%*s", (int)strlen(msg_header), "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");

  exit(1);
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

// Vector and Map are imported from [rui's 9cc](https://github.com/rui314/9cc)
Vector *new_vec()
{
  Vector *v = malloc(sizeof(Vector));
  v->data = malloc(sizeof(void *) * 16);
  v->capacity = 16;
  v->len = 0;
  return v;
}

void vec_push(Vector *v, void *elem)
{
  if (v->len == v->capacity)
  {
    v->capacity *= 2;
    v->data = realloc(v->data, sizeof(void *) * v->capacity);
  }
  v->data[v->len++] = elem;
}

void vec_pushi(Vector *v, int val)
{
  vec_push(v, (void *)(intptr_t)val);
}

void *vec_pop(Vector *v)
{
  assert(v->len);
  return v->data[--v->len];
}

void *vec_get(Vector *v)
{
  assert(v->len);

  void *ret = v->data[0];

  Vector *_v = new_vec();
  for (int i = 1; i < v->len; i++)
  {
    vec_push(_v, v->data[i]);
  }
  v->data = _v->data;
  v->capacity = _v->capacity;
  v->len = _v->len;
  return ret;
}

void *vec_last(Vector *v)
{
  assert(v->len);
  return v->data[v->len - 1];
}

bool vec_contains(Vector *v, void *elem)
{
  for (int i = 0; i < v->len; i++)
    if (v->data[i] == elem)
      return true;
  return false;
}

bool vec_union1(Vector *v, void *elem)
{
  if (vec_contains(v, elem))
    return false;
  vec_push(v, elem);
  return true;
}

Map *new_map(void)
{
  Map *map = malloc(sizeof(Map));
  map->keys = new_vec();
  map->vals = new_vec();
  return map;
}

void map_put(Map *map, char *key, void *val)
{
  vec_push(map->keys, key);
  vec_push(map->vals, val);
}

void map_puti(Map *map, char *key, int val)
{
  map_put(map, key, (void *)(intptr_t)val);
}

void *map_get(Map *map, char *key)
{
  for (int i = map->keys->len - 1; i >= 0; i--)
    if (!strcmp(map->keys->data[i], key))
      return map->vals->data[i];
  return NULL;
}

int map_geti(Map *map, char *key, int default_)
{
  for (int i = map->keys->len - 1; i >= 0; i--)
    if (!strcmp(map->keys->data[i], key))
      return (intptr_t)map->vals->data[i];
  return default_;
}
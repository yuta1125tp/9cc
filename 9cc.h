#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// コンテナ
typedef enum DataType
{
  INT, // int
  PTR  // pointer
} DataType;

typedef struct Type Type;

struct Type
{
  DataType ty;
  struct Type *ptr_to;
};

typedef struct Vector Vector;

struct Vector
{
  void **data;
  int capacity;
  int len;
};

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);
void vec_pushi(Vector *v, int val);
void *vec_pop(Vector *v);
void *vec_get(Vector *v);
void *vec_last(Vector *v);
bool vec_contains(Vector *v, void *elem);
bool vec_union1(Vector *v, void *elem);

typedef struct Map Map;

struct Map
{
  Vector *keys;
  Vector *vals;
};

Map *new_map(void);
void map_put(Map *map, char *key, void *val);
void map_puti(Map *map, char *key, int val);
void *map_get(Map *map, char *key);
int map_geti(Map *map, char *key, int default_);
bool map_exists(Map *map, char *key);

// ローカル変数
typedef struct LVar LVar;

// ローカル変数の型
struct LVar
{
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
  Type *type; // 変数の型;
};

// 抽象構文木のノードの種類
typedef enum NodeKind
{
  ND_ADD,        // +
  ND_SUB,        // -
  ND_MUL,        // *
  ND_DIV,        // /
  ND_LES,        // <
  ND_GRE,        // >
  ND_LEQ,        // <=
  ND_GEQ,        // >=
  ND_EQU,        // ==
  ND_NEQ,        // !=
  ND_NUM,        // 整数
  ND_ASSIGN,     // =
  ND_LVAR,       // ローカル変数
  ND_RETURN,     // return
  ND_IF,         // if
  ND_IFELSE,     // if-else
  ND_WHILE,      // while
  ND_FOR,        // for
  ND_BLOCK,      // block
  ND_FUNCTION,   // 関数
  ND_DEFINITION, // 関数定義
  ND_ADDR,       // アドレスを参照する
  ND_DEREF,      // 参照を返す
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
  NodeKind kind;      // ノードの型
  Node *lhs;          // 左辺
  Node *rhs;          // 右辺
  int val;            // kindがND_NUMの場合のみ使う
  int offset;         // kindがND_LVARの場合のみ使う
  Node *condition;    // if while and for
  Node *initialze;    // for
  Node *afterthought; // for and if-else
  Vector *block;
  Vector *arguments; // 関数で使う
};

// トークンの型を表す値
typedef enum TokenKind
{
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
  TK_RETURN,   // returnを表すトークン
  TK_IF,       // if
  TK_ELSE,     // else
  TK_WHILE,    // while
  TK_FOR,      // for
  TK_INT,      // int
} TokenKind;

typedef struct Token Token;

// トークンの型
struct Token
{
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列（エラーメッセージ用）
  int len;        // トークンの長さ
};

// コードジェネレータ
void gen(Node *node);

// 抽象構文木のパーザー
void tokenize();
void program();
Node *assign();
Node *stmt();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// コンテナ
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

// グローバル変数の宣言
// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern char *user_input;

// 入力プログラムのトーカナイズ結果
extern Node *code[100];

// ローカル変数
extern LVar *locals;

// アセンブリ中のラベル
extern int label_idx;

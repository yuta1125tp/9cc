// ローカル変数
typedef struct LVar LVar;

// ローカル変数の型
struct LVar
{
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
};

// 抽象構文木のノードの種類
typedef enum
{
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_LES,    // <
  ND_GRE,    // >
  ND_LEQ,    // <=
  ND_GEQ,    // >=
  ND_EQU,    // ==
  ND_NEQ,    // !=
  ND_NUM,    // 整数
  ND_ASSIGN, // =
  ND_LVAR,   // ローカル変数
  ND_RETURN, // return
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
  int offset;    // kindがND_LVARの場合のみ使う
};

// トークンの型を表す値
typedef enum
{
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
  TK_RETURN,   // returnを表すトークン
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

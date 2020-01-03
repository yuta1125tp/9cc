#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

// 新しいノードを作成する関数
Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_ident(char c)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->val = c-'a';
  return node;
}

// 新しいノードを作成する関数（整数ノード）
Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op)
{
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
bool expect(char *op)
{
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません", &op);
  token = token->next;
}

// 次のトークンが数値の場合に真を返す、数字じゃないなら偽を返す。
bool consume_number()
{
  if (token->kind == TK_NUM)
    return true;
  else;
    return false;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number()
{
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

// 次のトークンが識別子の場合、トークンを1つ読み進めてそのトークンを返す。
// それ以外の場合にはnullを返す。外でboolで確認して読み飛ばす
Token* consume_ident()
{
  
  if (token->kind != TK_IDENT)
    return NULL;
  Token *cur = token;
  token = token->next;
  return cur;
}


bool at_eof()
{
  return token->kind == TK_EOF;
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *stmt() {
  Node *node = expr();
  expect(";");
  return node;
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *expr()
{
  return assign();
}

Node *equality()
{
  Node *node = relational();
  for (;;)
  {
    if (consume("=="))
      node = new_node(ND_EQU, node, relational());
    else if (consume("!="))
      node = new_node(ND_NEQ, node, relational());
    else
      return node;
  }
}

Node *relational()
{
  Node *node = add();
  for (;;)
  {
    if (consume("<="))
      node = new_node(ND_LEQ, node, add());
    else if (consume("<"))
      node = new_node(ND_LES, node, add());
    else if (consume(">="))
      node = new_node(ND_GEQ, add(), node);
    else if (consume(">"))
      node = new_node(ND_GRE, add(), node);
    else
      return node;
  }
}

Node *add()
{
  Node *node = mul();
  for (;;)
  {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul()
{
  Node *node = unary();
  for (;;)
  {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// 単項に関するパーザー
Node *unary()
{
  if (consume("+"))
  {
    // +x -> x
    return primary();
  }
  else if (consume("-"))
  {
    // -x -> 0-x
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

Node *primary()
{
  // 次のトークンが"("なら"(" expr ")"のはず
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }

  // identかどうか確認する
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' + 1) * 8;
    return node;
  }

  // そうでなければ数字のはず
  return new_node_num(expect_number());
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// user_inputをトーカナイズしてそれを返す
void tokenize()
{
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p)
  {
    //空白文字をスキップ
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (memcmp(p, ">=", 2) == 0 ||
        memcmp(p, "<=", 2) == 0 ||
        memcmp(p, "==", 2) == 0 ||
        memcmp(p, "!=", 2) == 0)
    {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
      continue;
    }

    if (
      *p == '+'
      || *p == '-'
      || *p == '*'
      || *p == '/'
      || *p == '('
      || *p == ')'
      || *p == '>'
      || *p == '<'
      || *p=='='
      || *p==';'
      )
    {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p++);
      cur->len = 1;
      continue;
    }

    error_at(p, "トーカナイズできません");
  }
  new_token(TK_EOF, cur, p);
  token = head.next;
}
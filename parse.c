#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
LVar *find_lvar(Token *tok)
{
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

// 新しいノードを作成する関数
Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
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

// 次のトークンが期待している種類のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume_kind(int token_kind)
{
  if (token->kind != token_kind)
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op)
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
  else
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
Token *consume_ident()
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

Node *assign()
{
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *stmt()
{
  Node *node;

  if (consume_kind(TK_RETURN))
  {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
    expect(";");
  }
  else if (consume_kind(TK_IF))
  {
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    expect("(");
    node->condition = expr();
    expect(")");
    node->lhs = stmt();
    if (consume_kind(TK_ELSE))
    {
      node->kind = ND_IFELSE;
      node->afterthought = stmt();
    }
  }
  else if (consume_kind(TK_WHILE))
  {
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    expect("(");
    node->condition = expr();
    expect(")");
    node->lhs = stmt();
  }
  else if (consume_kind(TK_FOR))
  {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    expect("(");
    if (!consume(";"))
    {
      node->initialze = expr();
      expect(";");
    }
    if (!consume(";"))
    {
      node->condition = expr();
      expect(";");
    }
    if (!consume(")"))
    {
      node->afterthought = expr();
      expect(")");
    }
    node->lhs = stmt();
  }
  else if (consume("{"))
  {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->block = new_vec();
    while (!consume("}"))
    {
      vec_push(node->block, stmt());
    }
  }
  else
  {
    node = expr();
    expect(";");
  }

  return node;
}

void program()
{
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
  else if (consume("*"))
  {
    // *x
    return new_node(ND_DEREF, unary(), NULL);
  }
  else if (consume("&"))
  {
    // &x
    return new_node(ND_ADDR, unary(), NULL);
  }
  return primary();
}

// 識別子があるか確認する
// あるなら識別子のトークンを返す。ないならNULLを返す。
Token *check_identifier(Token *tok)
{
  Token *ret;
  if (tok->kind == TK_INT)
  {
    ret = tok;
    token = token->next;
    return ret;
  }
  return NULL;
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

  // 識別子があるか確認する
  Token *ident = check_identifier(token);

  // identかどうか確認する
  Token *tok = consume_ident();
  if (tok)
  {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar)
    {
      node->offset = lvar->offset;
    }
    else if (ident)
    {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals)
      {
        lvar->offset = locals->offset + 8;
      }
      else
      {
        lvar->offset = 8;
      }

      node->offset = lvar->offset;
      locals = lvar;
    }
    else
    {
      error("未定義の変数です");
    }
    if (consume("("))
    {
      node->kind = ND_FUNCTION;
      node->arguments = new_vec();
      while (!consume(")"))
      {
        vec_push(node->arguments, expr());
        consume(",");
      }
    }

    if (consume("{"))
    {
      node->kind = ND_DEFINITION;
      node->block = new_vec();
      while (!consume("}"))
      {
        vec_push(node->block, stmt());
      }
    }

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

// 与えられた文字がトークンを構成する文字
// すなわち英数字かアンダースコアかどうかを判定する関数
int is_alnum(char c)
{
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
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
        *p == '+' ||
        *p == '-' ||
        *p == '*' ||
        *p == '/' ||
        *p == '(' ||
        *p == ')' ||
        *p == '>' ||
        *p == '<' ||
        *p == '=' ||
        *p == ';' ||
        *p == '{' ||
        *p == '}' ||
        *p == ',' ||
        *p == '&' ||
        *p == '*')
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

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
    {
      cur = new_token(TK_RETURN, cur, p);
      p += 6;
      continue;
    }

    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2]))
    {
      cur = new_token(TK_IF, cur, p);
      p += 2;
      continue;
    }

    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4]))
    {
      cur = new_token(TK_ELSE, cur, p);
      p += 4;
      continue;
    }

    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5]))
    {
      cur = new_token(TK_WHILE, cur, p);
      p += 5;
      continue;
    }

    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3]))
    {
      cur = new_token(TK_FOR, cur, p);
      p += 3;
      continue;
    }

    if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3]))
    {
      cur = new_token(TK_INT, cur, p);
      p += 3;
      continue;
    }

    if (is_alnum(*p))
    {
      int varlen = 0;
      while (is_alnum(p[varlen]))
      {
        varlen += 1;
      }
      cur = new_token(TK_IDENT, cur, p);
      cur->len = varlen;
      p += varlen;
      continue;
    }

    error_at(p, "トーカナイズできません");
  }
  new_token(TK_EOF, cur, p);
  token = head.next;
}
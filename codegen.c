#include <stdio.h>

#include "9cc.h"

// アセンブリとコメントを出力する関数
void pop(char *pop_to)
{
  printf("  pop %s # スタックの先頭を%sにpop\n", pop_to, pop_to);
}

void push(char *push_this)
{
  printf("  push %s # スタックに%sをpush\n", push_this, push_this);
}

void cmp(char *a, char *b)
{
  printf("  cmp %s, %s # %sと%sを比較\n", a, b, a, b);
}

void mov(char *to, char *from)
{
  printf("  mov %s, %s # %sに%sからコピー\n", to, from, to, from);
}

void mov1(char *to_with_brackets, char *from)
{
  printf("  mov [%s], %s # %sが差すアドレスに%sの値をストア\n", to_with_brackets, from, to_with_brackets, from);
}

void mov2(char *to, char *from_with_brackets)
{
  printf("  mov %s, [%s] # %sに%sが差すアドレスからロード\n", to, from_with_brackets, to, from_with_brackets);
}

void mov3(char *to_with_brackets, char *from_with_brackets)
{
  printf("  mov [%s], [%s] # %sが差すアドレスに%sが差すアドレスからストア\n", to_with_brackets, from_with_brackets, to_with_brackets, from_with_brackets);
}

void gen_lval(Node *node)
{
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  mov("rax", "rbp");
  printf("  sub rax, %d\n", node->offset);
  push("rax");
}

LVar *find_lvar_by_node(Node *node)
{
  LVar *a = locals;
  int offset = node->offset;
  while (a)
  {
    if (a->offset == node->offset)
      return a;
    a = a->next;
  }
  return NULL;
}

void gen(Node *node)
{
  int offset = 0;
  int num_args = 0;
  int arg_idx = 0;
  LVar *_lvar;
  int current_label_idx = label_idx;
  label_idx += 1;
  switch (node->kind)
  {
  case ND_IF:
    gen(node->condition);
    pop("rax");
    cmp("rax", "0");
    printf("  je .Lend%03d\n", current_label_idx);
    gen(node->lhs);
    printf(".Lend%03d:\n", current_label_idx);
    return;
  case ND_IFELSE:
    gen(node->condition);
    pop("rax");
    cmp("rax", "0");
    printf("  je .Lelse%03d\n", current_label_idx);
    gen(node->lhs);
    printf("  jmp .Lend%03d\n", current_label_idx);
    printf(".Lelse%03d:\n", current_label_idx);
    gen(node->afterthought);
    printf(".Lend%03d:\n", current_label_idx);
    return;
  case ND_WHILE:
    printf(".Lbegin%03d:\n", current_label_idx);
    gen(node->condition);
    pop("rax");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%03d\n", current_label_idx);
    gen(node->lhs);
    printf("  jmp .Lbegin%03d\n", current_label_idx);
    printf(".Lend%03d:\n", current_label_idx);
    return;
  case ND_FOR:
    gen(node->initialze);
    printf(".Lbegin%03d:\n", current_label_idx);
    gen(node->condition);
    pop("rax");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%03d\n", current_label_idx);
    gen(node->lhs);
    gen(node->afterthought);
    printf("  jmp .Lbegin%03d\n", current_label_idx);
    printf(".Lend%03d:\n", current_label_idx);
    return;
  case ND_RETURN:
    printf("  # return;\n");
    gen(node->lhs);
    pop("rax"); // スタックトップの値をraxにコピー
    mov("rsp", "rbp");
    pop("rbp");
    printf("  ret\n");
    return;
  case ND_FUNCTION:
    num_args = node->arguments->len;
    while (node->arguments->len)
    {
      gen(vec_pop(node->arguments));
    }
    arg_idx = 0;
    while (arg_idx++ < num_args)
    {
      switch (arg_idx)
      {
      case 1:
        pop("rdi");
        break;
      case 2:
        pop("rsi");
        break;
      case 3:
        pop("rdx");
        break;
      case 4:
        pop("rcx");
        break;
      case 5:
        pop("r8");
        break;
      case 6:
        pop("r9");
        break;
      default:
        error("6つ以上の引数はサポートしていません。");
        break;
      }
    }
    // printf("  sub rsp, %d\n", 8);
    _lvar = find_lvar_by_node(node);
    printf(
        "  call %.*s\n",
        _lvar->len,
        _lvar->name);
    push("rax"); // callした返り値がraxにあるので、スタックトップにプッシュする
    return;
  case ND_DEFINITION:
    _lvar = find_lvar_by_node(node);
    printf("  .global %.*s\n",
           _lvar->len,
           _lvar->name);
    printf("  .type	%.*s, @function\n",
           _lvar->len,
           _lvar->name);
    printf("%.*s:\n",
           _lvar->len,
           _lvar->name);
    printf(".LFB%d:\n", label_idx);

    // プロローグ
    printf("  # プロローグ\n");
    printf("  push rbp # スタックにベースポインタをpush\n");
    printf("  mov rbp, rsp # スタックポインタをベースポインタにコピー\n");
    printf("  sub rsp, 1024 # 変数128個分の領域を確保する\n");

    num_args = node->arguments->len;
    arg_idx = 0;
    while (arg_idx++ < num_args)
    {
      gen_lval(vec_get(node->arguments));
      pop("rax");
      switch (arg_idx)
      {
      case 1:
        mov1("rax", "rdi");
        break;
      case 2:
        mov1("rax", "rsi");
        break;
      case 3:
        push("rdx");
        break;
      case 4:
        push("rcx");
        break;
      case 5:
        push("r8");
        break;
      case 6:
        push("r9");
        break;
      default:
        error("6つ以上の引数はサポートしていません。");
        break;
      }
    }

    while (node->block->len)
    {
      Node *sub_node = vec_get(node->block);
      gen(sub_node);
    }

    // エピローグ
    // 最後の式の結果がRAXに残っているので、それを返り値する。
    printf("  # エピローグ\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    pop("rax");
    mov2("rax", "rax");
    push("rax");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);
    pop("rdi");
    pop("rax");
    mov1("rax", "rdi");
    push("rdi");
    return;
  case ND_BLOCK:
    while (node->block->len)
    {
      Node *sub_node = vec_get(node->block);
      gen(sub_node);
      pop("rax");
    }
    return;
  case ND_ADDR:
    gen_lval(node->lhs);
    return;
  case ND_DEREF:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  pop("rdi");
  pop("rax");

  switch (node->kind)
  {
  case ND_ADD:
    printf("  add rax, rdi # 第1オペラントと第2オペラントを足して第1オペラントに格納する\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi # 第1オペラントから第2オペラントを引いて第1オペラントに格納する\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_LES:
  case ND_GRE:
    cmp("rax", "rdi");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LEQ:
  case ND_GEQ:
    cmp("rax", "rdi");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_EQU:
    cmp("rax", "rdi");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NEQ:
    cmp("rax", "rdi");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  }
  push("rax");
}

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
  printf("  mov %s, %s # %sから%sにコピー\n", to, from, to, from);
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

void gen(Node *node)
{
  int num_args = 0;
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
    gen(node->lhs);
    pop("rax");
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
    int arg_idx = 0;
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
    printf(
        "  call %.*s\n",
        locals[(locals->offset - node->offset) / 8].len,
        locals[(locals->offset - node->offset) / 8].name);
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
  }

  gen(node->lhs);
  gen(node->rhs);

  pop("rdi");
  pop("rax");

  switch (node->kind)
  {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
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

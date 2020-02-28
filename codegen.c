#include <stdio.h>

#include "9cc.h"

void gen_lval(Node *node)
{
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node)
{
  int current_label_idx = label_idx;
  label_idx += 1;
  switch (node->kind)
  {
  case ND_IF:
    gen(node->condition);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%03d\n", current_label_idx);
    gen(node->lhs);
    printf(".Lend%03d:\n", current_label_idx);
    return;
  case ND_IFELSE:
    gen(node->condition);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
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
    printf("  pop rax\n");
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
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%03d\n", current_label_idx);
    gen(node->lhs);
    gen(node->afterthought);
    printf("  jmp .Lbegin%03d\n", current_label_idx);
    printf(".Lend%03d:\n", current_label_idx);
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

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
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LEQ:
  case ND_GEQ:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_EQU:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NEQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}

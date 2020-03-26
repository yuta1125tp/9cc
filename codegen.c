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
  int num_args=0;
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
    printf("  mov rsp, rbp; \n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_FUNCTION:
    num_args=node->arguments->len;
    while (node->arguments->len)
    {
      gen(vec_pop(node->arguments));
    }
    int arg_idx=0;
    while (arg_idx++<num_args)
    {
      switch (arg_idx)
      {
      case 1:
        printf("  pop rdi\n");
        break;
      case 2:
        printf("  pop rsi\n");
        break;
      case 3:
        printf("  pop rdx\n");
        break;
      case 4:
        printf("  pop rcx\n");
        break;
      case 5:
        printf("  pop r8\n");
        break;
      case 6:
        printf("  pop r9\n");
        break;
      default:
        error("6つ以上の引数はサポートしていません。");    
        break;
      }
    }
    // printf("  sub rsp, %d\n", 8);
    printf("  call %.*s\n", locals[(locals->offset - node->offset) / 8].len, locals[(locals->offset - node->offset) / 8].name);  // "cde"
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
  case ND_BLOCK:
    while (node->block->len)
    {
      Node *sub_node = vec_get(node->block);
      gen(sub_node);
      printf("  pop rax\n");
    }
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

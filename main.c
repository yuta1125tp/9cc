#include <stdio.h>

#include "9cc.h"

// グローバル変数の定義
// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    error("引数の個数が正しくありません");
    return 1;
  }

  // トーカナイズしてパーズする。
  user_input = argv[1];
  token = tokenize();
  Node *node = expr();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残っているはずなので、それをRAXにロードして関数の返り値にする。
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
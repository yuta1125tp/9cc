#include <stdio.h>

#include "9cc.h"

// グローバル変数の定義
// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// 入力プログラムのトーカナイズ結果
Node *code[100];

LVar *locals;

int label_idx;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    error("引数の個数が正しくありません");
    return 1;
  }

  // トーカナイズしてパーズする
  // 結果はcodeに保存される
  user_input = argv[1];
  tokenize();
  program();

  label_idx = 0;

  // アセンブリの前半部分を出力
  printf("  .intel_syntax noprefix # intel記法で書く\n");

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++)
  {
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにpopしておく
    printf("  pop rax\n");
  }

  return 0;
}
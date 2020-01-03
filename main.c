#include <stdio.h>

#include "9cc.h"

// グローバル変数の定義
// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// 入力プログラムのトーカナイズ結果
Node *code[100];

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

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++)
  {
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにpopしておく
    printf("  pop rax\n");
  }

  // エピローグ 
  // 最後の式の結果がRAXに残っているので、それを返り値する。
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
C言語コンパイラ
---

## これは何？
[Ruiさんのテキスト](https://www.sigbus.info/compilerbook)を見ながらCコンパイラを実装している。

## VSCode環境について

VSCodeでC言語をステップ実行するための設定。  
Microsoft製のC/C++拡張はインストール済み  
launch.jsonにF5で実行(launch)する内容を記述し、tasks.jsonに依存するタスク（コンパイルやmake clean）を記述している。

デバッグ時の引数は.vscode/launch.jsonのargsに指定する。

`.vscode/launch.json`
```json
{
  // Use IntelliSense to learn about possible attributes.
  // Hover to view descriptions of existing attributes.
  // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
  "version": "0.2.0",
  "configurations": [
    {
      "name": "make and test 9cc",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/9cc",
      "args": [
        "foo=1+(1>10); bar=foo+1; return bar; return foo;"
      ],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}",
      "environment": [],
      "externalConsole": false,
      "MIMode": "gdb",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ],
      "preLaunchTask": "make 9cc",
      "miDebuggerPath": "/usr/bin/gdb"
    },
  ]
}
```

`.vscode/tasks.json`
```json
{
  "tasks": [
    {
      "type": "shell",
      "label": "make 9cc",
      "command": "make",
      "args": [
        "9cc"
      ],
      "group": "build",
      // "dependsOn": [
      //   "make clean",
      // ]
    },
    {
      "type": "shell",
      "label": "make clean",
      "command": "make",
      "args": [
        "clean"
      ]
    }
  ],
  "version": "2.0.0"
}
```

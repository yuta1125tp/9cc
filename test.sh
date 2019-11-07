#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 0 0
try 42 42
try 21 '5+20-4'
try 41 " 12 + 34 - 5 "

try 30 " 12 * 5 - 30 "
try 120 " 12 * 5 *2 "
try 22 " 12 + 5 *2 "
try 34 " ( 12 + 5) *2 "
try 10 "-10+20"
try 10 "(-10)+20"

echo OK

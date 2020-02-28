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
    echo "input was $input, $expected expected, but got $actual"
    exit 1
  fi
}

# try 0 "0;"
# try 42 "42;"
# try 21 '5+20-4;'
# try 41 " 12 + 34 - 5; "

# try 30 " 12 * 5 - 30; "
# try 120 " 12 * 5 *2; "
# try 22 " 12 + 5 *2; "
# try 34 " ( 12 + 5) *2; "
# try 10 "-10+20;"
# try 10 "(-10)+20;"

# try 2 "1+(1<10);"
# try 2 "(1<=10)+1;"
# try 2 "(10>=1)+1;"
# try 1 "1+(1>10);"
# try 2 "(10>1)+1;"
# try 2 "(10==10)+1;"
# try 2 "(10!=1)+1;"
# try 1 "1+(10<10);"
# try 0 "-(10<=10)+1;"
# try 1 "1+(10>10);"
# try 0 "-(10>=10)+1;"

# try 1 "(100<10)+1;"
# try 1 "(100<=10)+1;"
# try 1 "(10>100)+1;"
# try 1 "(10>=100)+1;"
# try 1 "(10==1000)+1;"
# try 1 "(10!=10)+1;"

# try 14 "a = 3;
# b = 5 * 6 - 8;
# a + b / 2;"

# try 14 "foo = 3;
# bar = 5 * 6 - 8;
# foo + bar / 2;"

# try 2 "foo=1+(1>10); bar=foo+1; return bar;"
# try 25 "x = 4;
# foo = 3;
# bar = 5 * 6 - 8 + foo;
# return foo;
# return bar;"

##========
## if and if-else
# try 0 "x = 1;
# if (x==4)
#   return x;
# else
#   return 0;"

# try 4 "x = 4;
# if (x==4)
#   return x;
# else
#   return 0;"

# try 2 "x = 4;
# if (x==4)
#   return 2;
# return 1;"

# try 1 "x = 4;
# if (x==4)
#   return 1;
# else if (x==2)
#   return 2;
# else
#   return 3;"

# #========
# # while
# try 10 "x=0;
# while(x<10)
#   x=x+1;
# return x;
# "

# #========
# # for
# try 100 "y=0;
# for(i=0;i<10;i=i+1)
#   y=y+1;
# return i*y;"


echo OK

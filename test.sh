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

try_with_helper() {
    expected="$1"
    input="$2"
    
    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s snippets/function_call/callee.s
    ./tmp
    actual="$?"
    
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "input was $input, $expected expected, but got $actual"
        exit 1
    fi
}

# try 0 "main(){return 0;};"
# try 42 "main(){return 42;};"
# try 21 "main(){return 5+20-4;};"
# try 41 "main(){return  12 + 34 - 5;};"

# try 30 " 12 * 5 - 30; "
# try 120 " 12 * 5 *2; "
# try 22 " 12 + 5 *2; "
# try 34 " ( 12 + 5) *2; "
# try 10 "-10+20;"
# try 10 "main(){(-10)+20;};"

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

# try 3 "main(){
#     a = 3;
#     return a;
# };"


# try 4 "main(){
#     a = 3;
#     b = 1;
#     return a+b;
# };"

# try 14 "main(){
#     a = 3;
#     b = 5 * 6 - 8;
#     return a + b / 2;
# };"

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

# #========
# # block
# try 17 "
# main(){
#     x=0;
#     for(i=0;i<10;i=i+1)
#     {
#         j=i*2;
#         if (j<5)
#         {
#             x=x+1;
#         }
#         else
#         {
#             x=x+2;
#         }
#         j=0;
#     }
#     return x;
# };
# "

# #========
# # function call 1
# input="func0();
# return 0;"
# ./9cc "$input" > tmp.s
# gcc -o tmp tmp.s snippets/function_call/callee.s
# printf "$input => "
# ./tmp

# #========
# # function call 2
# input="func1(3);
# return 0;"
# ./9cc "$input" > tmp.s
# gcc -o tmp tmp.s snippets/function_call/callee.s
# printf "$input => "
# ./tmp

# #========
# # function call 2
# input="main(){
#     func2(3,6);
#     return 0;
# };"
# ./9cc "$input" > tmp.s
# gcc -o tmp tmp.s snippets/function_call/callee.s
# printf "$input => "
# ./tmp

# ========
# function
# try 6 "main(){
#     y=1;
#     x=2;
#     return (x+y)*2;
# };"

# try 1 "foo()
# {
#     return 1;
# };
# main()
# {
#     return foo();
# };"

# try 10 "times2(i)
# {
#     return i*2;
# };
# main()
# {
#     return times2(5);
# };"


# try 3 "minus(i,j)
# {
#     return i-j;
# };
# main()
# {
#     return minus(9,6);
# };"


# # ================
# # fibonacci
# input="
# fibonacci(n)
# {
#     if (n==0)
#     {
#         return n;
#     } else if (n==1)
#     {
#         return n;
#     } else
#     {
#         return fibonacci(n - 2) + fibonacci(n - 1);
#     }
# };
# main()
# {
#     for(i=0;i<10;i=i+1)
#     {
#         print(fibonacci(i));
#     }
#     return 0;
# };"
# ./9cc "$input" > tmp.s
# gcc -o tmp tmp.s snippets/function_call/callee.s
# printf "$input => "
# ./tmp

# # ================
# # * and &

# try 3 "main()
# {
#     x = 3;
#     y = &x;
#     return *y;
# };"

# try 3 "main()
# {
#     x = 3;
#     y = 5;
#     z = &y + 8;
#     return *z;
# };"


# # int x
# try 42 "int main()
# {
#     int x;
#     x=42;
#     return x;
# };"

# try 42 "
# int foo(int x, int y)
# {
#     return x-y;
# };
# int main()
# {
#     int x;
#     x=52;
#     int y;
#     y = foo(x, 10);
#     return y;
# };"

# # =============
# # ポインタ型
# try 3 "
# int main()
# {
#     int x;
#     int *y;
#     y = &x;
#     *y = 3;
#     return x;
# };"

# try 3 "
# int main()
# {
#     int x;
#     int *y;
#     int **z;
#     y = &x;
#     z = &y;
#     **z = 3;
#     return x;
# };"

# ================
# ポインタの加算減算
try_with_helper 4 "
int alloc4(int **ptr, int i0, int i1, int i2, int i3);

int main()
{
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    q = p + 2;
    return *q;
};"

try_with_helper 8 "
int alloc4(int **ptr, int i0, int i1, int i2, int i3);

int main()
{
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    q = p + 2;
    *q;
    q = p + 3;
    return *q;
};"

echo OK

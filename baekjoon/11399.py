from functools import reduce

num = int(input())
arr = input().split(" ")
for i in range(num) :
    arr[i] = int(arr[i])
arr.sort()
print(arr)
sum = 0
for i in range(1,num) :
    sum += reduce(lambda x,y : x + y , arr[:i+1])
   
print(sum + arr[0])
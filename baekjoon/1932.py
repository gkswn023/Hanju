num = int(input())
arr = list()
answer = list()

for i in range(num) :
    tmp = input().split(" ")
    for j in range(len(tmp)) :
        tmp[j] = int(tmp[j])
    arr.append(tmp)

answer.append(arr[0])

for i in range(num) :
    if i == 0 : continue
    tmp = []
    for j in range(len(arr[i])) :
        if j == 0 :
            tmp.append(answer[i-1][0] + arr[i][0])
        elif(j != len(arr[i]) - 1) :
            tmp.append(max(answer[i-1][j-1], answer[i-1][j]) + arr[i][j])
        else :
            tmp.append(answer[i-1][j-1] + arr[i][j])
    answer.append(tmp)

answer[-1].sort(reverse = True)
print(answer[-1][0])




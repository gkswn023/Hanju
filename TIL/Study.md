1.  그리디(Greedy)

   직관적인 판단 필요.

   시간 제한이 매우 빡빡함

   풀이를 생각할 때,  대부분 "가장 큰~", "가장 긴~", "가장 빠른~ " 것 부터 해나가야하는 풀이가 대부분이다. 즉 정렬이 필요한 문제가 많음

   

2.  이분탐색(Binary Search)

3.  해시 

    dictionary 문제

    해시 테이블로 비교하는게 필요할 경우, 비교하는데 시간복잡도가 매우 좋아야 할 경우

4.  BFS

5.  DFS

6. DP

   점화식을 잘 세우자.

   기존 것을 어떻게 잘 활용할 것인가 생각하기

7. 트라이구조

   백과사전같은거 검색할 때, 문자열 prefix가 있을 때 빠르게 탐색 가능



- Sort 함수

  List  = []

  1.  List.sort()

  2. sorted(List)

  Key 사용 

  (key = lambda x : x[?])

  **x[0], x[1], -x[1] 등 다양하게 이용가능 **

  내림차순 적용

  (reverse = True)

  

- Reduce 함수

  from functools import reduce 로 사용

  list의 sum을 구하거나 할 때 사용

  arr = [1,2,3,4,5] 일때

  reduce(lambda x ,y : x + y , arr) 

  이면 ((((1+2)+3)+4)+5) 이런식의 연산 

  factorial 같은거 구할 때도 편하다.










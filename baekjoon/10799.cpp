#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<stack>
#include<vector>

using namespace std;

int main() {
	
	char bar[100001];
	stack<char> s;
	int i = 0;
	int answer = 0;

	cin >> bar;

	while (bar[i] == '(' || bar[i] == ')') {
		if(bar[i] == '(')
			s.push(bar[i]);
		else {
			s.pop();
			if (i > 0 && bar[i - 1] == ')')
				answer += 1;
			else answer += s.size();
		}
		i++;
	}

	cout << answer << endl;

	return 0;
}


#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<queue>
#include<string>
#include<vector>

using namespace std;

int main() {
	
	queue<int> q;
	int num;
	int a;
	string cmd;
	
	cin >> num;

	for (int i = 0; i < num; i++) {
		cin >> cmd;
		if (cmd == "push") {
			cin >> a;
			q.push(a);
		}
		else if (cmd == "front") {
			if (q.empty()) printf("-1\n");
			else cout << q.front() << endl;
		}
		else if (cmd == "back") {
			if (q.empty()) printf("-1\n");
			else cout << q.back() << endl;
		}
		else if (cmd == "size") {
			cout << q.size() << endl;
		}
		else if (cmd == "empty") {
			if (q.empty()) printf("1\n");
			else printf("0\n");
		}
		else if (cmd == "pop") {
			if (q.empty()) cout << "-1" << endl;
			else {
				cout << q.front() << endl;
				q.pop();
			}
		}
	}

	return 0;
}


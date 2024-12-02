// #include<vector>
// #include<iostream>
// using namespace std;

// int main(){
//     int n;
//     cin >> n;
//     vector<int> connect(n, -1);
//     int val;
//     for (int i = 0; i < n; i++)
//         cin >> connect[i];

//     int length = -1;
//     for (int index = 0; index < n; index++)
//     {
//         if (connect[index] == -1)
//             continue;
//         vector<int> ori(connect);
//         int cur = index, begin = index, cnt = 0;
//         while (ori[cur] != -1)
//         {
//             //确定路径
//             int dest = ori[cur];
//             ori[cur] = -1;
//             cur = dest;
//             cnt++;
//         }

//         //结束处理
//         if (cur == begin)
//         {
//             length = max(cnt, length);
//             connect = ori;
//         }
//     }
//     if (length == 0)
//         cout << -1;
//     else
//         cout << length;
//     return 0;
// }

#include <vector>
#include <iostream>
using namespace std;

int main() {
    int n;
    cin >> n;
    vector<int> edges(n);
    for (int i = 0; i < n; i++) {
        cin >> edges[i];
    }

    vector<int> state(n, 0);  // 0: 未访问, 1: 正在访问, 2: 已访问
    int longest_cycle = -1;

    for (int i = 0; i < n; i++) {
        if (state[i] == 2) continue;

        vector<int> path;  // 存储当前路径上的节点
        int current = i;

        while (current != -1 && state[current] != 2) {
            if (state[current] == 1) {
                int cycle_start = current;
                int cycle_length = 0;
                for (int j = path.size() - 1; j >= 0; j--) {
                    cycle_length++;
                    if (path[j] == cycle_start) break;
                }
                longest_cycle = max(longest_cycle, cycle_length);
                break;
            }

            state[current] = 1;
            path.push_back(current);
            current = edges[current];
        }

        for (int node : path) {
            state[node] = 2;
        }
    }

    cout << longest_cycle << endl;
    return 0;
}

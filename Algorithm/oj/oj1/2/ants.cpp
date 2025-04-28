#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct ant
{
    long long pos;
    int dir;	// 0: right, 1: left

    bool operator<(const ant& other) const
    {
        return pos < other.pos;
    }
};

long long merge(vector<ant>& arr, vector<ant>& temp, int left, int mid, int right) 
{
    int i = left;
    int j = mid + 1;
    int k = left;
    long long count = 0;
    
    while (i <= mid && j <= right) 
	{
        // ！！优先选择后方的蚂蚁
        if (arr[i].pos < arr[j].pos) 
		{
            temp[k++] = arr[i++];
        } 
		else 
		{
            count += mid - i + 1;
            temp[k++] = arr[j++];
        }
    }
    
    while (i <= mid) 
	{
        temp[k++] = arr[i++];
    }
    
    while (j <= right) 
	{
        temp[k++] = arr[j++];
    }
    
    for (i = left; i <= right; i++) 
	{
        arr[i] = temp[i];
    }
    
    return count;
}

long long merge_sort(vector<ant>& arr, vector<ant>& temp, int left, int right) 
{
    long long count = 0;
    if (left < right) 
	{
        int mid = left + (right - left) / 2;
        
        count += merge_sort(arr, temp, left, mid);
        count += merge_sort(arr, temp, mid + 1, right);
        
        count += merge(arr, temp, left, mid, right);
    }
    return count;
}

void solution()
{
    int n;
    long long m;
    cin >> n >> m;
    vector<ant> ants(n);	

    for (int i = 0; i < n; i++)
    {
        long long p;
        int d;
        cin >> p >> d;
        // 相对运动速度为 1，即往右速度为 0，往左速度为 -1
        ants[i] = {p, -d};
    }
    sort(ants.begin(), ants.end());
    
    for(auto& a:ants)
    {
        a.pos += a.dir * m;
    }

	vector<ant> temp(n);
	long long ans = merge_sort(ants, temp, 0, n - 1);

    cout << ans << endl;
}

int main()
{
    int t;
    cin >> t;
    for (int i = 0; i < t;i++)
    {
        solution();
    }

    return 0;
}
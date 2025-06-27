#include <iostream>
#include <vector>
using namespace std;

void merge_Count(vector<long long> &num, int left, int mid, int right, vector<long long> &inv_count)
{
	vector<long long> temp(num);

	int i = left;
	int j = mid + 1;
	int k = left;

	while (i <= mid && j <= right)
	{
		if (temp[i] <= temp[j])
		{
			num[k++] = temp[i++];
		}
		else
		{
			inv_count[temp[j]] += (mid - i + 1);
			num[k++] = temp[j++];
		}
	}

	while (i <= mid)
	{
		num[k++] = temp[i++];
	}

	while (j <= right)
	{
		num[k++] = temp[j++];
	}
}

void mergeSort_Count(vector<long long> &num, int left, int right, vector<long long> &inv_count)
{
	if (left < right)
	{
		int mid = left + (right - left) / 2;

		mergeSort_Count(num, left, mid, inv_count);
		mergeSort_Count(num, mid + 1, right, inv_count);
		merge_Count(num, left, mid, right, inv_count);
	}
}

vector<long long> count_inversion(vector<long long> &num)
{
	vector<long long> ori_num(num);
	vector<long long> cnt(num.size(), 0);

	mergeSort_Count(num, 0, num.size() - 1, cnt);
	for (int i = 0; i < num.size(); i++)
	{
		cnt[i] *= 2;
		cnt[i] += ori_num[i] - i;
	}

	return cnt;
}

void get_sum(vector<long long>& num)
{
	long long sum = 0;
	vector<long long> cnt = count_inversion(num);

	for (int i = 0; i < num.size();i++)
		sum += cnt[i];
	sum /= 2;

	cout << sum << endl;
}
void get_cnt(vector<long long>& num)
{
	vector<long long> cnt = count_inversion(num);

	for (int i = 0; i < num.size();i++)
		cout << cnt[i] << " ";
	cout << endl;
}

int main()
{
	long long T;
	cin >> T;

	for (long long i = 0; i < T; i++)
	{
		long long n, op;
		vector<long long> num;
		cin >> n >> op;

		long long tmp;
		for (long long j = 0; j < n; j++)
		{
			cin >> tmp;
			num.push_back(tmp);
		}

		if (op == 0)
			get_sum(num);
		else
			get_cnt(num);
	}

	return 0;
}
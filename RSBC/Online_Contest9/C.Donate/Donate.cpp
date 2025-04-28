#include<iostream>
#include<vector>
using namespace std;

int main(){
    long long N, M;
    cin >> N >> M;
    long long donate = 0;
    long long above_zero = 0;

    vector<long long> H(N);
    for (long long i = 0; i < N; i++)
    {
        cin >> H[i];
        donate += H[i];
        if (H[i] > 0)
            above_zero++;
    }

    donate -= M;
    long long average = 0;
    long long ans = 0;

    while (donate >= above_zero)
    {
        long long sz = H.size();
        average = donate / above_zero;
        donate = donate % above_zero;

        above_zero = 0;
        ans += average;
        for (long long i = 0; i < sz; i++)
        {
            if (H[i] <= 0)
                continue;
            else
                H[i] -= average;

            if (H[i] <= 0)
            {
                donate -= H[i];
                H[i] = 0;
            }
            else
                above_zero++;
        }
    }
    cout << ans;
    return 0;
}
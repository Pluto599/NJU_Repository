#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Washer {
    int start, end;
};

bool compare(Washer a, Washer b) 
{
    return a.end < b.end;
}

int main() {
    int n;
    cin >> n;

    vector<Washer> washers(n);

    for (int i = 0; i < n; i++) 
    {
        cin >> washers[i].start >> washers[i].end;
    }

    sort(washers.begin(), washers.end(), compare);

    int count = 0;
    int last_end_time = 0;

    for (int i = 0; i < n; i++) 
    {
        if (washers[i].start >= last_end_time) 
        { 
            count++;
            last_end_time = washers[i].end;
        }
    }

    cout << count;

    return 0;
}

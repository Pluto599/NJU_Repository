#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
using namespace std;

struct range
{
  int left; // 左边界
  int right; // 右边界

  range(int s, int f) 
  {
    left = s;
    right = f;
  }

  bool in_range(int x) const  // x in range r
  {
    return (left <= x && x <= right);
  }

  bool operator<(const range& other) const {
    if (right != other.right)
      return right < other.right;

    // 右边界相同时，按左边界排序
    return left < other.left;
  }
};

int main()
{
  unordered_map<int, set<range>> monsters;
  unordered_map<int, set<int>> bugs;
  int cnt = 0;

  int n;
  cin >> n;
  for (int i = 0; i < n; i++)
  {
    bool id;
    int s, f, y, z;
    cin >> id >> s >> f >> y >> z;

    if (id == 0)
    {
      range r(s, f);
      monsters[y].insert(r); 
    }
    else
    {
      if(s==f)
        bugs[y].insert(s);
    }
  }

  for (auto &group : monsters)
  {
    auto &objects = group.second;
    
    while(!objects.empty())
    {
      auto it = objects.begin();
      range r = *it;
      objects.erase(it);
      
      int shoot_position = r.right;
      cnt++;

      vector<range> to_remove;
      for (auto &obj : objects) {
        if (obj.in_range(shoot_position)) {
          to_remove.push_back(obj);
        }
      }
      
      for (const auto &obj : to_remove) {
        objects.erase(obj);
      }
    }
  }

  cout << cnt;

  return 0;
}
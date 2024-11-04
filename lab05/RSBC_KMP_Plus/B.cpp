#include<iostream>
#include<string>
#include<vector>
using namespace std;

class KMP_plus{
public:
    KMP_plus();
    string KMP(string &T);

private:
    vector<int> KMP();
    vector<int> build_next();
    vector<int> next;
    string patt[2], text;
    int patt_length;
};

KMP_plus::KMP_plus(){
    string p;
    cin >> p;
    patt_length = p.length();
    int index = p.find('*');
    if (index == 0 || index == p.length() - 1)
    {
        p.erase(p.begin() + index);
        patt[0] = p;
        build_next();
        patt[1] = (index == 0) ? "head" : "tail";
    }
    else
    {
        patt[0] = p.substr(0, index);
        build_next();
        patt[1] = p.substr(index + 1);
    }
}

string KMP_plus::KMP(string &T){
    text = T;

    if (patt[0].empty())
        return "true";
    vector<int> index = KMP();

    // 处理*在开头或结尾情况
    if (patt[1] == "head")
        return (!index.empty()) && (index[0] != 0) ? "true" : "false";
    else if (patt[1] == "tail")
        return (!index.empty()) && (index[0] + patt[0].length() != text.length()) ? "true" : "false";
    else if (text.length() < patt[0].length() + patt[1].length() + 1)
        return "false";

    //处理*在字符串中间情况
    for (int i = 0; i < index.size(); i++)
    {
        if (index[i] + patt_length > text.length())
            return "false";
        if (patt[1] == text.substr(index[i] + patt[0].length() + 1, patt[1].length()))
            return "true";
    }
    return "false";
}

/// @brief 寻找与前缀匹配的下标位置
/// @return 位置向量
vector<int> KMP_plus::KMP()
{
    vector<int> index;
    int i = 0;//主串指针下标
    int j = 0;//子串指针下标

    //匹配过程
    while (i < text.length()){
        if(text[i]==patt[0][j])
        {
            i++;
            j++;
        }
        else if (j > 0)
            j = next[j - 1];
        else if (j == 0)
            i++;

        if (j == patt[0].length()){
            index.push_back(i - j);
            j = next[j - 1];
        }
    }
    //匹配结束处理
    return index;
}

vector<int> KMP_plus::build_next()
{
    next.push_back(0);
    int next_len = 0;//记录当前最长匹配前后缀长度
    int i = 1;//记录处理位置
    while (i < patt[0].length())
        if (patt[0][next_len] == patt[0][i])
        {
            next_len++;
            i++;
            next.push_back(next_len);
        }
        else if (next_len == 0)
        {
            i++;
            next.push_back(0);
        }
        else
        {
            next_len = next[next_len - 1];
        }

    return next;
}

int main(){
    int n;
    string text;
    cin >> n;
    KMP_plus K;

    for (int i = 0; i < n; i++)
    {
        cin >> text;
        cout << K.KMP(text) << endl;
    }

    return 0;   
}
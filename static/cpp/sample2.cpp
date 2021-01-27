#include <bits/stdc++.h>
using namespace std;

int main() {
    string s, t;
    cin >> s >> t;
    
    map<char, vector<int>> pos;
    for(int i=0; i<s.size(); ++i) {
        pos[s[i]].push_back(i);
    }

    int cur;
    cur = -1;
    int back;
    back = 0;
    for(int i=0; i<t.size(); ++i) {
        char c;
        c = t[i];
        if(pos[c].size() == 0) {
            cout << -1 << endl;
            return 0;
        }
        auto itr = upper_bound(begin(pos[c]), end(pos[c]), cur);
        if(itr != end(pos[c])) {
            cur = *itr;
        } else {
            ++back;
            cur = pos[c][0];
        }
    }
    long long ans = back;
    ans *= s.size() + cur + 1; 
    cout << ans << endl;
}

#include <iostream>
#include <map>
using namespace std;
typedef long long int ll;

map<ll, ll> prime;
void factorize(int n) {
    for(int i = 2; i * i <= n; i++) {
        while(n % i == 0) {
            prime[i]++;
            n /= i;
        }
    }
    if(n != 1) {
        prime[n] = 1;
    }
}

int main() {
    int n;
    cin >> n;
    int ans  = 0;
    factorize(n);
    for(auto it = prime.begin();it != prime.end(); it++) {
        ans = ans / it->first;
        ans = ans * (it->first - 1);
    }
    cout << ans << endl;
}
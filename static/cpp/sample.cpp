#include <iostream>
#include <map>
using namespace std;
typedef long long int ll;
#define irep(it, stl) for (auto it = stl.begin(); it != stl.end(); it++)

map<ll, ll> prime;
void factorize(int n) {
    for (int i = 2; i * i <= n; i++) {
        while (n % i == 0) {
            prime[i]++;
            n /= i;
        }
    }
    if (n != 1) {
        prime[n] = 1;
    }
}

int main() {
    int n;
    cin >> n;
    int ans = n;
    factorize(n);
    irep(i, prime) {
        ans /= i->first;
        ans *= i->first - 1;
    }
    cout << (ans) << endl;
}

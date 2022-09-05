#include <iostream>

using namespace std;

int main(){
    int n = 0;
    cin >> n;
    double sqrt = n / 2;
    double temp = 0;
    while(sqrt != temp){
        temp = sqrt;
        sqrt = (n/temp + temp) / 2;
        cout << sqrt << endl;
    }
    system("pause");
    return 0;
}
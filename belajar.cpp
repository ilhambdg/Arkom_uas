#include <iostream>
#include <vector>

using namespace std;


void menu(){
    bool tes = true;
    int input;
    vector <int> isi;

    while (tes) {
        
        cout << "> ";
        cin >> input;
        if(cin.fail()) {
            cout<< "eror";
            break;
        }
        isi.push_back(input);
        

    }
    
}

int main(){
    cout << "Masukan kumpulan angka mu \n";
    menu();

    return 0;
}
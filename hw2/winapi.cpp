#include <windows.h>
#include <iostream>
#include <vector>
#include <random>


using namespace std;

struct ThreadData
{
    vector<int> rowData;
    int rowIndex;
    int result=1;
};

DWORD WINAPI ThreadCalc(LPVOID Thread){
    ThreadData* data = (ThreadData*)Thread;
    data->result=1;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 100);

    for(int& value: data->rowData){
        value = dis(gen);
        if (value % 2 != 0){
            data->result *=value;
        }
    }
    return 0;
};


int main(){
    int rows,cols;
    cout << "Enter number of rows: ";
    cin >> rows;
    cout << "Enter number of columns: ";
    cin >> cols; 

    vector<ThreadData> matrix(rows, {vector<int>(cols)});
    vector<HANDLE> threads(rows);

    for (int i=0; i < rows; ++i){
        matrix[i].rowIndex = i;
        threads[i] = CreateThread(
            nullptr,
            0,
            ThreadCalc,
            &matrix[i],
            0,
            nullptr
        );

        if (!threads[i]){
            cerr << "Error while creating " << i << "thread"<<endl;
        }
    }

    WaitForMultipleObjects(rows, threads.data(), true, INFINITE);

    for (int i = 0; i < rows; ++i) {    
        CloseHandle(threads[i]);

        cout << "Row " << i << ": ";
        for (int value : matrix[i].rowData) {
            cout << value << " ";
        }
        cout << "| Result: " << matrix[i].result << endl;
    }
    return 0;
}
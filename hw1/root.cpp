#include <iostream>
#include <math.h>
#include <xmmintrin.h>
#include <chrono>

using namespace ::std;

void normal_sqrt(float *a, float *b, int N)
{
    for (int i = 0; i < N; ++i)
        b[i] = sqrt(a[i]);
}

void paralel_sqrt(float *a, float *b, int N)
{
    // можем использовать SSE когда число элементов кратно 4, т.к. SSE обрабатывает данные в 128-битных регистрах, а float занимает 32 бита, т.е. один регистр может одновременно обработать 4 числа.
    __m128 *a_m128 = (__m128 *)a;
    __m128 *b_m128 = (__m128 *)b;

    int steps = N / 4;

    for (int i = 0; i < steps; ++i)
    {
        b_m128[i] = _mm_sqrt_ps(a_m128[i]);
    }
    for (int i = steps * 4; i < N; ++i)
    {
        b[i] = std::sqrt(a[i]);
    }
}

bool test(float *a, float *b, int N)
{
    for (int i = 0; i < N; i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

int main()
{
    const int samples = 100000;
    float x[samples], res_def[samples], res_sse[samples];
    for (int i = 0; i < samples; i++)
    {
        x[i] = int(rand());
    }

    auto start_default = chrono::high_resolution_clock::now();
    normal_sqrt(x,res_def,samples);
    auto end_default = chrono::high_resolution_clock::now();

    auto start_sse = chrono::high_resolution_clock::now();
    paralel_sqrt(x,res_sse,samples);
    auto end_sse = chrono::high_resolution_clock::now();

    auto duration_def = chrono::duration_cast<std::chrono::nanoseconds>(end_default - start_default).count();
    auto duration_sse = chrono::duration_cast<std::chrono::nanoseconds>(end_sse - start_sse).count();

    if(test(res_sse, res_def, samples)){
        cout << "Calculation complete" << endl;
        
    }else{
        cout << "Numerical mistake" << endl;
        return -1;
    }
    cout << "Time spent for " << samples << " vector length is:" << endl;
    cout << "By default:" << duration_def  << " ns"<< endl;
    cout << "By SSE:" << duration_sse << " ns"<< endl;
}
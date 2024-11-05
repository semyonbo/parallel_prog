#include <iostream>
#include <math.h>
#include <xmmintrin.h>
#include <chrono>

using namespace ::std;

float dot(float *x, float *y, int n)
{
    float s = 0;
    for (int i = 0; i < n; i++)
        s += x[i] * y[i];
    return s;
}

float dot_sse(float *x, float *y, int n)
{
    __m128 sum = _mm_setzero_ps();
    int i;
    for (i = 0; i <= n - 4; i += 4)
    {
        __m128 x_vec = _mm_loadu_ps(&x[i]);
        __m128 y_vec = _mm_loadu_ps(&y[i]);
        __m128 prod = _mm_mul_ps(x_vec, y_vec); 
        sum = _mm_add_ps(sum, prod);         
    } 
    
    float result[4];
    _mm_storeu_ps(result, sum);
    float scalar_sum = result[0] + result[1] + result[2] + result[3];

    for (; i < n; ++i)
    {
        scalar_sum += x[i] * y[i];
    }
    return scalar_sum;
}

bool test(float a, float b)
{
    float err = 10e-8

    ;
    if (abs(a - b) < err)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main()
{
    const int samples = 100000;
    float x[samples], y[samples];
    for (int i = 0; i < samples; i++)
    {
        x[i] = int(rand()/1e8);
        y[i] = int(rand()/1e8);
    }

    auto start_default = chrono::high_resolution_clock::now();
    float dot_def = dot(x,y,samples);
    auto end_default = chrono::high_resolution_clock::now();

    auto start_sse = chrono::high_resolution_clock::now();
    float dot_sse1 = dot_sse(x,y,samples);
    auto end_sse = chrono::high_resolution_clock::now();

    auto duration_def = chrono::duration_cast<std::chrono::nanoseconds>(end_default - start_default).count();
    auto duration_sse = chrono::duration_cast<std::chrono::nanoseconds>(end_sse - start_sse).count();

    if(test(dot_def, dot_sse1)){
        cout << "Calculation complete" << endl;
        
    }else{
        cout << "Numerical mistake" << endl;
        return -1;
    }
    cout << "Time spent for " << samples << " vector length is:" << endl;
    cout << "By default:" << duration_def  << " ns"<< endl;
    cout << "By SSE:" << duration_sse << " ns"<< endl;
}
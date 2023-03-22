/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

// Online C++ compiler to run C++ program online
#include <iostream>
using namespace std;

int main() {
    // Write C++ code here
    int n = 4;
    int arr[4] = {6,6,6,7};
  long long ans = 0;
        long long area = 0;
        long long l_area = 0;
        long long r_area = 0;
        //for each element finding right and left area
        for(int i=0; i<n; i++)
        {
            int j=i-1;
            int k=i+1;
            
            while( (j>=0) && (arr[j]>=arr[i]) )
            {
                l_area += arr[i];
                j--;
            }
            while( (k<sizeof(arr)) && (arr[k]>=arr[i]))
            {
                r_area += arr[i];
                k++;
            }
            area = l_area+r_area+arr[i];
            ans = max(area,ans);
            l_area = 0;
            r_area = 0;
            area = 0;
        }
        cout<<ans;
}
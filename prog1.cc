#include <iostream>
#include <malloc.h>

using namespace std;

bool check(int array[], int length)
{
	for(int i =1;i<length;i++)
	{
		if(array[i] != 0)
			return false;
	}

	return true;
}

int getDigit(int number,int position)
{
    int curr = 1;
    int rem;
    while(curr <= position)
    {
        rem = number%10;
        number = number/10;
        curr++;
    }
    return rem;
}

void radixsort(int array[], int length)
{
    bool done = false;
    //cout<<"here1";
    while(!done)
    {
    int * temp = (int*)malloc(sizeof(int)*length);
    int * temp1 = (int*)malloc(sizeof(int)*length);

    int digit = 1;
    for(int i=0;i < length;i++)
    {
        temp[i] = array[i];
    }
    cout<<"here";
    int bucket[10] = {0};

    for(int i =0; i < length;i++)
    {
        int k = getDigit(temp[i],digit);
        bucket[k]++;
        temp1[i] = k;
    }

    done = check(bucket,length);

    int sum = bucket[0];

    for(int i=1; i<length;i++)
    {
        bucket[i] = bucket[i] + sum;
        sum = bucket[i];
    }

    for(int i = length-1;i>=0 ;i--)
    {
        int k = bucket[temp1[i]];
        array[k] = temp[i];
    }
        digit++;
    }

}

int main()
{
    //cout<<"in main";
	int array[] = {42,32,1032,45,33,89,16,14,11,23,111,18};
	int length = sizeof(array)/sizeof(array[0]);
	cout<<"Before sorting";
	radixsort(array,length);
	cout<<"After sorting\n";
	for(int i =0; i < length; i++)
		cout<<array[i]<< " ";
	cout<<endl;
	return 0;
}

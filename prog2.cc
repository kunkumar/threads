#include <iostream>
#include <malloc.h>

using namespace std;

int getDigit(int number, int ith)
{
	int k=1,rem;
	while(k <= ith)
	{
		rem = number%10;
		number = number/10;
		k++;
	}
	return rem;
}

void print(int array[], int length)
{
	for(int i =0; i <length; i++)
		cout<<array[i]<<" ";

	cout<<endl;
}

bool check(int array[], int length)
{
	for(int i =1;i<length;i++)
	{
		if(array[i] != 0)
			return false;
	}
	//cout<<"ret true\n";
	return true;
}

void radixsort(int array[], int length)
{
	bool done = false;
	int dig =1;
	int size = length;
	while(!done){
	//int bucket[size] = {0};
	int * bucket = (int*)malloc(size*sizeof(int));
	for(int i = 0;i<length;i++)
	{
		bucket[getDigit(array[i],dig)]++;
	}
	//bucket[0];
	//cout<<"asdjksdkjf\n";
	//print(bucket,length);
	done = check(bucket,length);
	if(done)
		return;
	int u =0;
	for(int i=0;i<length;i++)
	{
		bucket[i] = bucket[i] + u;
		u = bucket[i];
	}

	//print(bucket,length);
	int size = bucket[length-1]+1;
	//cout<<"size = "<<size<<endl;
	int newarray[size];

	for(int i = length-1; i>=0; i--)
	{
		int k = bucket[getDigit(array[i],dig)];
		//cout<<k;
		newarray[k] = array[i];
		--bucket[getDigit(array[i],dig)];
	}

	//print(newarray,length+1);
	for(int i =0;i<length;i++)
		array[i] = newarray[i+1];
	dig++;
	//print(array,length);
	}
}

int main()
{
	int array[] = {42,32,1032,45,33,89,16,14,11,23,111,18};
	int length = sizeof(array)/sizeof(int);
	for(int i =0;i<length;i++)
	{
		cout<<array[i]<<" ";
	}
	cout<<endl;
	/*for(int i =0;i<length;i++)
	{
		cout<<getDigit(array[i],2)<<"  ";
	}
	cout<<endl;*/
	radixsort(array,length);
	cout<<"After sorting\n";
	for(int i =0;i<length;i++)
	{
		cout<<array[i]<<" ";
	}
	cout<<endl;
	return 0;
}

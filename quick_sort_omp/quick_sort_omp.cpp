#include <omp.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <iostream>

using namespace std;

/*!
* quickSort - реалізація алгоритму швидкого сортування
* check_calculations - перевірка правильності сортування
* @param arr - масив, що сортується
* @param number_of_elements - індекс останнього елементу
* @param number_of_threads - кількість потоків
* @param pivot - опорний елемент
* @param time_taken - час затрачений на виконання алгоритму
*/

void quickSort(int num, int* arr, const long number_of_elements) {
	long i = 0, j = number_of_elements;
	int pivot = arr[number_of_elements / 2];

	do {
		while (arr[i] < pivot) i++;
		while (arr[j] > pivot) j--;

		if (i <= j) {
			std::swap(arr[i], arr[j]);
			i++; j--;
		}
	} while (i <= j);

	if (number_of_elements < 100) {
		if (j > 0) quickSort(num * 2 + 1, arr, j);
		if (number_of_elements > i) quickSort(num * 2 + 2, arr + i, number_of_elements - i);
		return;
	}

#pragma omp task shared(arr)
	{
		if (j > 0) quickSort(num * 2 + 1, arr, j);
	}  
#pragma omp task shared(arr)
	{
		if (number_of_elements > i) quickSort(num * 2 + 2, arr + i, number_of_elements - i);
	} 
#pragma omp taskwait
}

void check_calculations(int* arr, int number_of_elements) {
	int i;
#pragma omp parallel \
	for private(i)shared(arr, number_of_elements)
		for (i = 1; i < number_of_elements; ++i) {
			if (arr[i] < arr[i - 1]) {
				printf("error\n");
				exit(1);
			}
		}
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("error arg\n");
		return 1;
	}

	int i;
	int number_of_elements;
	int number_of_threads;
	double time_taken;
	sscanf(argv[1], "%d", &number_of_elements);
	sscanf(argv[2], "%d", &number_of_threads);
	int* arr = new int[number_of_elements];

	omp_set_num_threads(number_of_threads);
	
#pragma omp parallel \
	for private(i)shared(number_of_elements, arr)
		for (i = 0; i < number_of_elements; ++i)
			arr[i] = rand();

	time_taken = omp_get_wtime();
#pragma omp parallel shared(arr)
	{
#pragma omp single nowait 
		{
			quickSort(0, arr, number_of_elements - 1);
		}
	}

	time_taken = omp_get_wtime() - time_taken;

	check_calculations(arr, number_of_elements);

	cout << "---------------------------------------\n";
	printf("Time is: %f seconds;\n"
		"Number of threads : % d;\n"
		"Amount of sorted elements : % d\n", 
		time_taken, 
		number_of_threads, 
		number_of_elements);
	cout << "---------------------------------------\n";
	delete[]arr;
	return 0;
}
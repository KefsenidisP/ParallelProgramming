/*
 * The algorithm detects the kth element in the array, if the
 * array was sorted. The array at the beginning is unsorted.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define Q 5
#define MAX_NUM 100

void print_array(int * array, int size)
{
    for (int i = 0; i < size; ++i)
        printf("%d ", array[i]);
    printf("\n");
}

/*
 * Sort the array chunk of size *size* and then return
 * the element at index k - 1.
*/
int SortLessThanQ(int * array, int size, int k)
{
    for (int i = 0; i < size; ++i)
    {
        int max_idx = i;

        for (int j = i + 1; j < size; ++j)
            if (array[max_idx] > array[j])
                max_idx = j;

        if (max_idx != i)
        {
            int tmp = array[max_idx];
            array[max_idx] = array[i];
            array[i] = tmp;
        }
    }

    return array[k - 1];
}

/*
 * As the previous one but with size Q.
 * Note: there is an algorithm that sorts a 5 element array with only 7 comparissons
*/
int SortSelect5(int * sub_array, int pos) 
{
    // Here I tried to implement this algorithm
    // int min_ab, min_cd;
    // int max_abcd;
    //
    // min_ab = ((sub_array[0] < sub_array[1]) ? sub_array[0] : sub_array[1]);
    // min_cd = ((sub_array[2] < sub_array[3]) ? sub_array[2] : sub_array[3]);
    //
    // max_abcd = ((min_ab < min_cd) ? min_cd : min_ab);
    //
    // if (max_abcd < sub_array[3])

    for (int i = 0; i < Q; ++i)
    {
        int max_idx = i;

        for (int j = i + 1; j < Q; ++j)
            if (sub_array[max_idx] > sub_array[j])
                max_idx = j;

        if (max_idx != i)
        {
            int tmp = sub_array[max_idx];
            sub_array[max_idx] = sub_array[i];
            sub_array[i] = tmp;
        }
    } 

    return sub_array[pos - 1];
}

/*
 * Mark the elements of th array that are equal, greater or lesser than the 
 * currently selected median
*/
void CountAndMark(int S[], int Marks[], int num, int median, int leg[3])
{
    for (int i = 0; i < num; i++) 
    {
        if (S[i] < median) {Marks[i] = 0; leg[0]++;} //less than
        else if (S[i] > median) {Marks[i] = 2; leg[2]++;} // greater than
        else {Marks[i] = 1; leg[1]++;} // equal to
    }
}

/*
 * Create a new array (sPack) from the S array consisting of medians, found 
 * by the SortSelect5 algorithm.
*/
void ArrayPack(int S[], int sPack[], int num, int Marks[], int scanSym)
{
    int j = 0;
    
    for (int i = 0; i < num; i++)
        if (Marks[i] == scanSym) sPack[j++] = S[i];
}

/*
 * This is the main algorithm.
*/
int SequentialSelect(int *S, int num, int k)
{
    // The size of the array S is equal or less than Q,
    // so sort it and return the kth element.
    if (num <= Q) return SortLessThanQ(S, num, k);
    
    int cNum = num / Q + 1; // This is the new size of the array, when calling again SequentialSelect
    int * Medians = (int *) calloc(cNum, sizeof(int));
    int i = 0;
    
    // Cut the array in sub arrays of size Q each and get their medians
    for (int j = 0; j < num / Q; j++) 
    {
        Medians[j] = SortSelect5(&S[i], 3); // find medians of subsequences
        i += Q;
    }

    // This is in case the division is not perfect (num / Q != 0)
    int lastNum = num - (Q * (num / Q));
    
    if (lastNum) 
    {
        int lastQ = Q * (num / Q);
        Medians[cNum-1] = SortLessThanQ(&S[lastQ], lastNum, (lastNum+1)/2);
    }

    else cNum--;
    
    // The median of medians will be returned here, after the end of all recursions
    int M = SequentialSelect(Medians, cNum, (cNum+1)/2); 
    
    // This array will contain the size of other arrays, after the CountAndMark function is called
    int leg[3] = {0,0,0};
    
    // Marking array used later by the ArrayPack function, to create the sub array of medians
    int *markS = (int *) calloc(num, sizeof(int));
    
    CountAndMark(S, markS, num, M, leg);

    // After the array is marked, do one of the following, according to the comparisson between
    // leg[i] and k
    if (leg[0] >= k)
    {
        int *sPack = (int *) calloc(leg[0], sizeof(int)); // An array of medians
        ArrayPack(S, sPack, num, markS, 0);
        return SequentialSelect(sPack, leg[0], k);
    }
    else if ((leg[0] + leg[1]) >= k) return M;
    else 
    {
        int *sPack = (int *) calloc(leg[2], sizeof(int)); // An array of medians
        ArrayPack(S, sPack, num, markS, 2);
        return SequentialSelect(sPack, leg[2], k-(leg[0]+leg[1]));
    }
}

void array_init(int * array, int size)
{
    for (int i = 0; i < size; ++i)
        array[i] = rand() % MAX_NUM + 1;
}

int main(int argc, char * argv[])
{
    int * arr;
    int size;
    int k;

    if (argc != 3)
    {
        printf("Usage: <%s> <size> <kth_element_to_search>\n", argv[0]);
        exit(1);
    }

    size = strtol(argv[1], NULL, 10);
    k = strtol(argv[2], NULL, 10);

    arr = (int *) malloc(size * sizeof(int));

    array_init(arr, size);

    clock_t time_clock = clock();
    int kth_el = SequentialSelect(arr, size, k);
    time_clock = clock() - time_clock;

    double time_elapsed = ((double) time_clock) / CLOCKS_PER_SEC;

    // printf("The kth element if the array was sorted would be: %d\n", kth_el);
    printf("Time elapsed: %f\n", time_elapsed);

    return 0;
}

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_CAPACITY 10

typedef enum
{
    exit_success,
    invalid_input,
    invalid_array_length,
    invalid_array_struct
} status_code_t;

status_code_t input(int *size, int array[]);
bool is_arr_size_valid(int size);
double get_average(int array[], int size);
void filter_gt_average(int array[], int size, double average, int new_array[], int *new_size);
void print_array(int array[], int size);
void print_err_msg(status_code_t status_code);

int main(void)
{
    int array[MAX_CAPACITY];
    int size;
    
    double average;

    int new_array[MAX_CAPACITY];
    int new_size;
    
    status_code_t result = exit_success;

    if ((result = input(&size, array)) != exit_success)
    {
        print_err_msg(result);
    }
    else
    {
        average = get_average(array, size);
        filter_gt_average(array, size, average, new_array, &new_size);
        print_array(new_array, new_size);
    }

    return result;
}

status_code_t input(int *size, int array[])
{
    status_code_t result = exit_success;

    printf("Введите имя входного файла для lab_02:\n");
    char filename[64];
    fgets(filename, 64, stdin);
    *strchr(filename, '\n') = '\0';
    printf("array filename = %s\n", filename);
    FILE* file = fopen(filename, "r");

    if (fscanf(file, "%d", size) != 1)
        result = invalid_input;
    else if (!(0 < *size && *size <= MAX_CAPACITY))
        result = invalid_array_length;
    else
    {
        int first_num;
        bool all_nums_are_same = true;

        for (int i = 0; i < *size; i++)
        {
            if (fscanf(file, "%d", &array[i]) != 1)
            {
                result = invalid_input;
                break;
            }
            printf("array[%d] = %d\n", i, array[i]);

            if (i == 0)
                first_num = array[0];
            else
                all_nums_are_same = all_nums_are_same && first_num == array[i];
        }

        if (result == exit_success && all_nums_are_same)
            result = invalid_array_struct;
    }

    fclose(file);    
    return result;
}

double get_average(int array[], int size)
{
    double result = 0.0;
    
    for (int i = 0; i < size; i++)
        result += (double)array[i];
    
    return result / size;
}

void filter_gt_average(int array[], const int size, double average, int new_array[], int *new_size)
{
    *new_size = 0;
    
    for (int i = 0; i < size; i++)
    {
        if (array[i] > average)
        {
            new_array[*new_size] = array[i];
            *new_size += 1;
        }
    }
}

void print_array(int array[], int n)
{
    printf("Результирующий массив: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d", array[i]);
        printf(i + 1 != n ? " " : "\n");
    }
}

void print_err_msg(status_code_t status_code)
{
    if (status_code == invalid_input)
        printf("incorrect input.\n");
    else if (status_code == invalid_array_length)
        printf("incorrect array length.\n");
    else if (status_code == invalid_array_struct)
        printf("incorrect array struct.\n");
}

#include <stdio.h>
#include <stdlib.h>

typedef struct list
{
    int value;
    struct list* next;
} list_t;

list_t* input_list(FILE* file)
{
    int val;
    if (fscanf(file, "%d", &val) != 1)
        return NULL;
    
    list_t* list = malloc(sizeof(list_t));
    list->value = val;
    list->next = input_list(file);
    return list;
}

void output_list(FILE* file, list_t* list)
{
    if (list == NULL)
        fprintf(file, "\n");
    else
    {
        fprintf(file, "%d ", list->value);
        output_list(file, list->next);
    }
}

list_t* reverse_list(list_t *list, list_t* tail)
{
    if (list == NULL)
        return tail;

    list_t* node = list;
    list = list->next;
    node->next = tail;
    return reverse_list(list, node);
}

void free_list(list_t* list)
{
    if (list != NULL)
        free_list(list->next);
    free(list);
}

int main(int argc, const char* argv[])
{
    char buffer[256];

    printf("lister input file: ");
    scanf("%s", buffer);

    FILE* file = fopen(buffer, "r");
    list_t* list = input_list(file);
    fclose(file);

    list = reverse_list(list, NULL);
    output_list(stdout, list);

    free_list(list);
    return EXIT_SUCCESS;
}

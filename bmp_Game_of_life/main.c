#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

//#define _CRT_SECURE_NO_WARNINGS

int count_size(unsigned char *info, unsigned int start, unsigned int end) {
    unsigned int count = 1, length = 0;
    for (unsigned int i = start; i < end; ++i)
    {
        length += count * info[i];
        count *= (16 * 16);
    }
    return length;
}


void completion_of_field(char **table, char *main_info, int HEIGHT, int WIDTH) {
    unsigned int count = 0;
    unsigned int RowSize = (WIDTH + 31) / 32 * 4;  // Число байтов в одной строке
    for (int i = HEIGHT - 1; i >= 0; --i)
    {
        for (int j = 0; j < RowSize; ++j)
        {
            // считываем один байт информации из нашего массива
            char byte = main_info[(HEIGHT - i - 1) * RowSize + j];
            for (int k = 0; k < 8; ++k) {
                // Разбиваем байт по битам
                if (8 * j + k < WIDTH) {
                    // битовый сдвиг для каждого бита
                    table[i][8 * j + k] = (char)!((byte >> (7 - k)) & 1);
                }
            }
        }
    }
}


void FromArr(char *arr, char **table, int HEIGHT, int WIDTH, int length)
{
    int RowSize = (WIDTH + 31) / 32 * 4; // Число байтов в одной строке
    for (int i = HEIGHT - 1; i >= 0; --i)
        for (int j = 0; j < RowSize; ++j)
        {
            // Инициализация одного байта
            int byte = 0;
            for (int k = 0; k < 8; ++k)
                if (8 * j + k < WIDTH)
                    // Накопление байта из битов
                    byte += ((!table[i][8 * j + k]) << (7 - k));
            // Заполняем массив
            arr[(HEIGHT - i - 1)*RowSize + j] = (char)byte;
        }
}


int count_neighbours(char **table, int i, int j, int HEIGHT, const int WIDTH) {
    int neighbours = 0;
    if (i + 1 < HEIGHT && j - 1 >= 0 && table[i + 1][j - 1] == 1) neighbours++;
    if (i + 1 < HEIGHT && table[i + 1][j] == 1) neighbours++;
    if (i + 1 < HEIGHT && j + 1 < WIDTH && table[i + 1][j + 1] == 1) neighbours++;
    if (j - 1 >= 0 && table[i][j - 1] == 1) neighbours++;
    if (j + 1 < WIDTH && table[i][j + 1] == 1) neighbours++;
    if (i - 1 >= 0 && j - 1 >= 0 && table[i - 1][j - 1] == 1) neighbours++;
    if (i - 1 >= 0 && table[i - 1][j] == 1) neighbours++;
    if (i - 1 >= 0 && j + 1 < WIDTH && table[i - 1][j + 1] == 1) neighbours++;

    return neighbours;
}


void iteration(char **tableA, char **tableB, int WIDTH, int HEIGHT) {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            int neighbours = count_neighbours(tableA, i, j, HEIGHT, WIDTH);
            //if (neighbours != 0) printf("%d   a\n", neighbours);
            if (neighbours == 2 && tableA[i][j] == 1) tableB[i][j] = (char) 1;
            else if (neighbours == 3) tableB[i][j] = 1;
            else tableB[i][j] = 0;
        }
    }

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            tableA[i][j] = tableB[i][j];
    }



int main(int argc, char *argv[]) {
    int DumpFreq = 1, MaxIter = 100, MaxIterFlag = 0, DumpFreqFlag = 0;
    char filename[100] = "", dirname[100] = "";

    for (int i = 0; i < argc; ++i) {
        // Обязательные аргументы
        if (argv[i][2] == 'i')
            strcpy(filename, argv[++i]);
        else if (argv[i][2] == 'o')
            strcpy(dirname, argv[++i]);

        // Необязательные аргументы
        else if (argv[i][2] == 'm') {
            MaxIter = atoi(argv[++i]);
            MaxIterFlag = 1;
        }
        else if (argv[i][2] == 'd') {
            DumpFreq = atoi(argv[++i]);
            DumpFreqFlag = 1;
        }
    }

    if (strlen(filename) == 0) {
        printf("Wrong filename");
        return 404;
    }
    if (strlen(dirname) == 0) {
        printf("Wrong dirname");
        return 404;
    }

    // полученные из командной строки
    strcat(dirname, "\\lab.bmp");
    FILE* file = fopen(filename, "rb");

    // Создание header
    unsigned char info[54];
    fread(info, 1, 54, file);
    int length = 0, shift = 0, WIDTH = 0, HEIGHT = 0;
    int count = 1;

    // Чтение header
    length = count_size(info, 2, 6);     // Длина
    shift = count_size(info, 10, 14);    // Сдвиг до начала файла
    WIDTH = count_size(info, 18, 22);    // Ширина
    HEIGHT = count_size(info, 22, 26);   // Длина

    // объявление остаточной информации картинки
    int rubbish_size = shift - 54;
    char *rubbish = (char*)malloc(sizeof(char) * rubbish_size);
    fread(rubbish, 1, rubbish_size, file);

    // Объявление основной информации картинки
    int main_info_size = length - shift;
    char *main_info = (char*)malloc(sizeof(char) * main_info_size);;
    fread(main_info, 1, main_info_size, file);

    // Объявление поля игры
    char **table = (char**)malloc(sizeof(char*) * HEIGHT);
    for (int i = 0; i < HEIGHT; ++i) table[i] = (char*)malloc(sizeof(char) * WIDTH);
    completion_of_field(table, main_info, HEIGHT, WIDTH);

    // Ссновной цикл программы
    for (int i = 0; i < MaxIter; ++i) {
        char **table_new = (char**)malloc(sizeof(char*) * HEIGHT);
        for (int j = 0; j < HEIGHT; ++j) table_new[j] = (char*)malloc(sizeof(char) * WIDTH);

        // Iteration of game of life
        iteration(table, table_new, WIDTH, HEIGHT);

        // Convert image to array
        char *arr_out = (char*)malloc(sizeof(char) * length);
        FromArr(arr_out, table_new, HEIGHT, WIDTH, length);

        // Write to file
        if (i % DumpFreq == 0) {
            FILE *file_out = fopen(dirname, "wb");
            fwrite(info, sizeof(char), 54, file_out);
            fwrite(rubbish, sizeof(char), shift - 54, file_out);
            fwrite(arr_out, sizeof(char), length, file_out);
            fclose(file_out);
        }
    }

    printf("%d\n", shift);
    printf("%d\n", length);
    printf("%d\n", WIDTH);
    printf("%d\n", HEIGHT);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    //объявление и открытие файла
    FILE *lab1_data;
    lab1_data = fopen(argv[2], "r");

    // объявление переменных
    char symbol;
    int line_counts = 1, word_counts = 0, symbol_counts = 0;

    // случай, для команды, требующий посчитать кол-во строк
    if ((strcmp(argv[1], "-l") == 0) || (strcmp(argv[1], "--lines") == 0)) {
        while ((symbol = fgetc(lab1_data)) != EOF) {
            if (symbol == '\n') {
                line_counts += 1;
            }
        }
        printf("%d", line_counts);
    }

    // случай, для команды, требующий посчитать кол-во слов
    else if ((strcmp(argv[1], "-w") == 0) || (strcmp(argv[1], "--words") == 0)) {
        int is_devider = 1;
        while ((symbol = fgetc(lab1_data)) != EOF) {
            if (((symbol != '\n') && (symbol != '\t') && (symbol != '\v') && (symbol != ' ')) && is_devider == 1) {
                word_counts += 1;
                is_devider = 0;
            } else if ((symbol == '\n') || (symbol == '\t') || (symbol == '\v') || (symbol == ' ')) {
                is_devider = 1;
            }
        }
        printf("%d", word_counts);
    }

    // Случай, для команды, требующий посчитать размер файла в байтах
    else if ((strcmp(argv[1], "-c") == 0) || (strcmp(argv[1], "--bytes") == 0)) {
        while ((symbol = fgetc(lab1_data)) != EOF) {
            symbol_counts += 1;
        }
        printf("%d", symbol_counts);
    }

    // Случай, для ввода некорректных данных
    else printf("%s", "incorrect command");
}



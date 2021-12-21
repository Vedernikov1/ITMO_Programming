#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct id3v2 {
    FILE *file;
    int size;
    char *str;
} id3v2;


int length (char* id3, int pos) {
    int b1 = id3[pos + 4] << 21;
    int b2 = id3[pos + 5] << 14;
    int b3 = id3[pos + 6] << 7;
    int b4 = id3[pos + 7];
    return b1 + b2 + b3 + b4;
}


id3v2 make(char *filename) {
    id3v2 id3;
    id3.file = fopen(filename, "r+");

    if (!id3.file) return id3;

    for (int i = 0; i < 3; ++i) char c = fgetc(id3.file);

    // Первые 3 байта после ID3
    char c1 = fgetc(id3.file);
    char c2 = fgetc(id3.file);
    char c3 = fgetc(id3.file);

    // 4 байта на размер метаданных
    char str_size[4];
    for(int i = 0; i < 4; ++i) str_size[i] = fgetc(id3.file);

    // Побитовый сдвиг для каждого из 4х байт
    id3.size = 10 + length(str_size, -4);

    // Занесение первых 6
    id3.str = (char*) malloc(id3.size * sizeof(char));
    id3.str[0] = 'I'; id3.str[1] = 'D', id3.str[2] = '3';
    id3.str[3] = c1; id3.str[4] = c2, id3.str[5] = c3;

    for (int i = 0; i < 4; ++i) id3.str[i + 6] = str_size[i];

    for (int i = 10; i < id3.size; ++i) {
        char c = fgetc(id3.file);
        if (c != 1) id3.str[i] = c;
    }
    return id3;
}


int check(char *s) {
    for (int i = 0; i < 4; ++i)
        if (!((s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9'))) return 0;
    for (int i = 4; i < 8; ++i)
        if (s[i] >= 128) return 0;
    return 1;
}



int search(char *prop_name, id3v2 id3) {
    for(int i = 0; i < id3.size - 3; ++i) {
        int flag = 1;
        for (int j = 0; j < 4; ++j) {
            if (prop_name[0 + j] != id3.str[i + j]) flag = 0;
        }
        if (flag) return i;
    }
    return -1;
}


void get(char *prop_name, id3v2 id3) {
    // Находим начало тега
    int pos = search(prop_name, id3);
    if (pos == -1) printf("ERROR");
    else {
        int len = length(id3.str, pos);
        // Выводим значение тега
        for (int i = pos + 10; i < pos + 10 + len; ++i)
            printf("%c", id3.str[i]);
    }
}


void show(id3v2 id3) {
    for (int i = 0; i < id3.size; ++i) {
        // Для каждого i проверяем, является ли следущая 4ка тегом
        if (check(id3.str + i)) {

            // Вывод тега
            for (int j = 0; j < 4; ++j) printf("%c", id3.str[i + j]);
            printf(" ---------- ");

            // Вывод значения тега
            int len = length(id3.str, i);
            for (int j = i + 10; j < i + 10 + len; ++j)
                printf("%c", id3.str[j]);

            printf("\n");
            i += len + 9;
        }
    }
}


void set(id3v2 id3, char *prop_name, char *prop_value) {
    int pos = search(prop_name, id3);
    if (pos == -1) printf("There's no such check");

    else {
        // считаем новый размер
        int newsize = 11 + strlen(prop_value);
        int size = length(id3.str, pos) + 10;

        char *line = malloc(id3.size - (pos + size));
        memcpy(line, id3.str + pos + size, id3.size - (pos + size));

        for(int i = 0; i < 4; ++i) id3.str[pos + i] = prop_name[i];
        id3.str[pos + 4] = ((newsize - 10) >> 21) % 128, id3.str[pos + 5] = ((newsize - 10) >> 14) % 128;
        id3.str[pos + 6] = ((newsize - 10) >> 7) % 128, id3.str[pos + 7] = (newsize - 10) % 128;

        // Flags->0
        id3.str[pos + 8] = 0;
        id3.str[pos + 9] = 0;
        id3.str[pos + 10] = 0;

        for (int i = pos + 11; i < pos + newsize; ++i) id3.str[i] = prop_value[i - pos - 11];
        for (int i = pos + newsize; i < id3.size; ++i) id3.str[i] = line[i - pos - newsize];

        free(line);
        fseek(id3.file, 0, SEEK_SET);
        for (int i = 0; i < id3.size; ++i)
            fputc(id3.str[i], id3.file);
    }
}


int main(int argc, char *argv[]) {
    int length = strlen(argv[1]) - 11;
    char *filename = (char*) malloc((length + 1) * sizeof(char));
    filename = argv[1] + 11;

    id3v2 id3 = make(filename);

    if (!strcmp(argv[2], "--show")) show(id3);
    else {

        char command[25];
        char *prop_name = malloc(20);

        // Код команды будет храниться в request
        for(int i = 0; i < 5; ++i) command[i] = argv[2][i];
        command[5] = '\0';

        // Вспомогательный вывод
        printf("%s\n", argv[2]);
        printf("%s\n", command);

        // Реализация команды get
        if (!strcmp(command, "--get")) {

            // Проверка имени на корректность
            if (strlen(argv[2]) == 10) {
                for(int i = 0; i < 4; ++i) prop_name[i] = argv[2][i + 6];

                get(prop_name, id3);
                free(prop_name);
            }

            // Если имя фрейма некорректно
            else printf("Incorrect check");
        }

        // Реализация команды set
        else if (!strcmp(command, "--set")) {
            for(int i = 0; i < 4; ++i) prop_name[i] = argv[2][i + 6];
            prop_name[4] = '\0';

            char *prop_value = argv[3] + 8;
            set(id3, prop_name, prop_value);
        }

        // Если не было ни одной корректной команды
        else printf("Incorrect command");
    }

    return 0;
}
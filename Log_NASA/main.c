#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>

// Константная переменная максимальной длины запроса
const int MAX_LEN = 500;

int *list_of_times;             // Лист временых значений каждого запроса в секундах
int size = 0, capacity = 2;     // Переменные для динамического массива секунд


// Парсинг строки
int * reformat_s (char *line) {
    int *time = (int *)malloc(sizeof (int) * 5);
    int c = 0, k = 0;

    // Цикл до начала временных переменных
    while (line[c] != '[') c++;
    c++;

    // Сдвиги при парсинге временных переменных
    int shift_seconds = 18;
    int shift_minutes = 15;
    int shift_hours = 12;
    int shift_years = 0;

    // Считывание временных переменных
    time[3] = (line[c + shift_seconds] - '0') * 10 + (line[c + shift_seconds + 1] - '0');     // Секунды
    time[2] = (line[c + shift_minutes] - '0') * 10 + (line[c + shift_minutes + 1] - '0');     // Минуты
    time[1] = (line[c + shift_hours] - '0') * 10 + (line[c + shift_hours + 1] - '0');         // Часы
    time[0] = (line[c + shift_years] - '0') * 10 + (line[c + shift_years + 1] - '0');         // Дни

    while (k < 2) {
        if (line[c] == '"') ++k;
        ++c;
    }

    time[4] = line[c + 1] - '0';
    return time;
}


// Функция подсчета секунд
int count_seconds (int day, int hrs, int mins, int sec) {
    return day * 86400 + hrs * 3600 + mins * 60 + sec;
}


// Функция добавления элемента в динамический массив
void append (int value) {
    if (size >= capacity - 1) {
        list_of_times = (int *) realloc(list_of_times, sizeof(int) * capacity * 2);
        capacity *= 2;
    }
    list_of_times[size] = value;
    size++;
}

// Главная функция
int main() {

    // Считывание файла
    FILE *file_input = fopen("C:\\Users\\User\\CLionProjects\\untitled9\\s170UUNeCtvi", "r");

    // Объявление переменных
    char *line = (char *)malloc(MAX_LEN);
    int error_counter = 0, counter = 0;
    list_of_times = (int *) malloc(sizeof(int) * capacity);

    // Цикл считывания строк
    while(fgets(line, MAX_LEN, file_input)) {
        int *elem = reformat_s(line);
        int seconds = count_seconds(elem[0], elem[1], elem[2], elem[3]);
        append(seconds);
        counter++;
        if (elem[4] == 5) {
            printf("%s", line);
            error_counter++;
        }
    }

    // Вывод числа ошибочных логов
    printf("%s", "Number of errors: ");
    printf("%d", error_counter);

    // Ввод количества секунд
    int time;
    printf("\n");
    printf("%s", "Number of seconds: ");
    scanf("%d", &time);

    // Поиск двумя указателями
    int left = 0, right = 0;
    int best = 0;
    while (left < size && right < size) {
        while (right < size && (list_of_times[right] - list_of_times[left]) <= time) right++;
        if (right - left >= best) best = right - left;
        left++;
    }

    // Вывод результата
    printf("\n");
    printf("%d", best);
    return 0;
}
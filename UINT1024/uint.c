#include <stdio.h>
#include <inttypes.h>
#include <string.h>


// Основание системы счисление (2^32)
const uint64_t base = 4294967296;


// Объявление структуры данных (массив из 32 элементов)
typedef struct uint {
    unsigned int elem[32];
} uint1024_t;


// Функция перевода числа из int в Uint-1024
uint1024_t from_uint(unsigned int x) {
    uint1024_t result;
    for (int i = 0; i < 32; i++) {
        result.elem[i] = 0;
    }
    result.elem[0] = x;
    return result;
}


// Функция сложения двух чисел типа Uint-1024
uint1024_t add_op(uint1024_t x, uint1024_t y) {
    uint1024_t result;
    int buff = 0;
    for (int i = 0; i < 32; i++) {
        uint64_t num_x = x.elem[i];
        uint64_t num_y = y.elem[i];
        result.elem[i] = (num_x + num_y + buff) % base;
        buff = (num_x + num_y + buff) / base;
    }
    return result;
}


// Функция макимума двух чисел типа Uint-1024
uint1024_t max(uint1024_t x, uint1024_t y) {
    for (int i = 31; i >= 0; i--) {
        if (x.elem[i] != y.elem[i]) {
            if (x.elem[i] > y.elem[i]) return x;
            else return y;
        }
    }
    return x;
}


// Функция сравнения двух чисел типа Uint-1024
int equal(uint1024_t x, uint1024_t y) {
    for (int i = 31; i >= 0; i--) {
        if (x.elem[i] != y.elem[i]) return 0;
    }
    return 1;
}


// Функция модуля разности двух чисел типа Uint-1024
uint1024_t subtr_op(uint1024_t x, uint1024_t y) {

    if (equal(max(x, y), y) == 1) {
        uint1024_t tmp = x;
        x = y;
        y = tmp;
    }

    uint1024_t result;
    int buff = 0;
    for (int i = 0; i < 32; i++) {
        uint64_t num_x = x.elem[i];
        uint64_t num_y = y.elem[i];
        if (num_x - buff >= num_y) {
            result.elem[i] = num_x - buff - num_y;
            buff = 0;
        } else {
            result.elem[i] = num_x + base - num_y - buff;
            buff = 1;
        }
    }
    return result;
}


// Функция умножения двух чисел типа Uint-1024
uint1024_t mult_op(uint1024_t x, uint1024_t y) {
    uint1024_t result = from_uint(0);
    for (int i = 0; i < 32; i++) {
        uint1024_t buff = from_uint(0);
        uint64_t reg = 0;
        uint64_t num_y = y.elem[i];
        for (int j = 0; j < 32; j++) {
            uint64_t num_x = x.elem[j];
            buff.elem[j] = (num_x * num_y + reg) % base;
            reg = (num_x * num_y + reg) / base;
        }
        for (int j = 0; j < i; j++) {
            for (int q = 31; q > 0; q--) {
                buff.elem[q] = buff.elem[q - 1];
            }
            buff.elem[0] = 0;
        }
        result = add_op(result, buff);
    }
    return result;
}


// Функция считывания числа типа Uint-1024
void scanf_value(uint1024_t *x) {
    char line[310];
    uint1024_t y = from_uint(0);
    scanf("%s", line);
    for (int i = 0; i < strlen(line); i++) {
        y = mult_op(y, from_uint(10));
        y = add_op(y, from_uint(line[i] - '0'));
    }
    for (int i = 0; i < 32; i++) {
        x->elem[i] = y.elem[i];
    }
}


// Функция вывода числа типа Uint-1024
void printf_value((uint1024_t x)) {
    uint1024_t res = from_uint(0);
    uint1024_t null = from_uint(0);
    char line[310];
    memset(line, '\0', 310);
    while (memcmp(&x, &null, sizeof(int) * 32) != 0) {
        uint64_t num_buff = 0;
        for (int i = 31; i >= 0; i--) {
            uint64_t num_x = x.elem[i];
            uint64_t num_base = base;
            uint64_t value = (num_x + num_buff * num_base);
            res.elem[i] = value / 10;
            num_buff = value % 10;
        }
        line[strlen(line)] = num_buff + '0';
        x = res;
    }
    for (int i = strlen(line) - 1; i >= 0; i--) {
        printf("%c", line[i]);
    }
}


int main() {

    // Тесты
uint1024_t x, y;
scanf_value(&x);
scanf_value(&y);

printf("Multiplication = ");
printf_value(mult_op(x, y));
printf("\n");
printf("Subtraction = ");
printf_value(subtr_op(x, y));
printf("\n");
printf("Sum = ");
printf_value(add_op(x, y));
printf("\n");

return 0;
}
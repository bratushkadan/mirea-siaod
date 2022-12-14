#include <iostream>

#pragma GCC diagnostic ignored "-Wshift-overflow"

namespace assignment1
{
    int operator1(int &num)
    {
        /* только с четными номерами */
        constexpr int mask = 0x55555555;
        num |= mask;

        return mask;
    }

    int operator2(int &num)
    {
        /* 7-й, 9-й, 11-й */
        constexpr int mask = 0xfffff57f;
        num &= mask;

        return mask;
    }

    void operator3(int &num)
    {
        /*  множитель — 16 */
        num <<= 4;
    }

    void operator4(int &num)
    {
        /*  делитель — 16 */
        num >>= 4;
    }

    int operator5(int &num, short n)
    {
        /* обнулить n-ый бит, используя маску пункта 1 */

        /* маска пункта 1 */
        constexpr int mask1 = 0x55555555;
        // ставим единицу в младший бит маски
        int one_bit_mask = (mask1 >> 30); // 0x00000001
        // выставляем n-ый бит в единицу
        one_bit_mask = (one_bit_mask << n);
        // инвертируем биты в маске — все биты кроме n становятся равны "1"
        one_bit_mask = ~one_bit_mask;

        num &= one_bit_mask;

        return one_bit_mask;
    }

    void task1() {
        std::cout << "\n=== Упражнение 1\n\n";
        int num = 0xa6;
        std::cout << std::bitset<32>(num) << " - число 0xa6 в двоичном представлении" << "\n";
        for (int i = 0; i < 16; i++)
        {
            std::cout << " ^";
        }
        std::cout << " - биты только с чётными номерами будут выставлены в 1\n";
        int mask = assignment1::operator1(num);
        std::cout << std::bitset<32>(mask) << " - двоичная маска оператора\n";
        std::cout << std::bitset<32>(num) << " - результат применения оператора\n";
    }
    void task2() {
        std::cout << "\n=== Упражнение 2\n\n";
        int num;
        std::cout << "Введите число: ";
        std::cin >> num;
        std::cout << std::bitset<32>(num) << " - введённое число\n";
        for (int i = 31; i >= 0; i--)
        {
            if (i == 7 || i == 9 || i == 11) {
                std::cout << '^';
            } else {
                std::cout << ' ';
            }
        }
        std::cout << " - биты на позициях 7, 9 и 11 будут выставлены в 0\n";
        int mask = assignment1::operator2(num);
        std::cout << std::bitset<32>(mask) << " - двоичная маска оператора\n";
        std::cout << std::bitset<32>(num) << " - результат применения оператора\n";
    }
    void task3() {
        std::cout << "\n=== Упражнение 3\n\n";
        int num;
        std::cout << "Введите число: ";
        std::cin >> num;
        std::cout << std::bitset<32>(num) << " (" << num << ")" << " - введённое число\nБиты числа будут сдвинуты на 4 влево (число будет умножено на 16)\n";
        assignment1::operator3(num);
        std::cout << std::bitset<32>(num) << " (" << num << ")"  << " - результат применения оператора\n";
    }
    void task4() {
        std::cout << "\n=== Упражнение 4\n\n";
        int num;
        std::cout << "Введите число: ";
        std::cin >> num;
        std::cout << std::bitset<32>(num) << " (" << num << ")" << " - введённое число\nБиты числа будут сдвинуты на 4 вправо (число будет поделено на 16)\n";
        assignment1::operator4(num);
        std::cout << std::bitset<32>(num) << " (" << num << ")"  << " - результат применения оператора\n";
    }
    void task5() {
        std::cout << "\n=== Упражнение 5\n\n";
        int num;
        short n;
        std::cout << "Введите число: ";
        std::cin >> num;
        std::cout << "Введите позицию n обнуляемого бита (0-31): ";
        std::cin >> n;
        std::cout << std::bitset<32>(num) << "(" << num << ")" << " - введённое число\n";
        std::cout << std::string(31 - n, ' ') << "^" << std::string(n, ' ') << " — " << n << "-й бит будет выставлен в 0\n";
        int mask = assignment1::operator5(num, n);
        std::cout << std::bitset<32>(mask) << " - двоичная маска оператора\n";
        std::cout << std::bitset<32>(num) << "(" << num << ")" << " - результат применения оператора\n";
    }
}

int main()
{
    assignment1::task1();
    assignment1::task2();
    assignment1::task3();
    assignment1::task4();
    assignment1::task5();
    assignment1::task5();
    assignment1::task5();

    return 0;
}
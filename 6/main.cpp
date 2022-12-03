#include <cmath>
#include <iostream>

long pal_max_len_helper(std::string pal, long **L, long n, long left, long right)
{
    if (L[left][right] == -1)
    {
        if (pal[left] == pal[right])
            L[left][right] = pal_max_len_helper(pal, L, n, left + 1, right - 1) + 2;
        else
            L[left][right] = std::max(pal_max_len_helper(pal, L, n, left + 1, right), pal_max_len_helper(pal, L, n, left, right - 1));
    }
    return L[left][right];
}

long palindrome_max_len(std::string pal)
{
    long n = pal.length();
    long **L = new long*[n];

    for (long i = 0; i < n; i++)
        L[i] = new long[n];

    for (long i = 0; i < n; i++)
    {
        for (long j = 0; j < n; j++) {
            if (i > j)
                L[i][j] = 0;
            else if (i == j)
                L[i][j] = 1;
            else
                L[i][j] = -1;
        }
    }

    return pal_max_len_helper(pal, L, n, 0, n - 1);
}

int main()
{
    std::string p1 = "AABACBGBBCDTYTDCBBAATA";
    std::string p1_exp1 = "(AA)B(A)CBG(BBCDTYTDCBBAA)T(A)";
    std::string p1_exp2 = "AAABBCDTYTDCBBAAA";
    std::string p2 = "A";
    std::string p3 = "ABCOPAB";
    std::string p4 = "GGOIPKABAPIIIIUGAG";
    std::string p4_exp1 = "GG(O)IP(K)ABAPI(IIIU)G(A)G";
    std::string p4_exp2 = "GGIPABAPIGG";

    std::cout << "Максимальные длины палиндромов при вычеркивании каких-либо символов из них:\n\n";
    printf("%s: %lu | Пояснение: %s -> %s < %lu символов\n", p1.c_str(), palindrome_max_len(p1), p1_exp1.c_str(), p1_exp2.c_str(), p1_exp2.length());
    printf("%s: %lu\n", p2.c_str(), palindrome_max_len(p2));
    printf("%s: %lu\n", p3.c_str(), palindrome_max_len(p3));
    printf("%s: %lu | Пояснение: %s -> %s < %lu символов\n", p4.c_str(), palindrome_max_len(p4), p4_exp1.c_str(), p4_exp2.c_str(), p4_exp2.length());

    return 0;
}
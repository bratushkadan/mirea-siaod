#include <iomanip>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>

// ключ графа
typedef int Key;

const int INF = 1000000;

class Graph
{
private:
    // признак направленности графа
    bool directed_graph;

    // имеют ли ребра вес
    bool has_weight;

    // количество элементов в графе
    int sz;

    // матрица смежности
    int **m;
    // сопоставление элементам индексов матрицы смежности
    std::unordered_map<Key, int> key_to_index;
    // сопоставление индексам матрицы смежности значений элементов
    std::unordered_map<int, Key> index_to_key;

private:
    // создает сопоставление значению вершины графа индексу матрицы смежности
    int map_insert(Key key)
    {
        int index = -1;
        if (key_to_index.find(key) != key_to_index.cend())
        {
            index = key_to_index[key];
        }

        if (index != -1)
        {
            return index;
        }

        index = key_to_index.size();
        for (; index > 0; index--)
        {
            if (key >= index_to_key[index - 1])
            {
                key_to_index[key] = index;
                index_to_key[index] = key;
                return index;
            }

            auto tmp = m[index];
            m[index] = m[index - 1];
            m[index - 1] = tmp;

            for (int i = 0; i < sz; i++)
            {
                int tmp = m[i][index];
                m[i][index] = m[i][index - 1];
                m[i][index - 1] = tmp;
            }

            key_to_index[index_to_key[index - 1]] = index;
            index_to_key[index] = index_to_key[index - 1];
        }
        key_to_index[key] = index;
        index_to_key[index] = key;
        return index;
    }

    void search_euler(int j, std::vector<int> &c)
    {
        for (int i = 0; i < sz; i++)
            if (m[j][i])
            {
                m[j][i] = m[i][j] = 0;
                search_euler(i, c);
            }
        c.push_back(j);
    }

public:
    Graph(int n, bool directed = false, bool has_weight = false)
    {
        directed_graph = directed;
        this->has_weight = has_weight;
        sz = n;


        m = new int *[sz];
        for (int i = 0; i < sz; i++)
        {
            m[i] = new int[sz];
        }
    }

    int get_size()
    {
        return sz;
    }

    // Ввод с клавиатуры графа (применение операции вставки ребра в граф)
    void insert(Key key, Key key2, int w = 1)
    {
        map_insert(key);
        int i1;
        int i2 = map_insert(key2);
        // ключ i1 может быть пересортирован
        i1 = map_insert(key);
        if (i1 == i2)
        {
            m[i1][i2] = m[i2][i1] = 0;
        } else
        {
            m[i1][i2] = w;
            m[i2][i1] = w * (directed_graph ? -1 : 1);
        }
    }

    // Найти и вывести Эйлеров цикл в графе
    void euler()
    {
        std::vector<int> c;

        // вычисляем степени вершин
        std::vector<int> degree(sz, 0);
        for (int i = 0; i < sz; i++)
            for (int j = 0; j < sz; j++)
                if (m[i][j])
                    degree[i]++;

        /* проверяем существует ли в данном графе эйлеров цикл */
        // количество степеней не кратных двум
        int count = 0;
        // начальная вершина
        int j = 0;
        for (int i = 0; i < sz; i++)
            if (degree[i] % 2 != 0)
            {
                ++count;
                if (count > 2)
                    return;
                j = i;
            }

        // находим цикл, проходящий по всем рёбрам только один раз
        search_euler(j, c);

        for (const auto &e: c)
            std::cout << e << " ";
        std::cout << '\n';
    }

    // Составить программу реализации алгоритма Прима построения остовного дерева минимального веса.
    void prim()
    {
        int no_edge = 0;

        std::vector<bool> selected(sz, false);
        selected[0] = true;

        int x = 0;
        int y = 0;

        std::cout << "Ребро" << " : " << "Вес\n";
        int res = 0;
        while (no_edge < sz - 1)
        {
            int min = INF;
            for (int i = 0; i < sz; i++)
            {
                if (selected[i])
                {
                    for (int j = 0; j < sz; j++)
                    {
                        if (!selected[j] && m[i][j])
                        {
                            if (min > m[i][j])
                            {
                                min = m[i][j];
                                x = i;
                                y = j;
                            }

                        }
                    }
                }
            }
            std::cout << index_to_key[x] << " - " << index_to_key[y] << " :  " << m[x][y] << '\n';
            res += m[x][y];
            selected[y] = true;
            no_edge++;
        }
        printf("Минимальная сумма весов ребер: %d\n", res);
    }

    // Разработать доступный способ (форму) вывода результирующего дерева на экран
    void print()
    {
        std::cout << std::string(15, '-') << '\n';
        int printed = 0;
        for (int i = 0; i < sz; i++)
        {
            if (printed++ != 0)
                std::cout << "\n";
            Key from_key = index_to_key[i];
            for (int k = 0; k < sz; k++)
            {
                if (m[i][k] == 0)
                {
                    continue;
                }
                std::string operation = !directed_graph ? "——" : m[i][k] > 0 ? "—>" : "<—";
                std::string weight = !has_weight ? "" : " (" + std::to_string(m[i][k]) + ")";
                printf("%d %s %d%s\n", from_key, operation.c_str(), index_to_key[k], weight.c_str());
            }
        }
        std::cout << std::string(15, '-') << '\n';
    }

    void print_matrix()
    {
        for (int i = 0; i < sz; i++)
        {
            for (int n = 0; n < sz; n++)
                std::cout << std::setw(2) << m[i][n] << " ";
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    bool get_has_weight()
    {
        return has_weight;
    };

    ~Graph()
    {
        for (int i = 0; i < sz; i++)
        {
            delete m[i];
        }
        delete m;
    }
};

class TestGraph
{
private:
    Graph *graph;
public:
    TestGraph()
    {
        std::cout << "Введите параметры графа:\n\n";
        std::cout << "Граф ориентирован (1) или нет (0): ";
        short _directed;
        bool directed;
        std::cin >> _directed;
        std::cin.ignore();

        directed = _directed == 1;

        std::cout << "Граф имеет вес (1) или нет (0): ";
        short _has_weight;
        bool has_weight;
        std::cin >> _has_weight;
        std::cin.ignore();

        has_weight = _has_weight == 1;

        std::cout << "Введите количество элементов вставляемых в граф: ";
        int n;
        std::cin >> n;
        std::cin.ignore();

        printf("У ориентированных графов дуга будет направлена от первой вершины ко второй: \n");
        printf("Введите пары «вершина - вершина» для создания связей между вершинами графа: \n");

        graph = new Graph(n, directed, has_weight);

        int a, b;
        while (true)
        {
            std::string s;

            getline(std::cin, s);

            if (s == "")
                break;

            int idx = s.find(' ');


            if (idx == -1)
            {
                a = std::atoi(s.c_str());
                b = a;
            } else
            {
                a = std::atoi(s.substr(0, idx).c_str());
                b = std::atoi(s.substr(idx).c_str());
            }

            if (has_weight)
            {
                std::string fragment = s.substr(idx + 1);
                int idx2 = fragment.find(' ');

                if (!idx2)
                {
                    graph->insert(a, b);
                    continue;
                }

                graph->insert(a, b, std::atoi(fragment.substr(idx2 + 1).c_str()));
            } else
            {
                graph->insert(a, b);
            }
        }
    }

    TestGraph(Graph *graph)
    {
        this->graph = graph;
    }

    // запустить программу
    void run()
    {
        int op = -1;
        while (op != 0)
        {
            this->print_options();
            scanf("%d", &op);
            std::cout << "\n";
            this->conduct_operation(op);
        }
    }

    void print_options()
    {
        std::cout << "Введите номер функции:\n";
        std::cout << "1. Отобразить граф\n";
        std::cout << "2. Отобразить матрицу смежности\n";
        std::cout << "3. Найти и вывести Эйлеров цикл в графе\n";
        std::cout << "4. Вывести результат работы алгоритма Прима (только если ребра имеют вес)\n";
        std::cout << "0. Выйти из программы\n";
    }

    // выполнить заданную операцию
    void conduct_operation(int n)
    {
        switch (n)
        {
            case 1:
            {
                graph->print();
                return;
            }
            case 2:
            {
                graph->print_matrix();
                return;
            }
            case 3:
            {
                graph->euler();
                return;
            }
            case 4:
            {
                if (!graph->get_has_weight())
                {
                    return;
                }

                graph->prim();
            }
            case 0:
            default:
                return;
        }
    }
};

int main()
{
//    Graph g(4, true);
//    g.insert(3, 5);
//    g.insert(5, 3);
//    g.insert(5, 6);
//    g.insert(6, 3);
//    g.insert(3, 4);
//    g.insert(4, 5);
//    g.insert(4, 6);
//    g.print_matrix();
//    g.print();

    TestGraph tg;
    tg.run();

    return 0;
}

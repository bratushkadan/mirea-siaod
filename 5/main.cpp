#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>

// ключ графа
typedef int Key;

class Graph
{
private:
    // признак направленности графа
    bool directed_graph;

    // количество элементов в графе
    int sz;

    // матрица смежности
    short **m;
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
        key_to_index[key] = index;
        index_to_key[index] = key;
        return index;
    }

public:
    Graph(int n, bool directed = false)
    {
        directed_graph = directed;
        sz = n;


        m = new short *[sz];
        for (int i = 0; i < sz; i++)
        {
            m[i] = new short[sz];
        }
    }

    int get_size()
    {
        return sz;
    }

    // Ввод с клавиатуры графа (применение операции вставки ребра в граф)
    void insert(Key key, Key key2)
    {
        int i1 = map_insert(key);
        int i2 = map_insert(key2);
        if (i1 == i2)
        {
            m[i1][i2] = m[i2][i1] = 0;
        } else
        {
            m[i1][i2] = 1;
            m[i2][i1] = 1 * (directed_graph ? -1 : 1);
        }
    }

    // Найти и вывести Эйлеров цикл в графе
    void eulers_cycle()
    {}

    // Составить программу реализации алгоритма Прима построения остовного дерева минимального веса.
    void prim()
    {}

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
            for (int k = i; k < sz; k++)
            {
                if (m[i][k] == 0)
                {
                    if (i == k)
                        printf("%d\n", index_to_key[k]);
                    continue;
                }
                std::string operation = !directed_graph ? "——" : m[i][k] == 1 ? "—>" : "<—";
                printf("%d %s %d\n", from_key, operation.c_str(), index_to_key[k]);
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

        std::cout << "Введите количество элементов вставляемых в граф: ";
        int n;
        std::cin >> n;
        std::cin.ignore();

        printf("У ориентированных графов дуга будет направлена от первой вершины ко второй: \n");
        printf("Введите пары «вершина - вершина» для создания связей между вершинами графа: \n");

        graph = new Graph(n, directed);

        int a, b;
        while (true)
        {
            std::string s;

            getline(std::cin, s);

            int idx = s.find(' ');

            if (idx == -1)
                break;

            a = std::atoi(s.substr(0, idx).c_str());
            b = std::atoi(s.substr(idx).c_str());

            graph->insert(a, b);
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
//            case 3:
//            {
//                std::cout << "Введите ключ, узел с которым будет удален: ";
//                std::cin >> d;
//                bool deleted = tree->remove(d);
//                if (deleted)
//                    printf("Узел с ключом d = %ld удален из дерева\n\n", d);
//                else
//                    printf("Узел с ключом d = %ld не удален из дерева, так как не найден в дереве\n\n", d);
//                return;
//            }
//            case 4:
//            {
//                std::cout << "Введите ключ, узел с которым будет найден: ";
//                std::cin >> d;
//                Node *node = tree->find(d);
//                if (node)
//                    printf("Узел с ключом d = %ld найден в дереве\n\n", d);
//                else
//                    printf("Узел с ключом d = %ld не найден в дереве\n\n", d);
//                return;
//            }
            case 0:
            default:
                return;
        }
    }
};

int main()
{
    Graph g(4, true);
    g.insert(5, 3);
    g.insert(5, 6);
    g.insert(6, 3);
    g.insert(3, 4);
    g.insert(4, 5);
    g.insert(4, 6);
    g.print_matrix();
    g.print();

    TestGraph tg;
    tg.run();

    return 0;
}

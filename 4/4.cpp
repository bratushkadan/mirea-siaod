#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>
#include <chrono>

struct Entry
{
    long phone;
    std::string address;
};

typedef long Key;

struct TItem
{
    Key key; // ключ
    Entry *data;

    int pos = -1; // позиция (строка) в файле

    TItem(Entry *data)
    {
        if (!data)
        {
            return;
        }

        key = data->phone;
        this->data = data;
    }

    std::string to_string()
    {
        std::string address = data->address;
        address.insert(address.size(), 50 - address.size(), ' ');
        return std::to_string(data->phone) + " " + address + " 0";
    }
};

TItem *item_from_string(std::string str)
{
    long phone = std::atoll(str.substr(0, 11).c_str());
    std::string address = str.substr(12, 50);

    TItem *item = new TItem(new Entry({
                                              .phone = phone,
                                              .address = address
                                      })
    );

    return item;
}

struct Node
{
    Node(Key key, int pos = -1)
    {
        this->key = key;
        this->pos = pos;

        this->left = nullptr;
        this->right = nullptr;
    }

    // ключ
    Key key;
    // позиция в файле
    int pos;
    Node *left;
    Node *right;
};

class Tree
{
protected:
    Node *root;

protected:
    int compare(Key key, Node *node)
    {
        if (key > node->key)
            return 1;
        if (key < node->key)
            return -1;
        return 0;
    }

    int compare(Node *node1, Node *node2)
    {
        return compare(node1->key, node2);
    }

public:
    void print()
    {
        if (!root)
        {
            std::cout << "  ";
            return;
        }

        std::queue < Node * > *q = new std::queue<Node *>({root->left, root->right});
        std::queue <std::string> *lq = new std::queue<std::string>({"(L)", "(R)"});
        std::queue<long> *pq = new std::queue<long>({root->key, root->key});

        std::cout << root->key << "\n";

        while (q->size())
        {
            Node *p = q->front();
            std::string label = lq->front();
            long parent_key = pq->front();
            q->pop();
            lq->pop();
            pq->pop();

            if (p == nullptr)
            {
                continue;
            }
            if (p->left != nullptr)
            {
                q->push(p->left);
                lq->push("(L)");
                pq->push(p->key);
            }
            if (p->right != nullptr)
            {
                q->push(p->right);
                lq->push("(R)");
                pq->push(p->key);
            }

            std::cout << parent_key << " > " << label << " " << p->key << "\n";
        }

        delete q;

        std::cout << "\n\n";
    }

    virtual void insert(Node *node)
    {};

    virtual bool remove(Key key)
    { return false; };

    virtual Node *find(Key key)
    { return nullptr; };
};

class BST : public Tree
{
public:
    BST()
    {
        this->root = nullptr;
    }

    // добавление узла
    void insert(Node *node)
    {
        Node **current = &root;

        while (*current)
        {
            int comparison = compare(node, *current);

            if (comparison > 0)
            {
                current = &((*current)->right);
            } else if (comparison < 0)
            {
                current = &((*current)->left);
            } else
                break;
        }

        *current = node;
    }

    // удаление узла с заданным ключом из дерева
    bool remove(Key key)
    {
        Node **parent = nullptr;
        Node **current = &root;

        while (*current)
        {
            int comparison = compare(key, *current);

            if (comparison > 0)
            {
                parent = current;
                current = &((*current)->right);
            } else if (comparison < 0)
            {
                parent = current;
                current = &((*current)->left);
            } else
            {
                if (!*parent || (!(*current)->left && !(*current)->right))
                {
                    delete *current;
                    *current = nullptr;
                } else if (!(*current)->left)
                {
                    Node *node = *current;
                    *current = (*current)->right;
                    delete node;
                } else if (!(*current)->right)
                {
                    Node *node = *current;
                    *current = (*current)->left;
                    delete node;
                } else
                {
                    // поставим на место удаленного узла левый узел
                    Node *node = *current;

                    *current = (*current)->left;

                    // вставим в правую ветвь левого узла удаляемого узла правый узел удаляемого узла:
                    Node **right_ptr = &((*current)->right);

                    while (*right_ptr)
                    {
                        right_ptr = &(*right_ptr)->right;
                    }

                    *right_ptr = node->right;


                    delete node;
                }
                return true;
            }
        }

        return false;
    }

    // поиск узла с заданным ключом
    Node *find(Key key)
    {
        int comparisons = 0;
        std::queue < Node * > q({root});

        while (q.size())
        {
            Node *node = q.front();
            q.pop();

            if (node == nullptr)
            {
                continue;
            }


            comparisons++;
            if (node->key == key)
            {
                std::cout << "Сравнений: " << comparisons << std::endl;
                return node;
            }

            q.push(node->left);
            q.push(node->right);
        }

        std::cout << "Сравнений: " << comparisons << std::endl;
        return nullptr;
    }
};

// в варианте #3 - splay tree (самоперестраивающееся/косое дерево)
class SplayTree : public BST
{
private:
    int rotations = 0;

    Node *rr_rotate(Node *k2)
    {
        this->rotations++;
        Node *k1 = k2->left;
        k2->left = k1->right;
        k1->right = k2;
        return k1;

    }

    Node *ll_rotate(Node *k2)
    {
        this->rotations++;
        Node *k1 = k2->right;
        k2->right = k1->left;
        k1->left = k2;
        return k1;

    }

    Node *splay(Key key, Node *root)
    {
        if (!root)
            return nullptr;
        Node header(-1);
        header.left = header.right = nullptr;
        Node *LeftTreeMax = &header;
        Node *RightTreeMin = &header;

        int comp = 0;
        while (true)
        {
            if (key < root->key)
            {
                comp += 1;
                comp += 1;
                if (!root->left)
                    break;
                comp += 1;
                if (key < root->left->key)
                {
                    comp += 1;
                    root = rr_rotate(root);
                    if (!root->left)
                        break;
                }
                RightTreeMin->left = root;
                RightTreeMin = RightTreeMin->left;

                root = root->left;

                RightTreeMin->left = nullptr;
            } else if (key > root->key)
            {
                comp += 1;
                comp += 1;
                if (!root->right)
                    break;

                comp += 1;
                if (key > root->right->key)
                {
                    root = ll_rotate(root);

                    comp += 1;
                    if (!root->right)
                        break;
                }

                LeftTreeMax->right = root;
                LeftTreeMax = LeftTreeMax->right;

                root = root->right;

                LeftTreeMax->right = nullptr;
            } else
                break;
        }

//        std::cout << "Сравнений: " << comp << std::endl;

        LeftTreeMax->right = root->left;
        RightTreeMin->left = root->right;
        root->left = header.right;
        root->right = header.left;

        return root;

    }

public:
    SplayTree()
    {
        this->root = nullptr;
    }

    int get_rotations()
    {
        return rotations;
    }

    void insert(Node *node)
    {
        static Node *p_node = nullptr;

        if (!p_node)
            p_node = node;
        else
            p_node->key = node->key;
        if (!root)
        {
            root = p_node;
            p_node = nullptr;
            return;
        }
        root = splay(node->key, root);

        int comparison = compare(root, node);

        if (comparison > 0)
        {
            p_node->left = root->left;
            p_node->right = root;
            root->left = nullptr;
            root = p_node;
        } else if (comparison < 0)
        {
            p_node->right = root->right;
            p_node->left = root;
            root->right = nullptr;
            root = p_node;
        } else
            return;

        p_node = nullptr;
    }


    bool remove(Key key)
    {
        Node *temp;

        if (!root)
            return false;

        root = splay(key, root);

        if (key != root->key)
            return false;
        else
        {
            if (!root->left)
            {
                temp = root;
                root = root->right;
            } else
            {
                temp = root;
                root = splay(key, root->left);
                root->right = temp->right;
            }

            delete temp;

            return true;
        }

    }

    Node *find(Key key)
    {
        root = splay(key, root);

        if (root->key == key)
            return root;

        return nullptr;
    }
};

class File
{
    // размер строки в байтах
    int row_size;

    std::fstream f; // поток чтения/записи файла
    Tree *tree; // бинарное дерево
    int pos = 0; // позиция (строка) в файле
private:
    void build_bst()
    {
        int line_number = 1;
        for (std::string line; std::getline(f, line); line_number++)
        {
            // обработка последней строки в файле
            if (line.size() == 0)
                continue;

            TItem *item = item_from_string(line);
            tree->insert(new Node(item->key, line_number));

            delete item;
        }
        this->pos = line_number;
    }

public:
    File(std::string path, Tree *tree)
    {
        // размер строки в байтах (+1 - символ переноса строки)
        row_size = 8 * sizeof(std::string::size_type) + 1;

        f.open(path, std::ios::in | std::ios::out | std::ios::binary);

        this->tree = tree;

        this->build_bst();
    }

    Tree *get_tree()
    {
        return this->tree;
    }

    // прочитать запись из указанной позиции (номеру/смещению - начиная с 1)
    std::string read_pos(long pos)
    {
        // индексация с 0, строки в файле нумеруются с 1
        pos -= 1;
        if (pos >= this->pos)
            return "";

        char result[row_size];

        f.clear();
        f.seekg(pos * row_size, std::ios::beg);
        f.read(result, row_size - 1);

        return std::string(result);
    }

    // найти позицию записи в файле с заданным ключом
    long find(Key key)
    {
        Node *node = tree->find(key);

        if (node == nullptr)
        {
            return -1;
        }

        return node->pos;
    }

    void print()
    {
        this->tree->print();
    }

    ~File()
    {
        delete tree;
        f.close();
        pos = 0;
    }
};

class TestTree
{
private:
    Tree *tree;
public:
    TestTree()
    {
        std::cout << "Введите параметры косого дерева:\n\n";
        std::cout << "Введите размер массива: ";
        int n;
        std::cin >> n;
        std::cin.ignore();

        printf("Введите %d целочисленных элементов бинарного дерева, разделенных пробелами: ", n);

        std::string s, num;

        std::getline(std::cin, s);
        std::stringstream ss(s);

        std::cin.ignore();

        tree = new SplayTree();

        for (int i = 0; i < n && std::getline(ss, num, ' '); i++)
        {
            tree->insert(new Node(atoi(num.c_str())));
        }
    }
    TestTree(Tree *tree) {
        this->tree = tree;
    }

    // запустить программу
    void run()
    {
        int op = -1;
        while (op != 0)
        {
            this->print_options();
            std::cin >> op;
            std::cout << "\n";
            this->conduct_operation(op);
        }
    }

    void print_options()
    {
        std::cout << "Введите номер функции:\n";
        std::cout << "1. Отобразить дерево\n";
        std::cout << "2. Добавить значение узла в двоичное дерево поиска\n";
        std::cout << "3. Удалить узел с заданным значением\n";
        std::cout << "4. Вывести, найден ли узел с заданным значением\n";
        std::cout << "0. Выйти из программы\n\n";
    }

    // выполнить заданную операцию
    void conduct_operation(int n)
    {
        long d;

        switch (n)
        {
            case 1:
            {
                SplayTree *st = dynamic_cast<SplayTree *>(tree);

                if (!st)
                    tree->print();
                else
                {
                    st->print();
                }

                return;
            }
            case 2:
            {
                std::cout << "Введите ключ, узел с которым будет добавлен: ";
                std::cin >> d;
                tree->insert(new Node(d));
                printf("В дерево добавлен узел с ключом d = %ld.\n\n", d);
                return;
            }
            case 3:
            {
                std::cout << "Введите ключ, узел с которым будет удален: ";
                std::cin >> d;
                bool deleted = tree->remove(d);
                if (deleted)
                    printf("Узел с ключом d = %ld удален из дерева\n\n", d);
                else
                    printf("Узел с ключом d = %ld не удален из дерева, так как не найден в дереве\n\n", d);
                return;
            }
            case 4:
            {
                std::cout << "Введите ключ, узел с которым будет найден: ";
                std::cin >> d;
                Node *node = tree->find(d);
                if (node)
                    printf("Узел с ключом d = %ld найден в дереве\n\n", d);
                else
                    printf("Узел с ключом d = %ld не найден в дереве\n\n", d);
                return;
            }
            case 0:
            default:
                return;
        }
    }
};

int main()
{
    /* 1.1 -- begin */
//    BST bst;
//
//    bst.insert(new Node(8, 1));
//    bst.insert(new Node(5, 1));
//    bst.insert(new Node(2, 1));
//    bst.insert(new Node(7, 1));
//    bst.insert(new Node(3, 1));
//    bst.insert(new Node(13, 1));
//    bst.insert(new Node(10, 1));
//    bst.insert(new Node(9, 1));
//    bst.insert(new Node(11, 1));
//    bst.insert(new Node(19, 1));
//    bst.insert(new Node(15, 1));
//    bst.insert(new Node(21, 1));
//    bst.insert(new Node(23, 1));
//
//    bst.print();
//
//    Node* res;
//
//    bst.print();
    /* 1.1 -- end */

    /* 1.2 -- begin */
    File f("records10.txt", new BST());
//    f.print();

    TestTree *tt = new TestTree(f.get_tree());
//    tt->run();

    Tree* t = f.get_tree();
    Node *node;
    std::chrono::steady_clock::time_point begin, end;

    begin = std::chrono::steady_clock::now();
    node = t->find(89721596747);
    end = std::chrono::steady_clock::now();
    assert(node != nullptr);
    printf("Поиск по ключу 89721596747 выполнен за     %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));

    begin = std::chrono::steady_clock::now();
    node = t->find(89225361459);
    end = std::chrono::steady_clock::now();
    assert(node != nullptr);
    printf("Поиск по ключу 89225361459 выполнен за     %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));

    begin = std::chrono::steady_clock::now();
    node = t->find(89372040084);
    end = std::chrono::steady_clock::now();
    assert(node != nullptr);
    printf("Поиск по ключу 89372040084 выполнен за     %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));

    /* 1.2 -- end */

    /* 2.1 -- start */
//    SplayTree st;
//
//    st.insert(new Node(1));
//    st.insert(new Node(2));
//    st.print();
//    st.insert(new Node(-5));
//    st.print();
//    st.insert(new Node(3));
//    st.print();
//    st.insert(new Node(4));
//    st.print();
//    st.insert(new Node(-3));
//    st.print();
    /*  2.1 -- end */

    /*  2.2 -- start */
//    std::cout << "Загрузка файла с 1.5m записями" << std::endl;
//    double c = 1500000;
//    File f("records1.5m.txt", new SplayTree());
//
//    SplayTree *st = dynamic_cast<SplayTree *>(f.get_tree());
//
//    if (st)
//    {
//        printf("Всего поворотов: r = %d\n", st->get_rotations());
//        printf("Среднее число поворотов (число поворотов / число вставленных ключей): %.3f\n", (double)(st->get_rotations()) / c);
//    }
//    else
//    {
//        std::cerr << "Невозможно привести класс файла к SplayTree\n";
//        exit(1);
//    }
//
////    f.print();
//    Tree* t = f.get_tree();
//    Node *node;
//    std::chrono::steady_clock::time_point begin, end;
//
//    begin = std::chrono::steady_clock::now();
//    node = t->find(89551509033);
//    end = std::chrono::steady_clock::now();
//    assert(node != nullptr);
//    printf("Поиск по ключу 89551509033 выполнен за     %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));
//
//    begin = std::chrono::steady_clock::now();
//    node = t->find(89858041013);
//    end = std::chrono::steady_clock::now();
//    assert(node != nullptr);
//    printf("Поиск по ключу 89858041013 выполнен за     %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));
//
//    begin = std::chrono::steady_clock::now();
//    node = t->find(89211699396);
//    end = std::chrono::steady_clock::now();
//    assert(node != nullptr);
//    printf("Поиск по ключу 89211699396 выполнен за     %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));

//    f.print();
//    tt->run();


    /*  2.2 -- end */

    /* 2.3 -- start */
//    TestSplayTree test_splay;
//    test_splay.run();
    /* 2.3 -- end */

    return 0;
}
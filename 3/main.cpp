#include <iostream>
#include <queue>
#include <sstream>

struct Node
{
    Node(int val)
    {
        this->val = val;
        this->left = nullptr;
        this->right = nullptr;
    }

    int val;
    Node *left;
    Node *right;
};

class PerfectlyBalancedBinaryTree
{
private:
    Node *root;

public:
    PerfectlyBalancedBinaryTree(int arr[], int n)
    {
        this->build_balanced_bin_tree(&root, arr, 0, n - 1);
    }

    void print()
    {
        if (!root)
        {
            std::cout << "  ";
            return;
        }

        std::queue<Node*> *q = new std::queue<Node*>({root});
        std::queue<Node*> *lq = new std::queue<Node*>;

        while (q->size() || lq->size())
        {
            if (q->size() == 0)
            {
                delete q;
                q = lq;
                lq = new std::queue<Node*>;
                std::cout << "\n";
            }

            Node* p = q->front();
            q->pop();

            if (p == nullptr)
            {
                std::cout << "x ";
                continue;
            }

            lq->push(p->left);
            lq->push(p->right);

            std::cout << p->val << " ";
        }

        delete q;
        delete lq;

        std::cout << "\n\n";
    }

    /* Отобразить дерево на экране, повернув его справа налево. */
    void print_rotated_left()
    {
        this->rotate_left();
        this->print();
    }

    void rotate_left()
    {
        if (root == nullptr || root->right == nullptr)
            return;

        Node* new_l = root;
        Node* new_root = root->right;
        new_l->right = new_root->left;
        new_root->left = new_l;
        root = new_root;
    }

    /* Определить количество листьев с положительными значениями */
    int count_positive_leaves()
    {
        return count_positive_leaves_recursive(root);
    }

    /* Определить, сколько узлов дерева содержат заданное число. */
    int count_vals(int val)
    {
        return count_vals_recursive(root, val);
    }

    /* Увеличить значения узлов вдвое, обходя дерево алгоритмом в ширину. */
    void double_tree()
    {
        if (!root) {
            return;
        }

        std::queue<Node*> q({root});

        while (q.size())
        {
            Node* p = q.front();
            q.pop();

            if (p->left)
                q.push(p->left);
            if (p->right)
                q.push(p->right);

            // не можем мультипликативно определить число, большее в два раза отрицательного числа
            // значит, игнорируем отрицательные числа
            if (p->val > 0) {
                p->val *= 2;
            }
        }
    }

private:
    void build_balanced_bin_tree(Node **root, int arr[], int start, int end)
    {
        if (start <= end)
        {

            int mid = (start + end + 1) / 2;

            *root = new Node(arr[mid]);

            build_balanced_bin_tree(&(*root)->left, arr, start, mid - 1);
            build_balanced_bin_tree(&(*root)->right, arr, mid + 1, end);
        }
    }

    int count_positive_leaves_recursive(Node *tree)
    {
        if (tree == nullptr)
            return 0;

        return (tree->left == nullptr && tree->right == nullptr && tree->val > 0 ? 1 : 0) + count_positive_leaves_recursive(tree->left) + count_positive_leaves_recursive(tree->right);
    }

    int count_vals_recursive(Node *root, int val)
    {
        if (root == nullptr)
            return 0;

        return (root->val == val ? 1 : 0) + count_vals_recursive(root->left, val) + count_vals_recursive(root->right, val);
    }
};

class Test
{
private:
    PerfectlyBalancedBinaryTree* tree;
public:
    Test()
    {
        std::cout << "Введите параметры идеально сбалансированного бинарного дерева:\n\n";
        std::cout << "Введите размер массива: ";
        int n;
        std::cin >> n;
        std::cin.ignore();

        int *arr = new int[n];

        printf("Введите %d целочисленных элементов бинарного дерева, разделенных пробелами: ", n);

        std::string s, num;

        std::getline(std::cin, s);
        std::stringstream ss(s);

        std::cin.ignore();

        for (int i = 0; i < n && std::getline(ss, num, ' '); i++)
        {
            arr[i] = atoi(num.c_str());
        }

        tree = new PerfectlyBalancedBinaryTree(arr, n);
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
        std::cout << "2. Отобразить дерево, повернув его налево (дерево перестаёт быть сбалансированным, операция мутирующая)\n";
        std::cout << "3. Вывести количество листьев с положительными значениями\n";
        std::cout << "4. Вывести количество узлов, значение которых равно заданному числу\n";
        std::cout << "5. Увеличить значения узлов вдвое, обходя дерево алгоритмом в ширину\n";
        std::cout << "0. Выйти из программы\n\n";
    }

    // выполнить заданную операцию
    void conduct_operation(int n)
    {
        switch (n)
        {
            case 1:
                tree->print();
                return;
            case 2:
                tree->print_rotated_left();
                return;
            case 3:
                printf("В дереве %d листьев с положительным значением.\n\n", tree->count_positive_leaves());
                return;
            case 4:
                int d;
                std::cout << "Введите число, количество узлов, равных которому, необходимо подсчитать: ";
                std::cin >> d;
                printf("В дереве %d узлов с значением, d = %d.\n\n", tree->count_vals(d), d);
                return;
            case 5:
                tree->double_tree();
                std::cout << "Значение положительных чисел в дереве увеличено в 2 раза.\n\n";
                return;
            case 0:
            default:
                return;
        }
    }
};

int main()
{
    Test t;
    t.run();

    return 0;
}


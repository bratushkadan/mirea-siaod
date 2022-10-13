#include <iostream>
#include <string>
#include <fstream>
#include <vector>

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
    bool open_address = true; // признак свободной для вставки ячейки
    bool was_deleted = false; // признак удаления

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
};

class HashTable
{
private:
    constexpr static float load_factor = 0.75; // коэффициент загрузки таблицы

    // размер таблицы
    int capacity = 3;
    // количество элементов в таблице
    int size = 0;
    // количество удаленных элементов в таблице
    int deleted = 0;
    // элементы хеш-таблицы
    TItem **table;

    // Хеш-функция использует метод деления на количество элементов таблицы
    // Этот метод приводит к коллизиям, только для ключей с одинаковыми последними L цифрами.
    int hash(Key key)
    {
        // 89xx-xxx-xx-xx
        /*
         * сначала возьмем остаток от деления на 1000000000, чтобы
         * отбросить первые 2 цифры номера телефона и прибавим 1, затем возьмем
         * остаток от деления на n - длину массива (таблицы)
         * */
        return ((key % 1000000000)) % capacity;
    }

    // метод двойного хеширования - хеш для смещения
    int disp_hash(int n)
    {
        if (n == 0)
        {
            return 1;
        }

        int hash = capacity % n;
        hash = hash != 0 ? hash : 1;

        return hash;
    }

    // производит поиск значения по ключу
    int find_index(Key key)
    {
        // индекс, с которого начинался поиск
        int start_idx = this->hash(key);
        int idx = start_idx;
        int disp = this->disp_hash(idx);

        while (!table[idx] || table[idx]->key != key)
        {
            idx = (idx + disp) % capacity;

            // не удалось найти элемент по ключу
            if (idx == start_idx)
                return -1;
        }

        // элемент удален
        if (table[idx]->was_deleted)
            return -1;

        return idx;
    }

public:
    HashTable()
    {
        this->table = new TItem *[this->capacity];
    }

    /* метод, определяющий, необходимо ли рехеширование */
    bool rehashing_required()
    {
        return size + deleted > capacity * load_factor;
    }

    // метод, производящий рехеширование
    void rehash()
    {
        this->print();
        capacity *= 2;
        deleted = 0;

        TItem **buf = new TItem *[capacity];


        for (int i = 0; i < capacity / 2; i++)
        {
            if (table[i] == nullptr || table[i]->was_deleted)
            {
                continue;
            }

            int idx = this->hash(table[i]->key);
            // смещение
            int disp = this->disp_hash(idx);

            // если элемент таблицы не nullptr И
            // если элемент таблицы не был удален (в этой ячейке он есть)
            // ТО увеличиваем индекс на смещение
//            printf("rehash %lu: idx = %d to idx = %d with disp %d\n", table[i]->key, i, idx, disp);
            while (buf[idx] != nullptr)
            {
                idx = (idx + disp) % capacity;
            }
//            printf("rehashed %lu: from idx = %d, to idx = %d\n", table[i]->key, i,  idx);

            buf[idx] = table[i];
        }

        delete table;
        table = buf;
    }

    // добавление элемента
    void add(TItem *item)
    {
        int idx = this->hash(item->key);
        // смещение
        int disp = this->disp_hash(idx);

        // если элемент таблицы не nullptr И
        // если элемент таблицы не был удален - то есть удаленный элемент (пустышка)
        // может быть заменен новым
        // ТО увеличиваем индекс на смещение
        while (table[idx] != nullptr && !table[idx]->was_deleted)
        {
            // обновляем элемент (удаляем старый, добавляем новый)
            if (table[idx]->key == item->key)
                break;

            idx = (idx + disp) % capacity;
        }

        if (table[idx])
            delete table[idx];
        if (table[idx] && table[idx]->was_deleted)
            this->deleted--;
        item->open_address = false;
        table[idx] = item;
        this->size++;

        // рехешируем, если таблица соответствующе загружена
        if (this->rehashing_required())
            this->rehash();
    }

    // удаление элемента таблицы по ключу
    bool remove(Key key)
    {
        int itemIdx = this->find_index(key);

        if (itemIdx == -1)
            return false;

        delete table[itemIdx]->data;
        table[itemIdx]->was_deleted = true;

        return true;
    }

    // поиск элемента таблицы по ключу
    TItem *find(Key key)
    {
       int itemIdx = this->find_index(key);

       if (itemIdx == -1)
           return nullptr;
       return table[itemIdx];
    }

    // вывод содержимого таблицы в консоль
    void print()
    {
        printf("[");
        for (int i = 0; i < capacity; i++)
        {
            if (!table[i] || table[i]->was_deleted)
            {
                printf(" x");
            } else
            {
                printf(" %s", table[i]->data->address.c_str());
            }
        }
        printf(" ] \n");
    }

    ~HashTable()
    {
        capacity = 0;
        size = 0;
        deleted = 0;

        delete this->table;
    }
};

class File
{
    std::ifstream IF; // поток чтения файла
    std::ofstream OF; // поток записи файла
    HashTable* t; // хеш-таблица
    int pos = 0; // позиция (строка) в файле
private:
    void fill_hash_table()
    {
        std::cout << "fill hash table\n";

        std::string foo;

        std::getline(this->IF, foo);

        std::cout << foo;


        for (std::string line; std::getline(IF, line);)
        {
            std::cout << line;
        }
    }
public:
    File(std::string path)
    {
        IF.open(path, std::ios::in | std::ios::binary);
        OF.open(path, std::ios::out | std::ios::binary);

        t = new HashTable();
        this->fill_hash_table();
    }

    void add(TItem* item)
    {
        // если находим старое значение, то нужно заменить его новым,
        // иначе добавляем новое значение в конец файла
        TItem* old_item = t->find(item->key);
        if (old_item) {
            std::cout << sizeof *(item->data) << " " "old item!\n";
            std::cout << item->data->address << " " "old item!\n";
        }
        if (old_item) {
            item->pos = old_item->pos;
//            t->add(item);

        } else {
            item->pos = pos;
            pos++;
            t->add(item);
            OF << item->data->phone << " " << item->data->address << "\n";
        }
    }

    bool remove(Key key)
    {
        TItem* item = this->t->find(key);
        if (!item)
            return false;

        int pos = item->pos;
        this->t->remove(key);

        return false;
    }

    // прочитать запись из указанной позиции (номеру/смещению)
    std::string read_pos(int pos)
    {
        if (pos >= this->pos)
            return "";

        return "";
    }

    void print()
    {
        this->t->print();
    }

    ~File()
    {
        delete t;
        IF.close();
        OF.close();
        pos = 0;
    }
};


int main()
{
    HashTable t;

    File f("foobar.txt");

    f.add(new TItem(new Entry({.phone = 89160723483, .address = "struve"})));
    f.add(new TItem(new Entry({.phone = 89160723484, .address = "btruve"})));
    f.add(new TItem(new Entry({.phone = 89160723485, .address = "ptruve"})));
    f.add(new TItem(new Entry({.phone = 89160723483, .address = "mruve"})));

    return 0;
}

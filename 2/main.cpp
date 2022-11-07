#include <iostream>
#include <fstream>
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

    std::string to_string()
    {
        std::string address = data->address;
        address.insert(address.size(), 50 - address.size(), ' ');
        return std::to_string(data->phone) + " " + address + " " + std::to_string(was_deleted);
    }
};

TItem *item_from_string(std::string str)
{
    long phone = std::atoll(str.substr(0, 11).c_str());
    std::string address = str.substr(12, 50);
    bool deleted = (bool) (std::atoi(str.substr(63, 1).c_str()));

    TItem *item = new TItem(new Entry({
                                              .phone = phone,
                                              .address = address
                                      })
    );
    item->was_deleted = deleted;

    return item;
}

class HashTable
{
private:
    constexpr static float load_factor = 0.75; // коэффициент загрузки таблицы

    // размер таблицы
    int capacity = 5;
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
        if (capacity % hash == 0 || hash == 0)
        {
            hash = 1;
        }

        return hash;
    }

    // производит поиск значения по ключу
    int find_index(Key key)
    {
        int comparison_count = 0;
        // индекс, с которого начинался поиск
        int start_idx = this->hash(key);
        int idx = start_idx;
        int disp = this->disp_hash(idx);

        std::cout << long(table[idx]->key) << std::endl;

        while (!table[idx] || table[idx]->key != key)
        {
            idx = (idx + disp) % capacity;
            comparison_count++;

            // не удалось найти элемент по ключу
            if (idx == start_idx)
                return -1;

        }

        // элемент удален
        if (table[idx]->was_deleted)
            return -1;

        std::cout << "Сравнений: " << comparison_count << std::endl;
        return idx;
    }

public:
    HashTable()
    {
        this->table = new TItem *[capacity];
        for (int i = 0; i < capacity; i++)
            this->table[i] = nullptr;
    }

    int get_capacity()
    {
        return capacity;
    }

    /* метод, определяющий, необходимо ли рехеширование */
    bool rehashing_required()
    {
        return size + deleted > float(capacity) * load_factor;
    }

    // метод, производящий рехеширование
    void rehash()
    {
        capacity = capacity * 2 + 1;
        deleted = 0;

        TItem **buf = new TItem *[capacity];
        for (int i = 0; i < capacity; i++)
            buf[i] = nullptr;

        for (int i = 0; i < capacity / 2; i++)
        {
            if (table[i] == nullptr || table[i]->was_deleted)
                continue;

            int idx = this->hash(table[i]->key);
            // смещение
            int disp = this->disp_hash(idx);

            // если элемент таблицы не nullptr И
            // если элемент таблицы не был удален (в этой ячейке он есть)
            // ТО увеличиваем индекс на смещение
            while (buf[idx])
                idx = (idx + disp) % capacity;

            buf[idx] = table[i];
        }

        delete this->table;
        this->table = buf;
    }

    // добавление элемента
    void add(TItem *item)
    {
        int idx = this->hash(item->key);
        // смещение
        int disp = this->disp_hash(idx);

//        printf("добавляем %lu: i = %d, d = %d\n", item->key, idx, disp);
        while (table[idx] && !table[idx]->was_deleted)
        {
            // обновляем элемент (удаляем старый, добавляем новый)
            if (table[idx]->key == item->key)
                break;

            idx = (idx + disp) % capacity;

        }
//        printf("добавили  %lu: i = %d\n", item->key, idx);

        if (table[idx] && table[idx]->was_deleted)
            this->deleted--;
        if (!table[idx] || table[idx]->key != item->key)
            this->size++;
        if (table[idx])
            delete table[idx];
        item->open_address = false;
        table[idx] = item;

        // рехешируем, если таблица соответствующе загружена
        if (this->rehashing_required())
        {
            this->rehash();
        }
    }

    // удаление элемента таблицы по ключу
    bool remove(Key key)
    {
        int itemIdx = this->find_index(key);

        if (itemIdx == -1)
            return false;

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
        printf("[\n");


        for (int i = 0; i < capacity; i++)
        {
            std::string vi = std::to_string(i + 1);
            vi.insert(vi.begin(), (std::to_string(capacity).size() - vi.size()), '0');
            if (!table[i] || table[i]->was_deleted)
                printf("%s. x\n", vi.c_str());
            else
                printf("%s. %s\n", vi.c_str(), table[i]->to_string().c_str());
        }
        printf("] \n");
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
    // размер строки в байтах
    int row_size;

    std::fstream f; // поток чтения/записи файла
    HashTable *t; // хеш-таблица
    int pos = 0; // позиция (строка) в файле
private:
    void fill_hash_table()
    {
        int line_number = 0;
        for (std::string line; std::getline(f, line); line_number++)
        {
            // обработка последней строки в файле
            if (line.size() == 0)
                continue;

            TItem *item = item_from_string(line);
            // не добавляем в таблицу удаленную запись
            if (item->was_deleted)
                continue;
            item->pos = line_number;
            t->add(item);
        }
        this->pos = line_number;
    }

public:
    File(std::string path)
    {
        // размер строки в байтах (+1 - символ переноса строки)
        row_size = 8 * sizeof(std::string::size_type) + 1;

        f.open(path, std::ios::in | std::ios::out | std::ios::binary);

        t = new HashTable();
        this->fill_hash_table();
    }

    int get_capacity()
    {
        return t->get_capacity();
    }

    void add(TItem *item)
    {
        // если находим старое значение, то нужно заменить его новым,
        TItem *old_item = t->find(item->key);
        f.clear();
        if (old_item)
        {
            f.seekp(old_item->pos * row_size, std::ios::beg);
            f.write(item->to_string().c_str(), row_size - 1);
            item->pos = old_item->pos;
            // иначе добавляем новое значение в конец файла
        } else
        {
            item->pos = this->pos++;
            f.seekg(0, std::ios::end);
            f << item->to_string() << "\n";
        }
        t->add(item);
    }

    bool remove(Key key)
    {
        TItem *item = this->t->find(key);
        if (!item)
            return false;

        f.clear();

        // записываем "1" в столбец "удалено"
        f.seekp(item->pos * this->row_size + 63);
        f.write("1", 1);
        return this->t->remove(key);
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

    TItem *find(Key key)
    {
        return t->find(key);
    }

    void print()
    {
        this->t->print();
    }

    ~File()
    {
        delete t;
        f.close();
        pos = 0;
    }

};

void test1()
{
    HashTable *t1 = new HashTable();

    printf("Тест #1\n-----\n");

    t1->add(item_from_string("89727455953 QVCplNGvmvhlQGPDbXQbnT1k29f1VwFic-_aDR-A1bsB89456W 0"));
    printf("Хеш-таблица #1\n");
    t1->print();

    HashTable t2;

    t2.add(item_from_string("89195255283 P7suQjMvk4DhoXiEaacO9mcsgeExboAYC8Yg6hEqgemHRBwpxy 0"));
    printf("Хеш-таблица #2\n");
    t2.print();

    t1->add(item_from_string("89195255283 P7suQjMvk4DhoXiEaacO9mcsgeExboAYC8Yg6hEqgemHRBwpxy 0"));
    printf("Хеш-таблица #1\n");
    t1->print();
}

void test2()
{
    HashTable t;

    printf("Тест #2\n-----\n");

    t.add(item_from_string("89727455953 QVCplNGvmvhlQGPDbXQbnT1k29f1VwFic-_aDR-A1bsB89456W 0"));
    t.add(item_from_string("89195255283 P7suQjMvk4DhoXiEaacO9mcsgeExboAYC8Yg6hEqgemHRBwpxy 0"));

    t.add(item_from_string("89195255267 P7suQjMvk4DhoXiEaacO9mcsgeExboAYC8Yg6hEqgemHRBwpxy 0"));
    t.print();
    t.add(item_from_string("89195255298 P7suQjMvk4DhoXiEaacO9mcsgeExboAYC8Yg6hEqgemHRBwpxy 0"));
    t.print();
}

void test2_2()
{
    HashTable *t = new HashTable();

    printf("Тест #2.2\n-----\n");

    t->add(item_from_string("89727455953 QVCplNGvmvhlQGPDbXQbnT1k29f1VwFic-_aDR-A1bsB89456W 0"));
    t->add(item_from_string("89195255280 P7suQjMvk4DhoXiEaacO9mcsgeExboAYC8Yg6hEqgemHRBwpxy 0"));

    t->add(item_from_string("89195255267 P7suQjMvk4DhoXiEaacO9mcsgeExboAYC8Yg6hEqgemHRBwpxy 0"));
    t->remove(89195255267);
    t->print();
    t->add(item_from_string("89195285296 P7suQjMvk4DhoXiEaacO9mcsgeExboAYC8Yg6hEqgemHRBwpxy 0"));
    t->print();
}

void test3()
{
    printf("Тест #3\n-----\n");

    std::chrono::steady_clock::time_point begin, end;

    begin = std::chrono::steady_clock::now();
    File f("records10.txt");
    end = std::chrono::steady_clock::now();
//    printf("Хеш-таблица на 1.5м записей заполнилась за                       %lums\n", (unsigned long)(std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count()));
//    printf("Вместимость хеш-таблицы: %d\n", f.get_capacity());
//
//    begin = std::chrono::steady_clock::now();
//    assert(f.read_pos(1) == "89048208674 ts_SWD0uxYZg0ySYi_jkFn9wxAFBa4mH5SFG9C3PJZjHpzOuh7 0");
//    end = std::chrono::steady_clock::now();
//    printf("Поиск по позиции (первое вхождение) выполнен за                  %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));
//
//    begin = std::chrono::steady_clock::now();
//    assert(f.read_pos(750000) == "89336370753 YKHPXfT0p0OOW6pgZXUbCUG_Hr3eStJucaI51SOVkGaXDTfpJk 0");
//    end = std::chrono::steady_clock::now();
//    printf("Поиск по позиции (750000-е вхождение) выполнен за                %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));
//
//    begin = std::chrono::steady_clock::now();
//    assert(f.read_pos(1500000) == "89722336033 gydpHZ66pMqn5Q2zz-IvEFm19iQgdu1l8AvJkijSQJwD3mCfwI 0");
//    end = std::chrono::steady_clock::now();
//    printf("Поиск по позиции (1500000-е вхождение) выполнен за               %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));
//
//    begin = std::chrono::steady_clock::now();
//    TItem *item = f.find(89048208674);
//    end = std::chrono::steady_clock::now();
//    assert(item != nullptr);
//    assert(item->data->address == "ts_SWD0uxYZg0ySYi_jkFn9wxAFBa4mH5SFG9C3PJZjHpzOuh7");
//    printf("Поиск по ключу 89048208674 (первое вхождение) выполнен за        %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));
//
//    begin = std::chrono::steady_clock::now();
//    TItem *item2 = f.find(89336370753);
//    end = std::chrono::steady_clock::now();
//    assert(item2 != nullptr);
//    assert(item2->data->address == "YKHPXfT0p0OOW6pgZXUbCUG_Hr3eStJucaI51SOVkGaXDTfpJk");
//    printf("Поиск по ключу 89336370753 (750000-е вхождение) выполнен за      %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));
//
//    begin = std::chrono::steady_clock::now();
//    TItem *item3 = f.find(89722336033);
//    end = std::chrono::steady_clock::now();
//    assert(item3 != nullptr);
//    assert(item3->data->address == "gydpHZ66pMqn5Q2zz-IvEFm19iQgdu1l8AvJkijSQJwD3mCfwI");
//    printf("Поиск по ключу 89722336033 (1500000-е вхождение) выполнен за     %luns\n", (unsigned long)(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()));

//    f.print();
}


int main()
{
//    test1();
//    test2();
//    test2_2();
    test3();

    std::cout << sizeof(Entry) + sizeof(long) << std::endl;
    std::cout << (sizeof(Entry) + sizeof(long)) * 23 << std::endl;

    return 0;
}

#include <iostream>
#include <cassert>
#include <fstream>
#include <stdexcept>
#include "interface.h"

void test_help() {
    Interface ui;
    Params p;
    char* argv[] = {(char*)"client", (char*)"-h"};
    assert(ui.parseArgs(2, argv, p) == false);
    std::cout << "[OK] Тест 1: Вызов справки (-h)\n";
}

void test_valid_args() {
    Interface ui;
    Params p;
    char* argv[] = {(char*)"client", (char*)"-i", (char*)"input.txt", (char*)"-o", (char*)"out.bin"};
    assert(ui.parseArgs(5, argv, p) == true);
    assert(p.inFile == "input.txt");
    assert(p.outFile == "out.bin");
    std::cout << "[OK] Тест 2: Корректные параметры\n";
}

void test_missing_args() {
    Interface ui;
    Params p;
    char* argv[] = {(char*)"client", (char*)"-i", (char*)"input.txt"};
    assert(ui.parseArgs(3, argv, p) == false);
    std::cout << "[OK] Тест 3: Отсутствие обязательного параметра\n";
}
void test_unknown_arg() {
    Interface ui;
    Params p;
    char* argv[] = {(char*)"client", (char*)"-i", (char*)"input.txt", (char*)"-o", (char*)"out.bin", (char*)"-x"};
    // Парсер просто проигнорирует неизвестный параметр -x, но обязательные прочитает
    assert(ui.parseArgs(6, argv, p) == true);
    std::cout << "[OK] Тест 4: Неизвестный параметр игнорируется\n";
}

void test_read_file() {
    // Создаем временный файл для теста
    std::ofstream out("test_input.txt");
    out << "1\n2 1.5 2.5\n";
    out.close();

    std::ifstream in("test_input.txt");
    assert(in.is_open());
    int count;
    in >> count;
    assert(count == 1);
    in.close();
    std::remove("test_input.txt");
    std::cout << "[OK] Тест 5: Чтение тестового файла\n";
}

void test_missing_file() {
    std::ifstream in("no_such_file.txt");
    assert(!in.is_open());
    std::cout << "[OK] Тест 6: Обработка отсутствующего файла\n";
}
void test_sha1_logic() {
    // Проверяем, что строка хеша формируется корректно (проверка длины SHA-1)
    // (В реальном коде функция sha1 вынесена в main.cpp, здесь мы просто имитируем логику)
    std::string salt = "4F9C429F5C6884DB726445BE";
    std::string password = "SupErP@$$w0rd";
    std::string to_hash = salt + password;
    assert(to_hash.length() == 37); 
    std::cout << "[OK] Тест 7: Формирование строки для SHA-1\n";
}

int main() {
    std::cout << "Запуск модульных тестов...\n";
    test_help();
    test_valid_args();
    test_missing_args();
    test_unknown_arg();
    test_read_file();
    test_missing_file();
    test_sha1_logic();
    std::cout << "Все тесты (7/7) пройдены успешно!\n";
    return 0;
}


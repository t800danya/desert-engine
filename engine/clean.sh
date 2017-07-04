#!/bin/bash
sed -i 's|#include "engine/|#include "|' *.cpp
sed -i 's|#include "engine/|#include "|' *.h
echo "Copyright 2017  Bogdan Kozyrev t800@kvkozyrev.org"
echo "Desert Engine project"
echo "Контрибьют от std::cin от 4.07.2017"
echo "Зачитка от Арктических Отложения в Header"
echo "Зачистка успено выполнена! :-D"
echo "Cписок файлов подвегрнутых санации:"
ls *.cpp *.h

/* df_test.h - D-Flat Unit Testing Framework (single-header library)
 *
 * =============================================================================
 * ОБЗОР ФРЕЙМВОРКА
 * =============================================================================
 *
 * D-Flat Test Framework — специализированный фреймворк для модульного
 * тестирования кода библиотеки D-Flat. Фреймворк спроектирован для работы
 * в среде DOS и Windows, с учётом ограничений компиляторов Borland C++,
 * Turbo C++ и Watcom C.
 *
 * Основные возможности:
 * - Макросы для assertions с автоматическим сообщением об ошибках
 * - Система тестовых наборов (suite) с подсчётом результатов
 * - Поддержка пропуска тестов (skip)
 * - Stub-система для подмены DOS-вызовов (getvideo, keyboard, mouse, timer)
 * - Assertion-механизм на основе флага ошибки в контексте тест-кейса
 *
 * =============================================================================
 * БЫСТРЫЙ СТАРТ
 * =============================================================================
 *
 * 1. Подключение фреймворка в тестовом файле:
 *
 *     #define DF_TEST_IMPLEMENTATION
 *     #include "df_test.h"
 *
 * 2. Определение тестового набора:
 *
 *     DF_TEST_SUITE(MySuite)
 *         DF_TEST_CASE(test_addition)
 *         DF_TEST_CASE(test_subtraction)
 *         DF_TEST_CASE_SKIP(test_not_ready)  // пропуск
 *     DF_TEST_SUITE_END(MySuite)
 *
 * 3. Запуск тестов:
 *
 *     int main(void) {
 *         DF_TEST_SUITE *suites[] = { MySuite() };
 *         df_test_run_all(suites, 1);
 *         df_test_print_report();
 *         return df_test_get_exit_code();
 *     }
 *
 * =============================================================================
 * ИСПОЛЬЗОВАНИЕ STUB-СИСТЕМЫ
 * =============================================================================
 *
 * Для подмены DOS-вызовов определите DF_TEST_ENABLE_STUBS перед включением:
 *
 *     #define DF_TEST_ENABLE_STUBS
 *     #define DF_TEST_IMPLEMENTATION
 *     #include "df_test.h"
 *
 * Пример подмены видеобуфера:
 *
 *     char video_buf[80*25*2];
 *     DF_STUB_GETVIDEO(video_buf, sizeof(video_buf));
 *
 * Пример подмены клавиатуры:
 *
 *     DF_STUB_KEYHIT(1);      // kbhit() вернёт true
 *     DF_STUB_GETKEY(0x1C);   // getch() вернёт Enter
 *
 * Пример подмены мыши:
 *
 *     DF_STUB_MOUSE(10, 5, 1);  // x=10, y=5, левая кнопка нажата
 *
 * Пример подмены таймера:
 *
 *     DF_STUB_TICKCOUNT(1000);           // установить 1000 мс
 *     DF_STUB_ADVANCE_TICK(500);         // добавить 500 мс
 *
 * =============================================================================
 */

#ifndef DF_TEST_H
#define DF_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* DF_TEST_EXPORT — макрос экспорта символов
 *
 * Используется для маркировки функций, экспортируемых из библиотеки.
 * В многоплатформенных сборках может заменяться на __declspec(dllexport)
 * или аналогичные директивы. В текущей реализации является пустым макросом.
 */
#define DF_TEST_EXPORT

/* =============================================================================
 * БАЗОВЫЕ ТИПЫ
 * ============================================================================= */

/* DF_TEST_FUNC — тип функции-теста
 *
 * Определяет сигнатуру функции, которая может быть использована в качестве
 * тестового случая. Все тестовые функции должны возвращать void и не
 * принимать параметров.
 *
 * Пример определения теста:
 *
 *     void test_rect_creation(void) {
 *         // тело теста
 *     }
 */
typedef void (*DF_TEST_FUNC)(void);

/* DF_TEST_SETUP — тип функции инициализации
 *
 * Сигнатура для функции, выполняемой перед каждым тестом в наборе.
 * Может использоваться для подготовки тестового окружения (alloc памяти,
 * открытие файлов, инициализация глобального состояния).
 *
 * Пример:
 *
 *     void setup_test_env(void) {
 *         memset(test_buffer, 0, sizeof(test_buffer));
 *     }
 */
typedef void (*DF_TEST_SETUP)(void);

/* DF_TEST_TEARDOWN — тип функции финализации
 *
 * Сигнатура для функции, выполняемой после каждого теста в наборе.
 * Используется для очистки ресурсов, выделенных в setup.
 *
 * Пример:
 *
 *     void teardown_test_env(void) {
 *         free(test_buffer);
 *     }
 */
typedef void (*DF_TEST_TEARDOWN)(void);

/* DF_TEST_CASE — структура отдельного теста
 *
 * Структура, описывающая один тестовый случай.
 *
 * Поля:
 *   name    — имя теста (строка, используется в отчётах)
 *   func    — указатель на функцию теста
 *   skipped — флаг пропуска (1 = тест пропущен, 0 = выполняется)
 *
 * Пример:
 *
 *     DF_TEST_CASE my_test = {
 *         "test_name",
 *         test_name,  // функция void test_name(void)
 *         0           // не пропущен
 *     };
 */
typedef struct {
    const char *name;    /* Имя тестового случая для отчётов */
    DF_TEST_FUNC func;   /* Указатель на функцию теста */
    int skipped;         /* Флаг пропуска: 1 = пропущен, 0 = выполняется */
} DF_TEST_CASE;

/* DF_TEST_SUITE — структура набора тестов
 *
 * Объединяет несколько тестов в логическую группу с общей статистикой.
 *
 * Поля:
 *   name          — имя набора (например, "RectTests", "VideoTests")
 *   tests        — массив тестовых случаев
 *   test_count   — количество тестов в массиве
 *   tests_run    — счётчик выполненных тестов
 *   tests_passed — счётчик успешных тестов
 *   tests_failed — счётчик проваленных тестов
 *   tests_skipped — счётчик пропущенных тестов
 *
 * Пример:
 *
 *     DF_TEST_SUITE my_suite = {
 *         "MyTests",
 *         my_tests,        // массив DF_TEST_CASE
 *         3,               // 3 теста
 *         0, 0, 0, 0       // счётчики (обнуляются при старте)
 *     };
 */
typedef struct {
    const char *name;        /* Имя тестового набора */
    DF_TEST_CASE *tests;     /* Массив тестовых случаев */
    int test_count;         /* Количество тестов в наборе */
    int tests_run;          /* Количество выполненных тестов */
    int tests_passed;       /* Количество успешных тестов */
    int tests_failed;       /* Количество проваленных тестов */
    int tests_skipped;      /* Количество пропущенных тестов */
} DF_TEST_SUITE;

/* =============================================================================
 * РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ
 * ============================================================================= */

/* DF_TEST_RESULT — структура результата упавшего теста
 *
 * Фиксирует информацию о провалившемся тесте для последующего отображения
 * в отчёте. Заполняется функциями df_test_assert_*_fail().
 *
 * Поля:
 *   suite_name   — имя набора тестов
 *   test_name    — имя упавшего теста
 *   file         — имя файла с тестом
 *   line         — номер строки с упавшим assertion
 *   failed_expr  — строка с выражением assertion (для DF_ASSERT)
 *   message      — форматированное сообщение об ошибке
 *
 * Пример сообщения:
 *   "Expected x (5) == y (10)"
 *   "String mismatch: "hello" vs "world""
 */
typedef struct {
    const char *suite_name;  /* Имя тестового набора */
    const char *test_name;   /* Имя упавшего теста */
    const char *file;        /* Исходный файл с тестом */
    int line;                /* Номер строки с assertion */
    const char *failed_expr; /* Выражение assertion (для DF_ASSERT) */
    char message[256];       /* Сообщение об ошибке */
} DF_TEST_RESULT;

/* DF_TEST_MAX_RESULTS — максимальное количество записей об ошибках
 *
 * Ограничивает размер внутреннего буфера для хранения результатов
 * провалившихся тестов. При достижении лимита новые ошибки игнорируются.
 */
#define DF_TEST_MAX_RESULTS 100

/* DF_TEST_REPORT — структура итогового отчёта
 *
 * Агрегирует все результаты тестирования: как успешные, так и ошибки.
 *
 * Поля:
 *   results — массив записей об ошибках (фиксированный размер)
 *   count   — количество фактических записей в results
 *   passed  — общее количество успешных тестов
 *   failed  — общее количество проваленных тестов
 *
 * Использование:
 *
 *     DF_TEST_REPORT *report = df_test_get_report();
 *     printf("Passed: %d, Failed: %d\n", report->passed, report->failed);
 */
typedef struct {
    DF_TEST_RESULT results[DF_TEST_MAX_RESULTS];  /* Массив ошибок */
    int count;        /* Фактическое количество записей */
    int passed;       /* Количество успешных тестов */
    int failed;       /* Количество проваленных тестов */
} DF_TEST_REPORT;

/* =============================================================================
 * КОНТЕКСТ ТЕСТИРОВАНИЯ
 * ============================================================================= */

/* DF_TEST_CONTEXT — внутренний контекст фреймворка
 *
 * Хранит состояние текущего выполнения теста.
 *
 * Поля:
 *   mock_count    — счётчик подменённых вызовов (для отладки stub-системы)
 *   test_status   — статус текущего теста
 *
 * Внутреннее использование: не требует прямого доступа со стороны
 * пользовательского кода.
 */
typedef struct {
    int mock_count;       /* Счётчик моков (для отладки) */
    int test_status;      /* Статус выполнения теста */
} DF_TEST_CONTEXT;

/* DF_TC_CONTEXT — контекст выполняемого тест-кейса
 *
 * Хранит состояние текущего выполняемого тест-кейса.
 * Используется assertion-макросами для проверки, не упал ли уже тест.
 *
 * Поля:
 *   failed     — флаг ошибки (1 = тест уже упал)
 *   suite_name — имя набора (для записи в отчёт)
 *   test_name  — имя теста (для записи в отчёт)
 *
 * Использование:
 *   1. df_test_run_suite() сбрасывает failed=0 перед каждым тестом
 *   2. DF_ASSERT_* проверяют failed перед выполнением
 *   3. После tc->func() проверяется failed для определения результата
 */
typedef struct {
    int failed;           /* 1 = тест уже упал на предыдущем assert */
    const char *suite_name;  /* Имя текущего набора */
    const char *test_name;   /* Имя текущего теста */
} DF_TC_CONTEXT;

extern DF_TC_CONTEXT _df_tc_ctx;

/* =============================================================================
 * МАКРОСЫ ASSERTIONS
 * =============================================================================
 *
 * Все assertion-макросы используют do-while(0) для безопасности:
 * они гарантируют, что макрос ведёт себя как одиночный оператор.
 *
 * При провале assertion:
 * 1. Устанавливается флаг _df_tc_ctx.failed = 1
 * 2. Записывается информация об ошибке в _df_report
 * 3. Возвращается к вызывателю
 *
 * Subsequent assertions в том же тесте пропускаются (shadowed).
 * После выхода из тест-функции df_test_run_suite() проверяет
 * _df_tc_ctx.failed и помечает тест как failed.
 */

/* DF_ASSERT — базовый макрос assertion
 *
 * Проверяет истинность выражения expr. При false вызывает
 * df_test_assert_fail() с информацией о файле и строке.
 *
 * Перед выполнением проверяет _df_tc_ctx.failed — если тест уже
 * упал на предыдущем assertion, пропускает проверку (shadowed).
 *
 * Параметры:
 *   expr — выражение, которое должно быть истинным
 *
 * Пример:
 *
 *     int *ptr = malloc(sizeof(int));
 *     DF_ASSERT(ptr != NULL);  // проверка аллокации
 *     DF_ASSERT(x > 0);        // проверка значения
 *
 * Генерируемое сообщение:
 *   "Assertion failed: <выражение> (expected non-zero, got 0)"
 */
#define DF_ASSERT(expr) \
    do { if (!_df_tc_ctx.failed && !(expr)) { \
        _df_tc_ctx.failed = 1; \
        df_test_assert_fail(#expr, (long)(expr), __FILE__, __LINE__); \
    } } while(0)

/* DF_ASSERT_TRUE — assertion для проверки истинности
 *
 * Эквивалентен DF_ASSERT(expr). Проверяет, что выражение истинно.
 *
 * Пример:
 *
 *     DF_ASSERT_TRUE(is_valid_handle(hwnd));
 */
#define DF_ASSERT_TRUE(expr) DF_ASSERT(expr)

/* DF_ASSERT_FALSE — assertion для проверки ложности
 *
 * Проверяет, что выражение ложно (равно 0).
 *
 * Пример:
 *
 *     DF_ASSERT_FALSE(is_empty(list));
 */
#define DF_ASSERT_FALSE(expr) DF_ASSERT(!(expr))

/* DF_ASSERT_EQ — assertion равенства (generic)
 *
 * Проверяет равенство двух значений. При неравенстве вызывает
 * df_test_assert_eq_fail() с отладочной информацией.
 *
 * Перед выполнением проверяет _df_tc_ctx.failed.
 *
 * Параметры:
 *   a, b — сравниваемые значения (приводятся к long)
 *
 * Пример:
 *
 *     DF_ASSERT_EQ(x, 10);
 *     DF_ASSERT_EQ(ptr, expected_ptr);
 *
 * Генерируемое сообщение:
 *   "Expected x (5) == y (10)"
 */
#define DF_ASSERT_EQ(a, b) \
    do { if (!_df_tc_ctx.failed && (a) != (b)) { \
        _df_tc_ctx.failed = 1; \
        df_test_assert_eq_fail((long)(a), (long)(b), #a, #b, __FILE__, __LINE__); \
    } } while(0)

/* DF_ASSERT_NE — assertion неравенства (generic)
 *
 * Проверяет неравенство двух значений. При равенстве вызывает
 * df_test_assert_ne_fail().
 *
 * Перед выполнением проверяет _df_tc_ctx.failed.
 *
 * Параметры:
 *   a, b — сравниваемые значения (приводятся к long)
 *
 * Пример:
 *
 *     DF_ASSERT_NE(result, NULL);
 *     DF_ASSERT_NE(status, ERROR_CODE);
 */
#define DF_ASSERT_NE(a, b) \
    do { if (!_df_tc_ctx.failed && (a) == (b)) { \
        _df_tc_ctx.failed = 1; \
        df_test_assert_ne_fail((long)(a), (long)(b), #a, #b, __FILE__, __LINE__); \
    } } while(0)

/* DF_ASSERT_NULL — assertion проверки на NULL
 *
 * Проверяет, что указатель равен NULL.
 *
 * Пример:
 *
 *     DF_ASSERT_NULL(empty_list->head);
 */
#define DF_ASSERT_NULL(ptr) DF_ASSERT((ptr) == NULL)

/* DF_ASSERT_NOT_NULL — assertion проверки на не-NULL
 *
 * Проверяет, что указатель не равен NULL. Предпочтительнее
 * использовать сразу после аллокации для проверки успеха.
 *
 * Пример:
 *
 *     char *buf = malloc(100);
 *     DF_ASSERT_NOT_NULL(buf);
 */
#define DF_ASSERT_NOT_NULL(ptr) DF_ASSERT((ptr) != NULL)

/* DF_ASSERT_INT_EQ — assertion равенства целых чисел
 *
 * Проверяет равенство двух целочисленных значений.
 * Явно приводит аргументы к int для согласованности.
 *
 * Пример:
 *
 *     DF_ASSERT_INT_EQ(width, 80);
 *     DF_ASSERT_INT_EQ(count, expected_count);
 */
#define DF_ASSERT_INT_EQ(a, b) DF_ASSERT_EQ((int)(a), (int)(b))

/* DF_ASSERT_INT_NE — assertion неравенства целых чисел
 *
 * Проверяет неравенство двух целочисленных значений.
 *
 * Пример:
 *
 *     DF_ASSERT_INT_NE(x, 0);  // x не должно быть нулём
 */
#define DF_ASSERT_INT_NE(a, b) DF_ASSERT_NE((int)(a), (int)(b))

/* DF_ASSERT_STR_EQ — assertion равенства строк
 *
 * Проверяет равенство двух строк (strcmp). При неравенстве
 * выводит оба значения в сообщении об ошибке.
 *
 * Перед выполнением проверяет _df_tc_ctx.failed.
 *
 * Параметры:
 *   a, b — сравниваемые строки (char *)
 *
 * Пример:
 *
 *     DF_ASSERT_STR_EQ(title, "Untitled");
 *     DF_ASSERT_STR_EQ(input, expected);
 *
 * Генерируемое сообщение:
 *   "String mismatch: "hello" vs "world""
 */
#define DF_ASSERT_STR_EQ(a, b) \
    do { if (!_df_tc_ctx.failed && strcmp((a), (b)) != 0) { \
        _df_tc_ctx.failed = 1; \
        df_test_assert_str_eq_fail((a), (b), __FILE__, __LINE__); \
    } } while(0)

/* DF_ASSERT_MEM_EQ — assertion равенства блоков памяти
 *
 * Проверяет побайтовое равенство двух блоков памяти.
 *
 * Перед выполнением проверяет _df_tc_ctx.failed.
 *
 * Параметры:
 *   a, b   — указатели на блоки памяти
 *   size   — размер для сравнения в байтах
 *
 * Пример:
 *
 *     char buf1[10] = {1,2,3,4,5};
 *     char buf2[10] = {1,2,3,4,5};
 *     DF_ASSERT_MEM_EQ(buf1, buf2, 10);  // OK
 *
 * Генерируемое сообщение:
 *   "Memory mismatch: 10 bytes differ"
 */
#define DF_ASSERT_MEM_EQ(a, b, size) \
    do { if (!_df_tc_ctx.failed && memcmp((a), (b), (size)) != 0) { \
        _df_tc_ctx.failed = 1; \
        df_test_assert_mem_eq_fail((a), (b), (size), __FILE__, __LINE__); \
    } } while(0)

/* =============================================================================
 * D-FLAT СПЕЦИФИЧНЫЕ ASSERTIONS
 * =============================================================================
 *
 * Следующие assertions специфичны для библиотеки D-Flat и работают
 * со структурами и типами, определёнными в dflat.h.
 */

/* DF_ASSERT_RECT_EQ — assertion равенства прямоугольников
 *
 * Проверяет равенство двух структур RECT (rect) по всем четырём
 * координатам: left, top, right, bottom.
 *
 * Параметры:
 *   r1, r2 — сравниваемые структуры RECT
 *
 * Пример:
 *
 *     RECT r1 = {0, 0, 10, 20};
 *     RECT r2 = {0, 0, 10, 20};
 *     DF_ASSERT_RECT_EQ(r1, r2);  // OK
 */
#define DF_ASSERT_RECT_EQ(r1, r2) do { \
    DF_ASSERT_INT_EQ((r1).lf, (r2).lf); \
    DF_ASSERT_INT_EQ((r1).tp, (r2).tp); \
    DF_ASSERT_INT_EQ((r1).rt, (r2).rt); \
    DF_ASSERT_INT_EQ((r1).bt, (r2).bt); \
} while(0)

/* DF_ASSERT_WNDRECT_EQ — assertion равенства окна и прямоугольника
 *
 * Проверяет, что координаты окна wnd совпадают с заданным
 * прямоугольником r. Сравнивает поле rc структуры WINDOW.
 *
 * Параметры:
 *   wnd — указатель на структуру WINDOW
 *   r   — структура RECT для сравнения
 *
 * Пример:
 *
 *     DF_ASSERT_WNDRECT_EQ(wnd, expected_rect);
 */
#define DF_ASSERT_WNDRECT_EQ(wnd, r) do { \
    DF_ASSERT_INT_EQ((wnd)->rc.lf, (r).lf); \
    DF_ASSERT_INT_EQ((wnd)->rc.tp, (r).tp); \
    DF_ASSERT_INT_EQ((wnd)->rc.rt, (r).rt); \
    DF_ASSERT_INT_EQ((wnd)->rc.bt, (r).bt); \
} while(0)

/* DF_ASSERT_BOOL_EQ — assertion равенства булевых значений
 *
 * Проверяет равенство двух булевых значений. Приводит значения
 * к 0/1 для корректного сравнения.
 *
 * Пример:
 *
 *     DF_ASSERT_BOOL_EQ(is_visible, was_visible);
 */
#define DF_ASSERT_BOOL_EQ(a, b) DF_ASSERT_INT_EQ((a) ? 1 : 0, (b) ? 1 : 0)

/* DF_ASSERT_RECT_VALID — assertion валидности прямоугольника
 *
 * Проверяет, что прямоугольник r имеет корректные координаты:
 * - left >= 0
 * - top >= 0
 * - right >= left
 * - bottom >= top
 *
 * Пример:
 *
 *     DF_ASSERT_RECT_VALID(wnd->rc);
 */
#define DF_ASSERT_RECT_VALID(r) do { \
    DF_ASSERT((r).lf >= 0); \
    DF_ASSERT((r).tp >= 0); \
    DF_ASSERT((r).rt >= (r).lf); \
    DF_ASSERT((r).bt >= (r).tp); \
} while(0)

/* =============================================================================
 * МАКРОСЫ ОПРЕДЕЛЕНИЯ ТЕСТОВЫХ НАБОРОВ
 * =============================================================================
 *
 * Макросы для декларативного определения тестовых наборов.
 * Используют технику X-macro для построения массивов тестов.
 *
 * Типичная структура определения набора:
 *
 *     DF_TEST_SUITE(SuiteName)
 *         DF_TEST_CASE(test_one)
 *         DF_TEST_CASE(test_two)
 *         DF_TEST_CASE_SKIP(test_skipped)
 *     DF_TEST_SUITE_END(SuiteName)
 *
 *     DF_TEST_SUITE *SuiteName(void) { return &SuiteName##_suite; }
 */

/* DF_TEST_SUITE — открывает определение тестового набора
 *
 * Начинает определение массива тестов с указанным именем.
 * Все последующие DF_TEST_CASE добавляются в этот массив.
 *
 * Параметры:
 *   suite_name — имя набора (становится частью имён переменных)
 *
 * Пример:
 *
 *     DF_TEST_SUITE(RectTests)
 *         // тесты...
 *     DF_TEST_SUITE_END(RectTests)
 *
 * Генерирует:
 *   static DF_TEST_CASE RectTests_tests[];
 */
#define DF_TEST_SUITE(suite_name) \
    static DF_TEST_CASE suite_name##_tests[] = {

/* DF_TEST_CASE — добавляет тест в набор
 *
 * Добавляет тестовый случай в массив, созданный DF_TEST_SUITE.
 * Функция test_name должна быть определена как void test_name(void).
 *
 * Параметры:
 *   test_name — имя теста (и функции, и идентификатора)
 *
 * Пример:
 *
 *     DF_TEST_SUITE(MyTests)
 *         DF_TEST_CASE(test_first)
 *         DF_TEST_CASE(test_second)
 *     DF_TEST_SUITE_END(MyTests)
 */
#define DF_TEST_CASE(test_name) \
    { #test_name, test_name, 0 },

/* DF_TEST_CASE_SKIP — добавляет пропускаемый тест
 *
 * Аналогичен DF_TEST_CASE, но тест помечается как пропущенный
 * (skipped = 1) и не выполняется при прогоне набора.
 *
 * Параметры:
 *   test_name — имя теста
 *
 * Пример:
 *
 *     DF_TEST_CASE_SKIP(test_not_implemented)
 */
#define DF_TEST_CASE_SKIP(test_name) \
    { #test_name, test_name, 1 },

/* DF_TEST_SUITE_END — завершает определение набора
 *
 * Завершает массив тестов и создаёт статическую структуру
 * DF_TEST_SUITE с инициализированными полями.
 *
 * После вызова доступна функция:
 *   DF_TEST_SUITE *suite_name(void);
 *
 * Параметры:
 *   suite_name — имя набора (должно совпадать с DF_TEST_SUITE)
 *
 * Пример:
 *
 *     DF_TEST_SUITE_END(MyTests)
 *
 *     int main(void) {
 *         DF_TEST_SUITE *s = MyTests();
 *         df_test_run_suite(s);
 *     }
 */
#define DF_TEST_SUITE_END(suite_name) \
    }; \
    static DF_TEST_SUITE suite_name##_suite = { \
        #suite_name, \
        suite_name##_tests, \
        sizeof(suite_name##_tests) / sizeof(DF_TEST_CASE), \
        0, 0, 0, 0 \
    }; \
    DF_TEST_SUITE *suite_name(void) { return &suite_name##_suite; }

/* =============================================================================
 * API ФУНКЦИИ ФРЕЙМВОРКА
 * =============================================================================
 *
 * Публичные функции для управления процессом тестирования.
 * Все функции доступны после определения DF_TEST_IMPLEMENTATION.
 */

/* df_test_init — инициализация фреймворка
 *
 * Сбрасывает внутренние счётчики и структуры данных фреймворка.
 * Вызывается автоматически из df_test_run_all(), но может быть
 * вызвана явно перед ручным запуском тестов.
 *
 * Использование:
 *
 *     df_test_init();
 *     df_test_run_suite(my_suite);
 */
DF_TEST_EXPORT void df_test_init(void);

/* df_test_run_suite — выполнение набора тестов
 *
 * Последовательно выполняет все тесты в указанном наборе.
 * Для каждого теста:
 *   1. Пропускает, если skipped = 1
 *   2. Сбрасывает _df_tc_ctx.failed = 0
 *   3. Вызывает тестовую функцию
 *   4. Проверяет _df_tc_ctx.failed:
 *      - Если 0: увеличивает tests_passed
 *      - Если 1: увеличивает tests_failed
 *
 * Параметры:
 *   suite — указатель на структуру DF_TEST_SUITE
 *
 * Использование:
 *
 *     DF_TEST_SUITE *suite = MyTests();
 *     df_test_run_suite(suite);
 */
DF_TEST_EXPORT void df_test_run_suite(DF_TEST_SUITE *suite);

/* df_test_run_all — выполнение всех наборов тестов
 *
 * Инициализирует фреймворк и последовательно выполняет
 * все указанные наборы тестов.
 *
 * Параметры:
 *   suites — массив указателей на DF_TEST_SUITE
 *   count  — количество элементов в массиве
 *
 * Использование:
 *
 *     DF_TEST_SUITE *suites[] = {
 *         RectTests(),
 *         VideoTests(),
 *         WindowTests()
 *     };
 *     df_test_run_all(suites, 3);
 */
DF_TEST_EXPORT void df_test_run_all(DF_TEST_SUITE **suites, int count);

/* df_test_get_report — получение указателя на отчёт
 *
 * Возвращает указатель на глобальную структуру отчёта.
 * Позволяет программно получить результаты тестирования.
 *
 * Возвращает:
 *   указатель на DF_TEST_REPORT
 *
 * Использование:
 *
 *     DF_TEST_REPORT *report = df_test_get_report();
 *     if (report->failed > 0) {
 *         printf("Tests failed: %d\n", report->failed);
 *     }
 */
DF_TEST_EXPORT DF_TEST_REPORT *df_test_get_report(void);

/* df_test_print_report — вывод отчёта в stdout
 *
 * Форматированный вывод итоговой статистики тестирования:
 * - Общее число успешных тестов
 * - Общее число проваленных тестов
 * - Список проваленных тестов с файлом, строкой и сообщением
 *
 * Использование:
 *
 *     df_test_run_all(suites, count);
 *     df_test_print_report();
 */
DF_TEST_EXPORT void df_test_print_report(void);

/* df_test_get_exit_code — получение кода завершения
 *
 * Возвращает код возврата для использования в exit():
 *   0 — все тесты прошли успешно
 *   1 — хотя бы один тест провалился
 *
 * Использование:
 *
 *     df_test_run_all(suites, count);
 *     return df_test_get_exit_code();
 */
DF_TEST_EXPORT int df_test_get_exit_code(void);

/* =============================================================================
 * ФУНКЦИИ ОБРАБОТКИ ASSERTION-ОВ (внутренние)
 * =============================================================================
 *
 * Вызываются автоматически при провале assertion-макросов.
 * Заполняют структуру отчёта. Флаг ошибки уже установлен макросом.
 */

/* df_test_assert_fail — обработка DF_ASSERT
 *
 * Внутренняя функция, вызываемая при провале DF_ASSERT.
 * Записывает информацию об assertion в отчёт.
 *
 * Параметры:
 *   expr — строковое представление выражения
 *   file — имя исходного файла
 *   line — номер строки
 */
DF_TEST_EXPORT void df_test_assert_fail(const char *expr, long actual_value,
    const char *file, int line);

/* df_test_assert_eq_fail — обработка DF_ASSERT_EQ
 *
 * Вызывается при неравенстве значений в DF_ASSERT_EQ.
 * Формирует сообщение с ожидаемыми и фактическими значениями.
 *
 * Параметры:
 *   a, b      — фактические значения
 *   expr_a, expr_b — строковые представления выражений
 *   file, line    — местоположение assertion
 */
DF_TEST_EXPORT void df_test_assert_eq_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line);

/* df_test_assert_ne_fail — обработка DF_ASSERT_NE
 *
 * Вызывается при равенстве значений в DF_ASSERT_NE.
 *
 * Параметры:
 *   a, b      — фактические значения
 *   expr_a, expr_b — строковые представления выражений
 *   file, line    — местоположение assertion
 */
DF_TEST_EXPORT void df_test_assert_ne_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line);

/* df_test_assert_str_eq_fail — обработка DF_ASSERT_STR_EQ
 *
 * Вызывается при несовпадении строк в DF_ASSERT_STR_EQ.
 * Обрабатывает NULL-строки корректно.
 *
 * Параметры:
 *   a, b    — сравниваемые строки
 *   file, line — местоположение assertion
 */
DF_TEST_EXPORT void df_test_assert_str_eq_fail(const char *a, const char *b, 
    const char *file, int line);

/* df_test_assert_mem_eq_fail — обработка DF_ASSERT_MEM_EQ
 *
 * Вызывается при несовпадении блоков памяти в DF_ASSERT_MEM_EQ.
 *
 * Параметры:
 *   a, b    — указатели на блоки памяти
 *   size    — размер сравниваемой области
 *   file, line — местоположение assertion
 */
DF_TEST_EXPORT void df_test_assert_mem_eq_fail(const void *a, const void *b, 
    size_t size, const char *file, int line);

/* =============================================================================
 * STUB-СИСТЕМА ДЛЯ ПОДМЕНЫ DOS-ВЫЗОВОВ
 * =============================================================================
 *
 * Позволяет тестировать код, зависящий от DOS-функций (видео, клавиатура,
 * мышь, таймер), путём подмены возвращаемых значений.
 *
 * Для активации stub-системы определите DF_TEST_ENABLE_STUBS перед
 * включением заголовочного файла.
 *
 * Пример использования:
 *
 *     #define DF_TEST_ENABLE_STUBS
 *     #define DF_TEST_IMPLEMENTATION
 *     #include "df_test.h"
 *
 *     void test_video_update(void) {
 *         char video_buf[80*25*2];
 *         DF_STUB_GETVIDEO(video_buf, sizeof(video_buf));
 *
 *         // Тестируемый код будет писать в наш буфер
 *         update_screen();
 *
 *         // Проверяем что записано
 *         DF_ASSERT_MEM_EQ(video_buf, expected, sizeof(video_buf));
 *     }
 */

#ifdef DF_TEST_ENABLE_STUBS

/* stub_getvideo_enabled — флаг активности подмены видеобуфера
 *
 * Устанавливается в 1 при вызове DF_STUB_GETVIDEO.
 * Тестируемый код должен проверять этот флаг.
 */
extern int stub_getvideo_enabled;

/* stub_video_buffer — указатель на подменённый видеобуфер
 *
 * Задаётся через DF_STUB_GETVIDEO. Тестируемый код,
 * использующий VideoBuffer (VIDEOPTR), будет писать в этот буфер.
 */
extern void *stub_video_buffer;

/* stub_screen_width — ширина экрана для stub-режима
 *
 * По умолчанию: 80
 */
extern int stub_screen_width;

/* stub_screen_height — высота экрана для stub-режима
 *
 * По умолчанию: 25
 */
extern int stub_screen_height;

/* DF_STUB_GETVIDEO — подмена видеобуфера
 *
 * Настраивает stub-систему на использование указанного буфера
 * вместо реального видеобуфера DOS.
 *
 * Параметры:
 *   buf  — указатель на буфер для записи
 *   size — размер буфера в байтах
 *
 * Пример:
 *
 *     char buf[80*25*2];
 *     DF_STUB_GETVIDEO(buf, sizeof(buf));
 */
#define DF_STUB_GETVIDEO(buf, size) \
    stub_getvideo_enabled = 1; \
    stub_video_buffer = (buf)

/* stub_keyhit_result — результат для stub-версии kbhit()
 *
 * Возвращается вместо реального вызова BIOS.
 * Задаётся через DF_STUB_KEYHIT.
 */
extern int stub_keyhit_result;

/* stub_getkey_result — результат для stub-версии getch()
 *
 * Возвращается вместо реального вызова BIOS.
 * Задаётся через DF_STUB_GETKEY.
 */
extern int stub_getkey_result;

/* DF_STUB_KEYHIT — подмена kbhit()
 *
 * Настраивает результат, который вернёт stub-версия kbhit().
 *
 * Параметры:
 *   result — значение для возврата (0 = нет нажатия, 1 = есть)
 *
 * Пример:
 *
 *     DF_STUB_KEYHIT(1);  // kbhit() вернёт "есть нажатие"
 */
#define DF_STUB_KEYHIT(result) stub_keyhit_result = (result)

/* DF_STUB_GETKEY — подмена getch()
 *
 * Настраивает результат, который вернёт stub-версия getch().
 *
 * Параметры:
 *   result — скан-код или ASCII-символ для возврата
 *
 * Пример:
 *
 *     DF_STUB_GETKEY(0x1C);  // getch() вернёт Enter
 *     DF_STUB_GETKEY('q');   // getch() вернёт 'q'
 */
#define DF_STUB_GETKEY(result) stub_getkey_result = (result)

/* stub_tick_count — счётчик тиков для stub-таймера
 *
 * Используется вместо реального счётчика прерываний BIOS.
 * Задаётся через DF_STUB_TICKCOUNT или DF_STUB_ADVANCE_TICK.
 *
 * Единица измерения: миллисекунды (условно, для совместимости
 * с логикой таймера D-Flat).
 */
extern unsigned long stub_tick_count;

/* DF_STUB_TICKCOUNT — установить значение счётчика тиков
 *
 * Параметры:
 *   value — новое значение счётчика
 *
 * Пример:
 *
 *     DF_STUB_TICKCOUNT(0);     // сброс таймера
 *     DF_STUB_TICKCOUNT(60000); // 60 секунд
 */
#define DF_STUB_TICKCOUNT(value) stub_tick_count = (value)

/* DF_STUB_ADVANCE_TICK — продвинуть счётчик тиков
 *
 * Увеличивает счётчик на указанное значение миллисекунд.
 *
 * Параметры:
 *   ms — количество миллисекунд для добавления
 *
 * Пример:
 *
 *     DF_STUB_TICKCOUNT(1000);    // 1 секунда
 *     DF_STUB_ADVANCE_TICK(500);  // ещё 0.5 секунды
 *     // теперь stub_tick_count = 1500
 */
#define DF_STUB_ADVANCE_TICK(ms) stub_tick_count += (ms)

/* stub_mouse_x — координата X для stub-мыши
 *
 * Задаётся через DF_STUB_MOUSE. Используется stub-версиями
 * функций работы с мышью.
 */
extern int stub_mouse_x;

/* stub_mouse_y — координата Y для stub-мыши
 *
 * Задаётся через DF_STUB_MOUSE.
 */
extern int stub_mouse_y;

/* stub_mouse_buttons — состояние кнопок мыши
 *
 * Битовые флаги:
 *   Bit 0 — левая кнопка (1 = нажата)
 *   Bit 1 — правая кнопка (1 = нажата)
 *   Bit 2 — средняя кнопка (1 = нажата)
 *
 * Задаётся через DF_STUB_MOUSE.
 */
extern int stub_mouse_buttons;

/* DF_STUB_MOUSE — подмена состояния мыши
 *
 * Устанавливает координаты и состояние кнопок для stub-мыши.
 *
 * Параметры:
 *   x        — координата X (0-based)
 *   y        — координата Y (0-based)
 *   buttons  — битовые флаги кнопок
 *
 * Пример:
 *
 *     // Левая кнопка нажата в позиции (10, 5)
 *     DF_STUB_MOUSE(10, 5, 1);
 *
 *     // Правая кнопка нажата
 *     DF_STUB_MOUSE(20, 15, 2);
 *
 *     // Обе кнопки нажаты
 *     DF_STUB_MOUSE(40, 10, 3);
 */
#define DF_STUB_MOUSE(x, y, buttons) \
    stub_mouse_x = (x); stub_mouse_y = (y); stub_mouse_buttons = (buttons)

#endif /* DF_TEST_ENABLE_STUBS */

/* =============================================================================
 * РЕАЛИЗАЦИЯ ФРЕЙМВОРКА
 * =============================================================================
 *
 * Всё, что находится внутри #ifdef DF_TEST_IMPLEMENTATION, является
 * реализацией. Эта секция включается ТОЛЬКО ОДИН раз в одном файле
 * (обычно в main-файле тестов).
 *
 * Внутренняя архитектура:
 *
 *   _df_test_ctx  — глобальный контекст выполнения теста
 *   _df_tc_ctx    — контекст выполняемого тест-кейса (failed, suite_name, test_name)
 *   _df_report    — глобальный отчёт о результатах
 *
 * Механизм assertion:
 *   1. df_test_run_suite() сбрасывает _df_tc_ctx.failed = 0
 *   2. Тестовая функция выполняется
 *   3. Assertion-макросы проверяют _df_tc_ctx.failed перед выполнением
 *   4. При провале: _df_tc_ctx.failed = 1, запись в _df_report, return
 *   5. Subsequent assertions пропускаются (shadowed)
 *   6. После выхода из тест-функции: проверка _df_tc_ctx.failed
 *
 * Этот механизм позволяет:
 * - Избежать множественных вложенных if для проверки ошибок
 * - Продолжить выполнение других тестов после провала одного
 * - Записать информацию о первой ошибке для отчёта
 */

#ifdef DF_TEST_IMPLEMENTATION

#ifndef DF_TEST_H_IMPLEMENTATION
#define DF_TEST_H_IMPLEMENTATION

/* _df_test_ctx — внутренний контекст фреймворка
 *
 * Хранит состояние текущего выполнения теста.
 * Обнуляется при каждом вызове df_test_init().
 */
static DF_TEST_CONTEXT _df_test_ctx = {0};

/* _df_tc_ctx — контекст выполняемого тест-кейса
 *
 * Глобальная переменная для хранения состояния текущего тест-кейса.
 * Используется assertion-макросами для раннего выхода при already failed.
 *
 * Примечание: определена в test_main.c (единственный файл с DF_TEST_IMPLEMENTATION)
 */
DF_TC_CONTEXT _df_tc_ctx = {0};

/* _df_report — глобальный отчёт о результатах
 *
 * Агрегирует результаты всех прогнанных тестов.
 * Заполняется assertion-функциями при провале.
 */
static DF_TEST_REPORT _df_report = {0};

/* df_test_init — инициализация фреймворка
 *
 * Сбрасывает все внутренние структуры в нулевое состояние.
 * Вызывается из df_test_run_all(), но может быть вызвана
 * явно для сброса состояния между независимыми прогонами.
 */
void df_test_init(void) {
    memset(&_df_test_ctx, 0, sizeof(_df_test_ctx));
    memset(&_df_tc_ctx, 0, sizeof(_df_tc_ctx));
    memset(&_df_report, 0, sizeof(_df_report));
}

/* df_test_assert_fail — обработка провала DF_ASSERT
 *
 * Алгоритм работы:
 *   1. Проверяем, есть ли место в буфере результатов
 *   2. Записываем file, line, failed_expr
 *   3. Формируем сообщение "Assertion failed: <expr>"
 *   4. Возвращаемся к вызывателю (тест продолжается, но asserts shadowed)
 *
 * Особенности:
 * - Использует snprintf для безопасной работы со строками
 * - _df_tc_ctx.failed устанавливается в 1 вызывающим макросом
 * - Возвращает управление в тест-функцию для продолжения выполнения
 */
void df_test_assert_fail(const char *expr, long actual_value,
    const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].suite_name = _df_tc_ctx.suite_name;
        _df_report.results[_df_report.count].test_name = _df_tc_ctx.test_name;
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        _df_report.results[_df_report.count].failed_expr = expr;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "Assertion failed: %s (expected non-zero, got %ld)", expr, actual_value);
        _df_report.count++;
    }
}

/* df_test_assert_eq_fail — обработка провала DF_ASSERT_EQ
 *
 * Формирует сообщение вида:
 *   "Expected x (5) == y (10)"
 *
 * Где:
 *   x, y — строковые представления выражений
 *   5, 10 — их числовые значения
 */
void df_test_assert_eq_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].suite_name = _df_tc_ctx.suite_name;
        _df_report.results[_df_report.count].test_name = _df_tc_ctx.test_name;
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "Expected %s (%ld) == %s (%ld)", expr_a, a, expr_b, b);
        _df_report.count++;
    }
}

/* df_test_assert_ne_fail — обработка провала DF_ASSERT_NE
 *
 * Формирует сообщение вида:
 *   "Expected x (5) != y (5)"
 */
void df_test_assert_ne_fail(long a, long b, 
    const char *expr_a, const char *expr_b, const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].suite_name = _df_tc_ctx.suite_name;
        _df_report.results[_df_report.count].test_name = _df_tc_ctx.test_name;
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "Expected %s (%ld) != %s (%ld)", expr_a, a, expr_b, b);
        _df_report.count++;
    }
}

/* df_test_assert_str_eq_fail — обработка провала DF_ASSERT_STR_EQ
 *
 * Формирует сообщение вида:
 *   "String mismatch: "hello" vs "world""
 *
 * Особенность: корректно обрабатывает NULL-строки,
 * выводя "(null)" вместо указателя.
 */
void df_test_assert_str_eq_fail(const char *a, const char *b, 
    const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].suite_name = _df_tc_ctx.suite_name;
        _df_report.results[_df_report.count].test_name = _df_tc_ctx.test_name;
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "String mismatch: \"%s\" vs \"%s\"", a ? a : "(null)", b ? b : "(null)");
        _df_report.count++;
    }
}

/* df_test_assert_mem_eq_fail — обработка провала DF_ASSERT_MEM_EQ
 *
 * Формирует сообщение вида:
 *   "Memory mismatch: 16 bytes differ"
 *
 * Примечание: не показывает, какие именно байты отличаются.
 * Для детальной диагностики используйте DF_ASSERT_MEM_EQ с
 * дополнительными проверками.
 */
void df_test_assert_mem_eq_fail(const void *a, const void *b, 
    size_t size, const char *file, int line) {
    if (_df_report.count < DF_TEST_MAX_RESULTS) {
        _df_report.results[_df_report.count].suite_name = _df_tc_ctx.suite_name;
        _df_report.results[_df_report.count].test_name = _df_tc_ctx.test_name;
        _df_report.results[_df_report.count].file = file;
        _df_report.results[_df_report.count].line = line;
        snprintf(_df_report.results[_df_report.count].message, 
            sizeof(_df_report.results[_df_report.count].message),
            "Memory mismatch: %u bytes differ", (unsigned)size);
        _df_report.count++;
    }
}

/* df_test_run_suite — выполнение набора тестов
 *
 * Алгоритм:
 *   1. Выводит заголовок набора "=== Running suite: name ==="
 *   2. Итерирует по всем тестам в массиве
 *   3. Для каждого теста:
 *      a. Проверяет флаг skipped — если да, увеличивает tests_skipped
 *      b. Выводит "RUN: testname ... "
 *      c. Сбрасывает _df_tc_ctx.failed = 0
 *      d. Увеличивает tests_run
 *      e. Выполняет тестовую функцию tc->func()
 *      f. После выполнения проверяет _df_tc_ctx.failed
 *         - Если 0: увеличивает tests_passed, выводит "PASS"
 *         - Если 1: увеличивает tests_failed, выводит "FAIL"
 *   4. Выводит итоговую статистику по набору
 *
 * Assertion-макросы проверяют _df_tc_ctx.failed перед выполнением.
 * При первом провале флаг устанавливается и последующие assertions
 * в этом тесте пропускаются (shadowed).
 */
void df_test_run_suite(DF_TEST_SUITE *suite) {
    printf("\n=== Running suite: %s ===\n", suite->name);
    
    for (int i = 0; i < suite->test_count; i++) {
        DF_TEST_CASE *tc = &suite->tests[i];
        
        if (tc->skipped) {
            suite->tests_skipped++;
            printf("  SKIP: %s\n", tc->name);
            continue;
        }
        
        printf("  RUN: %s ... ", tc->name);
        fflush(stdout);
        
        suite->tests_run++;
        
        _df_tc_ctx.failed = 0;
        _df_tc_ctx.suite_name = suite->name;
        _df_tc_ctx.test_name = tc->name;
        
        tc->func();
        
        if (_df_tc_ctx.failed) {
            suite->tests_failed++;
            _df_report.failed++;
            printf("FAIL\n");
        } else {
            suite->tests_passed++;
            _df_report.passed++;
            printf("PASS\n");
        }
    }
    
    printf("\nResults: %d/%d passed, %d failed, %d skipped\n",
        suite->tests_passed, suite->tests_run, 
        suite->tests_failed, suite->tests_skipped);
}

/* df_test_run_all — выполнение всех наборов тестов
 *
 * Обёртка над df_test_run_suite() для удобного запуска
 * нескольких наборов. Автоматически инициализирует фреймворк.
 *
 * Алгоритм:
 *   1. Вызывает df_test_init() для сброса состояния
 *   2. Итерирует по массиву suites
 *   3. Для каждого вызывает df_test_run_suite()
 */
void df_test_run_all(DF_TEST_SUITE **suites, int count) {
    df_test_init();
    
    for (int i = 0; i < count; i++) {
        df_test_run_suite(suites[i]);
    }
}

/* df_test_get_report — получение указателя на отчёт
 *
 * Возвращает указатель на глобальную структуру _df_report.
 * Позволяет программно анализировать результаты тестирования
 * без вывода на экран.
 *
 * Возвращает: DF_TEST_REPORT*
 */
DF_TEST_REPORT *df_test_get_report(void) {
    return &_df_report;
}

/* df_test_print_report — вывод итогового отчёта
 *
 * Форматирует и выводит итоговый отчёт в stdout:
 *
 *   ========================================
 *            FINAL TEST REPORT              
 *   ========================================
 *   Total passed:  50
 *   Total failed:  2
 *   Total results: 2
 *
 *   Failed tests:
 *     - test_rect.c:45: Expected x (5) == y (10)
 *     - test_video.c:78: String mismatch: "red" vs "blue"
 *   ========================================
 *
 * Если нет провалившихся тестов, секция "Failed tests" не выводится.
 */
void df_test_print_report(void) {
    printf("\n");
    printf("========================================\n");
    printf("         FINAL TEST REPORT              \n");
    printf("========================================\n");
    printf("Total passed:  %d\n", _df_report.passed);
    printf("Total failed:  %d\n", _df_report.failed);
    printf("Total results: %d\n", _df_report.count);
    
    if (_df_report.count > 0) {
        printf("\nFailed tests:\n");
        for (int i = 0; i < _df_report.count; i++) {
            printf("  - %s:%d: %s\n", 
                _df_report.results[i].file,
                _df_report.results[i].line,
                _df_report.results[i].message);
        }
    }
    printf("========================================\n");
}

/* df_test_get_exit_code — получение кода завершения
 *
 * Возвращает код возврата для передачи в exit():
 *   0 — все тесты прошли (нет провалов)
 *   1 — есть провалившиеся тесты
 *
 * Позволяет интегрировать тесты в CI/CD системы.
 */
int df_test_get_exit_code(void) {
    return _df_report.failed > 0 ? 1 : 0;
}

#endif /* DF_TEST_H_IMPLEMENTATION */

/* =============================================================================
 * STUB-ПЕРЕМЕННЫЕ (реализация)
 * =============================================================================
 *
 * Реализация extern-переменных stub-системы.
 * Эти переменные должны быть определены (выделена память) ровно один раз.
 * По умолчанию stub-система выключена (stub_getvideo_enabled = 0).
 */

#ifdef DF_TEST_ENABLE_STUBS

int stub_getvideo_enabled = 0;
void *stub_video_buffer = NULL;
int stub_screen_width = 80;
int stub_screen_height = 25;

int stub_keyhit_result = 0;
int stub_getkey_result = 0;

unsigned long stub_tick_count = 0;

int stub_mouse_x = 0;
int stub_mouse_y = 0;
int stub_mouse_buttons = 0;

#endif /* DF_TEST_ENABLE_STUBS */

#endif /* DF_TEST_IMPLEMENTATION */

#endif /* DF_TEST_H */
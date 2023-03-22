#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODEL_LEN 256
#define MAX_CODE_LEN 20
#define BUFSIZE 216

struct HuffmanNode {
    int symbol;
    int frequency;
};

typedef struct HuffmanNode HuffmanNode;

int read_from_file(HuffmanNode *arr_model, char *filename){
    FILE *input = fopen(filename, "rb");
    int count = 0;
    unsigned char buffer[1];
    int bytes_length = 1;
    //Ilosc odczytanych liter
    int len = 0;
    //Ilosc liter w modelu
    int letter_count = 0;
    while (count = fread(buffer, sizeof(unsigned char), bytes_length, input)){
        //0 - litery jeszcze nie ma w modelu
        //1 - litera juz jest w modelu
        int test = 0;
        int i = 0;
        for (i = 0; i < letter_count + 1; i++){
            if (arr_model[i].symbol == buffer[0]){
                arr_model[i].frequency++;
                test = 1;
            }
        }
        if (test == 0){
            arr_model[letter_count].symbol = buffer[0];
            arr_model[letter_count].frequency = 1;
            letter_count++;
        }
        len++;
    }
    printf("Plik wejsciowy: %s\n\n", filename);
    fclose(input);

    return letter_count;
}

void write_to_file(HuffmanNode *arr_model, int letter_count, char *filename){
    FILE *output;
    if ((output = fopen(filename, "w")) == NULL) {
        printf("Nie udalo sie otworzyc pliku");
    }
    else{
        fprintf(output,"%d\n", letter_count);
        int i = 0;
        for (i = 0; i < letter_count; i++) {
            fprintf(output, "%d:%d\n", arr_model[i].symbol, arr_model[i].frequency);
        }
    }
    fclose(output);
    printf("Plik wyjsciowy 1 - Model zrodla: %s\n", filename);
    printf("Model zrodla zostal zapisany do pliku.\n\n");
}

int compare_huffman_nodes(const void *item1, const void *item2){
    HuffmanNode *node1 = (HuffmanNode *) item1;
    HuffmanNode *node2 = (HuffmanNode *) item2;

    int compare_result = (node1->frequency - node2->frequency);

    if (compare_result == 0){
        compare_result = (node1->symbol - node2->symbol);
    }

    return -compare_result;
}

void sort_huffman_model(HuffmanNode *arr_model, int arr_length) {
    qsort(arr_model, arr_length, sizeof(HuffmanNode), compare_huffman_nodes);
}

void print_in_console(HuffmanNode *arr_model, int letter_count){
    printf("%d\n", letter_count);
    int i = 0;
    for (i = 0; i < letter_count; i++){
        printf("%d:%d - %c\n", arr_model[i].symbol, arr_model[i].frequency, arr_model[i].symbol);
    }
}

struct TreeNode {
    unsigned freq;
    int item;
    struct TreeNode *parent;
    struct TreeNode *left, *right;
};

struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct TreeNode **array;
};

// Creating Huffman tree node
struct TreeNode *create_new_node(int item, int freq) {
    struct TreeNode *new_node = (struct TreeNode*)malloc(sizeof(struct TreeNode));

    new_node->parent = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->item = item;
    new_node->freq = freq;

    return new_node;
}

//Zamienia kolejnoscia dwa elementy w kopcu minimalnym
void swap_nodes(struct TreeNode **a, struct TreeNode **b) {
    struct TreeNode *temp = *a;
    *a = *b;
    *b = temp;
}

//Przywraca wlasnosci kopca minimalnego do listy symboli i czestotliwosci
//Podstawowy podprogram heapsort'u
void min_heapify(struct MinHeap *min_heap, int index) {
    int mini = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < min_heap->size)
        if (min_heap->array[left]->freq < min_heap->array[mini]->freq)
            mini = left;

    if (right < min_heap->size)
        if (min_heap->array[right]->freq < min_heap->array[mini]->freq)
            mini = right;

    if (mini != index) {
        swap_nodes(&min_heap->array[index], &min_heap->array[mini]);
        min_heapify(min_heap, mini);
    }
}

//Budowanie struktury kopca minimalnego
void build_min_heap(struct MinHeap *min_heap) {
    int n = min_heap->size - 1;

    int i = 0;
    for (i = (n - 1) / 2; i >= 0; --i)
        min_heapify(min_heap, i);
}

//Pelny proces budowania kopca minimalnego, potrzebnego do zbudowania drzewa kodowania
struct MinHeap *complete_min_heap(HuffmanNode arr_model[], int size) {
    struct MinHeap *min_heap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    min_heap->size = 0;
    min_heap->capacity = size;
    min_heap->array = (struct TreeNode **)malloc(min_heap->capacity * sizeof(struct TreeNode*));

    int i = 0;
    for (i = 0; i < size; ++i)
        min_heap->array[i] = create_new_node(arr_model[i].symbol, arr_model[i].frequency);

    min_heap->size = size;
    build_min_heap(min_heap);

    return min_heap;
}

//Znajduje wezly o najmniejszej czestotliwosci wystapienia
struct TreeNode *get_min(struct MinHeap *min_heap) {
    struct TreeNode *temp = min_heap->array[0];
    min_heap->array[0] = min_heap->array[min_heap->size - 1];

    min_heap->size--;
    min_heapify(min_heap, 0);

    return temp;
}

//Wstawianie kolejnych elementow
void insert_top_node(struct MinHeap *min_heap, struct TreeNode *top_node) {
    min_heap->size++;
    int i = min_heap->size - 1;

    while (i != 0 && top_node->freq < min_heap->array[(i - 1) / 2]->freq) {
        min_heap->array[i] = min_heap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    min_heap->array[i] = top_node;
}

//Budowanie struktury drzewa kodowania
struct TreeNode *build_huffman_tree(HuffmanNode arr_model[], int size) {
    struct MinHeap *min_heap = complete_min_heap(arr_model, size);

    struct TreeNode *left, *right, *top;

    int hash_index = 1;

    //Dopoki nie dojdzie do najwyzszego hash'u
    while (min_heap->size != 1) {
        left = get_min(min_heap);
        right = get_min(min_heap);

        top = create_new_node(MODEL_LEN + hash_index, left->freq + right->freq);

        top->left = left;
        top->right = right;

        left->parent = top;
        right->parent = top;

        insert_top_node(min_heap, top);

        hash_index++;
    }
    return get_min(min_heap);
}

//Wypisanie do konsoli kodow dla znakow w modelu
void write_codes(struct TreeNode *root, int arr_codes[], int top, FILE *output) {
    //Wybor lewego wezla wiec wpisanie 0 do kodu
    if (root->left) {
        arr_codes[top] = 0;
        write_codes(root->left, arr_codes, top + 1, output);
    }
    //Wybor prawego wezla wiec wpisanie 1 do kodu
    if (root->right) {
        arr_codes[top] = 1;
        write_codes(root->right, arr_codes, top + 1, output);
    }
    //Jezeli wezel jest lisciem to go wypisz
    if (!(root->left) && !(root->right)) {
        fprintf(output,"%d-", root->item);
        int i = 0;
        for (i = 0; i < top; ++i){
            fprintf(output,"%d", arr_codes[i]);
        }
        fprintf(output,"\n");
        return;
    }
}

void write_tree(struct TreeNode *root, FILE *output){

    //Wypisanie liscia
    if (root->left == NULL && root->right == NULL){
        fprintf(output, "%d:%d   Left: NULL   Right: NULL   Parent: #%d\n", root->item, root->freq, root->parent->item%MODEL_LEN);
        return;
    }

    //Wypisanie lewego poddrzewa
    if (root->left != NULL) {
        write_tree(root->left, output);
    }

    //Wypisanie symbolu i czestotliwosci
    if (root->item > 255){
        fprintf(output,"#%d:%d   ", root->item%MODEL_LEN, root->freq);
    }
    else{
        fprintf(output,"%d:%d   ", root->item, root->freq);
    }
    //Wypisanie lewego potomka
    if (root->left->item > 255){
        fprintf(output,"Left: #%d   ", root->left->item%MODEL_LEN);
    }
    else {
        fprintf(output, "Left: %d   ", root->left->item);
    }
    //Wypisanie prawego potomka
    if (root->right->item > 255){
        fprintf(output,"Right: #%d   ", root->right->item%MODEL_LEN);
    }
    else {
        fprintf(output,"Right: %d   ", root->right->item);
    }
    //Wypisanie rodzica
    if (root->parent == NULL){
        fprintf(output,"Parent: NULL\n");
    }
    else if (root->parent->item > 255){
        fprintf(output,"Parent: #%d\n", root->parent->item%MODEL_LEN);
    }
    else{
        fprintf(output,"Parent: %d\n", root->parent->item);
    }

    //Wypisanie prawego poddrzewa
    if (root->right != NULL) {
        write_tree(root->right, output);
    }

    return;
}

void write_tree_to_file(struct TreeNode *root, char* filename){
    FILE *output;
    if ((output = fopen(filename, "w")) == NULL) {
        printf("Nie udalo sie otworzyc pliku");
    }
    else{
        write_tree(root, output);
    }
    fclose(output);
    printf("Plik wyjsciowy 2 - Drzewo kodowania: %s\n", filename);
    printf("Drzewo kodowania zostalo zapisane do pliku.\n\n");
}

void generate_and_write_codes(struct TreeNode *root, int arr_codes[], int top, int letter_count, char *filename){
    FILE *output;
    if ((output = fopen(filename, "w")) == NULL) {
        printf("Nie udalo sie otworzyc pliku");
    }
    else{
        fprintf(output, "%d\n", letter_count);
        write_codes(root, arr_codes, 0, output);
    }
    fclose(output);
    printf("Plik wyjsciowy 3 - Tablica kodowa: %s\n", filename);
    printf("Tablica kodowa zostala zapisana do pliku.\n\n");
}

struct DictionaryNode {
    int symbol;
    char code[MAX_CODE_LEN];
};

int read_huffman_tree_from_file(struct DictionaryNode *arr_dictionary, char *filename){
    FILE *input = fopen(filename, "rb");
    int count = 0;
    unsigned char buffer[1];
    int bytes_length = 1;
    char arr_symbol[4];
    char arr_code[MAX_CODE_LEN];
    char symbol_count_string[4];
    int symbol_count = 0;
    int index = 0;
    int k = 0;
    int is_dash_checked = 0;
    int first_number_checked = 0;
    while (count = fread(buffer, sizeof(unsigned char), bytes_length, input)){
        //Sprawdzanie pierwszej linii, czyli liczby symboli w tablicy
        if (first_number_checked == 0){
            if (buffer[0] == '\n'){
                symbol_count_string[k] = '\0';
                first_number_checked = 1;
                k = 0;
                continue;
            }
            else if (buffer[0] == '\r'){
                continue;
            }
            else {
                symbol_count_string[k] = buffer[0];
                k++;
                continue;
            }
        }
        //Odczyt kodu ASCII
        if (buffer[0] != '-' && k < 4 && first_number_checked == 1 && is_dash_checked == 0){
            arr_symbol[k] = buffer[0];
            k++;
            continue;
        }
        //Sprawdzenie czy odczytano myslnik, czyli znak dzielacy czesc z symbolem i kodem
        if (buffer[0] == '-'){
            arr_symbol[k] = '\0';
            k = 0;
            is_dash_checked = 1;
            continue;
        }
        //Odczyt kodu odpowiadajacego symbolowi z danej linii
        if (is_dash_checked == 1) {
            if (buffer[0] == '\r') {
                continue;
            }
            else if (buffer[0] == '\n') {
                arr_code[k] = '\0';
                arr_dictionary[index].symbol = atoi(arr_symbol);
                strcpy(arr_dictionary[index].code, arr_code);
                k = 0;
                is_dash_checked = 0;
                index++;
            }
            else {
                arr_code[k] = buffer[0];
                k++;
            }
        }
    }
    symbol_count = atoi(symbol_count_string);
    printf("Plik z ktorego zostaje utworzona tablica kodowa: %s\n\n", filename);
    fclose(input);
    return symbol_count;
}

void write_compressed_file(struct DictionaryNode *arr_dictionary, int symbol_count, char *filename_input, char *filename_output, char *filename_bits){

    FILE *output=NULL;
    output = fopen(filename_output,"wb+");

    if (output == NULL)
    {
        printf("Nie mozna utworzyc pliku: %s  \n", filename_output);
        exit(EXIT_FAILURE);
    }

    FILE *input = NULL;
    input = fopen(filename_input,"rb");
    if (input == NULL)
    {
        printf("Nie mozna otworzyc pliku: %s  \n", filename_input);
        exit(EXIT_FAILURE);
    }

    int bit_count = 0;
    int bytes_read;
    int i, j;
    int code_index = 0;
    int position = 0;
    int sign;

    unsigned char buffer[BUFSIZE];
    unsigned char symbol;

    unsigned char byte = 255;

    while(bytes_read = fread(buffer, sizeof(unsigned char), BUFSIZE, input)) {
        for (i = 0; i < bytes_read; i++) {
            symbol = buffer[i];
            //Szukanie kodu dla symbolu
            for (j = 0; j < symbol_count; j++) {
                if (symbol == arr_dictionary[j].symbol) {
                    while (arr_dictionary[j].code[code_index] != '\0') {
                        sign = arr_dictionary[j].code[code_index] - 48;
                        if (sign == 1) {
                            byte = (byte << 1) + 1;
                        }
                        else {
                            byte = byte << 1;
                        }
                        if (position < 7) {
                            position++;
                        }
                        else {
                            position = 0;
                            fwrite(&byte, sizeof(unsigned char),1, output);
                            //printf("%d\n", byte);
                            byte = 255;
                        }
                        bit_count++;
                        code_index++;
                    }
                    code_index = 0;
                    break;
                }
            }
        }
    }
    if (position > 0){
        for (i = 0; i < 8 - position; i++){
            byte = byte << 1;
        }
        fwrite(&byte, sizeof(unsigned char),1, output);
        //printf("%d\n", byte);
    }

    printf("Plik wejsciowy: %s\n", filename_input);
    printf("Plik wyjsciowy: %s\n", filename_output);

    fclose(input);
    fclose(output);

    FILE *bits_output = NULL;
    bits_output = fopen(filename_bits,"w");
    if (bits_output == NULL){
        printf("Nie mozna utworzyc pliku: %s  \n", filename_bits);
        exit(EXIT_FAILURE);
    }

    fprintf(bits_output, "%d", bit_count);

    fclose(bits_output);

}

void write_decompressed_file(struct DictionaryNode *arr_dictionary, int symbol_count, char *filename_input, char *filename_output, char *filename_bits){

    int bytes_read;
    unsigned char buffer[1];
    char string_bits_count[100];
    int m = 0;

    FILE *bits_file = NULL;
    bits_file = fopen(filename_bits,"rb+");

    if (bits_file == NULL)
    {
        printf("Nie mozna otworzyc pliku: %s  \n", filename_bits);
        exit(EXIT_FAILURE);
    }

    while(bytes_read = fread(buffer, sizeof(char), 1, bits_file)){
        string_bits_count[m] = buffer[0];
        m++;
    }
    string_bits_count[m] = '\0';

    int bits_count = atoi(string_bits_count);
    int bits_current = 0;

    fclose(bits_file);

    FILE *output=NULL;
    output = fopen(filename_output,"wb+");

    if (output == NULL)
    {
        printf("Nie mozna utworzyc pliku: %s  \n", filename_output);
        exit(EXIT_FAILURE);
    }

    FILE *input = NULL;
    input = fopen(filename_input,"rb");
    if (input == NULL)
    {
        printf("Nie mozna otworzyc pliku: %s  \n", filename_input);
        exit(EXIT_FAILURE);
    }

    int i = 0;
    int j = 0;
    int k = 0;
    int index = 0;
    int test_matched = 0;

    unsigned char ascii_code;
    unsigned char byte[8];
    unsigned char code[255];

    while(bytes_read = fread(buffer, sizeof(unsigned char), 1, input)) {
        ascii_code = buffer[0];
        for (i = 0; i < 8; i++){
            if (ascii_code / 128 == 0){
                memset(byte + i,'0', 1);
            }
            else{
                memset(byte + i,'1', 1);
            }
            ascii_code = ascii_code << 1;
        }

        for (i = 0; i < 8; i++){
            code[index] = byte[i];
            code[index + 1] = '\0';
            index++;
            bits_current++;
            for (k = 0; k < symbol_count; k++){
                if (strcmp(code, arr_dictionary[k].code) == 0){
                    if (bits_current <= bits_count) {
                        fwrite(&arr_dictionary[k].symbol, sizeof(unsigned char), 1, output);
                    }
                    test_matched = 1;
                    break;
                }
            }
            if (test_matched == 1){
                for (j = 0; j < index + 1; j++) {
                    code[j] = NULL;
                }
                test_matched = 0;
                index = 0;
            }
        }
    }

    fclose(input);
    fclose(output);
    return;
}



int main() {
    HuffmanNode arr_model[MODEL_LEN];

    //READ
    int letter_count = read_from_file(arr_model, "test.txt");

    //CONSOLE OUT
    //printf("Model przed posortowaniem:\n");
    //print_in_console(arr_model, letter_count);

    //SORT
    sort_huffman_model(arr_model,letter_count);

    //CONSOLE OUT
    //printf("Model po posortowaniu:\n");
    //print_in_console(arr_model, letter_count);

    //WRITE
    write_to_file(arr_model, letter_count, "tree.model");

    //GENERATE HUFFMAN TREE FROM MODEL
    struct TreeNode *root = build_huffman_tree(arr_model, letter_count);

    //WRITE HUFFMAN TREE TO FILE
    write_tree_to_file(root, "tree.graf");

    //GENERATE CODE TABLE FROM TREE, WRITE CODE TABLE TO FILE
    //Nie moglem rozdzielic tych dwoch funkcji z powodu rekurencyjnego przeszukiwania drzewa
    //Zapis do pliku odbywa sie od razu po rekurencyjnym wygenerowaniu sciezki do liscia
    //Sciezka do liscia  na najglebszym poziomie rekurencji jest zapisana w arr_codes
    int arr_codes[MAX_CODE_LEN];
    generate_and_write_codes(root, arr_codes, 0, letter_count, "tree.code");

    printf("\nKompresja i dekompresja:\n\n");

    int wybor = 0;

    printf("Jaka operacje wykonac: \n1. Kompresja\n2. Dekompresja\n3. Wyjscie\n");
    scanf("%d", &wybor);
    printf("\n\n");

    if (wybor == 1) {
        //CREATE DICTIONARY FROM FILE
        struct DictionaryNode arr_dictionary_encode[MODEL_LEN];
        int symbol_count_encode = read_huffman_tree_from_file(arr_dictionary_encode, "tree.code");

        //ENCODE TEXT FILE USING DICTIONARY
        remove("tree");
        write_compressed_file(arr_dictionary_encode, symbol_count_encode, "test.txt", "tree", "bit_count.txt");
    }

    if (wybor == 2){
        //CREATE DICTIONARY FROM FILE
        struct DictionaryNode arr_dictionary_decode[MODEL_LEN];
        int symbol_count_decode = read_huffman_tree_from_file(arr_dictionary_decode, "tree.code");

        //DECODE TEXT FILE USING DICTIONARY
        write_decompressed_file(arr_dictionary_decode, symbol_count_decode, "tree", "test.txt.decompressed", "bit_count.txt");
    }

    if (wybor == 3){
        ;
    }

    //DICTIONARY CONSOLE OUT
//    int i = 0;
//    for (i = 0; i < symbol_count_encode; i++){
//        printf("%c - %d - %s\n", arr_dictionary_encode[i].symbol, arr_dictionary_encode[i].symbol, arr_dictionary_encode[i].code);
//    }

    printf("Koniec");

    return 0;
}

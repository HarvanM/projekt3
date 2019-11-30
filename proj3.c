#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

int searchForNumber(char firstChar);

int numberFromString(char *string, int  *stringPosition);

bool isborder(Map *map, int r, int c, int border);

int start_border(Map *map, int r, int c, int leftright);

int saveInput(int argc, char *argv[], int *startingX, int *startingY, char *fileName, char *startingParameter);

int readMap(char *fileName, Map *map);

int main(int argc, char *argv[]){
    int startingX = 0;
    int startingY = 0;
    char fileName[100];
    char startingParameter[100];
    saveInput(argc, argv, &startingX, &startingY, fileName, startingParameter);
    Map map;
    readMap(fileName, &map);
    free(map.cells);
    return 0;
}

int searchForNumber(char firstChar){
    //search for number
    //firstChar = char that we want to check
    //returns 1 if its number
    //returns 0 if its whitespace
    //returns -1 if its something esle
    if (firstChar >= '0' && firstChar <= '9'){
        return 1; //number
    }
    if (firstChar == ' '){
        return 0; //whitespace
    }
    else {
        return -1; //char
    }
}

int saveInput(int argc, char *argv[], int *startingX, int *startingY, char *fileName, char *startingParameter){
    if (argc == 2){
        strcpy(fileName, argv[1]);
    }
    if (argc == 3){
        strcpy(startingParameter, argv[1]);
        strcpy(fileName, argv[2]);
    }
    if (argc == 5){
        strcpy(startingParameter, argv[1]);
        *startingX = atoi(argv[2]);
        *startingY = atoi(argv[3]);
        strcpy(fileName, argv[4]);
    }
    return 0;
}
int readMap(char *fileName, Map *map){
    //open file
    FILE *file;
    file = fopen(fileName, "r");
    //return -1 if file is not open
    if (file == NULL){
        return -1;
    }
    char buffer[100];
    //read line from file
    fgets(buffer, 99, file);
    int bufferPosition = 0;
    //save parameters of map
    map->rows = numberFromString(buffer, &bufferPosition);
    map->cols = numberFromString(buffer, &bufferPosition);
    bufferPosition = 0;
    printf("%d \n", map->rows);
    printf("%d \n", map->cols);
    map->cells = malloc((map->rows * map->cols) * sizeof(char));
    if(map->cells == NULL){
        return -2;
    }
    int readRowsCount = 0;
    char *oneRow = malloc(2 * map->rows * sizeof(char));
    if(oneRow == NULL){
        return -2;
    }
    int oneRowPosition;
    while(readRowsCount < map->rows){
        fgets(oneRow, 99, file);
        for(int i = 0; i < map->cols; i++){
            map->cells[(readRowsCount * map->cols) + i] = numberFromString(oneRow, &oneRowPosition);
        }
        readRowsCount++;
        oneRowPosition = 0;
    }
    printf("%d\n", map->cells[0]);
    printf("%d\n", map->cells[1]);
    printf("%d\n", map->cells[2]);
    printf("%d\n", map->cells[3]);
    printf("%d\n", map->cells[4]);
    printf("%d\n", map->cells[5]);
    printf("%d\n", map->cells[6]);
    printf("%d\n", map->cells[7]);
    printf("%d\n", map->cells[8]);
    printf("%d\n", map->cells[9]);
    printf("%d\n", map->cells[10]);
    fclose(file);
    free(oneRow);
    return 0;
}

int numberFromString(char *string, int  *stringPosition){
    int stringLength = strlen(string);
    char *temp = malloc(stringLength * sizeof(char));
    if(temp == NULL){
        return -2;
    }
    int tempLength = 0;
    bool succesfulRead = false;
    if(string[*stringPosition] == ' ' && *stringPosition < stringLength){
        *stringPosition += 1;
    }
    for(;string[*stringPosition] != ' ' && (*stringPosition < stringLength); *stringPosition += 1){
        temp[tempLength] = string[*stringPosition];
        tempLength++;
        succesfulRead = true;
    }
    temp[tempLength] = '\0';
    int finalNumber = atoi(temp);
    free(temp);
    if (succesfulRead == true){
        return finalNumber;
    }
    if(*stringPosition == stringLength){
        return -1;
    }
    return -2;
}
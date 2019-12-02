#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

enum directions {up, left, down, right};

int searchForNumber(char firstChar);

int numberFromString(char *string, int  *stringPosition);

bool isborder(Map *map, int r, int c, int border);

int start_border(Map *map, int r, int c, int leftright);

int saveInput(int argc, char *argv[], int *startingX, int *startingY, char *fileName, char *startingParameter);

int readMap(char *fileName, Map *map);

int typeOfTriangle(int r, int c);

int searchForExit(Map *map, int startingX, int startingY, char *startingParameter);

enum directions LookLeftOrRight(enum directions heading, int x, int y, int leftOrRight);

int moveTo(int *x, int *y, enum directions move, Map *map);

bool leftAndRightAlgo(Map *map, int startingX, int startingY, int leftOrRight);

int main(int argc, char *argv[]){
    int startingX = 0;
    int startingY = 0;
    char fileName[100];
    char startingParameter[100];
    saveInput(argc, argv, &startingX, &startingY, fileName, startingParameter);
    Map map;
    readMap(fileName, &map);
    searchForExit(&map, startingX, startingY, startingParameter);
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
    //printf("%d \n", map->rows);
    //printf("%d \n", map->cols);
    map->cells = malloc(((map->rows + 1) * (map->cols + 1)) * sizeof(char));
    if(map->cells == NULL){
        return -2;
    }
    int readRowsCount = 1;
    char *oneRow = malloc(2 * map->rows * sizeof(char));
    if(oneRow == NULL){
        return -2;
    }
    int oneRowPosition;
    while(readRowsCount <= map->rows){
        fgets(oneRow, 1000, file);
        for(int i = 1; i <= map->cols; i++){
            map->cells[(readRowsCount * map->cols) + i] = numberFromString(oneRow, &oneRowPosition);
        }
        readRowsCount++;
        oneRowPosition = 0;
    }
    //printf("%d\n", map->cells[(2 * map->cols) + 1]);
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

int typeOfTriangle(int r, int c){
    
    //functions returs 0 when its normal triangle and 1 if its inverted triangle
    if(r % 2 == 1){
        if(c % 2 == 1){
            return 1;
        }
        else {
            return 0;
        }
    }
    if(r % 2 == 0){
        if(c % 2 == 1){
            return 0;
        }
        else {
            return 1;
        }
    }
    return -1;
}
bool isborder(Map *map, int r, int c, int border){
    //enum directions {up, left, down, right};
    //border 0 = up, 1 = left 2 = down, 3 = right
    int cell = map->cells[(r * map->cols) + c];
    //printf("sme v borderi, cell %d\n", cell);
    int binnaryCell[3];
    for (int i = 0; i < 3; i++){
        binnaryCell [i] = (cell % 2);
        cell = cell / 2;
    }
    if (border == 1){
        if(binnaryCell[0] == 1){
        //printf("tlacim true\n");
        return true;
        }
        else{
            return false;
        }
    }
    if (border == 3){
        if(binnaryCell[1] == 1){
        return true;
        }
        else{
            return false;
        }
    }
    if (border == 0 && typeOfTriangle(r, c) == 0){
        return true;
    }
    if (border == 0 && typeOfTriangle(r, c) == 1){
        if(binnaryCell[2] == 1){
        //printf("vraciam true\n");
        return true;
        }
        else{
            return false;
        }
    }
    if (border == 2 && typeOfTriangle(r, c) == 1){
        return true;
    }
    if (border == 2 && typeOfTriangle(r, c) == 0){
        if(binnaryCell[2] == 1){
        //printf("vraciam true\n");
        return true;
        }
        else{
            return false;
        }
    }
    return false;
    //enum directions {up, left, down, right};
}
int searchForExit(Map *map, int startingX, int startingY, char *startingParameter){
    bool finnishFound = false;
    if(strcmp(startingParameter, "--rpath") == 0){
        finnishFound = leftAndRightAlgo(map, startingX, startingY, 1);
    }
    if(strcmp(startingParameter, "--lpath") == 0){
        finnishFound = leftAndRightAlgo(map, startingX, startingY, 0);
    }
    if (finnishFound == true){
        return 1;
    }
    return -1;
}

enum directions LookLeftOrRight(enum directions heading, int x, int y, int leftOrRight){
    if(leftOrRight == 1){
        if (heading > 0){
            return (heading - 1);
        }
        if (heading == 0){
            return 3;
        }
    }
    if(leftOrRight == 0){
        if (heading < 3){
            return (heading + 1);
        }
        if (heading == 3){
            return 0;
        }
    }
    return -1;
}

int moveTo(int *x, int *y, enum directions move, Map *map){
    if(move == up){
        *x = *x - 1;
    }
    if(move == down){
        *x = *x + 1;
    }
    if(move == left){
        *y = *y - 1;
    }
    if(move == right){
        *y = *y + 1;
    }
    printf("%d,%d\n",*x,*y);
    return 0;
}

bool leftAndRightAlgo(Map *map, int startingX, int startingY, int leftOrRight){
    enum directions heading;
    enum directions move;
    heading = right;
    move = heading;
    int x = startingX;
    int y = startingY;
    printf("%d,%d\n",x,y);
    bool moveDone = false;
    bool finnishFound = false;
    //printf("%d\n", leftOrRight);
    while(finnishFound == false){
        //printf("Zaciname krok\n");
        //printf("Heading %d\n", heading);
        if(leftOrRight == 1){
            move = LookLeftOrRight(heading, x, y, 1);
        }
        if(leftOrRight == 0){
            move = LookLeftOrRight(heading, x, y, 0);
        }
        //printf("move %d\n", move);
        while(moveDone == false){
            //printf("x %d y %d\n", x, y);
            //ked sme nenasli border
            if(isborder(map, x, y, move) == true){

                if(leftOrRight == 1){
                    move = LookLeftOrRight(move, x, y, 0);
                }
                if(leftOrRight == 0){
                    move = LookLeftOrRight(move, x, y, 1);
                }
                //printf("move vo funkcii %d\n", move);
            }
            if(isborder(map, x, y, move) == false){
                moveDone = true;
            }
        }
        if (moveDone == true){
            if (moveTo(&x, &y, move, map) == 1){
                finnishFound = true;
                //printf("letim");
            }
            else {
                heading = move;
            }
        }
        moveDone = false;
    }
    return finnishFound;
}
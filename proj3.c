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

bool checkForExit(Map *map, int x, int y);

int shortestAlgo(Map *map, int startingX, int startingY);

int findInDatabase(int *database, int numberOfCels, int x, int y);

int addToDatabase(int *database, int *numberOfCels, int x, int y, int distance, int directionsCount, int idOfCell);

int findUnvisitedCells(int *database, int numberOfCels);

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
    char buffer[10000];
    //read line from file
    fgets(buffer, 1000, file);
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
    char *oneRow = malloc(3 * map->rows * sizeof(char));
    if(oneRow == NULL){
        return -2;
    }
    int oneRowPosition;
    while(readRowsCount <= map->rows){
        fgets(oneRow, 10000, file);
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
    if(strcmp(startingParameter, "--shortest") == 0){
        finnishFound = shortestAlgo(map, startingX, startingY);
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
        //turn left
        if(leftOrRight == 1){
            move = LookLeftOrRight(heading, x, y, 1);
        }
        //turn right
        if(leftOrRight == 0){
            move = LookLeftOrRight(heading, x, y, 0);
        }
        //printf("move %d\n", move);
        while(moveDone == false){
            //printf("x %d y %d\n", x, y);
            //check if we found border
            //if yes
            if(isborder(map, x, y, move) == true){
                //look right
                if(leftOrRight == 1){
                    move = LookLeftOrRight(move, x, y, 0);
                }
                //look left
                if(leftOrRight == 0){
                    move = LookLeftOrRight(move, x, y, 1);
                }
                //printf("move vo funkcii %d\n", move);
            }
            //if there is no border, move there
            if(isborder(map, x, y, move) == false){
                moveDone = true;
            }
        }
        //move to the new cell
        moveTo(&x, &y, move, map);
        //check if its exit from maze
        if(checkForExit(map, x, y) == true){
            //if yes return true
            return true;
        }
        //if not print new 
        else{
            printf("%d,%d\n", x, y);
            //set heading to last move
            heading = move;
            //prepare for next move
            moveDone = false;
        }
        
    }
    return false;
}

bool checkForExit(Map *map, int x, int y){
    if (x == 0 || y == 0){
        return true;
    }
    if (x == (map->rows + 1) || y == (map->cols + 1)){
        return true;
    }
    return false;
}

int shortestAlgo(Map *map, int startingX, int startingY){
    enum directions heading;
    enum directions move;
    enum directions unvisitedMove;
    int x = startingX;
    int y = startingY;
    move = 0;
    int *database = malloc(5 * map->rows * map->cols * sizeof(int));
    int numberOfCels = 0;
    int idOfCell = 0;
    int distance = 0;
    int exitDatabase[100];
    int exitDatabaseCount = 0;
    addToDatabase(database, &numberOfCels, x, y, distance, 4, idOfCell);
    //najde sa dalsi prvok z databazy
    while(findUnvisitedCells(database, numberOfCels) != -1){
        idOfCell = findUnvisitedCells(database, numberOfCels);
        printf("zaciname nasu put %d\n", idOfCell);
        while(database[idOfCell * 5 + 3] > 0){
            x = database[idOfCell * 5 + 0];
            y = database[idOfCell * 5 + 1];
            move = LookLeftOrRight(move, x, y, 0);
            if (isborder(map, x, y, move) == false){
                int unvisitedX = x;
                int unvisitedY = y;
                moveTo(&unvisitedX, &unvisitedY, move, map);
                printf("unvisited x %d\n", unvisitedX);
                printf("unvisited y %d\n", unvisitedY);
                unvisitedMove = move;
                if(findInDatabase(database, numberOfCels, unvisitedX, unvisitedY) == -1){
                    int directionsCount = 0;
                    for(int i = 0; i < 4; i++){
                        unvisitedMove = LookLeftOrRight(unvisitedMove, unvisitedX, unvisitedY, 0);
                        if (isborder(map, unvisitedX, unvisitedY, unvisitedMove) == false){
                            directionsCount++;
                            printf("som dnu\n");
                        }
                    }
                    printf("vzdialenost %d\n", (database[idOfCell * 5 + 2] + 2));
                    if(checkForExit(map, unvisitedX, unvisitedY) == false){
                        addToDatabase(database, &numberOfCels, unvisitedX, unvisitedY, (database[idOfCell * 5 + 2] + 1), directionsCount, idOfCell);
                        printf("Pridavame prvok Number of cells %d\n", numberOfCels);
                        printf("x %d\n", database[((numberOfCels -1)* 5) + 0]);
                        printf("y %d\n", database[((numberOfCels -1)* 5) + 1]);
                        printf("vzdialenost %d\n", database[((numberOfCels -1)* 5) + 2]);
                        printf("pocet susedov %d\n", database[((numberOfCels -1)* 5) + 3]);
                        printf("z ktorej sa tam dostanes %d\n", database[((numberOfCels -1)* 5) + 4]);
                    }
                    if(checkForExit(map, unvisitedX, unvisitedY) == true){
                        exitDatabase[exitDatabaseCount] = idOfCell;
                        exitDatabaseCount++;
                        printf("markujem exit %d \n", idOfCell);
                    }
                }
                if(findInDatabase(database, numberOfCels, unvisitedX, unvisitedY) != -1) {
                    int idOfFoundCell = findInDatabase(database, numberOfCels, unvisitedX, unvisitedY);
                    
                    if((database[idOfCell * 5 + 2] + 1) < database[idOfFoundCell * 5 + 2]){
                        printf("prvok s nizsou vzdialenostou %d\n", idOfFoundCell);
                        database[idOfFoundCell * 5 + 2] = (database[idOfCell * 5 + 2] + 1);
                        database[idOfFoundCell * 5 + 4] = idOfCell;
                        printf("x %d\n", database[((idOfFoundCell)* 5) + 0]);
                        printf("y %d\n", database[((idOfFoundCell)* 5) + 1]);
                        printf("vzdialenost %d\n", database[((idOfFoundCell)* 5) + 2]);
                        printf("pocet susedov %d\n", database[((idOfFoundCell)* 5) + 3]);
                        printf("z ktorej sa tam dostanes %d\n", database[((idOfFoundCell)* 5) + 4]);
                    }
                }
                database[idOfCell * 5 + 3] -= 1;
            }
        }
    }
    //find lowest distance
    int lowestDistance = 100000;
    int bestExitIndex = 0;
    for(int i = 0; i < exitDatabaseCount; i++){
        if((database[exitDatabase[i] * 5 + 2] < lowestDistance) && (database[exitDatabase[i] * 5 + 2] > 0)){
            lowestDistance = database[exitDatabase[i] * 5 + 2];
            bestExitIndex = exitDatabase[i];
            printf("lowest distance %d\n", lowestDistance);
            printf("bestExitIndex %d\n", bestExitIndex);
        }
    }
    int *finalPath = malloc((lowestDistance + 2) * sizeof(int));
    finalPath[lowestDistance] = bestExitIndex;
    for (int i = lowestDistance ; i > 0; i--){
        finalPath[i - 1] = database[finalPath[i] * 5 + 4];
        printf("path %d\n",finalPath[i]);
    }
    for(int i = 0; i <= lowestDistance; i++){
        printf("%d,%d\n", database[finalPath[i] * 5 + 0], database[finalPath[i] * 5 + 1]);
    }
    free(database);
    free(finalPath);
    return 0;
}

int findInDatabase(int *database, int numberOfCels ,int x, int y){
    for(int i = 0; i < numberOfCels; i++){
        if(database[i * 5 + 0] == x && database[i * 5 + 1] == y){
            return database[i + 4];
        }
    }
    return -1;
}

int addToDatabase(int *database, int *numberOfCels, int x, int y, int distance, int directionsCount, int idOfCell){
    database[(*numberOfCels * 5) + 0] = x;
    database[(*numberOfCels * 5) + 1] = y;
    database[(*numberOfCels * 5) + 2] = distance;
    database[(*numberOfCels * 5) + 3] = directionsCount;
    database[(*numberOfCels * 5) + 4] = idOfCell;
    *numberOfCels += 1;
    return 0;
}

int findUnvisitedCells(int *database, int numberOfCels){
    for(int i = 0; i < numberOfCels; i++){
        if(database[i * 5 + 3] > 0){
            return i;
        }
    }
    return -1;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define DATABASE_COLS 5
#define DATABASE_X 0
#define DATABASE_Y 1
#define DATABASE_DISTANCE 2
#define DATABASE_NEIGHBOURS 3
#define DATABASE_LASTCELL 4

typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

enum directions {up, left, down, right};

int searchForNumber(char firstChar);

bool isborder(Map *map, int r, int c, int border);

int start_border(Map *map, int r, int c, int leftright);

int saveInput(int argc, char *argv[], int *startingX, int *startingY, char *fileName, char *startingParameter);

int readMap(char *fileName, Map *map);

int typeOfTriangle(int r, int c);

int searchForExit(Map *map, int startingX, int startingY, char *startingParameter);

enum directions LookLeftOrRight(enum directions heading, int leftOrRight);

int moveTo(int *x, int *y, enum directions move, Map *map);

bool leftAndRightAlgo(Map *map, int startingX, int startingY, int leftOrRight);

bool checkForExit(Map *map, int x, int y);

bool shortestAlgo(Map *map, int startingX, int startingY);

int findInDatabase(int *database, int databaseRowsCount, int x, int y);

int addToDatabase(int *database, int *databaseRowsCount, int x, int y, int distance, int directionsCount, int idOfSearchedCell);

int findUnvisitedCells(int *database, int databaseRowsCount);

bool findLowestDistancePath(int *database, int *exitDatabase, int exitDatabaseCount);

int main(int argc, char *argv[]){
    int startingX = 0;
    int startingY = 0;
    char fileName[100];
    char startingParameter[100];
    int inputType = saveInput(argc, argv, &startingX, &startingY, fileName, startingParameter);
    if(inputType == 1){
        if(strcmp(startingParameter, "--help") == 0){
            printf("Program mozme spustit s argmumentom --test a menom suboru. Program vypise ci je dany subor validna mapa.\n");
            printf("dalej je mozne program spustit s argumentami --lpath, --rpath a --shortest. Za tieto argumenty sa dava x a y suradnica a nasledne meno suboru kde sa ma cesta najst\n");
            return 0;
        }
        else return -1;
    }
    Map map;
    readMap(fileName, &map);
    searchForExit(&map, startingX, startingY, startingParameter);
    free(map.cells);
    return 0;
}

int searchForNumber(char firstChar){
    //search for number
    //firstChar = char that we want to check
    //returns 1 if its number or whitespace
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
        strcpy(startingParameter, argv[1]);
        return 1;
    }
    else if (argc == 3){
        strcpy(startingParameter, argv[1]);
        strcpy(fileName, argv[2]);
    }
    else if (argc == 5){
        strcpy(startingParameter, argv[1]);
        *startingX = atoi(argv[2]);
        *startingY = atoi(argv[3]);
        strcpy(fileName, argv[4]);
    }
    else {
        return -1;
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
    char buffer[1000];
    //read line from file
    fgets(buffer, 999, file);
    char *bufferSubstring = strtok(buffer, " ");
    //save parameters of map
    map->rows = atoi(bufferSubstring);
    bufferSubstring = strtok(NULL, " ");
    map->cols = atoi(bufferSubstring);
    map->cells = malloc(((map->rows + 1) * (map->cols + 1)) * sizeof(char));
    if(map->cells == NULL){
        return -2;
    }
    int readRowsCount = 1;
    char *oneRow = malloc(3 * map->cols * sizeof(char));
    if(oneRow == NULL){
        return -2;
    }
    char *cellSubstring;
    while(readRowsCount <= map->rows){
        fgets(oneRow, 3 * map->cols, file);
        cellSubstring = strtok(oneRow, " ");
        for(int i = 1; cellSubstring != NULL; i++){
            map->cells[readRowsCount * map->cols + i] = atoi(cellSubstring);
            cellSubstring = strtok(NULL, " ");
        }
        readRowsCount++;
    }
    fclose(file);
    free(oneRow);
    return 0;
}

int typeOfTriangle(int r, int c){
    //functions returs 0 when its normal triangle and 1 if its inverted triangle
    if(r % 2 == 1){
        if(c % 2 == 1) return 1;
        else return 0;
    }
    if(r % 2 == 0){
        if(c % 2 == 1) return 0;
        else return 1;
    }
    return -1;
}
bool isborder(Map *map, int r, int c, int border){

    //border 0 = up, 1 = left 2 = down, 3 = right
    int cell = map->cells[(r * map->cols) + c];
    if (border == left){
        if(cell & (1 << 0)) return true;
        else return false;
    }
    if (border == right){
        if(cell & (1 << 1)) return true;
        else return false;
    }
    if (border == up && typeOfTriangle(r, c) == 0) return true;
    if (border == up && typeOfTriangle(r, c) == 1){
        if(cell & (1 << 2)) return true;
        else return false;
    }
    if (border == down && typeOfTriangle(r, c) == 1) return true;
    if (border == down && typeOfTriangle(r, c) == 0){
        if(cell & (1 << 2)) return true;
        else return false;
    }
    return false;
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

enum directions LookLeftOrRight(enum directions heading, int leftOrRight){
    //enum directions {up, left, down, right};
    if(leftOrRight == left){
        if (heading > up) return (heading - 1);
        if (heading == up) return right;
    }
    if(leftOrRight == 0){
        if (heading < right) return (heading + 1);
        if (heading == right) return up;
    }
    return -1;
}

int moveTo(int *x, int *y, enum directions move, Map *map){
    if(move == up) *x = *x - 1;
    if(move == down) *x = *x + 1;
    if(move == left) *y = *y - 1;
    if(move == right) *y = *y + 1;
    return 0;
}

bool leftAndRightAlgo(Map *map, int x, int y, int leftOrRight){
    enum directions heading;
    enum directions move;
    if(start_border(map, x, y, leftOrRight) == -1) return false;
    heading = start_border(map, x, y, leftOrRight);
    move = heading;
    printf("%d,%d\n",x,y);
    bool moveDone = false;
    bool finnishFound = false;
    while(finnishFound == false){
        if(leftOrRight == 1) move = LookLeftOrRight(heading, 1);
        //turn right
        if(leftOrRight == 0) move = LookLeftOrRight(heading, 0);
        while(moveDone == false){
            //check if we found border
            if(isborder(map, x, y, move) == true){
                //look right
                if(leftOrRight == 1) move = LookLeftOrRight(move, 0);
                //look left
                if(leftOrRight == 0) move = LookLeftOrRight(move, 1);
            }
            //if there is no border, move there
            if(isborder(map, x, y, move) == false) moveDone = true;
        }
        //move to the new cell
        moveTo(&x, &y, move, map);
        //check if its exit from maze
        if(checkForExit(map, x, y) == true) return true;
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
    if (x == 0 || y == 0) return true;
    if (x == (map->rows + 1) || y == (map->cols + 1)) return true;
    return false;
}

bool shortestAlgo(Map *map, int x, int y){
    enum directions move = 0;
    enum directions unvisitedMove;
    int *database = malloc(DATABASE_COLS * map->rows * map->cols * sizeof(int));
    if(database == NULL){
        return false;
    }
    int databaseRowsCount = 0;
    int idOfSearchedCell = 0;
    int distance = 0;
    int exitDatabase[100];
    int exitDatabaseCount = 0;
    addToDatabase(database, &databaseRowsCount, x, y, distance, 4, idOfSearchedCell);
    //loop while there are no unvisited cells in database
    while(findUnvisitedCells(database, databaseRowsCount) != -1){
        //select cell for which we want to search their neighbours
        idOfSearchedCell = findUnvisitedCells(database, databaseRowsCount);
        //loop while we find all neighbours of that cell
        while(database[idOfSearchedCell * DATABASE_COLS + DATABASE_NEIGHBOURS] > 0){
            //set cordinates of cell that we are standing on
            int unvisitedX = database[idOfSearchedCell * DATABASE_COLS + DATABASE_X];
            int unvisitedY = database[idOfSearchedCell * DATABASE_COLS + DATABASE_Y];
            //rotate to new direciton
            move = LookLeftOrRight(move, 0);
            //check if there is wall in front of us
            if (isborder(map, unvisitedX, unvisitedY, move) == false){
                //if there is no wall, go to the neigbour cell
                moveTo(&unvisitedX, &unvisitedY, move, map);
                //save where we were looking
                unvisitedMove = move;
                //if we didnt find the neigbour cell in database, and if its not possible exit
                if(findInDatabase(database, databaseRowsCount, unvisitedX, unvisitedY) == -1 && checkForExit(map, unvisitedX, unvisitedY) == false){
                    int directionsCount = 0;
                    //count how many neighbours this neihbour cell has
                    for(int i = 0; i < 4; i++){
                        unvisitedMove = LookLeftOrRight(unvisitedMove, 0);
                        if (isborder(map, unvisitedX, unvisitedY, unvisitedMove) == false) directionsCount++;
                    }
                    //add this neigbour cell to database, with its x and y cordinates, with distance +1, with its possible neighbours and with cell id of last cell
                    int distance = database[idOfSearchedCell * DATABASE_COLS + DATABASE_DISTANCE] + 1;
                    addToDatabase(database, &databaseRowsCount, unvisitedX, unvisitedY, distance, directionsCount, idOfSearchedCell);
                }
                //if this cell is possible exit
                if(checkForExit(map, unvisitedX, unvisitedY) == true){
                        //add it to exit database
                        exitDatabase[exitDatabaseCount] = idOfSearchedCell;
                        exitDatabaseCount++;
                }
                //if we found that neigbour cell in database
                if(findInDatabase(database, databaseRowsCount, unvisitedX, unvisitedY) != -1) {
                    //save its ID
                    int idOfFoundCell = findInDatabase(database, databaseRowsCount, unvisitedX, unvisitedY);
                    //if the distance of neigbour cell in database is bigger than new possible distance from our searched cell, update its data
                    if((database[idOfSearchedCell * DATABASE_COLS + DATABASE_DISTANCE] + 1) < database[idOfFoundCell * DATABASE_COLS + DATABASE_DISTANCE]){
                        //update its new shortest possible distance and cell from where we can get there
                        database[idOfFoundCell * DATABASE_COLS + DATABASE_DISTANCE] = (database[idOfSearchedCell * DATABASE_COLS + DATABASE_DISTANCE] + 1);
                        database[idOfFoundCell * DATABASE_COLS + DATABASE_LASTCELL] = idOfSearchedCell;
                    }
                }
                //after we checked one neighbour of seached cell, decrease the number of its possible neighbours
                database[idOfSearchedCell * DATABASE_COLS + DATABASE_NEIGHBOURS] -= 1;
            }
        }
    }
    //find lowest distance
    findLowestDistancePath(database, exitDatabase, exitDatabaseCount);
    free(database);
    return true;
}

int findInDatabase(int *database, int databaseRowsCount ,int x, int y){
    for(int i = 0; i < databaseRowsCount; i++){
        if(database[i * DATABASE_COLS + DATABASE_X] == x && database[i * DATABASE_COLS + DATABASE_Y] == y){
            return database[i + DATABASE_LASTCELL];
        }
    }
    return -1;
}

int addToDatabase(int *database, int *databaseRowsCount, int x, int y, int distance, int directionsCount, int idOfSearchedCell){
    database[(*databaseRowsCount * DATABASE_COLS) + DATABASE_X] = x;
    database[(*databaseRowsCount * DATABASE_COLS) + DATABASE_Y] = y;
    database[(*databaseRowsCount * DATABASE_COLS) + DATABASE_DISTANCE] = distance;
    database[(*databaseRowsCount * DATABASE_COLS) + DATABASE_NEIGHBOURS] = directionsCount;
    database[(*databaseRowsCount * DATABASE_COLS) + DATABASE_LASTCELL] = idOfSearchedCell;
    *databaseRowsCount += 1;
    return 0;
}

int findUnvisitedCells(int *database, int databaseRowsCount){
    for(int i = 0; i < databaseRowsCount; i++){
        if(database[(i * DATABASE_COLS) + DATABASE_NEIGHBOURS] > 0){
            return i;
        }
    }
    return -1;
}

int start_border(Map *map, int x, int y, int leftright){
    //right = 1
    //looking from left side
    int borderID = 0;
    if(y == 1 && x >= 1 && x <= map->rows){
        if(isborder(map, x ,y, left) == false) borderID += 1;
    }
    //looking from up side
    if(x == 1 && y >= 1 && y <= map->cols){
        if(isborder(map, x ,y, up) == false) borderID += 2;
    }
    //looking from down side
    if(x == map->rows && y >= 1 && y <= map->cols){
        if(isborder(map, x ,y, down) == false) borderID += 4;
    }
    //looking from right side
    if(y == map->cols && x >= 1 && x <= map->rows){
        if(isborder(map, x ,y, right) == false) borderID += 8;
    }
    if(borderID == 0) return -1;
    if(borderID == 1) return right;
    if(borderID == 2) return down;
    if(borderID == 4) return up;
    if(borderID == 8) return left;
    if(borderID == 3){
        if(leftright == 1) return right;
        else return down;
    }
    if(borderID == 5){
        if(leftright == 1) return up;
        else return right;
    }
    if(borderID == 10){
        if(leftright == 1) return down;
        else return left;
    }
    if(borderID == 12){
        if(leftright == 1) return left;
        else return up;
    }
    return -1;
}

bool findLowestDistancePath(int *database, int *exitDatabase, int exitDatabaseCount){
    int lowestDistance = INT_MAX;
    int bestExitIndex = 0;
    for(int i = 0; i < exitDatabaseCount; i++){
        if((database[exitDatabase[i] * DATABASE_COLS + DATABASE_DISTANCE] < lowestDistance) && (database[exitDatabase[i] * DATABASE_COLS + DATABASE_DISTANCE] > 0)){
            lowestDistance = database[exitDatabase[i] * DATABASE_COLS + DATABASE_DISTANCE];
            bestExitIndex = exitDatabase[i];
        }
    }
    if(bestExitIndex != 0){
        int *finalPath = malloc((lowestDistance + 2) * sizeof(int));
        if(finalPath == NULL){
            return false;
        }
        finalPath[lowestDistance] = bestExitIndex;
        for (int i = lowestDistance ; i > 0; i--){
            finalPath[i - 1] = database[finalPath[i] * DATABASE_COLS + DATABASE_LASTCELL];
        }
        for(int i = 0; i <= lowestDistance; i++){
            printf("%d,%d\n", database[finalPath[i] * DATABASE_COLS + DATABASE_X], database[finalPath[i] * DATABASE_COLS + DATABASE_Y]);
        }
        free(finalPath);
    }
    else{
        printf("%d,%d\n", database[bestExitIndex * DATABASE_COLS + DATABASE_X], database[bestExitIndex * DATABASE_COLS + DATABASE_Y]);
    }

    return true;
}

int throwError
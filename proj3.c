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
#define DATABASE_ENTRY(x, y) ((x - 1) * map->cols * DATABASE_COLS) + ((y - 1) * DATABASE_COLS)
#define DATABASE_EMPTY_CELL -1

#define WRONG_INPUT -1
#define ALLOC_ERR -2
#define WRONG_FILE -3
#define WRONG_MAP -4

#define NORMAL_TRIANGEL 0
#define INVERTED_TRIANGEL 1

#define ROTATE_CLOCKWISE 1
#define ROTATE_COUNTER_CLOKWISE 0
#define NUMBER_OF_ROTATIONS 4

#define RIGHT_HAND_RULE 1
#define LEFT_HAND_RULE 0

typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

enum directions {up, left, down, right};

int checkIfItsNumbers(char *string);

bool isborder(Map *map, int r, int c, int border);

int start_border(Map *map, int r, int c, int leftright);

int saveInput(int argc, char *argv[], int *startingX, int *startingY, char *fileName, char *startingParameter);

int readMap(char *fileName, Map *map);

int typeOfTriangle(int r, int c);

int searchForExit(Map *map, int startingX, int startingY, char *startingParameter);

enum directions LookLeftOrRight(enum directions heading, int leftOrRight);

int moveTo(int *x, int *y, enum directions move);

bool leftAndRightAlgo(Map *map, int startingX, int startingY, int leftOrRight);

bool checkForExit(Map *map, int x, int y);

bool shortestAlgo(Map *map, int startingX, int startingY);

int findInDatabase(int *database, int x, int y, Map *map);

void addToDatabase(int *database, Map *map, int x, int y, int distance, int directionsCount, int idOfSearchedCell);

int findUnvisitedCells(int *database, int databaseRowsCount, int *lastUnvisitedCell);

bool findLowestDistancePath(int *database, int *exitDatabase, int exitDatabaseCount);

int checkForCorrectMap(Map *map);

int throwError(int errorType);

void printHelp();

int saveMapCells(Map *map, char *oneRow, FILE *file, int *readRowsCount);

int validateMap(Map *map, char *startingParameter, char *fileName);

int main(int argc, char *argv[]){
    //initialize variabiles
    int startingX = 0;
    int startingY = 0;
    char fileName[100];
    char startingParameter[100];
    //save input from parameters
    int numberOfInputs = saveInput(argc, argv, &startingX, &startingY, fileName, startingParameter);
    if(numberOfInputs == 1){
        //check if we want to print help
        if(strcmp(startingParameter, "--help") == 0){
            printHelp();
            return 0;
        }
        //else throwError
        else return throwError(WRONG_INPUT);
    }
    //if input wasnt valid
    if(numberOfInputs == WRONG_INPUT) return throwError(WRONG_INPUT);
    //create struct map
    Map map;
    //check if we want to test map
    if(numberOfInputs == 2){
        return validateMap(&map, startingParameter, fileName);
    }
    //if we want to search for path, save map
    if (readMap(fileName, &map) != 0) return throwError(WRONG_INPUT);
    //start search for exit
    searchForExit(&map, startingX, startingY, startingParameter);
    //at the end, free mallocs
    free(map.cells);
    return 0;
}

int checkIfItsNumbers(char *string){
    //functions check if string contains only numbers and whitespaces
    //string = pointer to string that we want to check
    //returns 0 if its correct
    //returns -1 if its incorrect
    bool correctInput = false;
    //loop through whole string
    for(int i = 0; string[i] != '\0'; i++){
        //check if all chars are numbers or whitespaces
        if((string[i] >= '0' && string[i] <= '9') || string[i] == ' ' || string[i] == '\n' || string[i] == '\r'){
            correctInput = true;
        }
        //if not return -
        else{
            correctInput = false;
            return WRONG_INPUT;
        }
    }
    //if everything was correct, return 0
    if(correctInput == true) return 0;
    return WRONG_INPUT;
}

int saveInput(int argc, char *argv[], int *startingX, int *startingY, char *fileName, char *startingParameter){
    //functions save inputs from arguments
    //argc = arg count
    //argv = array of arguments
    //*startingX = pointer to int, where we want to store x
    //*startingY = pointer to int, where we want to store y
    //*fileName = pointer to char array, where we want to store fileName
    //*startingParameter = pointer to char array, where we want to store startingParameter
    //functions returns number of succesfully read arguments, or -1, if input wasnt valid
    if (argc == 2){
        strcpy(startingParameter, argv[1]);
        return argc - 1;
    }
    else if (argc == 3){
        strcpy(startingParameter, argv[1]);
        strcpy(fileName, argv[2]);
        return argc - 1;
    }
    else if (argc == 5){
        strcpy(startingParameter, argv[1]);
        *startingX = atoi(argv[2]);
        *startingY = atoi(argv[3]);
        strcpy(fileName, argv[4]);
        return argc - 1;
    }
    else {
        return WRONG_INPUT;
    }
    return 0;
}
int readMap(char *fileName, Map *map){
    //functions reads from file, and save values into struct map
    //*fileName = char pointer to name of file that we want to open
    //*map = pointer to struct map, that we want to write to
    //function returns -1 when input is wrong, 0 when read was correct

    //open file
    FILE *file;
    file = fopen(fileName, "r");
    //return error if file is not open
    if (file == NULL){
        return throwError(WRONG_FILE);
    }
    //prepare buffer for first line
    char buffer[1000];
    //read line from file
    fgets(buffer, 999, file);
    //check if line is correct
    if(checkIfItsNumbers(buffer) != 0) return WRONG_INPUT;
    //split buffer to substring by whitespace
    char *bufferSubstring = strtok(buffer, " ");
    //save parameters of map
    map->rows = atoi(bufferSubstring);
    //get next substring
    bufferSubstring = strtok(NULL, " ");
    map->cols = atoi(bufferSubstring);
    //check if there are no more data, if yes map is wrong
    bufferSubstring = strtok(NULL, " ");
    if(bufferSubstring != NULL) return WRONG_INPUT;
    //malloc array for all cell
    map->cells = malloc(((map->rows + 1) * (map->cols + 1)) * sizeof(char));
    if(map->cells == NULL){
        return ALLOC_ERR;
    }
    //prepare string for reading one line at time from file
    int readRowsCount = 1;
    char *oneRow = malloc(3 * map->cols * sizeof(char));
    if(oneRow == NULL){
        return ALLOC_ERR;
    }
    //loop while we are at the end of FILE
    if(saveMapCells(map, oneRow, file, &readRowsCount) == WRONG_INPUT) return WRONG_INPUT;
    //close file and free temporary array
    fclose(file);
    free(oneRow);
    return 0;
}

int typeOfTriangle(int r, int c){
    //functions returs 0 when its normal triangle and 1 if its inverted triangle
    //r = x cordinate of cell
    //c = y cordinate of cell

    //if we are on odd row
    if(r % 2 == 1){
        //if we are on odd cell
        if(c % 2 == 1) return INVERTED_TRIANGEL;
        else return NORMAL_TRIANGEL;
    }
    //if we are on even row
    if(r % 2 == 0){
        //if we are on even cell
        if(c % 2 == 1) return NORMAL_TRIANGEL;
        else return INVERTED_TRIANGEL;
    }
    return WRONG_INPUT;
}
bool isborder(Map *map, int r, int c, int border){
    //function returns true, if there is border, false if there isnt border
    //*map = pointer to struct map,
    //r = x cordinate of cell
    //c = y cordinate of cell
    //border = enum directions {up, left, down, right};

    const int firstBit = 0;
    const int secondBit = 1;
    const int thirdBit = 2;
    //save current cell number
    int cell = map->cells[(r * map->cols) + c];
    //for left border, check first bit
    if (border == left){
        if(cell & (1 << firstBit)) return true;
        else return false;
    }
    //for right border, check second border
    if (border == right){
        if(cell & (1 << secondBit)) return true;
        else return false;
    }
    //return true, if we want to move up on normal triangle
    if (border == up && typeOfTriangle(r, c) == NORMAL_TRIANGEL) return true;
    //for up border, on inverted triangle
    if (border == up && typeOfTriangle(r, c) == INVERTED_TRIANGEL){
        if(cell & (1 << thirdBit)) return true;
        else return false;
    }
    //return true, if we want to move down on inverted triangle
    if (border == down && typeOfTriangle(r, c) == INVERTED_TRIANGEL) return true;
    //for down border, on normal triangle
    if (border == down && typeOfTriangle(r, c) == NORMAL_TRIANGEL){
        if(cell & (1 << thirdBit)) return true;
        else return false;
    }
    //return false if we didnt find that cell
    return false;
}
int searchForExit(Map *map, int startingX, int startingY, char *startingParameter){
    //functions starts algorithm that we want to use, to find path in maze
    //*map = pointer to struct map
    //startingX = x coordinate of start
    //startingY = y coordinate of start
    //*startingParameter = algo that we want to use
    //function returns 1 when we found finnish, -1 if input was wrong
    bool finnishFound = false;
    //start rpath algo
    if(strcmp(startingParameter, "--rpath") == 0){
        //check if map is correct
        if(checkForCorrectMap(map) != 0) return throwError(WRONG_INPUT);;
        finnishFound = leftAndRightAlgo(map, startingX, startingY, RIGHT_HAND_RULE);
    }
    //start lpath algo
    if(strcmp(startingParameter, "--lpath") == 0){
        //check if map is correct
        if(checkForCorrectMap(map) != 0) return throwError(WRONG_INPUT);;
        finnishFound = leftAndRightAlgo(map, startingX, startingY, LEFT_HAND_RULE);
    }
    //start Dijkstra shortest algo
    if(strcmp(startingParameter, "--shortest") == 0){
        //check if map is correct
        if(checkForCorrectMap(map) != 0) return throwError(WRONG_INPUT);;
        finnishFound = shortestAlgo(map, startingX, startingY);
    }
    //return 1 when finnish was found
    if(finnishFound == true){
        return 1;
    }
    //if something went wrong, throw error
    return throwError(WRONG_INPUT);
}

enum directions LookLeftOrRight(enum directions heading, int leftOrRight){
    //function takes heading, and returns new heading turned once left or right
    //heading = input direction where we are looking
    //lefOrRight == 1 when we want to rotate clokwise, == 0 when we want to rotate counterclokwise
    
    //take heading, and rotate clokwise once
    if(leftOrRight == ROTATE_CLOCKWISE){
        if (heading > up) return (heading - 1);
        if (heading == up) return right;
    }
    //take heading, adn rotate counterclokwise once
    if(leftOrRight == ROTATE_COUNTER_CLOKWISE){
        if (heading < right) return (heading + 1);
        if (heading == right) return up;
    }
    return WRONG_INPUT;
}

int moveTo(int *x, int *y, enum directions move){
    //functions will take x and y and move it one cell to desired direction
    //*x = pointer to x coordinate that we want to move
    //*y = pointer to y coordinate that we want to move
    //move = our desired direction of move

    //move x and y accordingy to move direction
    if(move == up) *x = *x - 1;
    if(move == down) *x = *x + 1;
    if(move == left) *y = *y - 1;
    if(move == right) *y = *y + 1;
    return 0;
}

bool leftAndRightAlgo(Map *map, int x, int y, int leftOrRight){
    //function will find path according to left or right hand rule
    //map = pointer to struct map
    //x = x coordinate of start
    //y = y coordinate of start
    //leftOrRight = 1 when we are using right hand rule, = 0 when we are using left hand rule
    //functions returns true when we found exit, else it returns false
    enum directions heading;
    enum directions move;
    //if we are starting outside of possible starts, return false
    if(start_border(map, x, y, leftOrRight) == WRONG_INPUT) return false;
    //set initial heading accordig to start border function
    heading = start_border(map, x, y, leftOrRight);
    //set first move direction same as heading
    move = heading;
    //print starting coordinates
    printf("%d,%d\n",x,y);
    bool moveDone = false;
    bool finnishFound = false;
    //loop until we will find finnish
    while(finnishFound == false){
        //if algo is left,  turn to left
        if(leftOrRight == RIGHT_HAND_RULE) move = LookLeftOrRight(heading, ROTATE_CLOCKWISE);
        //if algo is right, turn to right
        if(leftOrRight == LEFT_HAND_RULE) move = LookLeftOrRight(heading, ROTATE_COUNTER_CLOKWISE);
        //loop while we will move to next cell
        while(moveDone == false){
            //check if we found border
            if(isborder(map, x, y, move) == true){
                //if algo is left, look right
                if(leftOrRight == RIGHT_HAND_RULE) move = LookLeftOrRight(move, ROTATE_COUNTER_CLOKWISE);
                //if algo is right, look left
                if(leftOrRight == LEFT_HAND_RULE) move = LookLeftOrRight(move, ROTATE_CLOCKWISE);
            }
            //if there is no border in new move, go there, set move done as true
            if(isborder(map, x, y, move) == false) moveDone = true;
        }
        //move to the new cell
        moveTo(&x, &y, move);
        //check if its exit from maze, if yes return true
        if(checkForExit(map, x, y) == true) return true;
        //if not print where we are standing
        else{
            //print where we are standing
            printf("%d,%d\n", x, y);
            //set heading to last move
            heading = move;
            //prepare for next move
            moveDone = false;
        }
    }
    //if we didnt find exit, return false
    return false;
}

bool checkForExit(Map *map, int x, int y){
    //function returns true if we found possible exit, else it returns false
    //*map = pointer to struct map
    //x = x coordinate of cell that we want to check
    //y = y coordinate of cell that we want ot check

    //check if we are outside of map
    if (x == 0 || y == 0) return true;
    if (x == (map->rows + 1) || y == (map->cols + 1)) return true;
    return false;
}

bool shortestAlgo(Map *map, int x, int y){
    //funciton finds shortest path in map, based on Dijkstra algo
    //*map = pointer to struct map
    //x = x coordinate of start cell
    //y = y coordinate of start cell

    //check if we have valid entrance into map
    if(start_border(map, x, y, RIGHT_HAND_RULE) == WRONG_INPUT) return false;
    enum directions move = up;
    enum directions unvisitedMove;
    //prepare database for use
    int databaseSize = map->rows * map->cols;
    int *database = malloc(DATABASE_COLS * databaseSize * sizeof(int));
    if(database == NULL){
        return false;
    }
    //initialize database with empty cells
    for(int i = 0; i < DATABASE_COLS * databaseSize; i++){
        database[i] = DATABASE_EMPTY_CELL;
    }
    int *unvisitedDatabase = malloc(databaseSize * sizeof(int));
    int unvisitedDatabaseCount = 0;
    int idOfSearchedCell = 0;
    int distance = 0;
    int exitDatabase[100];
    int exitDatabaseCount = 0;
    //add starting cell to database, with distance 0, and 4 neighbours
    addToDatabase(database, map, x, y, distance, NUMBER_OF_ROTATIONS, idOfSearchedCell);
    //add cell to unvisited database
    unvisitedDatabase[unvisitedDatabaseCount] = DATABASE_ENTRY(x, y);
    unvisitedDatabaseCount++;
    int lastUnvisitedCell = 0;
    bool oneCellDone = false;
    //loop while there are no unvisited cells in database
    while(idOfSearchedCell != DATABASE_EMPTY_CELL){
        //save id of unvisited cell
        idOfSearchedCell = findUnvisitedCells(unvisitedDatabase, unvisitedDatabaseCount, &lastUnvisitedCell);
        oneCellDone = false;
        //loop while we find all neighbours of that cell
        while(oneCellDone == false && idOfSearchedCell != DATABASE_EMPTY_CELL){
            //set cordinates of cell that we are standing on
            int unvisitedX = database[unvisitedDatabase[idOfSearchedCell] + DATABASE_X];
            int unvisitedY = database[unvisitedDatabase[idOfSearchedCell] + DATABASE_Y];
            //rotate to new direciton
            move = LookLeftOrRight(move, ROTATE_COUNTER_CLOKWISE);
            //check if there is wall in front of us
            if(isborder(map, unvisitedX, unvisitedY, move) == false){
                //if there is no wall, go to the neigbour cell
                moveTo(&unvisitedX, &unvisitedY, move);
                //save where we were looking
                unvisitedMove = move;
                int idOfFoundCell = findInDatabase(database, unvisitedX, unvisitedY, map);
                //if we didnt find the neigbour cell in database, and if its not possible exit
                if(idOfFoundCell == DATABASE_EMPTY_CELL && checkForExit(map, unvisitedX, unvisitedY) == false){
                    int directionsCount = 0;
                    //count how many neighbours this neihbour cell has
                    for(int i = 0; i < NUMBER_OF_ROTATIONS; i++){
                        //rotate right
                        unvisitedMove = LookLeftOrRight(unvisitedMove, 0);
                        //check if there is border, if yes add dirrections count
                        if(isborder(map, unvisitedX, unvisitedY, unvisitedMove) == false) directionsCount++;
                    }
                    //add this neigbour cell to database, with its x and y cordinates, with distance +1, with its possible neighbours and with cell id of last cell
                    int distance = database[unvisitedDatabase[idOfSearchedCell] + DATABASE_DISTANCE] + 1;
                    addToDatabase(database, map, unvisitedX, unvisitedY, distance, directionsCount, unvisitedDatabase[idOfSearchedCell]);
                    //add this neigbour cell to unvisited database for future searching
                    unvisitedDatabase[unvisitedDatabaseCount] = DATABASE_ENTRY(unvisitedX, unvisitedY);
                    unvisitedDatabaseCount++;
                }
                //if this cell is possible exit
                if(checkForExit(map, unvisitedX, unvisitedY) == true){
                        //add it to exit database
                        exitDatabase[exitDatabaseCount] = unvisitedDatabase[idOfSearchedCell];
                        exitDatabaseCount++;
                }
                //if we found that neigbour cell in database
                if(idOfFoundCell != DATABASE_EMPTY_CELL) {
                    //if the distance of neigbour cell in database is bigger than new possible distance from our searched cell, update its data
                    if((database[unvisitedDatabase[idOfSearchedCell] + DATABASE_DISTANCE] + 1) < database[idOfFoundCell + DATABASE_DISTANCE]){
                        //update its new shortest possible distance and cell from where we can get there
                        database[idOfFoundCell + DATABASE_DISTANCE] = (database[unvisitedDatabase[idOfSearchedCell] + DATABASE_DISTANCE] + 1);
                        database[idOfFoundCell + DATABASE_LASTCELL] = unvisitedDatabase[idOfSearchedCell];
                    }
                }
                //after we checked one neighbour of seached cell, decrease the number of its possible neighbours
                database[unvisitedDatabase[idOfSearchedCell] + DATABASE_NEIGHBOURS] -= 1;
                //check if cell has zero unvisited neighbours
                if(database[unvisitedDatabase[idOfSearchedCell] + DATABASE_NEIGHBOURS] == 0){
                    //remove it from unvisited database
                    unvisitedDatabase[idOfSearchedCell] = DATABASE_EMPTY_CELL;
                    oneCellDone = true;
                }
            }
        }
    }
    //find lowest distance and print
    findLowestDistancePath(database, exitDatabase, exitDatabaseCount);
    //free malloc
    free(database);
    free(unvisitedDatabase);
    return true;
}

int findInDatabase(int *database,int x, int y, Map *map){
    //functions checks if cell is already in database
    //*database = pointer to database of cells
    //databaseRowsCount = count of entries in database
    //x = x coordinate of cell that we are looking for
    //y = y coordinate of cell that we are looking for
    //function returns id of cell, if we found it or -1 if its not in database

    //Check if we are searching for exiting cell
    if(checkForExit(map, x, y) == true) return DATABASE_EMPTY_CELL;
    //if cell is in database, return its ID
    if(database[DATABASE_ENTRY(x, y) + DATABASE_X] != DATABASE_EMPTY_CELL) return (DATABASE_ENTRY(x, y));
    //else return -1
    return DATABASE_EMPTY_CELL;
}

void addToDatabase(int *database, Map *map, int x, int y, int distance, int directionsCount, int idOfSearchedCell){
    //functions will add new entry into database
    //*database = pointer to database
    //*databaseRowsCount = pointer to count of entries in database
    //x = x coordinate of cell that we want to add
    //y = y coordinate of cell that we want to add
    //distance = distance to the cell from start
    //directionsCount = count of cell possible neighbours
    //idOfSearchedCell = id of cell from where we can get to our new cell
    //function returns 0 when everything is OK

    //add new cell entry into database
    database[DATABASE_ENTRY(x, y) + DATABASE_X] = x;
    database[DATABASE_ENTRY(x, y) + DATABASE_Y] = y;
    database[DATABASE_ENTRY(x, y) + DATABASE_DISTANCE] = distance;
    database[DATABASE_ENTRY(x, y) + DATABASE_NEIGHBOURS] = directionsCount;
    database[DATABASE_ENTRY(x, y) + DATABASE_LASTCELL] = idOfSearchedCell;
}

int findUnvisitedCells(int *unvisitedDatabase, int unvisitedDatabaseRowsCount, int *lastUnvisitedCell){
    //function will find cells in database, that have unvisited neighbours
    //*unvisitedDatabase = pointer to database of unvisited cells
    //unvisitedDatabaseRowsCount = count of entries in database
    //*lastUnvisitedCell = pointer to last cell id we checked
    //function will return id of cell in database that has unvisited neighbours
    //if there are no cell with unvisited neighbours it returns false

    //loop through every entry in database
    for(int i = *lastUnvisitedCell; i < unvisitedDatabaseRowsCount; i++){
        //chech if cell has unvisited neighbours
        if(unvisitedDatabase[i] != DATABASE_EMPTY_CELL){
            //if yes return its id
            *lastUnvisitedCell = i;
            return i;
        }
    }
    //else return -1
    return DATABASE_EMPTY_CELL;
}

int start_border(Map *map, int x, int y, int leftright){
    //functions will find direction, from where we entered the MAP
    //*map = pointer to struct map
    //x = x coordinate of starting cell
    //y = y coordinate of starting cell
    //leftright = if we want to use left or right hand algo, 1 for right hand, 0 for left hand
    //function returns direction, from where we entered the Map

    //ID of different starts
    int borderID = 0;
    //if we started from left side
    if(y == 1 && x >= 1 && x <= map->rows){
        //if there is no border on left, add +1 identificator
        if(isborder(map, x ,y, left) == false) borderID += 1;
    }
    //if we started from up side
    if(x == 1 && y >= 1 && y <= map->cols){
        //if there is no border on up, add +2 id
        if(isborder(map, x ,y, up) == false) borderID += 2;
    }
    //if we started from down side
    if(x == map->rows && y >= 1 && y <= map->cols){
        //if there is no border on down, add +4 id
        if(isborder(map, x ,y, down) == false) borderID += 4;
    }
    //if we started from right side
    if(y == map->cols && x >= 1 && x <= map->rows){
        //if there is no border on right, add +8 id
        if(isborder(map, x ,y, right) == false) borderID += 8;
    }
    //if we cant enter map
    if(borderID == 0) return WRONG_INPUT;
    //if we are entering from left
    if(borderID == 1) return right;
    //if we are entering from up
    if(borderID == 2) return down;
    //if we are entering from down
    if(borderID == 4) return up;
    //if we are entering from right
    if(borderID == 8) return left;
    //if we are on corners
    if(borderID == 3){
        //check what algo we are using
        if(leftright == RIGHT_HAND_RULE) return right;
        else return down;
    }
    //if we are on corners
    if(borderID == 5){
        //check what algo we are using
        if(leftright == RIGHT_HAND_RULE) return up;
        else return right;
    }
    //if we are on corners
    if(borderID == 10){
        //check what algo we are using
        if(leftright == RIGHT_HAND_RULE) return down;
        else return left;
    }
    //if we are on corners
    if(borderID == 12){
        //check what algo we are using
        if(leftright == RIGHT_HAND_RULE) return left;
        else return up;
    }
    //return -1 if we are not entering correctly
    return WRONG_INPUT;
}

bool findLowestDistancePath(int *database, int *exitDatabase, int exitDatabaseCount){
    //functions will take Database of possible exits and will build shortestPath
    //*database = pointer to database of all cells
    //*exitdatabase = pointer to database of possible exits
    //exitDatabaseCount = number of possible exits

    //set lowest distance to max possible value
    int lowestDistance = INT_MAX;
    int bestExitIndex = 0;
    //loop while we go through all possible exits
    for(int i = 0; i < exitDatabaseCount; i++){
        //find exit with smallest distance from start
        if((database[exitDatabase[i] + DATABASE_DISTANCE] < lowestDistance) && (database[exitDatabase[i] + DATABASE_DISTANCE] > 0)){
            //save lowest distance and index of cell that is best
            lowestDistance = database[exitDatabase[i] + DATABASE_DISTANCE];
            bestExitIndex = exitDatabase[i];
        }
    }
    //if there is possible exit
    if(lowestDistance < INT_MAX){
        //prepare array of finalPath from start to finnish
        int *finalPath = malloc((lowestDistance + 1) * sizeof(int));
        //check if malloc was succesful
        if(finalPath == NULL){
            throwError(ALLOC_ERR);
            return false;
        }
        finalPath[lowestDistance] = bestExitIndex;
        //go backwards from best exit to starting cell and save its ID into final path
        for (int i = lowestDistance ; i > 0; i--){
            finalPath[i - 1] = database[finalPath[i] + DATABASE_LASTCELL];
            //printf("final path %d\n", finalPath[i - 1]);
        }
        //when we are finnished building finalPath, print every cell from it
        for(int i = 0; i <= lowestDistance; i++){
            printf("%d,%d\n", database[finalPath[i] + DATABASE_X], database[finalPath[i] + DATABASE_Y]);
        }
        //free malloc
        free(finalPath);
    }
    //if there was no possible exit, print only starting cell
    else{
        printf("%d,%d\n", database[exitDatabase[0] + DATABASE_X], database[exitDatabase[0] + DATABASE_Y]);
    }
    return true;
}

int throwError(int errorType){
    //functions with print error based on error type
    //errorType = type of error
    //function returns -1
    if(errorType == ALLOC_ERR){
        fprintf(stderr, "Program cant allocate memmory!\n");
        return WRONG_INPUT;
    }
    if(errorType == WRONG_INPUT){
        fprintf(stderr, "Wrong Input!\n");
        return WRONG_INPUT;
    }
    if(errorType == WRONG_FILE){
        fprintf(stderr, "Unable to open file!\n");
        return WRONG_INPUT;
    }
    if(errorType == WRONG_MAP){
        fprintf(stderr, "Invalid\n");
        return WRONG_INPUT;
    }
    return 0;
}

int checkForCorrectMap(Map *map){
    //function will check if borders of cell are correct
    //*map = pointer to struct map
    //function returns -1 when map is incorrect, 0 when its correct

    //set start heading
    enum directions heading = down;
    //set move direction to direction of move
    enum directions move = heading;
    //loop while we go through all cell of map
    for(int i = 1; i <= map->rows; i++){
        for(int j = 1; j <= map->cols; j++){
            //for every cell go to every possible direction
            for(int l = 0; l < NUMBER_OF_ROTATIONS; l++){
                //firts, look right
                heading = LookLeftOrRight(heading, ROTATE_CLOCKWISE);
                //save if ther is border in front of us
                bool borderInCell = isborder(map, i, j, heading);
                //set new coordinates
                int unvisitedX = i;
                int unvisitedY = j;
                //move new coordinates to desired heading
                moveTo(&unvisitedX, &unvisitedY, heading);
                //chcek if we are still inside map
                if(checkForExit(map, unvisitedX, unvisitedY) == false){
                    move = heading;
                    //look clokwise two times
                    move = LookLeftOrRight(move, ROTATE_CLOCKWISE);
                    move = LookLeftOrRight(move, ROTATE_CLOCKWISE);
                    //save if ther is border in front of us, it should be the same border as, in defautl cell
                    bool unvisitedBorder = isborder(map, unvisitedX, unvisitedY, move);
                    //check if both borders are the same, if not return -1
                    if(borderInCell != unvisitedBorder) return WRONG_INPUT;
                }
            }
        }
    }
    //if all borders are defined correctly, return 0
    return 0;
}

void printHelp(){
    //functions prints help when called
    printf("Run it with argumenst: ./proj3 [algorithm] R C map.txt\n");
    printf("algorithm:\n");
    printf("        --lpath     Program finds path based on left hand rule\n");
    printf("        --rpath     Program finds path based on right hand rule\n");
    printf("        --shortest  Program finds shortest path based on Dijkstra algorithm\n");
    printf("        --test      Program will test if your map is correct\n");
    printf("R           Starting row\n");
    printf("C           Starting coll\n");
    printf("map.txt     TXT file that defines map\n");
}

int saveMapCells(Map *map, char *oneRow, FILE *file, int *readRowsCount){
    //function will save cells from file, into map
    //*map = pointer to struct map
    //*onerow = pointer to malloced temporary row
    //file = pointer to file from which we want to read
    //readRowsCount = pointer to number of rows read
    
    //loop until we are at the end of file
    while(fgets(oneRow, 3 * map->cols, file) != NULL){
        char *cellSubstring;
        //check if line is valid
        if(checkIfItsNumbers(oneRow) != 0) return WRONG_INPUT;
        //split line into substrings
        cellSubstring = strtok(oneRow, " ");
        int readCellCount = 0;
        //loop while we go through entire line
        for(int i = 1; cellSubstring != NULL; i++){
            //save number describing cell into array
            map->cells[*readRowsCount * map->cols + i] = atoi(cellSubstring) % 8;    //modulo 8 to get last 3 bits
            //get new substring
            cellSubstring = strtok(NULL, " ");
            readCellCount++;
            //check if map is bigger than expected
            if(readCellCount > map->cols) return WRONG_INPUT;
        }
        *readRowsCount += 1;
        //check if map is bigger than expected
        if(*readRowsCount > map->rows + 1) return WRONG_INPUT;
    }
    return 0;
}

int validateMap(Map *map, char *startingParameter, char *fileName){
    if(strcmp(startingParameter, "--test") == 0){
        //read map
        int mapStatus = readMap(fileName, map);
        //throw error acording to status
        if(mapStatus == WRONG_INPUT) return throwError(WRONG_MAP);
        if(mapStatus == ALLOC_ERR) return throwError(ALLOC_ERR);
        if(mapStatus == 0){
            //check if walls of map are valid
            if (checkForCorrectMap(map) == WRONG_INPUT) return throwError(WRONG_MAP);
            else {
                //print valid if map was valid
                printf("Valid\n");
                return 0;
            }
        }
    }
    //input wasnt valid
    return throwError(WRONG_INPUT);
}

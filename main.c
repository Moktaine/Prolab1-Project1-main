#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <curl/curl.h>

const int window_WIDTH = 1920;
int window_HEIGHT = 600;

int polygonCount = 0;
int polygonVertexCounts[3];
int squareCounts[3];

//Boolean for Checking if variables are already printed so they don't get printed several times
bool IsVariablesPrinted = false;

//Values to be printed
int platformCount = 0, drillCount = 0;
int drillCost, platformCost;
int totalArea = 0;

//Coordinate Structure
struct Coordinate{
    int x;
    int y;
};

//2D Array for storing vertex coordinates of polygons
struct Coordinate polygons[3][10];

//2D Arrays for storing every unit square inside polygon areas
struct Coordinate SquaresInsidePolygon[3][500];
struct Coordinate Default_SquaresInsidePolygon[3][500];






//Returns the index of the given unit square in SquaresInsidePolygon array
int SquareIndexInArray(int x, int y, int polygonIndex){
    for(int i = 0; i < squareCounts[polygonIndex]; i++){
        if(SquaresInsidePolygon[polygonIndex][i].x == x && SquaresInsidePolygon[polygonIndex][i].y == y){
            return i;
        }
    }
    return -1;
}


//Checks if any unit square inside given square is either not inside polygon or occupied
bool CanDrawSquare(int x, int y, int edgeLength, int polygonIndex){
    for(int i = 0; i < edgeLength; i++){
        for(int j = 0; j < edgeLength; j++){
            int checkX = x + (j*10);
            int checkY = y + (i*10);
            if(SquareIndexInArray(checkX,checkY ,polygonIndex) == -1){
                return false;
            }
        }
    }
    return true;
}


//Tags every unit square inside given square as occupied
void RemoveSquareFromSquareArray(int x, int y, int edgeLength, int polygonIndex){

    for(int i = 0; i < edgeLength; i++){
        for(int j = 0; j < edgeLength; j++){
            int removeX = x + (j*10);
            int removeY = y + (i*10);

            int index = SquareIndexInArray(removeX, removeY, polygonIndex);
            SquaresInsidePolygon[polygonIndex][index].x = -1;
            SquaresInsidePolygon[polygonIndex][index].y = -1;
        }
    }
}



//Draws the square with given edge length
void DrawBigSquare(int x, int y, int edgeLength, int polygonIndex,bool isNeighbour){
    //Checks if given square can be drawn and if so draw
    if(CanDrawSquare(x,y,edgeLength/10,polygonIndex)){
        //Set Color

        switch(edgeLength/10){
            case 16:
                glColor4f(1,0,0,.5);
                break;
            case 8:
                glColor4f(0,0,1,.5);
                break;
            case 4:
                glColor4f(1,.5,0,.5);
                break;
            case 2:
                glColor4f(1,1,0,.5);
                break;
            case 1:
                glColor4f(1,0,1,.5);
                break;
        }

        //Draw the square
            glBegin(GL_POLYGON);
                glVertex2i(x, y);
                glVertex2i(x + edgeLength, y);
                glVertex2i(x + edgeLength, y + edgeLength);
                glVertex2i(x, y + edgeLength);
            glEnd();

        //Draw the outline of the square
            glColor4f(0,0,0,0.7);
            glBegin(GL_LINE_LOOP);
                glVertex2i(x, y);
                glVertex2i(x + edgeLength, y);
                glVertex2i(x + edgeLength, y + edgeLength);
                glVertex2i(x, y + edgeLength);
            glEnd();

            platformCount++;

        RemoveSquareFromSquareArray(x, y , edgeLength/10, polygonIndex);
    }
}



//Checks if a big square's edges are occupied or not valid for drawing the neighbour square
bool CanDrawBigSquare(int x, int y, int edgeLength, int polygonIndex){
    edgeLength = edgeLength * 10;

    //Check Top Edge
    for(int i = x; i < x + edgeLength; i+=10){
        if(SquareIndexInArray(i, y-10,polygonIndex) != -1){
            for(int j = y-20; j > y - (edgeLength/2) -10; j-=10){
                if(SquareIndexInArray(i,j,polygonIndex) == -1){
                    return false;
                }
            }
        }
    }

    if(edgeLength > 20 ){
        if(!CanDrawBigSquare(x,y-edgeLength/2,edgeLength/20,polygonIndex) || !CanDrawBigSquare(x+edgeLength/2,y-edgeLength/2,edgeLength/20,polygonIndex)){
            return false;
        }
    }




    //Check Bottom Edge
    for(int i = x; i < x + edgeLength; i+=10){
        if(SquareIndexInArray(i, y + edgeLength,polygonIndex) != -1){
            for(int j = y+edgeLength; j < y +(edgeLength*3/2); j+=10){
                if(SquareIndexInArray(i,j,polygonIndex) == -1){
                    return false;
                }
            }
        }
    }

    if(edgeLength > 20 ){
        if(!CanDrawBigSquare(x,y+edgeLength,edgeLength/20,polygonIndex) || !CanDrawBigSquare(x+edgeLength/2,y+edgeLength,edgeLength/20,polygonIndex)){
            return false;
        }
    }



    //Check Right Edge

    for(int i = y; i < y + edgeLength; i+=10){
        if(SquareIndexInArray(x+edgeLength, i,polygonIndex) != -1){
            for(int j = x + edgeLength + 10; j < x + (edgeLength * 3/2); j+=10){
                if(SquareIndexInArray(j,i,polygonIndex) == -1){
                    return false;
                }
            }
        }
    }

    if(edgeLength > 20 ){
        if(!CanDrawBigSquare(x+edgeLength,y,edgeLength/20,polygonIndex) || !CanDrawBigSquare(x+edgeLength,y+edgeLength/2,edgeLength/20,polygonIndex)){
            return false;
        }
    }


    //Check Left Edge
    for(int i = y; i < y + edgeLength; i+=10){
        if(SquareIndexInArray(x-10, i,polygonIndex) != -1){
            for(int j = x-20; j > x - (edgeLength/2) - 10 ; j-=10){
                if(SquareIndexInArray(j,i,polygonIndex) == -1){
                    return false;
                }
            }
        }
    }

    if(edgeLength > 20 ){

        if(!CanDrawBigSquare(x-edgeLength/2,y,edgeLength/20,polygonIndex) || !CanDrawBigSquare(x-edgeLength/2,y+edgeLength/2,edgeLength/20,polygonIndex)){

            return false;
        }
    }


    return true;
}



//Draws the neighbour squares of given big square
void DrawNeighbourSquares(int x, int y, int edgeLength, int polygonIndex){
    int newEdgeLength = edgeLength/2;
    int squaresPerEdge = edgeLength/10;
    struct Coordinate Top, Bottom, Right, Left;

    Top.y = y-newEdgeLength;
    Bottom.y = y+edgeLength;

    Top.x = Bottom.x = x;

    Right.x = x+edgeLength;
    Left.x = x-newEdgeLength;

    Right.y = Left.y = y;

    //Draw the neighbour squares of given square
    if(edgeLength > 20 ){
        //Top edge squares
        if(CanDrawSquare(Top.x, Top.y - newEdgeLength, squaresPerEdge, polygonIndex) && CanDrawBigSquare(Top.x, Top.y - newEdgeLength, squaresPerEdge, polygonIndex)){
            DrawBigSquare(Top.x, Top.y - newEdgeLength, edgeLength, polygonIndex, true);
            DrawNeighbourSquares(Top.x, Top.y - newEdgeLength, edgeLength, polygonIndex);
        }else{
            DrawBigSquare(Top.x, Top.y, newEdgeLength, polygonIndex, true);
            DrawBigSquare(Top.x + newEdgeLength, Top.y, newEdgeLength, polygonIndex,true);

                //Draw Neighbour
            DrawNeighbourSquares(Top.x, Top.y, newEdgeLength,polygonIndex);
            DrawNeighbourSquares(Top.x + newEdgeLength, Top.y, newEdgeLength,polygonIndex);
        }

        //Bottom edge squares
        if(CanDrawSquare(Bottom.x, Bottom.y, squaresPerEdge, polygonIndex) && CanDrawBigSquare(Bottom.x, Bottom.y, squaresPerEdge, polygonIndex)){
            DrawBigSquare(Bottom.x, Bottom.y, edgeLength, polygonIndex, true);
            glPointSize(5.0);

            DrawNeighbourSquares(Bottom.x, Bottom.y, edgeLength, polygonIndex);
        }else{
            DrawBigSquare(Bottom.x, Bottom.y,newEdgeLength, polygonIndex,true);
            DrawBigSquare(Bottom.x + newEdgeLength, Bottom.y,newEdgeLength, polygonIndex,true);

                //Draw Neighbour
            DrawNeighbourSquares(Bottom.x, Bottom.y,newEdgeLength, polygonIndex);
            DrawNeighbourSquares(Bottom.x + newEdgeLength, Bottom.y,newEdgeLength, polygonIndex);
        }

        //Right edge squares
        if(CanDrawSquare(Right.x, Right.y, squaresPerEdge, polygonIndex) && CanDrawBigSquare(Right.x, Right.y, squaresPerEdge, polygonIndex)){
            DrawBigSquare(Right.x, Right.y, edgeLength, polygonIndex, true);
            DrawNeighbourSquares(Right.x, Right.y, edgeLength, polygonIndex);
        }else{
            DrawBigSquare(Right.x, Right.y,newEdgeLength, polygonIndex,true);
            DrawBigSquare(Right.x, Right.y + newEdgeLength, newEdgeLength, polygonIndex,true);

                //Draw Neighbour
            DrawNeighbourSquares(Right.x, Right.y, newEdgeLength, polygonIndex);
            DrawNeighbourSquares(Right.x, Right.y + newEdgeLength, newEdgeLength, polygonIndex);
        }

        //Left edge squares
        if(CanDrawSquare(Left.x - newEdgeLength, Left.y, squaresPerEdge, polygonIndex) && CanDrawBigSquare(Left.x - newEdgeLength, Left.y, squaresPerEdge, polygonIndex)){
            DrawBigSquare(Left.x - newEdgeLength, Left.y, edgeLength, polygonIndex, true);
            DrawNeighbourSquares(Left.x - newEdgeLength, Left.y, edgeLength, polygonIndex);
        }else{
            DrawBigSquare(Left.x, Left.y, newEdgeLength, polygonIndex,true);
            DrawBigSquare(Left.x, Left.y + newEdgeLength, newEdgeLength, polygonIndex,true);

                //Draw Neighbour
            DrawNeighbourSquares(Left.x, Left.y, newEdgeLength, polygonIndex);
            DrawNeighbourSquares(Left.x, Left.y + newEdgeLength, newEdgeLength, polygonIndex);
        }
    }

}



//Checks every big square and finds how to draw least amount of square to fully cover the area
void CheckBigSquares(int squaresPerEdge){
    int edgeLength = squaresPerEdge * 10;

    struct Coordinate TempSquaresInsidePolygon[3][500];
    struct Coordinate DrawableSquares[400];
    int squareCount;



    for(int i = 0; i < polygonCount; i++){
        squareCount = 0;


        //Find Drawable Big Squares
        for(int j = 0; j < squareCounts[i]; j++){
            int currentX = SquaresInsidePolygon[i][j].x;
            int currentY = SquaresInsidePolygon[i][j].y;

            if(currentX == -1 || currentY == -1){continue;}


            if(CanDrawSquare(currentX, currentY, squaresPerEdge, i) && CanDrawBigSquare(currentX, currentY, squaresPerEdge, i)){

                if(squaresPerEdge == 1){
                    DrawBigSquare(currentX, currentY, edgeLength,i,false);
                    continue;
                }
                DrawableSquares[squareCount].x = currentX;
                DrawableSquares[squareCount].y = currentY;
                squareCount++;
            }
        }



        int maxSquareCount=0;
        int maxSquareCountIndex=0;
        int squareCounter;


        //Sets the TempSquaresInsidePolygon
        for(int j = 0; j < squareCounts[i]; j++){
            TempSquaresInsidePolygon[i][j].x = SquaresInsidePolygon[i][j].x;
            TempSquaresInsidePolygon[i][j].y = SquaresInsidePolygon[i][j].y;
        }


        //Checks every big square index to find most amount of big square that can be drawn at the same time
        for(int j = 0; j < squareCount; j++){
            squareCounter = 0;

            //Checks squares after index
            for(int k = j; k < squareCount; k++){
                int currentX = DrawableSquares[k].x;
                int currentY = DrawableSquares[k].y;

                if(CanDrawSquare(currentX, currentY, squaresPerEdge, i) ){
                    squareCounter++;
                    RemoveSquareFromSquareArray(DrawableSquares[k].x, DrawableSquares[k].y, squaresPerEdge,i);
                }
            }

            //Checks squares before index
            for(int k = j; k >= 0; k--){
                int currentX = DrawableSquares[k].x;
                int currentY = DrawableSquares[k].y;

                if(CanDrawSquare(currentX, currentY, squaresPerEdge, i) ){
                    squareCounter++;
                    RemoveSquareFromSquareArray(DrawableSquares[k].x, DrawableSquares[k].y, squaresPerEdge,i);
                }
            }

            //Checks if counter is the new max count
            if(squareCounter > maxSquareCount){
                maxSquareCount = squareCounter;
                maxSquareCountIndex = j;
            }


            //Resets the SquaresInsidePolygon array
            for(int k = 0; k < squareCounts[i]; k++){
                SquaresInsidePolygon[i][k].x = TempSquaresInsidePolygon[i][k].x;
                SquaresInsidePolygon[i][k].y = TempSquaresInsidePolygon[i][k].y;
            }
        }

        struct Coordinate DrawnSquares[200];
        int drawnSquareCount = 0;


        //Draws the first square in maxSquareCountIndex to make sure first one is drawn
        DrawBigSquare(DrawableSquares[maxSquareCountIndex].x, DrawableSquares[maxSquareCountIndex].y, edgeLength, i, false);
        DrawnSquares[drawnSquareCount].x = DrawableSquares[maxSquareCountIndex].x;
        DrawnSquares[drawnSquareCount].y = DrawableSquares[maxSquareCountIndex].y;
        drawnSquareCount++;



        //Draws squares after maxSquareCountIndex
        for(int j = maxSquareCountIndex; j < squareCount; j++){
            int currentX = DrawableSquares[j].x;
            int currentY = DrawableSquares[j].y;

            if(CanDrawSquare(currentX, currentY, squaresPerEdge, i) && CanDrawBigSquare(currentX, currentY, squaresPerEdge, i)){
                DrawBigSquare(currentX, currentY, edgeLength, i, false);
                DrawnSquares[drawnSquareCount].x = currentX;
                DrawnSquares[drawnSquareCount].y = currentY;
                drawnSquareCount++;
            }

        }


        //Draws squares before maxSquareCountIndex
        for(int j = maxSquareCountIndex; j >= 0; j--){
            int currentX = DrawableSquares[j].x;
            int currentY = DrawableSquares[j].y;

            if(CanDrawSquare(currentX, currentY, squaresPerEdge, i) && CanDrawBigSquare(currentX, currentY, squaresPerEdge, i)){
                DrawBigSquare(currentX, currentY, edgeLength, i, false);
                DrawnSquares[drawnSquareCount].x = currentX;
                DrawnSquares[drawnSquareCount].y = currentY;
                drawnSquareCount++;
            }

        }



        //Draws the neighbour squares
        for(int j = 0; j < squareCount; j++){
            int currentX = DrawnSquares[j].x;
            int currentY = DrawnSquares[j].y;
            DrawNeighbourSquares(currentX, currentY, edgeLength, i);
        }
    }

}



//Checks if a point is inside polygon
bool IsPointInsidePolygonRayTrace(GLint xPoint, GLint yPoint, int polygonIndex){
    float x1Line,x2Line,y1Line,y2Line, x0;
    int count = 0;
    bool IsPointOnEdge = false;


    //Checks how many edges crossed with the line that goes straight towards right from initial point
    for(int i = 0; i < polygonVertexCounts[polygonIndex]; i++){

        //Sets coordinates of the first vertex of the edge line
        x1Line = polygons[polygonIndex][i].x;
        y1Line = polygons[polygonIndex][i].y;

        //If the point is on line's first vertex, count as 1
        if(xPoint == x1Line && yPoint == y1Line){
            count = 1;
            break;
        }else{
            //If for-loop is on last vertex of polygon set next vertex as first vertex
            if((i == polygonVertexCounts[polygonIndex] - 1)){
                x2Line = polygons[polygonIndex][0].x;
                y2Line = polygons[polygonIndex][0].y;
            }else{
            //If not, set second vertex of the line as next vertex of polygon
                x2Line = polygons[polygonIndex][i+1].x;
                y2Line = polygons[polygonIndex][i+1].y;
            }


            //x0 is the distance between x of the first vertex of the line and the crossing point of the
            //line that goes straight towards right from the initial point and the edge line
            x0 = x1Line + ( (y1Line - yPoint) / (y1Line - y2Line) ) * (x2Line-x1Line);


            //Checks if initial point is between to lines' y values and xPoint <= 0
            if( ( (yPoint < y1Line) != (yPoint < y2Line) ) && xPoint <= x0 ){
                if(xPoint == x0){IsPointOnEdge = true;}
                count++;
            }
        }
    }

    //If the initial point is on a edge don't count the point as inside
    if(IsPointOnEdge && count == 1){
        return false;
    }

    return(count%2);
}



//Checks every corner of the unit square if any corner is inside polygon square counts as inside polygon
bool IsSquareInsidePolygon(GLint topLeftX, GLint topLeftY, int polygonIndex){

    if(IsPointInsidePolygonRayTrace(topLeftX, topLeftY, polygonIndex)){return true;}
    if(IsPointInsidePolygonRayTrace(topLeftX + 10, topLeftY, polygonIndex)){return true;}
    if(IsPointInsidePolygonRayTrace(topLeftX + 10, topLeftY + 10, polygonIndex)){return true;}
    if(IsPointInsidePolygonRayTrace(topLeftX, topLeftY + 10, polygonIndex)){return true;}

    return false;

}


//Finds the min and max values of x and y in vertices of a polygon
struct Coordinate *FindEdgeVertices(int polygonIndex){
    //Find the min and max x and y values
    int minX = 0, maxiX = 0, minY = 0, maxiY = 0;

    static struct Coordinate edgeVertices[4];

    //Set max, min x y values to polygons first vertex values
    maxiX = minX = polygons[polygonIndex][0].x ;
    maxiY = minY = polygons[polygonIndex][0].y ;

    //Find and set edge values
    for(int i = 0; i < polygonVertexCounts[polygonIndex]; i++)
    {
        if(polygons[polygonIndex][i].x >= maxiX)
        {
            maxiX = polygons[polygonIndex][i].x;
        }
        else if(polygons[polygonIndex][i].x <= minX)
        {
            minX = polygons[polygonIndex][i].x;
        }

        if(polygons[polygonIndex][i].y >= maxiY)
        {
            maxiY = polygons[polygonIndex][i].y;
        }
        else if(polygons[polygonIndex][i].y <= minY)
        {
            minY = polygons[polygonIndex][i].y;
        }
    }

    //Set edge vertices coordinates. {(minX,minY),(maxiX,minY),(maxiX,maxiY),(minX,maxiY)}
    edgeVertices[0].x = edgeVertices[3].x = minX;
    edgeVertices[1].x = edgeVertices[2].x = maxiX;
    edgeVertices[0].y = edgeVertices[1].y = minY;
    edgeVertices[2].y = edgeVertices[3].y = maxiY;

    if(maxiY > window_HEIGHT/2){
        window_HEIGHT = maxiY + 200;
    }

    return edgeVertices;
}


//Checks every vertex in given array and splits the vertices in to polygons
void splitPolygonVertices(struct Coordinate vertices[], int vertexCount){
    //Create polygons array
    struct Coordinate startVertex = vertices[0];


    int polygonStartIndex = 0, polygonEndIndex = 0;

    //Split polygon vertices and store to polygons array
    for(int i = 1; i < vertexCount; i++){
        //Detect if the vertex is equal to the starting vertex of the polygon
        if(vertices[i].x == startVertex.x && vertices[i].y == startVertex.y){
            polygonEndIndex = i;
            i++;

            //Store the vertices of the polygon to polygon array
            for(int j = polygonStartIndex; j < polygonEndIndex; j++){
                polygons[polygonCount][j - polygonStartIndex] = vertices[j];
            }

            //Set polygon count and store vertex count of all polygons to array
            polygonVertexCounts[polygonCount] = polygonEndIndex - polygonStartIndex;
            polygonCount++;

            //Set new start vertex
            polygonStartIndex = i;
            startVertex = vertices[i];
        }
    }


}


//Gets a string and finds the vertex coordinate values and stores them in an array and calls splitPolygonVertices
void setCoordinates(char *buffer){
    int searchStartIndex = 0;

    //Find the starting index of the coordinates
    while(true){
        if(buffer[searchStartIndex] == 'B'){break;}
        searchStartIndex++;
    }

    char coordinateString[20] = "";
    int coordinateInt = 0;

    bool IsAssigningCoordinate_x = true;

    int numberStartIndex = -1,numberEndIndex = -1;
    int currentVertexIndex = 0;

    //Define VertexStart and Vertices array
    struct Coordinate vertices[30];
    int vertexCount = 0;

    //Find and assign Coordinates to array
    for(int i = searchStartIndex+1; i < strlen(buffer); i++){

        if(isdigit((int)buffer[i])){
            //Set Start Index of Number
            if(numberStartIndex == -1){
                numberStartIndex = i;
           }else{
            //If Start Index is already set skip to the next index
            continue;
           }
        }else if(numberStartIndex != -1){
            //Set End Index of Number when found a non-digit character
            numberEndIndex = i;

            //Convert the substring(start index to end index) of the number to integer
            strncpy(coordinateString, buffer + numberStartIndex, numberEndIndex-numberStartIndex);
            coordinateInt = atoi(coordinateString);

            //Assign x coordinate
            if(IsAssigningCoordinate_x){
                vertices[currentVertexIndex].x = coordinateInt *10;
                IsAssigningCoordinate_x = false;
            }
            //Assign y coordinate
            else{
                vertices[currentVertexIndex].y = coordinateInt*10;
                currentVertexIndex++;
                vertexCount++;
                IsAssigningCoordinate_x = true;
            }

            //Reset "coordinateString" string
            memset(coordinateString, '\0', sizeof(coordinateString));
            coordinateInt = 0;

            //Reset Start and End Index
            numberStartIndex = -1;
            numberEndIndex = -1;
        }

    }

    //Call the function to split the vertices to polygons
    splitPolygonVertices(vertices, vertexCount);

}


//Determines and draws shapes in first window
void renderScene(void){
    //Set Background color
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT);

    //Set origin point to top left corner
    //{
    glViewport(0,0, window_WIDTH, window_HEIGHT);
    glLoadIdentity();
    glOrtho(0,window_WIDTH, window_HEIGHT, 0,-1,1);
    //}

    //Draw Square grid
    glColor4f(.3,.3,.3,.5);
    for(int i = 0; i<window_WIDTH; i+=10){
        for(int j = 0; j < window_HEIGHT; j+=10){
            glBegin(GL_LINE_LOOP);
                glVertex2i(i,j);
                glVertex2i(i+10,j);
                glVertex2i(i+10,j+10);
                glVertex2i(i,j+10);
            glEnd();
        }
    }

    //Draw Polygons
    for(int i = 0; i < polygonCount; i++){

        glColor4f(1.0,0.0,0.0,0.5);
        glBegin(GL_POLYGON);
        for(int j = 0; j < polygonVertexCounts[i]; j++){
            glVertex2i(polygons[i][j].x, polygons[i][j].y);
        }
        glEnd();
    }


    squareCounts[0] = squareCounts[1] = squareCounts[2] = 0;

    glColor4f(.3,.3,.3,.5);
    for(int i = 0; i < polygonCount; i++){
        struct Coordinate *edgeVertices;
        edgeVertices = FindEdgeVertices(i);

        int area = 0;
        int squareCount = 1;
        //Calculate and draw the squares that are inside the polygon
        for(int y = edgeVertices[0].y; y < edgeVertices[2].y; y+=10){
            for(int x = edgeVertices[0].x; x < edgeVertices[1].x; x+=10){
                squareCount++;
                if(IsSquareInsidePolygon(x,y,i)){
                    glBegin(GL_POLYGON);
                        glVertex2i(x, y);
                        glVertex2i(x+10, y);
                        glVertex2i(x+10, y+10);
                        glVertex2i(x, y+10);
                    glEnd();
                    SquaresInsidePolygon[i][squareCounts[i]].x = Default_SquaresInsidePolygon[i][squareCounts[i]].x = x;
                    SquaresInsidePolygon[i][squareCounts[i]].y = Default_SquaresInsidePolygon[i][squareCounts[i]].y = y;
                    squareCounts[i]++;
                    area++;
                }
            }
        }
        if(!IsVariablesPrinted){printf("\n%d. Polygon Area: %d br^2\n", i+1, area);}

        totalArea += area;
        drillCount = totalArea;
        area = 0;
    }


    glutSwapBuffers();
}



//Determines and draws shapes in second window
void renderSecond(void){
      //Set Background color
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT);

    //Set origin point to top left corner
    //{
    glViewport(0,0, window_WIDTH, window_HEIGHT);
    glLoadIdentity();
    glOrtho(0,window_WIDTH, window_HEIGHT, 0,-1,1);
    //}

    //Draw Square grid
    glColor4f(.3,.3,.3,.5);
    for(int i = 0; i<window_WIDTH; i+=10){
        for(int j = 0; j < window_HEIGHT; j+=10){
            glBegin(GL_LINE_LOOP);
                glVertex2i(i,j);
                glVertex2i(i+10,j);
                glVertex2i(i+10,j+10);
                glVertex2i(i,j+10);
            glEnd();
        }
    }

    //Reset SquaresInsidePolygon
    for(int i = 0; i < polygonCount; i++){
        for(int j = 0; j < squareCounts[i]; j++){
            SquaresInsidePolygon[i][j].x = Default_SquaresInsidePolygon[i][j].x;
            SquaresInsidePolygon[i][j].y = Default_SquaresInsidePolygon[i][j].y;
        }
    }

    //Calls the function that splits polygons into big squares
    CheckBigSquares(16);
    CheckBigSquares(8);
    CheckBigSquares(4);
    CheckBigSquares(2);
    CheckBigSquares(1);


    //Prints the Results
    if(!IsVariablesPrinted){
        int profit, totalDrillCost, totalPlatformCost, sourceValue, totalCost;

        sourceValue = totalArea *10;
        printf("\nRezerv Degeri: %d\n", totalArea*10);

        totalPlatformCost = platformCost * platformCount;
        printf("\nToplam platform sayisi: %d\n", platformCount);
        printf("Toplam platfom maliyeti: %d\n", totalPlatformCost);

        totalDrillCost = drillCost * drillCount;
        printf("\nToplam sondaj sayisi: %d\n", drillCount);
        printf("Toplam sondaj maliyeti: %d\n", totalDrillCost);

        totalCost = totalDrillCost + totalPlatformCost;
        printf("\nToplam Maliyet: %d\n", totalCost);

        profit = sourceValue - totalCost;
        printf("\nKar Miktari: %d\n", profit);
        IsVariablesPrinted = true;
    }


    glutSwapBuffers();
}



//Writes data from contents to output
size_t curl_write(void *contents, size_t size, size_t nmemb, char **output) {
    size_t total_size = size * nmemb;
    *output = (char *)malloc(total_size + 1);
    if (*output) {
        strncpy(*output, (char *)contents, total_size);
        (*output)[total_size] = '\0';
    }
    return total_size;
}


//Reads and returns a specific line from the given URL
char * ReadLineFromURL(){
    CURL *curl = curl_easy_init();

    char url[] = "https://bilgisayar.kocaeli.edu.tr/prolab1/prolab1.txt";

    curl_easy_setopt(curl, CURLOPT_URL, url);

    char *FullText = NULL;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &FullText);

    curl_easy_perform(curl);

    int numberRead = 0, line_To_Read = 0;

    //Get Input for line to read
    printf("Cizilecek satir numarasini giriniz: ");
    numberRead = scanf("%d", &line_To_Read);

    //If input is not a number clear scanf and give an error.
    while(numberRead != 1){
        printf("Gecersiz!!\n");
        scanf("%*[^\n]");
        printf("Cizilecek satir numarasini giriniz: ");
        numberRead = scanf("%d", &line_To_Read);
    }

    int lineCounter = 0, lineStartIndex = 0, lineEndIndex = 0;
    char *lineText;

    lineText = (char *) malloc(500);

    //Find the line to read and return it
    for(int i = 0; i < strlen(FullText); i++){
        if(FullText[i] == 'F'){
            lineCounter++;
            lineEndIndex = i+1;

            if(lineCounter == line_To_Read){
                strncpy(lineText, FullText + lineStartIndex, lineEndIndex - lineStartIndex);
                break;
            }else{
                lineStartIndex = i+2;
            }
        }
    }
    return lineText;
}

int main(int argc, char **argv)
{

    //Get Cost Inputs
    printf("Birim sondaj maliyetini giriniz: ");
    scanf("%d", &drillCost);

    printf("Birim platform maliyetini giriniz: ");
    scanf("%d", &platformCost);


    //Read URL and set coordinates of the polygon
    setCoordinates(ReadLineFromURL());


    //CREATE WINDOWS AND DISPLAY THE SCENES
    //{

    //Create window
    glutInit(&argc, argv);

    //Create First Window
    glutInitDisplayMode(GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(1500,window_HEIGHT);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Window1");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    glutDisplayFunc(renderScene);



    //Create Second Window
    glutInitDisplayMode(GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(1500,window_HEIGHT);
    glutInitWindowPosition(300,50);
    glutCreateWindow("Window2");

    //Set blend function to control transparency via alpha channel
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    //Display the scene
    glutDisplayFunc(renderSecond);



    glutMainLoop();
    //}


    return 0;
}

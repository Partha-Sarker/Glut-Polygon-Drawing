#include<bits/stdc++.h>
#include<GL/glut.h>
#include<math.h>

#define MOUSE_BUTTON_DOWN 0
#define MOUSE_BUTTON_UP 1
#define NOTHING 0
#define POINT 1
#define LINE 2
#define TRIANGLE 3
#define RECTANGLE 4
#define POLYGON 5
#define TRANSLATION 6
#define ROTATION 7
#define SCALE 8

using namespace std;

bool canChangePointSize = true, canClear = true, canTransform = true, isTransforming = false, hasPolygonFinished = false;
int mouseButton, downX, downY, pointCount = 0, mode = POINT, pointSize = 3, transformationMode;

struct vertice{
    int x, y, initialX, initialY, verticeSize = 1;
}pivot, mouse;

vector<vertice>points;

void resetEverything()
{
    points.clear();
    canChangePointSize = true, canClear = true, canTransform = true, isTransforming = false, hasPolygonFinished = false;
    mouseButton, downX, downY, pointCount = 0, mode = POINT, pointSize = 3, transformationMode;
    glutPostRedisplay();
}

void myInit (void)
{
    pivot.verticeSize = 1;
    mouse.verticeSize = 1;
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(pointSize);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1280, 0, 720);
    glMatrixMode(GL_MODELVIEW);
}

void clearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void drawPoint(vertice point)
{
    glPointSize(point.verticeSize);
    glBegin(GL_POINTS);
        glVertex2i(point.x, point.y);
    glEnd();
}

void drawLine(vertice point1, vertice point2)
{
    vertice v;
    v.verticeSize = (point1.verticeSize + point2.verticeSize)/2;
    int x1 = point1.x, y1 = point1.y, x2 = point2.x, y2 = point2.y;
    float dx = x2-x1;
    float dy = y2-y1;
    float step = abs(dx);
    if(abs(dy) > abs(dx))
        step = abs(dy);
    float xInc = dx / step;
    float yInc = dy / step;
    float temX = x1, temY = y1;
    for(int i=1; i<=step; i++){
        v.x = round(temX), v.y = round(temY);
        drawPoint(v);
        temX += xInc;
        temY += yInc;
    }
}

void drawTriangle(vertice point1, vertice point2, vertice point3)
{
    drawLine(point1, point2);
    drawLine(point2, point3);
    drawLine(point3, point1);
}

void drawRectangle(vertice point1, vertice point2, vertice point3, vertice point4)
{
    drawLine(point1, point2);
    drawLine(point2, point3);
    drawLine(point3, point4);
    drawLine(point4, point1);
}

void drawPolygon()
{
    int length = points.size();
    for (int i = 1; i<length; i++)
        drawLine(points[i-1], points[i]);
    if(hasPolygonFinished)
        drawLine(points[length-1], points[0]);
}

double radianAngleBetweenTwoVertice(double x1, double y1, double x2, double y2)
{
    return atan2(y2 - y1, x2 - x1);
}

void translatePoints(int mouseX, int mouseY)
{
    int length = points.size();
    int offsetX = pivot.x - mouseX, offsetY = pivot.y - mouseY;
    for(int i=0; i<length; i++)
        points[i].x = points[i].initialX - offsetX, points[i].y = points[i].initialY - offsetY;

}

void rotatePoints(int mouseX, int mouseY)
{
    int length = points.size();
    double angle = radianAngleBetweenTwoVertice(pivot.x, pivot.y, mouseX, mouseY);
    for(int i=0; i<length; i++){
        int currentX = points[i].initialX, currentY = points[i].initialY;
        currentX -= pivot.x, currentY -= pivot.y;
        int tempX = currentX, tempY = currentY;

        currentX = tempX*cos(angle) - tempY*sin(angle);
        currentY = tempX*sin(angle) + tempY*cos(angle);

        currentX += pivot.x, currentY += pivot.y;

        points[i].x = currentX, points[i].y = currentY;
    }
}

void scalePoints(int mouseX, int mouseY)
{
    int length = points.size();

    float xScale = (float)(mouseX - pivot.x)/50, yScale = (float)(mouseY - pivot.y)/50;
    for(int i=0; i<length; i++){
        float currentX = points[i].initialX, currentY = points[i].initialY;
        currentX -= pivot.x, currentY -= pivot.y;

        currentX *= xScale, currentY *= yScale;

        currentX += pivot.x, currentY += pivot.y;
        points[i].x = currentX, points[i].y = currentY;
    }
}

void calculatePivot()
{
    int length = points.size();
    int sumX = 0, sumY = 0;
    for (int i = 0; i<length; i++){
        points[i].initialX = points[i].x, points[i].initialY = points[i].y;
        sumX += points[i].x, sumY += points[i].y;
    }
    pivot.x = sumX/length, pivot.y = sumY/length;

}

void mouseListener(int button, int state, int x, int y)
{
    y = 720 - y;

    if(state == MOUSE_BUTTON_DOWN){
        downX = x, downY = y;
    }

    else if(state == MOUSE_BUTTON_UP){

        if(isTransforming)
            return;

        if(downX == x && downY == y){

            if(mode == POLYGON){
                if(hasPolygonFinished){
                    hasPolygonFinished = false;
                    pointCount = 0;
                    points.clear();
                }
                if(!hasPolygonFinished && pointCount > 0){
                    int initialX = points[0].x, initialY = points[0].y;
                    if(abs(x - initialX) <= 8 && abs(y - initialY) <= 8){
                        hasPolygonFinished = true;
                        calculatePivot();
                        return;
                    }
                }
            }

            if(mode == POINT){
                points.clear();
                pointCount = 0;
            }

            if((mode == LINE || mode == TRIANGLE || mode == RECTANGLE) && pointCount >= 2){
                points.clear();
                pointCount = 0;
            }

            vertice v;
            v.x = x;
            v.y = y;
            v.verticeSize = pointSize;

            pointCount++;
            if(mode == POINT || mode == LINE || mode == POLYGON || pointCount == 1)
                points.push_back(v);

            if(mode == TRIANGLE && pointCount == 2){
                vertice v2, v3;
                v2.x = x, v2.y = points[0].y, v2.verticeSize = pointSize;
                v3.x = (points[0].x + x)/2, v3.y = y, v3.verticeSize = pointSize;

                points.push_back(v2);
                points.push_back(v3);
                pointCount++;
                calculatePivot();
                return;
            }

            if(mode == RECTANGLE && pointCount == 2){
                vertice v1 = points[0], v2, v3 = v, v4;
                v2.x = v1.x, v2.y = y, v2.verticeSize = pointSize;
                v4.x = x, v4.y = v1.y, v4.verticeSize = pointSize;
                points.push_back(v2);
                points.push_back(v3);
                points.push_back(v4);
                pointCount += 2;
                calculatePivot();
                return;
            }

            if(mode == POINT)
                calculatePivot();
            if(mode == LINE && pointCount == 2)
                calculatePivot();
        }
    }
}

void mouseMotionListener(int x, int y)
{
    if(!isTransforming)
        return;
    y = 720 - y;
    if(transformationMode == TRANSLATION)
        translatePoints(x, y);
    if(transformationMode == ROTATION)
        rotatePoints(x, y);
    if(transformationMode == SCALE)
        scalePoints(x, y);
    mouse.x = x, mouse.y = y;
    glutPostRedisplay();
}

void keyboardListener(unsigned char key, int X, int Y)
{
    if(key == 'x'){
        resetEverything();
        mode = POINT;
    }

    if(key == 'c' && canClear)
        resetEverything();

    if(key == '+' && canChangePointSize){
        pointSize++;
        glPointSize(pointSize);
    }

    if(key == '-' && canChangePointSize){
        if(pointSize > 1){
            pointSize--;
            glPointSize(pointSize);
        }
    }

    if(key == '1'){
        resetEverything();
        mode = POINT;
    }

    if(key == '2'){
        resetEverything();
        mode = LINE;
    }

    if(key == '3'){
        resetEverything();
        mode = TRIANGLE;
    }

    if(key == '4'){
        resetEverything();
        mode = RECTANGLE;
    }

    if(key == 'n'){
        resetEverything();
        mode = POLYGON;
    }

    if(key == 't' && canTransform){
        calculatePivot();
        isTransforming = true;
        transformationMode = TRANSLATION;
    }

    if(key == 'r' && canTransform){
        calculatePivot();
        isTransforming = true;
        transformationMode = ROTATION;
    }

    if(key == 's' && canTransform){
        calculatePivot();
        isTransforming = true;
        transformationMode = SCALE;
    }
}

void draw()
{
    int length = points.size();
    if(length == 0)
        return;

    if(mode == POINT)
        drawPoint(points[0]);

    if(mode == LINE ){
        drawPoint(points[0]);
        if(length == 2)
            drawLine(points[0], points[1]);
    }

    if(mode == TRIANGLE){
        drawPoint(points[0]);
        if(length == 3)
            drawTriangle(points[0], points[1], points[2]);
    }

    if(mode == RECTANGLE){
        drawPoint(points[0]);
        if(length == 4)
            drawRectangle(points[0], points[1], points[2], points[3]);
    }

    if(mode == POLYGON){
        drawPoint(points[0]);
        drawPolygon();
    }

    if(isTransforming){
        drawLine(pivot, mouse);
    }
}

void myDisplay(void)
{
    clearScreen();
    draw();
    glutSwapBuffers();
}

void showInstructions()
{
    cout<<"Instructions--------------------------------------------------------------------"<<endl<<endl;
    cout<<"Select a shape drawing mode and then click on the canvas to draw"<<endl;
    cout<<"1. Press 1 to select point drawing mode."<<endl;
    cout<<"2. Press 2 to select line drawing mode."<<endl;
    cout<<"3. Press 3 to select triangle drawing mode."<<endl;
    cout<<"4. Press 4 to select rectangle drawing mode."<<endl;
    cout<<"5. Press n to select polygon drawing mode."<<endl;
    cout<<"   (For drawing polygon, click near the first drawn point to complete the polygon.)"<<endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;
    cout<<"After drawing a shape, select a transformation mode to transform the shape"<<endl;
    cout<<"1. Press t to translate the shape."<<endl;
    cout<<"2. Press r to rotate the shape."<<endl;
    cout<<"3. Press s to scale the shape."<<endl;
    cout<<"--------------------------------------------------------------------------------"<<endl;
    cout<<"Press enter to start drawing!"<<endl<<endl;
    while(cin.get() != '\n'){
        cout<<"Press enter to start drawing!"<<endl;
    }
}

int main (int argc, char** argv)
{
    showInstructions();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("Shape Tool");
    glutKeyboardFunc(keyboardListener);
    glutMouseFunc(mouseListener);
    glutPassiveMotionFunc(mouseMotionListener);
    myInit();
    clearScreen();
    glutDisplayFunc(myDisplay);
    glutMainLoop();
}


//void drawLine(vertice point1, vertice point2)
//{
//    glLineWidth((point1.verticeSize + point2.verticeSize)/2);
//    glBegin(GL_LINES);
//        glVertex2d(point1.x, point1.y);
//        glVertex2d(point2.x, point2.y);
//    glEnd();
//}

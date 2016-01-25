/*
CMPT 361 Assignment 1 - FruitTetris implementation Sample Skeleton Code

- This is ONLY a skeleton code showing:
How to use multiple buffers to store different objects
An efficient scheme to represent the grids and blocks

- Compile and Run:
Type make in terminal, then type ./FruitTetris

This code is extracted from Connor MacLeod's (crmacleo@sfu.ca) assignment submission
by Rui Ma (ruim@sfu.ca) on 2014-03-04. 

Modified in Sep 2014 by Honghua Li (honghual@sfu.ca).
*/

#include "include/Angel.h"

#include <cstdlib>
#include <iostream>

using namespace std;


// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 400; 
int ysize = 720;
int rot=0;  //remember how much it rotate
int shapeType=0;
// current tile
vec2 tile[4]; // An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilepos; // The position of the current tile using grid coordinates ((0,0) is the bottom left corner)
vec4 tileColors[4];
vec2 l = vec2(-1,0);
vec2 r = vec2(1,0);
vec2 d = vec2(0,-1);

bool terminal=false;
// An array storing all possible orientations of all possible tiles
// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
vec2 allRotationsLshape[4][4] = 
	{{vec2(-1,-1),vec2(-1,0), vec2(0, 0), vec2(1, 0)},
	{vec2(1, -1), vec2(0,-1), vec2(0, 0), vec2(0, 1)},     
	{vec2(1, 1),  vec2(1, 0), vec2(0, 0), vec2(-1,0)},  
	{vec2(-1,1), vec2(0, 1), vec2(0, 0), vec2(0, -1)}};//0

vec2 allRotationsIshape[4][4]={{vec2(-2,0),vec2(-1,0),vec2(0,0),vec2(1,0)},
								{vec2(0,-1),vec2(0,-2),vec2(0,0),vec2(0,1)}};//1

vec2 allRotationsSshape[4][4]={{vec2(-1,-1),vec2(0,-1),vec2(0,0),vec2(1,0)},
								{vec2(1,-1),vec2(1,0),vec2(0,0), vec2(0,1),}};//2

// colors
vec4 purple = vec4(1.0, 0.0, 1.0, 1.0);//0
vec4 red    = vec4(1.0, 0.0, 0.0, 1.0);//1
vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);//2
vec4 green  = vec4(0.0, 1.0, 0.0, 1.0);//3
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);//4
vec4 white  = vec4(1.0, 1.0, 1.0, 1.0);
vec4 black  = vec4(0.0, 0.0, 0.0, 1.0); 
 
//board[x][y] represents whether the cell (x,y) is occupied
bool board[10][20]; 

//An array containing the colour of each of the 10*20*2*3 vertices that make up the board
//Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
//will be set to the appropriate colour in this array before updating the corresponding VBO
vec4 boardcolours[1200];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;

// VAO and VBO
GLuint vaoIDs[3]; // One VAO for each object: the grid, the board, the current piece
GLuint vboIDs[6]; // Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updatetile()
{
	// Bind the VBO containing current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]); 

	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{
		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x; 
		GLfloat y = tilepos.y + tile[i].y;

		// Create the 4 corners of the square - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		// Two points are used by two triangles each
		vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4}; 

		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newpoints); 
	}

	glBindVertexArray(0);
}
bool checkWhetherMove(){
	for(int i=0;i<4;i++){
		int posX=tilepos.x+tile[i].x;
		if(board[posX][19]==true){
			return false;
		}
	}
	return true;
}
bool checkDown(){
	for(int i=0;i<4;i++){
		int posY=tile[i].y+tilepos.y;
		int posX=tile[i].x+tilepos.x;
		if(posY<= 0 || board[posX][posY-1]==true){
			return false;
		}
	}
	return true;
}
//-------------------------------------------------------------------------------------------------------------------

// Called at the start of play and every time a tile is placed
int newtile()
{	

	int shape=rand()%3;

	tilepos = vec2(rand()%5+3 , 19); // Put the tile at the top of the board  
	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++){
		if(shape==0){
			tile[i] = allRotationsLshape[0][i]; // Get the 4 pieces of the new tile
		}else if(shape==1){
			tile[i] = allRotationsIshape[0][i]; // Get the 4 pieces of the new tile
		}else{
			tile[i] = allRotationsSshape[0][i]; // Get the 4 pieces of the new tile

		}
	}
	if(checkDown()){
		updatetile(); 

		// Update the color VBO of current tile
		vec4 newcolours[24];
		vec4 color;
		for (int i = 0; i < 4; i++){
			int choice=rand()%5;
			if(choice==0){
				color=purple;
			}else if(choice==1){
				color=red;
			}else if(choice==2){
				color=yellow;
			}else if(choice==3){
				color=green;
			}else{
				color=orange;
			}
			newcolours[i*6] =color;
			newcolours[i*6+1] =color;
			newcolours[i*6+2] =color;
			newcolours[i*6+3] =color;
			newcolours[i*6+4] =color;
			newcolours[i*6+5] =color;
			tileColors[i]=color;

		}
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
	
	return shape;
}

//-------------------------------------------------------------------------------------------------------------------

void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[64]; // Array containing the 64 points of the 32 total lines to be later put in the VBO
	vec4 gridcolours[64]; // One colour per vertex
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] = vec4((33.0 + (33.0 * i)), 33.0, 0, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 0, 1);
		
	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[22 + 2*i] = vec4(33.0, (33.0 + (33.0 * i)), 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 0, 1);
	}
	// Make all grid lines white
	for (int i = 0; i < 64; i++){
		gridcolours[i] = white;
	}


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]); // Bind the first VAO
	glGenBuffers(2, vboIDs); // Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); // Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridpoints, GL_STATIC_DRAW); // Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPosition); // Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); // Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridcolours, GL_STATIC_DRAW); // Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor); // Enable the attribute
}


void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[1200];
	for (int i = 0; i < 1200; i++)
		boardcolours[i] = black; // Let the empty cells on the board be black
	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			
			// Two points are reused
			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
		}
	}



	// Initially no cell is occupied
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false; 


	// *** set up buffer objects
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

}

// No geometry for current tile initially
void initCurrentTile()
{
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);


}

void init()
{
	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(3, &vaoIDs[0]);

	// Initialize the grid, the board, and the current tile
	initGrid();
	initBoard();
	initCurrentTile();

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");

	// Game initialization
	shapeType=newtile(); // create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
	terminal=false;
}

//-------------------------------------------------------------------------------------------------------------------

// Rotates the current tile, if there is room
void rotate(){      
	rot++;
	vec2 tempTile[4];
	if(shapeType==0){
		for(int i=0;i<4;i++){
			tempTile[i]=allRotationsLshape[rot%4][i];
		}
	}else if(shapeType==1){
		for(int i=0;i<4;i++){
			tempTile[i]=allRotationsIshape[rot%2][i];
		}
	}else{
		for(int i=0;i<4;i++){
			tempTile[i]=allRotationsSshape[rot%2][i];
		}
	}
	for(int i=0;i<4;i++){
		int x=tempTile[i].x+tilepos.x;
		int y=tempTile[i].y+tilepos.y;
		if(x<0||y<0||x>9){
			return;
		}
	}

	for(int i=0;i<4;i++){
		tile[i]=tempTile[i];
	}

}

//-------------------------------------------------------------------------------------------------------------------

// Checks if the specified row (0 is the bottom 19 the top) is full
// If every cell in the row is occupied, it will clear that cell and everything above it will shift down one row
bool checkfullrow(int row)
{
	for (int j = 0; j < 10; j++){
		 if(board[j][row]==false){
		 	return false;
		 }
	}
	return true;

}
void checkWholeRow(){
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	for(int i=0;i<20;i++){
		if(checkfullrow(i)){
			for(int row=i;row<19;row++){
				for(int j=0;j<10;j++){
					boardcolours[6*(10*row+ j)] = boardcolours[6*(10*(row+1)+ j)];
					boardcolours[6*(10*row+ j)+1] = boardcolours[6*(10*(row+1)+ j)+1];
					boardcolours[6*(10*row+ j)+2] = boardcolours[6*(10*(row+1)+ j)+2];
					boardcolours[6*(10*row+ j)+3] = boardcolours[6*(10*(row+1)+ j)+3];
					boardcolours[6*(10*row+ j)+4] = boardcolours[6*(10*(row+1)+ j)+4];
					boardcolours[6*(10*row+ j)+5] = boardcolours[6*(10*(row+1)+ j)+5];
					board[j][row]=board[j][row+1];
				}
			}
			
			for(int c=0;c<10;c++){
				for(int j=0;j<6;j++){
					boardcolours[6*(10*19+ c)+j]=black;
				
					board[c][19]=false;
				}
			
			}
		}
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, 1200*sizeof(vec4), boardcolours);
	glBindVertexArray(0);
	
}
bool checkColor(vec4 colora, vec4 colorb){
	if(colora.x==colorb.x && colora.y==colorb.y && colora.z==colorb.z &&colora.w==colorb.w){
		
			return true;
		
	}
	return false;
}
int checkSameColorRow(int row){
	//checkColor(0);
	for(int i=0;i<10;i++){
		if(!checkColor(boardcolours[6*(10*row+ i)],black)){
			if(checkColor(boardcolours[6*(10*row+ i)],boardcolours[6*(10*row+ i+1)])){
				if(checkColor(boardcolours[6*(10*row+ i)],boardcolours[6*(10*row+ i+2)])){
					return i;
				}
				
			}
		}
	}
	return -1;
}
void moveThreeSameColorRow(int row, int index){
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	for(int i=row;i<19;i++){
		for(int j=0;j<3;j++){
			boardcolours[6*(10*i+index+j)]=boardcolours[6*(10*(i+1)+index+j)];
			boardcolours[6*(10*i+index+j)+1]=boardcolours[6*(10*(i+1)+index+ j)+1];
			boardcolours[6*(10*i+index+j)+2]=boardcolours[6*(10*(i+1)+index+ j)+2];
			boardcolours[6*(10*i+index+j)+3]=boardcolours[6*(10*(i+1)+index+ j)+3];
			boardcolours[6*(10*i+index+j)+4]=boardcolours[6*(10*(i+1)+index+ j)+4];
			boardcolours[6*(10*i+index+j)+5]=boardcolours[6*(10*(i+1)+index+ j)+5];
			board[index+j][i]=board[index+j][i+1];
		}
		for(int c=index;c<3;c++){
			for(int j=0;j<6;j++){
				boardcolours[6*(10*19+ c)+j]=black;
				board[c][19]=false;
			}
		
		}

	}
	

	glBufferSubData(GL_ARRAY_BUFFER, 0, 1200*sizeof(vec4), boardcolours);
	glBindVertexArray(0);

}
void wholeSameColorRow(){
	for(int i=0;i<20;i++){
		int index=checkSameColorRow(i);
		if(index>0){
			moveThreeSameColorRow(i,index);
			i--;
		}
		
	}

}
int checkSameColorCol(int col){
	for(int i=0;i<18;i++){
		if(!checkColor(boardcolours[6*(10*i+ col)],black)){
			if(checkColor(boardcolours[6*(10*i+ col)],boardcolours[6*(10*(i+1)+ col)])){
				if(checkColor(boardcolours[6*(10*i+ col)],boardcolours[6*(10*(i+2)+ col)])){
					cout<<"COl: "<<col<<endl;
					return i;
				}
				
			}
		}
	}
	return -1;
}
void moveThreeSameColorCol(int col, int index){

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	cout<<index<<endl;
	for(int i=index;i<17;i++){
		
		boardcolours[6*(10*i+col)]=boardcolours[6*(10*(i+3)+col)];
		boardcolours[6*(10*i+col)+1]=boardcolours[6*(10*(i+3)+col)+1];
		boardcolours[6*(10*i+col)+2]=boardcolours[6*(10*(i+3)+col)+2];
		boardcolours[6*(10*i+col)+3]=boardcolours[6*(10*(i+3)+col)+3];
		boardcolours[6*(10*i+col)+4]=boardcolours[6*(10*(i+3)+col)+4];
		boardcolours[6*(10*i+col)+5]=boardcolours[6*(10*(i+3)+col)+5];
		board[col][i]=board[col][i+3];
	}
	for(int c=17;c<20;c++){
		for(int j=0;j<6;j++){
			boardcolours[6*(10*c+col)+j]=black;
		}
		board[col][c]=false;	
	
	}
	
	
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, 1200*sizeof(vec4), boardcolours);
	glBindVertexArray(0);	


}
void wholeSameColorCol(){
	for(int i=0;i<10;i++){
		int index=checkSameColorCol(i);
		if(index>0){
			moveThreeSameColorCol(i,index);

		}
		
	}

}
//-------------------------------------------------------------------------------------------------------------------

// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void settile()
{	
	for(int i=0;i<4;i++){
		int posX=tile[i].x+tilepos.x;
		int posY=tile[i].y+tilepos.y;
		boardcolours[6*(10*posY+ posX)] = tileColors[i];
		boardcolours[6*(10*posY+ posX)+1] = tileColors[i];
   		boardcolours[6*(10*posY+ posX)+2] = tileColors[i];
		boardcolours[6*(10*posY+ posX)+3] = tileColors[i];
		boardcolours[6*(10*posY+ posX)+4] = tileColors[i];
		boardcolours[6*(10*posY+ posX)+5] = tileColors[i];
		board[posX][posY]=true;
	}
	checkWholeRow();
	wholeSameColorRow();
	wholeSameColorCol();


	for(int i=0;i<20;i++){
		for(int j=0;j<10;j++){
			cout<<board[j][19-i]<<" ";
		}
		cout<<endl;
	}
	cout<<endl<<endl;
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

bool checkRight(){
	for(int i=0;i<4;i++){
		//int posY=tile[i].y+tilepos.y;
		int posX=tile[i].x+tilepos.x+1;
		if(posX>9){
			return false;
		}
	}
	return true;
}
bool checkLeft(){
	for(int i=0;i<4;i++){
		int posX=tile[i].x+tilepos.x-1;
		if(posX<0){
			return false;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------------------------

// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
void movetile(vec2 direction)
{
	tilepos.x=tilepos.x+direction.x;
	tilepos.y=tilepos.y+direction.y;
}
//-------------------------------------------------------------------------------------------------------------------

// Starts the game over - empties the board, creates new tiles, resets line counters
void restart()
{
	init();
}

//-------------------------------------------------------------------------------------------------------------------
GLfloat i = 0.0;
// Draws the game
void moveDown(int i){
	glutTimerFunc(500, moveDown, 0);
	if(checkDown()){
		movetile(d);
		
	}else if(!terminal){
		
		settile();
		
		shapeType=newtile();
		if(!checkDown()){
			terminal=true;
		}
	}

	updatetile();
}


void display()
{

	glClear(GL_COLOR_BUFFER_BIT);
	glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	glUniform1i(locysize, ysize);

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	glDrawArrays(GL_TRIANGLES, 0, 1200); // Draw the board (10*20*2 = 400 triangles)

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 24); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 64); // Draw the grid lines (21+11 = 32 lines)
	glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------------

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}
void space(){
	vec4 tempColors[4];
	for(int i=0;i<4;i++){
		tempColors[i]=tileColors[i];
	}
	for(int i=0;i<3;i++){
		tileColors[i]=tempColors[i+1];
	}
	tileColors[3]=tempColors[0];
	vec4 newcolours[24];
	for (int i = 0; i < 4; i++){
		newcolours[i*6] =tileColors[i];
		newcolours[i*6+1] =tileColors[i];
		newcolours[i*6+2] =tileColors[i];
		newcolours[i*6+3] =tileColors[i];
		newcolours[i*6+4] =tileColors[i];
		newcolours[i*6+5] =tileColors[i];

	}
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Handle arrow key keypresses
void special(int key, int x, int y)
{
	switch(key) 
	{
		case GLUT_KEY_UP:
			if(!terminal){
				rotate();
		 	}
			 
			break;
		case GLUT_KEY_RIGHT:
			if(!terminal){
				if(checkRight()){
					movetile(r);
				}
			}
			break;
		case GLUT_KEY_LEFT:
			if(!terminal){
				if(checkLeft()){
					movetile(l);
				}
			}
			break;
		case GLUT_KEY_DOWN:
			if(!terminal){
				while(checkDown()){
				movetile(d);
				}
				settile();
				shapeType=newtile();
			}
			
		
			
			break;

	}
	glutPostRedisplay();
}


//-------------------------------------------------------------------------------------------------------------------

// Handles standard keypresses
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 033: // Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'r': // 'r' key restarts the game
			restart();
			break;
		case ' ':
			space();
			break;

	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

void idle(void)
{
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 178); // Center the game window (well, on a 1920x1080 display)
	glutCreateWindow("Fruit Tetris");
	glewInit();
	init();

	// Callback functions
	moveDown(1);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop(); // Start main loop
	return 0;
}

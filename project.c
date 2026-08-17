#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>

#define height 20
#define width 10

int Grid[height][width];
int points=0;

int blocks[7][4][4]={
  {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},  //I
  {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, //O
  {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},//T
  {{0,0,1,1},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, //S
  {{1,1,0,0},{0,0,1,1},{0,0,0,0},{0,0,0,0}}, //Z
  {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, //L
  {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}} //J
};

typedef struct // for each individual shape
{
  int shape[4][4];
  int x,y; // For xy Plane

} Piece;

Piece present; // new variable of piece
void NewGrid(){
	int i,j;
	for(i=0;i<height;i++){
		for(j=0;j<width;j++){
			Grid[i][j]=0;
		}
	}
}
void newpiece(){
  int type=rand() %7;//for selecting random tetriminos out of the block array
  int i,j;
  for( i=0;i<4;i++){
    for( j=0;j<4;j++){
      present.shape[i][j]=blocks[type][i][j];// to copy varaible from block to present shape
      present.x=width/2 -2;
      present.y=0;
    }
  }
} 

void sector(){
  system("cls");// for clearing screen after every refresh
  int i,j;
  for( i=0;i<height;i++){
    for( j=0;j<width;j++){
      if(Grid[i][j])
      printf("#");// for saving permament blocks
      else{
        int space=0;
        int x,y;
        for( x=0;x<4;x++){
          for( y=0;y<4;y++){
            if(present.shape[x][y] && i==present.y+x && j==present.x+y){
              space=1;
            }
          }
        }
        printf(space ? "*" : ".");//for present blocks and free space
      }
    }
    printf("\n");
  }
  printf("Points: %d\n", points);
}

int checkbump(int cx,int cy){     //cx=change in x ; cy=change in y
int i,j;
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
        if(present.shape[i][j]){
          int nx=present.x+j+cx; // nx and ny for new x and y
          int ny=present.y+i+cy;
        if(nx<0 || nx>=width || ny>=height || (ny>0 && Grid[ny][nx])){
          return 1;
        }
        }
      }
    }
    return 0;
}
void PieceFixed(){
	int i,j;
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
        if(present.shape[i][j]){
            int nx=present.x+j; 
            int ny=present.y+i;
          if(ny>0){
            Grid[ny][nx]=1;
          }
        }

      }
    }  
  points+=1;    
}

void VanishLines(){
  int vanish=0;
  int i,j;
  for(i=0;i<height;i++){
    int fill=1;
      for (j=0;j<width;j++){
        if(!Grid[i][j]){
          fill=0;
          break;
        }
    }

    if(fill){
    	int k,i,j;
      for(k=i;k>0;k--){
        for( j=0;j<width;j++){
          Grid[k][j]=Grid[k-1][j];
        }
      }
      for(j=0;j<width;j++){
        Grid[0][j]=0; //vanish the filled line from the grid
      }
      vanish++;
    }
  }
  if(vanish>0){
    points+=vanish*10; //adding number of vanish lines 
  }
}

void SpinPiece(){
    int temp[4][4];
    int i,j;

    for( i=0;i<4;i++){
      for( j=0;j<4;j++){
        temp[j][3-i]=present.shape[i][j];
      }
    }

    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
        present.shape[i][j]=temp[i][j];
      }
    }
}

void SaveScore(int score){
	FILE *f=fopen("highscore","a");
	if(f == NULL){
		printf("Erorr in Saving Score!");
		return;
	}
	fprintf(f,"%d\n",score);
	fclose(f);
}
void ShowAllScores(){
	FILE *f=fopen("highscore","r");
	if(f==NULL){
		printf("No Previous Score\n");
		getch();
		return;
	}
	int scores[100],count=0;
	while(fscanf(f,"%d",&scores[count])==1){
		count++;
	}
	fclose(f);
	int i,j;
	for(i=0;i<count-1;i++){
		for(j=i+1;j<count;j++){
			if(scores[j]>scores[i]){
				int temp=scores[i];
				scores[i]=scores[j];
				scores[j]=temp;
			}
		}
	}
	printf("\nAll previous Scores (Highest to lowest)\n");
	for(i=0;i<count;i++){
		printf("%d\n", scores[i]);
	}
	printf("Press any key to return...");
	getch();
	}
void MainMenu(){
	system("cls");
	printf("\n\n\t\t\t\t\t\t   |TETRIS.com|\t\t\t\t\t\t\t\n\n\n\n");
	
	printf("\t\t\t\t\t\t1.Start New Game\n");
	printf("\t\t\t\t\t\t2.Previous Scores\n");
	printf("\t\t\t\t\t\t3.Exit\n\n\n");
	printf("Enter the Choice\n");
}
int main(){
	int choice;
	while(1){
		MainMenu();
		scanf("%d", &choice);
		if(choice==1){
		points=0;
		NewGrid();
		srand(time(NULL));
 		newpiece();
  
  while (1)
  {
    sector();
    Sleep(200);
    if(kbhit()){
      char c = getch();
      if(c =='a' && !checkbump(-1,0)){
      present.x--;}
      if(c =='d' && !checkbump(1,0)){
      present.x++;}
      if(c =='s' && !checkbump(0,1)){
      present.y++;}
      if(c =='w'){
      SpinPiece();}
    }

    if(!checkbump(0,1)){
      present.y++;
    }
    else{
      PieceFixed();
      VanishLines();
      newpiece();
    
      if(checkbump(0,0)){
        sector();
        printf("\n\t\t GAME OVER!  \n\t\tFINAL SCORE= %d" ,points);
        SaveScore(points);
        printf("Press any key to return\n");
        getch();
        break;
      }
    }
  }	
	}
	else if(choice == 2){
		ShowAllScores();
	}
	else if(choice == 3){
	printf("Exiting..\n");
	return 0;
	}
	else{
		printf("Invalid Choice\n");
		getch();
	}
}
}
  

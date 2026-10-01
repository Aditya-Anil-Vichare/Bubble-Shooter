#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <time.h>
#include <unistd.h>

#include <termios.h>


int col_grid = 20;
int row_grid = 21;

void moveShooter(char (*arr)[row_grid], int (*curr_gun_pos), int dirn, char curr_bullet){

	if (!((*curr_gun_pos)+dirn<1 || (*curr_gun_pos)+dirn>row_grid-2)){
		arr[row_grid-3][(*curr_gun_pos)]=' ';
		arr[row_grid-4][(*curr_gun_pos)]=' ';
		arr[row_grid-5][(*curr_gun_pos)]=' ';

		arr[row_grid-3][(*curr_gun_pos)-1]=' ';
		arr[row_grid-3][(*curr_gun_pos)+1]=' ';

		arr[row_grid-3][(*curr_gun_pos)+dirn]=curr_bullet;
		arr[row_grid-4][(*curr_gun_pos)+dirn]='-';
		arr[row_grid-5][(*curr_gun_pos)+dirn]='|';

		arr[row_grid-3][(*curr_gun_pos)+dirn-1]='|';
		arr[row_grid-3][(*curr_gun_pos)+dirn+1]='|';

		(*curr_gun_pos)+=dirn;
	}	
}

void shooter(char (*arr)[row_grid], int gun_pos, char curr_bullet){
	for (int i=0; i<row_grid; i++){
		arr[row_grid-5][i]=' ';
		arr[row_grid-4][i]=' ';
		arr[row_grid-3][i]=' ';
	}

	arr[row_grid-3][gun_pos]=curr_bullet;
	arr[row_grid-4][gun_pos]='-';
	arr[row_grid-5][gun_pos]='|';

	arr[row_grid-3][gun_pos-1]='|';
	arr[row_grid-3][gun_pos+1]='|';
	
}

void pop_bubble(char (*arr)[row_grid], char curr, int x, int y, int* count){
	for (int r=-1; r<=1; r++){
		for (int c=-1; c<=1; c++){
			if (r==0 && c==0){
				continue;
			}
			else{
				if (arr[y+r][x+c]==curr){
					arr[y+r][x+c]=' ';
					if (count[y+r-1]>0){
						--count[y+r-1];
					}
					pop_bubble(arr, curr, x+c, y+r, count);
				}
			}
		}
	}
}

bool checkwin(int* count){
	int sum=0;
	for (int i=0; i<row_grid-6; i++){
		sum+=count[i];
	}
	if (sum==0){
		//printf("\nWin");
		//sleep(10);
		return 1;
	}
	else{
		return 0;
	}

}

bool checklost(char (*arr)[row_grid], char curr){
	for (int c=1; c<col_grid; c++){
		if (arr[row_grid-6][c]==' ' || arr[row_grid-6][c]==curr){
			//sleep(1);
			return 0;
		}
		else{
			continue;
		}
	}
	//printf("\nLost");
	//sleep(10);
	return 1;
} 

void pattern(char (*arr)[row_grid]){
	for (int r=1; r<row_grid-10; r++){
		for (int c=1; c<col_grid; c++){
			arr[r][c] = (char)(48 + (rand()%10));
		}
	}
}

void display(char (*arr)[row_grid]){
	system("cls");

	printf("\nBubble Shooter (Brutal)\n\n");

	for (int row=0; row<col_grid; row++){
		for (int col=0; col<row_grid; col++){
			printf(" %c", arr[row][col]);
		}
		printf("\n");
	}

	printf("\n");
	
}

void shoot(char (*arr)[row_grid], char* curr, char* next, int shooter_x, int* count){
	int bullet_y = row_grid-6;
	if (arr[row_grid-6][shooter_x]==' '){
		arr[bullet_y][shooter_x]=*curr;
		while (1){
			display(arr);
			if (arr[bullet_y-1][shooter_x]==' '){
				--bullet_y;
				arr[bullet_y][shooter_x]=*curr;
				arr[bullet_y+1][shooter_x]=' ';
			}
			else{
				++count[bullet_y-1];
				break;
			}
		}
		pop_bubble(arr, *curr, shooter_x, bullet_y, count);
		*curr = *next;
		*next = (char)(48 + rand()%10);
		arr[row_grid-3][shooter_x]=*curr;
	}
	else if (arr[row_grid-6][shooter_x]==*curr){
		pop_bubble(arr, *curr, shooter_x, 14, count);
		*curr = *next;
		*next = (char)(48 + rand()%10);

		shooter(arr, shooter_x, *curr);
	}
}

bool BubbleShooter(){
	srand(time(NULL));

	char grid[col_grid][row_grid];

	for (int i=0; i<row_grid+2; i++){
		grid[0][i]='-';
		grid[row_grid-2][i]='-';
	}

	for (int i=0; i<col_grid; i++){
		grid[i][0]=' ';
		grid[i][col_grid]=' ';
	}

	pattern(grid);

	int gun = col_grid/2;
	char curr_bullet = (char)(48 + (rand()%10));
	char nxt_bullet = (char)(48 + (rand()%10));

	int row[row_grid-6];

	for (int i=0; i<10; i++){
		row[i]=row_grid-2;
	}
	for (int j=10; j<row_grid-6; j++){
		row[j]=0;
	}

	shooter(grid, gun, curr_bullet);

	for (int r=11; r<row_grid-5; r++){
		for (int c=1; c<row_grid-1; c++){
			grid[r][c]=' ';
		}
	}
	struct termios oldt, newt;

	/* Save current terminal settings */
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;

	/* Disable canonical mode (no Enter needed) and echo */
	newt.c_lflag &= ~(ICANON | ECHO);
	newt.c_cc[VMIN]  = 1;  /* read returns after 1 byte */
	newt.c_cc[VTIME] = 0;  /* no timeout */

	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	while(1){

		display(grid);

		char command=getchar(); // s=shoot, l=left, r=right
								// lll = 3 times left
								// rrrr = 4 times right
								// lrlr = left right left right
								// s = shoot
		display(grid);

		if (command=='l'){
			moveShooter(grid, &gun, -1, curr_bullet);
		}
		else if (command=='r'){
			moveShooter(grid, &gun, 1, curr_bullet);
		}
		else if (command=='s'){
			shoot(grid, &curr_bullet, &curr_bullet, gun, row);
			if (checklost(grid, curr_bullet)){
				display(grid);
				printf("\n\nYou Lost\nBetter Luck Next Time !!\n\n");
				return 1;
			}
			else if (checkwin(row)){
				display(grid);
				printf("\n\nCongratulations You Won !!!!\n\n");
				return 1;
			}
		}
		else if (command=='\n'){
			break;
		}
		else{
			printf("\n%c :: Invalid Move\n", command);
			sleep(1);
		}


	}
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /* Restore original terminal settings */

	sleep(5);

	return 0;
}

int main(){
	BubbleShooter();
	return 0;
}

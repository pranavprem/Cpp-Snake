#include <cstdlib>
#include<conio.h>
#include <iostream>
#include <graphics.h>
using namespace std;
#define div 20
#define max 15

/*
How this code works:
It splits your screen up into a grid. Dimensions determined by the div in the #define above
The entire code then works on this grid based system using an X-Y coordinate system to track which grid you are referring to.
Functions scale up the x-y coordinates to get pixel positions.


*/



struct snake
       {int x;
        int y;
        int dir;
       };






void shade (int x, int y)
     {int p;
     p=rand()%15;
     
     if(p==0)p++;
     //cout<<"COLOR"<<p<<endl;
     setcolor(p);
     setfillstyle(SOLID_FILL,p);
     rectangle(x*div+div/10,y*div+div/10,(x+1)*div-div/10,(y+1)*div-div/10);
     floodfill((x*div)+div/2,(y*div)+div/2,p);
       
     }            



void move(snake *snake)
     {switch(snake->dir)
                   {case 1:{snake->x++;
                         break;
                         }
                    case 2:{snake->y++;
                         break;
                         }
                    case 3:{snake->x--;
                         break;
                         }
                    case 4:{snake->y--;
                         break;
                         }
                    }
     }                  
                         


void addfruit(snake *fruit){
     int i;
     for(i=0;fruit[i].dir!=-1;++i);
     fruit[i].x=(rand()%max)+1;
     fruit[i].y=(rand()%max)+1;
     fruit[i].dir=0;
     }

void drawfruit(snake * fruit){
     int p;
     for(int i=0;fruit[i].dir!=-1;++i){
                     p=rand()%15;
                     if(p==0)p++;
                     
                     setcolor(p);
                     setfillstyle(SOLID_FILL,p);
                     arc(fruit[i].x*div,(fruit[i].y+1)*div,0,90,div);
                     arc(fruit[i].x*div+div,fruit[i].y*div,180,270,div);
                    floodfill((fruit[i].x*div)+div/2,(fruit[i].y*div)+div/2,p);          
             }    
}
void drawsnake(snake *snake)
     {int p=rand()%15;
     if(p==1)++p;
     setcolor(p);
     setfillstyle(SOLID_FILL,p);
     switch(snake[0].dir)
                      {case 4:{circle(snake[0].x*div+div/2,(snake[0].y+1)*div,div/2);
                      floodfill(snake[0].x*div+div/2,(snake[0].y+1)*div,p);
                             break;
                             }
                      case 1:{circle(snake[0].x*div,(snake[0].y*div+div/2),div/2);
                             floodfill(snake[0].x*div,(snake[0].y*div+div/2),p);
                             break;
                             }
                      case 2:{circle(snake[0].x*div+div/2,snake[0].y*div,div/2);
                            floodfill(snake[0].x*div+div/2,snake[0].y*div,p);
                             break;
                             }
                      case 3:{circle((snake[0].x+1)*div,(snake[0].y*div+div/2),div/2);
                             floodfill((snake[0].x+1)*div,(snake[0].y*div+div/2),p);
                             break;
                             }
                      }
     
     
     
     int i;
     for(i=1;snake[i].x!=-1;++i)
                      shade(snake[i].x,snake[i].y);
     }

                         
                         
//draw grid -debugging                         
void grid()
     {int i;
     for( i=0; i<max*div; i+=div)
        {line(i,0,i,max*div);
         line(0,i,max*div,i);
        }
     }
     
void addsnake(snake * snake){
     int i;
     
     for(i=0;snake[i].dir!=-1;++i);
     snake[i].dir=snake[i-1].dir;
      switch(snake[i].dir)
                      {case 4:{snake[i].x=snake[i-1].x;
                               snake[i].y=snake[i-1].y+1;
                             break;
                             }
                      case 1:{snake[i].x=snake[i-1].x-1;
                               snake[i].y=snake[i-1].y;
                             break;
                             }
                      case 2:{snake[i].x=snake[i-1].x;
                               snake[i].y=snake[i-1].y-1;
                             break;
                             }
                      case 3:{snake[i].x=snake[i-1].x+1;
                               snake[i].y=snake[i-1].y;
                             break;
                             }
                      }
     
     }
     
     



     
bool eatfruitcheck(snake * snake, snake * fruit){
     int i,j;
     for(i=0;fruit[i].dir!=-1;++i)
     {
         if(fruit[i].x==snake[0].x && fruit[i].y==snake[i].y)
                                   {
                                   for(j=i;fruit[j].dir!=-1;++j)
                                                                {
                                                                    fruit[j].x=fruit[j+1].x;
                                                                    fruit[j].y=fruit[j+1].y;
                                                                    fruit[j].dir=fruit[j+1].dir;
                                                                }
                                   
                                  addsnake(snake);
                                  return true;
                                  }                              
     }
     return false;        
     }
     
     

bool islose(snake *snake){
     int i,z;
     for(i=0; snake[i].dir!=-1; ++i)
                  {
                  
             if(snake[i].x<0 || snake[i].y<0 || snake[i].x>(max-1) || snake[i].y>(max-1)){
                             outtextxy(max/2,max/2,"LOSER SNAKE");
                             return true;
                             }
             
           }
     return false;
     }

bool iswin(snake * snake){
     int i;
     for(i=0;snake[i].dir!=-1;++i);
     if(i>10){
              outtextxy(max/2,max/2, "VICTORIOUS SNAKE");
             return true;
             }
     else
         return false;
         }

bool iseatself(snake * snake){
     int i,j;
     for(i=1;snake[i].dir!=-1;++i){
                                   if(snake[0].x==snake[i].x && snake[0].y==snake[i].y){
                                                             outtextxy(max/2,max/2, "LOSER SNAKE");
                                                             return true;
                                                             }
                                   }
     return false;
     }

void initsnake(snake *s)
     {int i;
      s[0].x=9;
      s[0].y=9;
      s[0].dir=1;
                     
     for(i=1;i<20;++i)
         {s[i].x=-1;
          s[i].y=-1;
          s[i].dir=-1;
          }
     }
void drawScore(int score){
     char x1[10]="Score: ";
     char x2[30];
     char *x3;
     itoa(score,x2,10);
     x3=strcat(x1, x2);
     outtextxy(div, div, x3);
     
     }

int main()
{   snake snake[50], cache[100], fruit[20];
    //Snake stores the entire snake. When fruit is eaten, new snake element is added to array
    //Fruit stores the various locations of fruits. When new fruit is created, new element is added to array. When fruit is eaten, element is removed
    //Cache is so named because it stores a cache of all the movements the snake has to perform. Element is created every time the snake is instructed to move in a direction by keyboard. Entry is removed when full snake moves in instructed direction
    int score=0;
    int check=1, i, j = 0, gd = DETECT, gm, c,k;
    initwindow(div*max,div*max,"Pranav's Snake Game");
    initsnake(snake);
    initsnake(cache);
    initsnake(fruit);
    fruit[0].x=-1;
    fruit[0].y=-1;
    fruit[0].dir=-1;
    cache[0].x=-1;
    snake[1].x=8;
    snake[1].y=9;
    snake[1].dir=1;
    snake[2].x=7;
    snake[2].y=9;
    snake[2].dir=1;
   grid();
   bool winFlag;

                while(check)
               {
               c=0;
               while(!kbhit())
                            {
                             //This whole section moves the snake. It does the major logic of movement as well.          
                               for(i=0;snake[i].x!=-1;++i)
                                   {for(j=0;cache[j].x!=-1;++j)
                                        {if(snake[i].x==cache[j].x && snake[i].y==cache[j].y)
                                                               {
                                                               snake[i].dir=cache[j].dir;
                                                               
                                                                if(snake[i+1].x==-1)//if the entire snake has moved in this direction
                                                                                {//remove this element from cache
                                                                                for(k=j;k<18;++k)
                                                                                                      {cache[k].x=cache[k+1].x;
                                                                                                       cache[k].y=cache[k+1].y;
                                                                                                       cache[k].dir=cache[k+1].dir;
                                                                                                       }
                                                                                if(k==18)
                                                                                         {cache[19].x=-1;
                                                                                         }
                                                                                }
                                                                
                                                                }  
                                        }
                                   }
                                                
                                
                                
                                cleardevice();
                                
                            //grid();
                            //Move the snake in the GUI
                            for(i=0;snake[i].x!=-1;++i)
                                 {move(&snake[i]);
                                 }      
                            drawsnake(snake);
                            //one in 9 chances that a fruit is created
                            if(rand()%9==7){
                                           addfruit(fruit);
                                           }
                                           
                            drawfruit(fruit);
                            drawScore(score);
                            if(eatfruitcheck(snake,fruit)){
                                                       score+=50;
                                                       //addsnake(a);
                                                       }
                            if(islose(snake)){
                                              winFlag=false;
                                              break;
                                       }
                            if(iseatself(snake)){winFlag=false;
                                              break;
                                             }
                            if(iswin(snake)){
                                             winFlag=true;
                                             break;
                                             }
                                                       
                            if(score>900)//speeding up the snake
                                         delay(100);
                            else
                             delay(1000-score);         
                                     
                            
                            }
               c=getch();//cout<<c<<endl;
               for(i=0;cache[i].x!=-1;++i);
               //Next 4 if statements are meant to move the snake in the instructed direction by creating entry in cache
               if(c==119 && snake[0].dir!=2)
                         {cache[i].x=snake[0].x;
                         cache[i].y=snake[0].y;
                         cache[i].dir=4;
                         }
               else if(c==97 && snake[0].dir!=1)
                         {cache[i].x=snake[0].x;
                         cache[i].y=snake[0].y;
                         cache[i].dir=3;
                         }
               else if(c==100 && snake[0].dir!=3)
                         {cache[i].x=snake[0].x;
                         cache[i].y=snake[0].y;
                         cache[i].dir=1;
                         }
               else if(c==115 && snake[0].dir!=4)
                         {cache[i].x=snake[0].x;
                         cache[i].y=snake[0].y;
                         cache[i].dir=2;
                         }
               /*cout<<"\nsnake\n";
               for(i=0;i<20;++i)
                                cout<<snake[i].x<<" "<<snake[i].y<<" "<<snake[i].dir<<"\n";
               cout<<"\ncache\n";
               for(i=0;i<20;++i)
                                cout<<cache[i].x<<" "<<cache[i].y<<" "<<cache[i].dir<<"\n";
               */
               
               }
   cleardevice();      
  
   
   getch();
   return 0;
}

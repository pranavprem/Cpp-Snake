#include <cstdlib>
#include<conio.h>
#include <iostream>
#include <graphics.h>
using namespace std;
#define div 30



struct snake
       {int x;
        int y;
        int dir;
       };









void shade (int x, int y)
     {int p;
     p=rand()%15+1;
     setcolor(p);
     setfillstyle(SOLID_FILL,p);
           rectangle(x*div+div/10,y*div+div/10,(x+1)*div-div/10,(y+1)*div-div/10);
      floodfill((x*div)+div/2,(y*div)+div/2,p);
       
     }            



void move(snake *s)
     {switch(s->dir)
                   {case 1:{s->x++;
                         break;
                         }
                    case 2:{s->y++;
                         break;
                         }
                    case 3:{s->x--;
                         break;
                         }
                    case 4:{s->y--;
                         break;
                         }
                    }
     }                  
                         


void drawsnake(snake *a)
     {switch(a[0].dir)
                      {case 4:{circle(a[0].x*div+div/2,(a[0].y+1)*div,div/2);
                             break;
                             }
                      case 1:{circle(a[0].x*div,(a[0].y*div+div/2),div/2);
                             break;
                             }
                      case 2:{circle(a[0].x*div+div/2,a[0].y*div,div/2);
                             break;
                             }
                      case 3:{circle((a[0].x+1)*div,(a[0].y*div+div/2),div/2);
                             break;
                             }
                      }
     
     
     int i;
     for(i=1;a[i].x!=-1;++i)
                      shade(a[i].x,a[i].y);
     }

                         
                         
                         
void grid()
     {int i;
     for( i=0; i<600; i+=div)
        {line(i,0,i,600);
         line(0,i,600,i);
        }
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


int main()
{   snake a[20], cache[20];
    int check=1, i, j = 0, gd = DETECT, gm, c,k;
    initwindow(600,600,"abc");
    initsnake(a);
    initsnake(cache);
    cache[0].x=-1;
    a[1].x=8;
    a[1].y=9;
    a[1].dir=1;
    a[2].x=7;
    a[2].y=9;
    a[2].dir=1;
   grid();

                while(check)
               {
               c=0;
               while(!kbhit())
                            {  
               for(i=0;a[i].x!=-1;++i)
                   {for(j=0;cache[j].x!=-1;++j)
                        {if(a[i].x==cache[j].x && a[i].y==cache[j].y)
                                               {
                                               a[i].dir=cache[j].dir;
                                                if(a[i+1].x==-1)
                                                                {cout<<i<<endl;
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
                            for(i=0;a[i].x!=-1;++i)
                                 {move(&a[i]);
                                 }      
                            drawsnake(a);
                             delay(500);         
                                     
                            
                            }
               c=getch();//cout<<c<<endl;
               for(i=0;cache[i].x!=-1;++i);
               
               if(c==119 && a[0].dir!=2)
                         {cache[i].x=a[0].x;
                         cache[i].y=a[0].y;
                         cache[i].dir=4;
                         }
               if(c==97 && a[0].dir!=1)
                         {cache[i].x=a[0].x;
                         cache[i].y=a[0].y;
                         cache[i].dir=3;
                         }
               if(c==100 && a[0].dir!=3)
                         {cache[i].x=a[0].x;
                         cache[i].y=a[0].y;
                         cache[i].dir=1;
                         }
               if(c==115 && a[0].dir!=4)
                         {cache[i].x=a[0].x;
                         cache[i].y=a[0].y;
                         cache[i].dir=2;
                         }
               /*cout<<"\nsnake\n";
               for(i=0;i<20;++i)
                                cout<<a[i].x<<" "<<a[i].y<<" "<<a[i].dir<<"\n";
               cout<<"\ncache\n";
               for(i=0;i<20;++i)
                                cout<<cache[i].x<<" "<<cache[i].y<<" "<<cache[i].dir<<"\n";
               */
               
               }
               
   
   
   getch();
   return 0;
}

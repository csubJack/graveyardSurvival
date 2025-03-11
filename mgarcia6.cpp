// empty file for now

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

void zombieHealthBar()
{
    int zombieHealth = 7;
    cout << '{';
    for (int j=0; j < zombieHealth; j++)
    {
        if (j < zombieHealth)
        {
            cout << '#';
        }
        else
        {
            cout << ' ';
        }
    }
}




    //zombieHealth-- Under if (dist < (a->radius*a->)
    //then call for the new healthbar by doing drawHealthBar(zombieHealth));
    //
    //
    //when zombie dies 
    //if (zombieHealth == 0) {
    //  cout << "Zombie defeated!" << endl;
    //  Asteroid *savea = a-> next;
    //  deleteAsteroid(&g, a);
    //  a = savea;
    //  g.nasteroids--;
    //
    //Tryingy to find a way to attach the healthbar to the zombie similar to thrust if that works
    //
    //      
                                 

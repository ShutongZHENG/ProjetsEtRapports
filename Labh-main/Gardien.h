#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include <math.h>
#include <iostream>
#include <ctime>
using std::cout;
using std::endl;
using std::min;
using std::max;
using std::abs;

const double PI = 3.1415926535;

class Labyrinthe;


class Gardien : public Mover {
public:
	int speed_shoot; // Vitesse de tir des gardes
	int collision_time ; // Temps de perforation
	double move_ds ; // Vitesse de déplacement des gardes
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{
		speed_shoot = 0;
		collision_time = 0;
		move_ds = 0.1;

	}

	// mon gardien pense tr�s mal!
	void update (void) {
		
	// //Si le garde heurte le mur 3 fois, le garde se déplace dans la direction opposée.
		if(collision_time == 3){
			move_ds = 0 - move_ds;
			collision_time = 0;
		}
			
		 

		//Déterminez si les gardes chargent le mur.
		if(!move(move_ds,move_ds))
			collision_time ++;
	

		int x_player = _l->_guards[0]->_x;
		int y_player = _l->_guards[0]->_y;
		int x_enemy = _x;
		int y_enemy = _y;
		//Calcul de la distance entre le joueur et les gardes
		float dis_y = (float)abs(y_player - y_enemy);
		float dis_x = (float)abs(x_player - x_enemy);
		float dis_total = dis_y*dis_y + dis_x*dis_x ;

		//Temps d'attente du joueur - 1, ou si le temps d'attente du joueur est négatif, laissez-le à 0.
		((Chasseur*)_l->_guards[0])->wait_time --;
	
		if (((Chasseur*)_l->_guards[0])->wait_time < 0)
		{
			((Chasseur*)_l->_guards[0])->wait_time = 0;
		}
		//Si le joueur attend 0 et que la hp n'est pas égale à 100, la récupération du sang commence.
		if(((Chasseur*)_l->_guards[0])->wait_time == 0 && ((Chasseur*)_l->_guards[0])->hp != 100){
			((Chasseur*)_l->_guards[0])->hp ++;
			message("Hunter HP : %d",((Chasseur*)_l->_guards[0])->hp);
		}
// Si la distance entre le joueur et le garde est <= 25000 et qu'il n'y a pas de mur entre les deux. le joueur sera repéré et attaqué par l'ennemi.
	if(dis_total <= 25000 && !existWall()){
		speed_shoot ++;
		int angle = (int)(atan(dis_y/dis_x)*180/PI) ;
		//L'ennemi se retourne
		if (y_enemy == y_player && x_player > x_enemy)
			_angle = 270;
		else if (y_enemy == y_player && x_player < x_enemy)
			_angle = 90;
		else if (x_player == x_enemy && y_player > y_enemy)
			_angle = 0;
		else if (x_player == x_enemy && y_player < y_enemy)
			_angle = 180;
		//右下角Coin inférieur droit
		else if (x_player > x_enemy &&  y_player < y_enemy)
		 	_angle = 270 -angle;
		//左下角Coin inférieur gauche
		else if (x_player < x_enemy && y_player < y_enemy)
			_angle = 90+angle;
		//左上角En haut à gauche
		else if (x_player < x_enemy && y_player > y_enemy)
			_angle= 90-angle;
		
		// //右上角Coin en haut à droite
		else if (x_player > x_enemy && y_player > y_enemy)
		    _angle= 270+angle;
		if(speed_shoot == 100){
		fire(0);
		speed_shoot = 0;
		}	
	

	}


		
	};
	// Les gardes se déplacent en zigzag pour les patrouilles.
	bool move (double dx, double dy) { 

	int old_x = _x;
	int old_y = _y;

	
	
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale))    || ( (int)old_x/Environnement::scale ==(int)((_x+dx) / Environnement::scale)  && (int)old_y/Environnement::scale ==(int)((_y+dy) / Environnement::scale) ) )
	{
		
		if(_l->data((int)(old_x / Environnement::scale) , (int)(old_y / Environnement::scale) ) == 1)
			((Labyrinthe*)_l)->setData((int)(old_x / Environnement::scale),(int)(old_y / Environnement::scale),EMPTY);
		
		_x += dx;
		_y += dy;

		((Labyrinthe*)_l)->setData((int)(_x / Environnement::scale),(int)(_y / Environnement::scale),1);
	
		return true;
	}
	
	return false;
 }

	//Les gardes tirent
	void fire (int angle_vertical) {
	
		int angle_H = 360-_angle+random()%5;
		if (angle_H <0 )
		{
			/* code */
			angle_H = 0;
		}
		
		_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, angle_H);
		
		
	}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { 



	if(
		(int)((_fb -> get_x () + dx) / Environnement::scale)  == (int)(_l->_guards [0] -> _x / Environnement::scale)
		&&
		(int)((_fb -> get_y () + dy) / Environnement::scale) ==(int)(_l->_guards [0] -> _y / Environnement::scale)
	){
		
		((Chasseur*)_l->_guards[0])->hurt();
		return false;
	}
	else if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		
		return true;
	}
	else 
		return false;

	}

//Déterminer s'il y a un mur entre le joueur et le gardien
bool existWall(){  
	
	 int x_player =  (int)(_l->_guards[0] ->_x/Environnement::scale);
	 int y_player = (int)(_l->_guards[0] -> _y/Environnement::scale);
	 int x_gardien = (int)(this->_x/Environnement::scale);
	 int y_gardien = (int)(this->_y/Environnement::scale);




		for( int i = min(x_player,x_gardien); i<= max(x_player, x_gardien) ;i++ )
			for (int j = min(y_player,y_gardien); j <= max(y_player,y_gardien); j++)
			{
				/* code */

				if((i== x_player && j == y_player) || (i==x_gardien && j == y_gardien))
				continue;
				
				if(_l->data( i, j) == 1){
					
					return true;
				}
					

			}
		return false;

}

 };

#endif

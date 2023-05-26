#include "Chasseur.h"
#include <iostream>
using std::cout;
using std::endl;

/* function:  Tente un deplacement.
*  parameters : double dx, double dy
* returns : bool
*/
bool Chasseur::move_aux (double dx, double dy)
{


	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;

		//如果玩家抵达宝藏位置则获胜，并给出获胜画面
        //Si le joueur atteint l'emplacement du trésor, il gagne et reçoit l'écran gagnant.
		if((_l->_treasor._x == (int)_x/Environnement::scale  && _l->_treasor._y == (int)_y/Environnement::scale+1) ||(_l->_treasor._x == (int)_x/Environnement::scale  && _l->_treasor._y == (int)_y/Environnement::scale-1))
			partie_terminee(true);


	// 如果玩家移动到传送门则给传送到另一个传送门
    //Si le joueur se déplace vers un portail, il est téléporté vers un autre portail.
	if((int) (_x/Environnement::scale) == _l->_marks[0]._x && (int) (_y/Environnement::scale) == _l->_marks[0]._y  ){
			_x = _l->_marks[1]._x * Environnement::scale ;
			_y = _l->_marks[1]._y * Environnement::scale ;

		cout <<"here 1 "<<endl;
	}
	else if ((int) (_x/Environnement::scale) == _l->_marks[1]._x  && (int) (_y/Environnement::scale) == _l->_marks[1]._y ){
		_x = _l->_marks[0]._x * Environnement::scale ;
		_y = _l->_marks[0]._y * Environnement::scale ;
	
		cout<< "here 2 "<<endl;
	}
		
		return true;
	}
	return false;
}

/* function: Chasseur  Constructeur
*  parameters : Labyrinthe* l
* returns : Chasseur
*/
Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	// initialise les sons.
	//初始化血量为100 ， 等待时间为300 

	hp=100;
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
	message("Hunter HP : %d",this->hp);
	wait_time = 300;
}

/* function: Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
*  parameters : float dx, float dy
* returns : bool
*/
bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Hunter HP : %d  Woooshh ..... %d\n", hp,(int) dist2);
		// il y a la place.
		return true;
	}

// 如果火球打到守卫，则守卫会跌倒
//Si la boule de feu touche le garde, celui-ci tombera.
	for(int i = 1 ; i <_l->_nguards; i++){

	

	if(
		(int)((_fb -> get_x () + dx) / Environnement::scale)  == (int)(_l->_guards [i] -> _x / Environnement::scale)
		&&
		(int)((_fb -> get_y () + dy) / Environnement::scale) ==(int)(_l->_guards [i] -> _y / Environnement::scale)
	 ){
		_l->_guards[i]->tomber();
		_hunter_hit ->play();

	 }




	}


	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Hunter HP : %d  Booom...\n",this->hp);
	
	return false;
}

/*
 * function: Tire sur un ennemi
*  parameters : int angle_vertical
*  returns :
*/
void Chasseur::fire (int angle_vertical)
{
	message ("Hunter HP : %d  Woooshh...",this->hp);
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
	


	
}

/*
 * function: Clic droit: par defaut fait tomber le premier gardien. Inutile dans le vrai jeu, mais c'est juste pour montrer ,ne utilisation des fonctions � tomber � et � rester_au_sol �
*  parameters : bool shift, bool control
*  returns :
*/
void Chasseur::right_click (bool shift, bool control)
{
	
	if (shift)
		_l -> _guards [1] -> rester_au_sol ();
	else
		_l -> _guards [1] -> tomber ();
}

//玩家受到火球伤害，则hp-5 如果hp=0，则游戏结束。
//Le joueur subit les dégâts de la boule de feu, puis le hp -5 Si hp=0, le jeu est terminé.
void Chasseur::hurt(){
	this->hp  = this->hp -5;
	message("Hunter HP : %d",this->hp);
	if(this->hp == 0)
	partie_terminee(false);
	wait_time = 300*(_l->_nguards-1);

}

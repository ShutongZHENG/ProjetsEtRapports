#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::ifstream;

Sound *Chasseur::_hunter_fire; // bruit de l'arme du chasseur.
Sound *Chasseur::_hunter_hit;  // cri du chasseur touch�.
Sound *Chasseur::_wall_hit;	   // on a tap� un mur.

/*
 *	Pour que l'interface puisse appeler le constucteur de
 *	la classe Labyrinthe sans en avoir les sources...
 */

Environnement *Environnement::init(char *filename)
{
	return new Labyrinthe(filename);
}


Labyrinthe::Labyrinthe(char *filename)
{
	vector<vector<char> > map = this->loadMap(filename);
	_width = map.size();
	_height = getMapWidth(map);

	vector<Picture> pictures = getPictures(map);
	vector<Object> boxes = getObjects('x', map);
	vector<Object> guards = getObjects('G', map);
	vector<Object> treasures = getObjects('T', map);
	vector<Object> player = getObjects('C',map);
	vector<Object> marks = getObjects('M',map);
	vector<SousWall> walls =  getWalls(map);
 	_nwall = walls.size();
 	_walls = new Wall [_nwall];
	
 	_npicts = pictures.size();
 	_picts = new Wall [_npicts];

 	_nboxes = boxes.size();
 	_boxes = new Box [_nboxes];
	
	_nmarks = marks.size();
	_marks = new Box [_nmarks];

//	Placer les  murs.

for (int i = 0; i < _nwall; i++)
{
	_walls [i]._x1 = walls[i].x1;
	_walls [i]._y1 = walls[i].y1;
	_walls [i]._x2 = walls[i].x2;
	_walls [i]._y2 = walls[i].y2;
	_walls [i]._ntex = 0;
}

//  placer les affiches; Attention: pour des raisons de rapport d'aspect,
	char	tmp [128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
for (int i = 0; i < _npicts; i++)
{

	_picts [i]._x1 = pictures[i].x;
	_picts [i]._y1 = pictures[i].y;
	_picts [i]._ntex = 0;
	if (pictures[i].direction == 0)
	{
		_picts [i]._y2 = pictures[i].y+2; 
		_picts [i]._x2 = pictures[i].x;
	}else{
		_picts [i]._y2 = pictures[i].y; 
		_picts [i]._x2 = pictures[i].x+2;
	}

	if (pictures[i].type_P == 'b')
	{
		_picts [i]._ntex = wall_texture(tmp);
	}
	
	
	
	
}


	//  placer les caisses.
	char tmp2 [128];
	sprintf (tmp, "%s/%s", texture_dir, "boite.jpg");
	sprintf (tmp2, "%s/%s", texture_dir, "boite-2.jpg");
	for (int i = 0; i < boxes.size(); i++)
	{
	
		_boxes [i]._x = boxes[i].x;
		_boxes [i]._y = boxes[i].y;

		switch (i % 3)
		{
		case 1:
			_boxes [i]._ntex = wall_texture(tmp);
			break;
		case 2:
			_boxes [i]._ntex = wall_texture(tmp2);
			break;
		default:
			_boxes [i]._ntex = 0;
			break;
		}

		
	}




	// 6.1 allouer un tableau de pointeurs de lignes.
	_data = new char* [width()];
	// 6.2 faire pointer chaque ligne sur son tableau de colonnes.
	for (int i = 0; i < width(); ++i)
	_data [i] = new char [height ()];

	// 7. indiquer l'emplacement des murs en marquant les cases du sol.
	for (int i = 0; i < width (); ++i)
		for (int j = 0; j < height (); ++j) {
			// if (i == 0 || i == width ()-1 || j == 0 || j == height ()-1)
			// 	_data [i][j] = 1;
			// else
			// 	_data [i][j] = EMPTY;

			if(j >= map[i].size() ){
					_data[i][j] = EMPTY;
					continue;
			}

			else if(map[i][j] =='a' || map[i][j] == 'b'  || map[i][j] == '+'  || map[i][j] == '|'  || map[i][j] == '-' )
				_data[i][j] = 1;
			else 
					_data[i][j] = EMPTY;


		}



	// 7.1 indiquer l'emplacement des caisses au sol.
	for (int i = 0; i < _nboxes; i++)
	{
			_data [_boxes [i]._x][_boxes [i]._y] = 1;
	}


	// 7.2 marks;

	for (int i = 0; i < _nmarks; i++)
	{
		/* code */
		_marks [i]._x = marks[i].x;
		_marks [i]._y = marks[i].y;
		switch (i)
		{
		case 0:
			sprintf (tmp, "%s/%s", texture_dir, "p3.gif");
			break;
		
		default:
			sprintf (tmp, "%s/%s", texture_dir, "p4.gif");
			break;
		}
		_marks [i]._ntex = wall_texture(tmp); 

	}
	


	// 8. le tr�sor.
	
	_treasor._x = treasures[0].x;
	_treasor._y = treasures[0].y;
	// 8.1 indiquer l'emplacement du tr�sor au sol.
	_data [_treasor._x][_treasor._y] = 1;




	// 9. allouer le chasseur et les 4 gardiens.
	_nguards = guards.size()+1;
	_guards = new Mover* [_nguards];
	_guards [0] = new Chasseur (this);
	_guards [0] -> _x = player[0].x *10;
	_guards [0] -> _y = player[0].y *10;


	for (int i = 1; i < _nguards; i++)
	{
		
		switch ( i%8)
		{
		case 0:
			_guards[i] = new Gardien (this, "Blade");
			_guards[i] -> _x = guards[i-1].x *10;
			_guards[i] -> _y = guards[i-1].y *10;
			break;
		
		case 1:
			_guards[i] = new Gardien (this, "drfreak");
			_guards[i] -> _x = guards[i-1].x*10;
			_guards[i] -> _y = guards[i-1].y*10;
			break;
		case 2:
			_guards[i] = new Gardien (this, "garde");
			_guards[i] -> _x = guards[i-1].x*10;
			_guards[i] -> _y = guards[i-1].y*10;
			break;
		
		case 3:
			_guards[i] = new Gardien (this, "Lezard");
			_guards[i] -> _x = guards[i-1].x*10;
			_guards[i] -> _y = guards[i-1].y*10;
			break;
		case 4:
			_guards[i] = new Gardien (this, "Marvin");
			_guards[i] -> _x = guards[i-1].x*10;
			_guards[i] -> _y = guards[i-1].y*10;
			break;
		
		case 5:
			_guards[i] = new Gardien (this, "Potator");
			_guards[i] -> _x = guards[i-1].x*10;
			_guards[i] -> _y = guards[i-1].y*10;
			break;
		case 6:
			_guards[i] = new Gardien (this, "Samourai");
			_guards[i] -> _x = guards[i-1].x*10;
			_guards[i] -> _y = guards[i-1].y*10;
			break;
		default:
			_guards[i] = new Gardien (this, "Serpent");
			_guards[i] -> _x = guards[i-1].x*10;
			_guards[i] -> _y = guards[i-1].y*10;
			break;
		}

		_data [(int)(_guards [i] -> _x / scale)][(int)(_guards [i] -> _y / scale)] = 1;


	}
	

	


}
////Lecture d'un fichier de carte se terminant par .txt sur le disque dur et renvoie un tableau vectoriel de classe char, R^2.
vector<vector<char> > Labyrinthe::loadMap(char *filename)

{
	vector<vector<char> > map;
	ifstream readfile(filename);
	if (!readfile)
	{
		cout << "Error! This file can't be opened!" << endl;
	}
	else
	{
		const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE];
		while (!readfile.eof())
		{
			memset(buffer, '\0', sizeof(buffer));
			readfile.getline(buffer, BUFFER_SIZE);
			vector<char> sub_map;
			for (int i = 0; i < BUFFER_SIZE; i++)
			{
				if (buffer[i] != '\0')
				{
					sub_map.push_back(buffer[i]);
				}
				else
				{
					map.push_back(sub_map);
					break;
				}
			}
		}
	}

	readfile.close();
	cout<<"LoadMap ok !"<<endl;
	return map;
}
//Obtenir la largeur de la carte. et retourner int
int getMapWidth(vector<vector<char> > map)
{
	int width = 0;
	for (int i = 0; i < map.size(); i++)
	{
		if (map[i].size() > width)
		{
			width = map[i].size();
		}
	}
	return width;
}
//Obtenir un tableau d'images sur le mur
vector<Picture> getPictures(vector<vector<char> > map)
{
	vector<Picture> ps;
	for (int i = 0; i < map.size(); i++)
	{
		for (int j = 0; j < map[i].size(); j++)
		{
			if (map[i][j] == 'a' )
			{
				
		

				if ((map[i][j+2] == '-' || map[i][j+2] == '+'  )&& j+2 < map[i].size()  )
				{
				Picture p;
				p.type_P = 'a';
				p.x = i;
				p.y = j;
				p.direction = 0;
				ps.push_back(p);
				}
				else if ((map[i+2][j] == '|' || map[i+2][j] == '+'  )&& i+2 < map[i].size() ){
				Picture p;
				p.type_P = 'a';
				p.x = i;
				p.y = j;
				p.direction = 1;
				ps.push_back(p);

				}
				
				
				
			}
			if (map[i][j] == 'b')
			{
				if ((map[i][j+2] == '-' || map[i][j+2] == '+'  )&& j+2 < map[i].size()  )
				{
				Picture p;
				p.type_P = 'b';
				p.x = i;
				p.y = j;
				p.direction = 0;
				ps.push_back(p);
				}
				else if ((map[i+2][j] == '|' || map[i+2][j] == '+'  )&& i+2 < map[i].size() ){
				Picture p;
				p.type_P = 'b';
				p.x = i;
				p.y = j;
				p.direction = 1;
				ps.push_back(p);

				}
			}
		}
	}
	return ps;
}
// Obtenir le tableau de contenu correspondant en fonction du contenu de l'objet.
vector<Object> getObjects(char obj, vector<vector<char > > map)
{

	vector<Object> objects;
	for (int i = 0; i < map.size(); i++)
	{
		for (int j = 0; j < map[i].size(); j++)
		{
			if (map[i][j] == obj)
			{
				Object obj;
				obj.x = i;
				obj.y = j;
				objects.push_back(obj);
			}
		}
	}
	return objects;
}

//Obtenir un tableau de murs
vector<SousWall> getWalls(vector<vector<char > > map ){
	vector<SousWall> walls;
	vector<Object> plus  = getObjects('+',map);
	for(int a = 0 ; a<plus.size(); a++){

	
		int x = plus[a].x;
		int y = plus[a].y;
		
		//Search the + below
		if( (x+1 < map.size()) &&(map[x+1][y] == '|' || map[x+1][y] == 'a' || map[x+1][y] == 'b' || map[x+1][y] == '+')  )
		{
		for (int i = x+1; i < map.size(); i++)
			{
				if (map[i][y] == '+')
				{
					SousWall sousWall;
					sousWall.x1 = x;
					sousWall.y1 = y;
					sousWall.x2 = i;
					sousWall.y2 = y;
					walls.push_back(sousWall);
					break;

				}
				
			}
		

		}

		//Search the + right
		if ( y+1 < map[x].size() && (map[x][y+1] == '-' || map[x][y+1] == 'a' || map[x][y+1] == 'b' || map[x][y+1] == '+'))
		{
			for (int i = y+1; i < map[x].size(); i++)
				{
					if(map[x][i] == '+'){
						SousWall sousWall;
						sousWall.x1 = x;
						sousWall.y1 = y;
						sousWall.x2 = x;
						sousWall.y2 = i;
						walls.push_back(sousWall);
						break;
					}
				}
				


		}

	}

	return walls;
}

#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"
#include <vector>
using std::vector;


class Labyrinthe : public Environnement {
private:
	int		_width;
	int		_height;
	char**	_data;
	vector<vector<char > > loadMap(char * filename);  

public:
	Labyrinthe (char*);
	int width () { return _width;}	// retourne la largeur du labyrinthe.
	int height () { return _height;}	// retourne la longueur du labyrinthe.
	char data (int i, int j)
	{
		return _data [i][j];
	}	// retourne la case (i, j).
	void setData(int i , int j , char c){
		_data[i][j] = c;
	}
};



//structure Picture pour sauver les peintures murales
struct Picture
{
	char type_P;
	int x ;
	int y;
	int direction; 
};

// La structure Objet est utilisée pour indiquer l'emplacement des boîtes, des joueurs, des gardes, des portails, des trésors, etc.
struct Object
{
	int x ;
	int y;
};
// La structure SousWall est utilisée pour stocker les positions de départ et d'arrivée de la clôture.
struct SousWall
{
	int x1;
	int y1;
	int x2;
	int y2;

};

int getMapWidth(vector<vector<char> > map );

vector<Picture>  getPictures(vector<vector<char> > map );
vector<Object> getObjects(char obj, vector<vector<char> > map);
vector<SousWall> getWalls(vector<vector<char> > map);

#endif

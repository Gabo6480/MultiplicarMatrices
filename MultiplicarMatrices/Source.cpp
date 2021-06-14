#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include "resource.h"
#include <math.h>
//#include <gdiplus.h>
//#include <strsafe.h>
//#include <commctrl.h>

using namespace std;



//Estructuras
struct Matriz {
	char ID[7] = {'M','0','0','0','0','0','0' };
	int Columnas = 0; //Maximo de columnas (Recordar que empieza desde 0)
	int Filas = 0;  //Maximo de filas (Recordar que empieza desde 0)
	float Matrix[10][10] = { 0 }; // Matrix[Filas+1][Columnas+1]
	Matriz *nxt=0;
	Matriz *prv=0;
	vector <int> conect;
};

Matriz Punto{
   /*ID*/{ 'P','0','0','0','0','0','0' },
   /*Columnas*/0 ,
   /*Filas*/3,
   /*Matrix*/ {{0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},{0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},{0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},{1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}},
   /*nxt*/ 0,
   /*prv*/0,
   /*conect*/{0}
};

struct Cubo {
	char ID[7] = { 'C','0','0','0','0','0','0' };
	
	vector <Matriz*> MatrizCompuesta;
	vector <Matriz*> Puntos;
	
	int R = 255;
	int G = 255;
	int B = 255;

	Cubo *nxt = 0;
	Cubo *prv = 0;
};

//     Raices 
Matriz *RaizMatrices;
Cubo *RaizFiguras;
HINSTANCE hInst;
COLORREF gColor = RGB(255, 255, 255);
COLORREF ShowColorDialog(HWND hWnd) {

	CHOOSECOLOR cc;
	static COLORREF crCustClr[16];

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWnd;
	cc.lpCustColors = (LPDWORD)crCustClr;
	cc.rgbResult = RGB(0, 255, 120);
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	ChooseColor(&cc);

	if (IsDlgButtonChecked(hWnd, IDC_RADIOCUBO) || IsDlgButtonChecked(hWnd, IDC_RADIOTETRA) || IsDlgButtonChecked(hWnd, IDC_RADIOPIRAMIDE) || IsDlgButtonChecked(hWnd, IDC_RADIODIAMANTE)) {
	EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
    }
	return cc.rgbResult;
}

//Contador Hexadecimal
void HexaDeciMas(char *ID,int I, bool Re) {

	switch (ID[I]) {
	case '0':
		if ((I == 5) || (Re == true)) {
			ID[I] = '1';
		}
		break;
	case '1':
		if ((I == 5) || (Re == true)) {
			ID[I] = '2';
		}
		break;
	case '2':
		if ((I == 5) || (Re == true)) {
			ID[I] = '3';
		}
		break;
	case '3':
		if ((I == 5) || (Re == true)) {
			ID[I] = '4';
		}
		break;
	case '4':
		if ((I == 5) || (Re == true)) {
			ID[I] = '5';
		}
		break;
	case '5':
		if ((I == 5) || (Re == true)) {
			ID[I] = '6';
		}
		break;
	case '6':
		if ((I == 5) || (Re == true)) {
			ID[I] = '7';
		}
		break;
	case '7':
		if ((I == 5) || (Re == true)) {
			ID[I] = '8';
		}
		break;
	case '8':
		if ((I == 5) || (Re == true)) {
			ID[I] = '9';
		}
		break;
	case '9':
		if ((I == 5) || (Re == true)) {
			ID[I] = 'A';
		}
		break;
	case 'A':
		if ((I == 5) || (Re == true)) {
			ID[I] = 'B';
		}
		break;
	case 'B':
		if ((I == 5) || (Re ==true)) {
			ID[I] = 'C';
		}
		break;
	case 'C':
		if ((I == 5) || (Re == true)) {
			ID[I] = 'D';
		}
		break;
	case 'D':
		if ((I == 5) || (Re == true)) {
			ID[I] = 'E';
		}
		break;
	case 'E':
		if ((I == 5) ||(Re == true)) {
			ID[I] = 'F';
		}
		break;
	case 'F':
		if ((I == 5) || (Re == true)) {
			if (I != 0) {
				ID[I] = '0';
				HexaDeciMas(ID, I - 1, true);
			}
			else {
				return;
			}
		}
		break;

	}
	return;
}
//Administacion de Nodos
//         Matrices
void Delete(Matriz **Raiz, Matriz *MatrElim) {
	if (MatrElim->nxt != 0) {
		MatrElim->nxt->prv = MatrElim->prv;
	}
	if (MatrElim->prv != 0) {
		MatrElim->prv->nxt = MatrElim->nxt;
	}
	else {
		(*Raiz) = MatrElim->nxt;
	}
	MatrElim->conect.clear();
	delete MatrElim;
}
void DelAll(Matriz *Raiz) {
	if (Raiz != 0) {
		Matriz *ante;
		ante = Raiz;
		Raiz = Raiz->nxt;
		while (Raiz != 0) {
			ante->conect.clear();
			delete ante;
			ante = Raiz;
			Raiz = Raiz->nxt;
		}
		ante->conect.clear();
		delete ante;
	}
}
void AddToList(Matriz *p, Matriz **Raiz) {
	//Recuerda que siempre que lo uses el validar que el caracter 0 de la cadena no sea 'Z', porque si lo es, ya llenaron el maximo de ID disponibles
	char ID[6] = { '0','0','0','0','0','0' };
	char IDante[6] = { '0','0','0','0','0','0' };
	char IDCAP[6] = { 'F','F','F','F','F','F' };
	char IDauxBUFFER[6] = { '0','0','0','0','0','0' };
	char IDanteBUFFER[6] = { '0','0','0','0','0','0' };
	

	Matriz *aux, *ante;
	aux = *Raiz;
	ante = aux;
	//Conectar los nodos
	if ((*Raiz) == 0) {
		(*Raiz) = p;
		p->nxt = 0;
		p->prv = 0;
		return;
	}

	for (; (aux->nxt != 0) || (aux->prv == 0); ante = aux, aux = aux->nxt) {
		for (int K = 0; K < 6; K++) {
			//Creamos los Buffers de las ID del nodo actual y el anterior para comparar
			IDauxBUFFER[K] = aux->ID[K+1];
			if (aux != (*Raiz)) {
				IDanteBUFFER[K] = ante->ID[K + 1];
			}
		}

		if ((strcmp(IDauxBUFFER, ID)==0)) {
			if (strcmp(IDanteBUFFER, IDante)!=0) {
				for (int I = 1; I < 7; I++) {
					p->ID[I] = ID[I - 1];
				}
				
				p->nxt = aux;
				p->prv = aux->prv;
				aux->prv = p;
				if (ante != aux) {
					ante->nxt = p;
					return;
				}
				(*Raiz) = p;
				return;
			}
		}
		else if ((aux->nxt == 0) && (aux->prv == 0)){
				aux->nxt = p;
				p->prv = aux;
				p->nxt = 0;
				for (int I = 0; I < 6; I++) {
					HexaDeciMas(ID, I, false);
				}
				for (int I = 1; I < 7; I++) {
					p->ID[I] = ID[I - 1];
				}
				return;

			}
		for (int I = 0; I < 6; I++) {

			HexaDeciMas(ID, I, false);
			if (aux != (*Raiz)) {
				HexaDeciMas(IDante, I, false);
			}
		}

	}
	if (strcmp(IDauxBUFFER, IDCAP)==0) {
		p->ID[0] = 'Z';
		aux->nxt = p;
		p->prv = aux;
		p->nxt = 0;
		return;
	}
	
		for (int I = 0; I < 6; I++) {
			HexaDeciMas(ID, I,false);
		}
		for (int I = 1; I < 7; I++) {
			p->ID[I] = ID[I - 1];
		}
		aux->nxt = p;
		p->prv = aux;
		p->nxt = 0;

};
Matriz *Search(Matriz **Raiz, char ID[7]) {
	Matriz *aux = (*Raiz);
	for (; aux != 0; aux = aux->nxt) {
		if (aux->ID == ID) {
			return aux;
		}
	}
	return 0;
}
//         Figuras
void AddToShapeList(Cubo *p, Cubo **Raiz) {
	//Recuerda que siempre que lo uses el validar que el caracter 0 de la cadena no sea 'Z', porque si lo es, ya llenaron el maximo de ID disponibles
	char ID[6] = { '0','0','0','0','0','0' };
	char IDante[6] = { '0','0','0','0','0','0' };
	char IDCAP[6] = { 'F','F','F','F','F','F' };
	char IDauxBUFFER[6] = { '0','0','0','0','0','0' };
	char IDanteBUFFER[6] = { '0','0','0','0','0','0' };


	Cubo *aux, *ante;
	aux = *Raiz;
	ante = aux;
	//Conectar los nodos
	if ((*Raiz) == 0) {
		(*Raiz) = p;
		p->nxt = 0;
		p->prv = 0;
		return;
	}

	for (; (aux->nxt != 0) || (aux->prv == 0); ante = aux, aux = aux->nxt) {
		for (int K = 0; K < 6; K++) {
			//Creamos los Buffers de las ID del nodo actual y el anterior para comparar
			IDauxBUFFER[K] = aux->ID[K + 1];
			if (aux != (*Raiz)) {
				IDanteBUFFER[K] = ante->ID[K + 1];
			}
		}

		if ((strcmp(IDauxBUFFER, ID) == 0)) {
			if (strcmp(IDanteBUFFER, IDante) != 0) {
				for (int I = 1; I < 7; I++) {
					p->ID[I] = ID[I - 1];
				}

				p->nxt = aux;
				p->prv = aux->prv;
				aux->prv = p;
				if (ante != aux) {
					ante->nxt = p;
					return;
				}
				(*Raiz) = p;
				return;
			}
		}
		else if ((aux->nxt == 0) && (aux->prv == 0)) {
			aux->nxt = p;
			p->prv = aux;
			p->nxt = 0;
			for (int I = 0; I < 6; I++) {
				HexaDeciMas(ID, I, false);
			}
			for (int I = 1; I < 7; I++) {
				p->ID[I] = ID[I - 1];
			}
			return;

		}
		for (int I = 0; I < 6; I++) {

			HexaDeciMas(ID, I, false);
			if (aux != (*Raiz)) {
				HexaDeciMas(IDante, I, false);
			}
		}

	}
	if (strcmp(IDauxBUFFER, IDCAP) == 0) {
		p->ID[0] = 'Z';
		aux->nxt = p;
		p->prv = aux;
		p->nxt = 0;
		return;
	}

	for (int I = 0; I < 6; I++) {
		HexaDeciMas(ID, I, false);
	}
	for (int I = 1; I < 7; I++) {
		p->ID[I] = ID[I - 1];
	}
	aux->nxt = p;
	p->prv = aux;
	p->nxt = 0;

};
void DeleteShape(Cubo **Raiz, Cubo *CuboAElim) {
	if (CuboAElim->nxt != 0) {
		CuboAElim->nxt->prv = CuboAElim->prv;
	}
	if (CuboAElim->prv != 0) {
		CuboAElim->prv->nxt = CuboAElim->nxt;
	}
	else {
		(*Raiz) = CuboAElim->nxt;
	}

	DelAll(CuboAElim->Puntos[0]);
	if (CuboAElim->MatrizCompuesta.size() > 0) {
		DelAll(CuboAElim->MatrizCompuesta[0]);
	}
	CuboAElim->MatrizCompuesta.clear();
	CuboAElim->Puntos.clear();
	delete CuboAElim;
}
void DelAllShapes(Cubo *Raiz) {
	if (Raiz != 0) {
		Cubo *ante;
		ante = Raiz;
		Raiz = Raiz->nxt;
		while (Raiz != 0) {
			if (ante->Puntos.size() != 0) {
				DelAll(ante->Puntos[0]);
			}
			if (ante->MatrizCompuesta.size() != 0) {
				DelAll(ante->MatrizCompuesta[0]);
			}
			ante->MatrizCompuesta.clear();
			ante->Puntos.clear();
			delete ante;
			ante = Raiz;
			Raiz = Raiz->nxt;
		}
		if (ante->Puntos.size() != 0) {
			DelAll(ante->Puntos[0]);
		}
		//No se elimina lo de la matriz compuesta porque ya fue eliminado debido a que los punteros de las matrices en el vector apuntan a matrices que se encuentran atadas a la cadena de nodos principal de las matrices y se ven eliminadas en el DelAll(&RaizMatrices)
		ante->MatrizCompuesta.clear();
		ante->Puntos.clear();
		delete ante;
	}
}


//Calculos
Matriz *MultiMatrices(Matriz **Raiz, Matriz *mat1 , Matriz *mat2 ) {
	if (mat1->Columnas != mat2->Filas) {
		return 0;
	}
	Matriz *resul = new Matriz;
	resul->Filas = mat1->Filas;
	resul->Columnas = mat2->Columnas;
	AddToList(resul, Raiz);

	for (int J = 0; J < resul->Columnas+1; J++) {
		for (int I = 0; I < resul->Filas+1; I++) {
			for (int K = 0; K < mat1->Columnas+1; K++) {
				resul->Matrix[I][J] += mat1->Matrix[I][K] * mat2->Matrix[K][J];
			};
		};
	};
	
	return resul;

}

Matriz *SumResMatrices(Matriz **Raiz, Matriz *mat1, Matriz *mat2 , bool Oper) {
	//Si Oper es TRUE entonces es una SUMA, si es FALSE es una RESTA
	if (mat1->Columnas != mat2->Columnas) {
		return 0;
	}
	if (mat1->Filas != mat2->Filas) {
		return 0;
	}
	Matriz *resul = new Matriz;
	resul->Filas = mat1->Filas;
	resul->Columnas = mat2->Columnas;
	AddToList(resul, Raiz);

	for (int J = 0; J < resul->Columnas+1; J++) {
		for (int I = 0; I < resul->Filas+1; I++) {
			if (Oper == true) {
				resul->Matrix[I][J] = mat1->Matrix[I][J] + mat2->Matrix[I][J];
			}
			else {
				resul->Matrix[I][J] = mat1->Matrix[I][J] - mat2->Matrix[I][J];
			}
		};
	};

	return resul;

}

Matriz *IgualdadMatriz(Matriz **Raiz,int Columnas, int Filas) {
	Matriz *resul = new Matriz;
	resul->Filas = Filas;
	resul->Columnas = Columnas;
	AddToList(resul, Raiz);

	for (int J = 0; J < resul->Columnas+1; J++) {
		for (int I = 0; I < resul->Filas+1; I++) {
			if(J==I){
				resul->Matrix[I][J] = 1;
			}
		};
	};

	return resul;

}

float Determ(Matriz *mat) {

	float det = 0;

	if (mat->Columnas != mat->Filas) {
		return ERROR;
	}
	if (mat->Filas == 0) {
		return mat->Matrix[0][0];
	}
	else if (mat->Filas == 1) {
		det = (mat->Matrix[0][0] * mat->Matrix[1][1] - mat->Matrix[0][1] * mat->Matrix[1][0]);
		return det;
	}
	else {
		Matriz *temp = new Matriz;
		temp->Columnas = mat->Columnas - 1;
		temp->Filas = mat->Filas - 1;
		for (int h,k,p = 0; p < mat->Filas+1; p++) {
			h = 0;
			k = 0;
			for (int j = 1; j < mat->Columnas+1; j++) {
				for (int i = 0; i < mat->Filas+1;i++) {
					if (i == p) {
						continue;
					}
					temp->Matrix[k][h] = mat->Matrix[i][j];
					k++;
					if (k > temp->Filas) {
						h++;
						k = 0;
					}
				}
			}

			det = (float) (det + mat->Matrix[0][p] * pow(-1, p)*Determ(temp));
		}
		delete temp;
		return det;
	}
}

Matriz *MatrizAdj(Matriz **Raiz, Matriz *mat) {
	if (mat->Columnas != mat->Filas) {
		return 0;
	}
	Matriz *resul = new Matriz;
	resul->Columnas = mat->Columnas;
	resul->Filas = mat->Filas;
	AddToList(resul, Raiz);

	

	for (int J = 0; J < mat->Columnas+1; J++) {
		for (int I = 0,h,k; I < mat->Filas+1; I++) {

			Matriz *temp = new Matriz;
			temp->Columnas = mat->Columnas - 1;
			temp->Filas = mat->Filas - 1;

			h = 0;
			k = 0;
			for (int j = 0; j < mat->Columnas + 1; j++) {
				if (j == J) {
					continue;
				}
				for (int i = 0; i < mat->Filas + 1; i++) {
					if (i == I) {
						continue;
					}
					temp->Matrix[k][h] = mat->Matrix[i][j];
					k++;
					if (k > temp->Filas) {
						h++;
						k = 0;
					}
				}
			}

			resul->Matrix[I][J] =(float) ( pow(-1, J+I)*Determ(temp));

			delete temp;
		}
	}

	
	return resul;
};

Matriz *Transpuesta(Matriz **Raiz , Matriz *mat) {
	Matriz *resul = new Matriz;
	resul->Columnas = mat->Filas;
	resul->Filas = mat->Columnas;
	AddToList(resul, Raiz);

	for (int J = 0; J < mat->Columnas+1; J++) {
		for (int I = 0; I < mat->Filas+1; I++) {
			resul->Matrix[J][I] = mat->Matrix[I][J];
		}
	}
	return resul;
};

Matriz *MultMatrXEsc(Matriz **Raiz, Matriz *mat, float Esc) {
	Matriz *resul = new Matriz;
	resul->Columnas = mat->Columnas;
	resul->Filas = mat->Filas;
	AddToList(resul, Raiz);

	for (int J = 0; J < mat->Columnas+1; J++) {
		for (int I = 0; I < mat->Filas+1; I++) {
			resul->Matrix[I][J] = mat->Matrix[I][J] * Esc;
		}
	}

	return resul;
}

Matriz *Inversa(Matriz **Raiz, Matriz *mat) {
	if (mat->Columnas != mat->Filas) {
		return 0;
	}
	if (Determ(mat) == 0) {
		return 0;
	}
	Matriz *resul = new Matriz;
	

	
	Matriz *Adje = MatrizAdj(Raiz, mat);
	Matriz *Trans = Transpuesta(Raiz, Adje);
	resul = MultMatrXEsc(Raiz, Trans, (1 / Determ(mat)) );

	Delete(Raiz, Trans);
	Delete(Raiz, Adje);
	//resul = MultMatrXEsc(Raiz,MatrizAdj(Raiz,Transpuesta(Raiz,mat)),(1/Determ(mat)));

	return resul;
}


//Matrices Graficas
Matriz *Traslacion(Matriz **Raiz, float X, float Y, float Z)
{
	Matriz *resul = IgualdadMatriz(Raiz, 3, 3);
	resul->Matrix[0][3] = X;
	resul->Matrix[1][3] = Y;
	resul->Matrix[2][3] = Z;

	return resul;
}

Matriz *RotaX(Matriz **Raiz, float angulo)
{
	float Cos = (float)(cos(angulo * 3.141592 / 180.0));
	float Sen = (float)(sin(angulo * 3.141592 / 180.0));
	Matriz *resul = IgualdadMatriz(Raiz,3, 3);
	
	resul->Matrix[1][1] = Cos;
	resul->Matrix[2][2] = Cos;
	resul->Matrix[2][1] = Sen * (-1);
	resul->Matrix[1][2] = Sen;

	return resul;
}

Matriz *RotaY(Matriz **Raiz, float angulo)
{
	float Cos = (float)(cos(angulo * 3.141592 / 180.0));
	float Sen = (float)(sin(angulo * 3.141592 / 180.0));
	Matriz *resul = IgualdadMatriz(Raiz, 3, 3);

	resul->Matrix[0][0] = Cos;
	resul->Matrix[2][2] = Cos;
	resul->Matrix[2][0] = Sen;
	resul->Matrix[0][2] = Sen * (-1);

	return resul;
}

Matriz *RotaZ(Matriz **Raiz, float angulo)
{
	float Cos = (float)(cos(angulo * 3.141592 / 180.0));
	float Sen = (float)(sin(angulo * 3.141592 / 180.0));
	Matriz *resul = IgualdadMatriz(Raiz, 3, 3);

	resul->Matrix[0][0] = Cos;
	resul->Matrix[1][1] = Cos;
	resul->Matrix[1][0] = Sen * (-1);
	resul->Matrix[0][1] = Sen;


	return resul;
}

Matriz *Escalar(Matriz **Raiz, float X, float Y, float Z) {
	Matriz *resul = IgualdadMatriz(Raiz, 3, 3);

	resul->Matrix[0][0] = X;
	resul->Matrix[1][1] = Y;
	resul->Matrix[2][2] = Z;

	return resul;
}

Matriz *Cuater(Matriz **Raiz,Matriz *Vector, float angulo) {
	if (Vector->Filas != 3 || Vector->Columnas != 0) {
		return 0;
	}
	float s = (float)(cos((angulo / 2) * 3.141592 / 180.0));
	float Sen = (float)(sin((angulo / 2) * 3.141592 / 180.0));
	float Magni = sqrt((Vector->Matrix[0][0]* Vector->Matrix[0][0]+ Vector->Matrix[1][0] * Vector->Matrix[1][0] + Vector->Matrix[2][0] * Vector->Matrix[2][0] ));
	float a = Vector->Matrix[0][0] / Magni * Sen;
	float b = Vector->Matrix[1][0] / Magni * Sen;
	float c = Vector->Matrix[2][0] / Magni * Sen;
    
	Matriz *resul = new Matriz;
	resul->Columnas = 3;
	resul->Filas = 3;
	AddToList(resul,Raiz);
	resul->Matrix[0][0] = 1 - 2 * b*b - 2 * c*c;
	resul->Matrix[1][0] = 2*a*b+2*s*c;
	resul->Matrix[2][0] = 2*a*c-2*s*b;
	//resul->Matrix[3][0] = ;
	resul->Matrix[0][1] = 2*a*b-2*s*c;
	resul->Matrix[1][1] = 1-2*a*a-2*c*c;
	resul->Matrix[2][1] = 2*b*c+2*s*a;
	//resul->Matrix[3][1] = ;
	resul->Matrix[0][2] = 2*a*c+2*s*b;
	resul->Matrix[1][2] = 2*b*c-2*s*a;
	resul->Matrix[2][2] = 1-2*a*a-2*b*b;
	/*resul->Matrix[3][2] = ;
	resul->Matrix[0][3] = ;
	resul->Matrix[1][3] = ;
	resul->Matrix[2][3] = ;*/
	resul->Matrix[3][3] = 1;

	return resul;
}




//Hacer Figuras
Cubo *HacerCubo(Cubo **Raiz) {
	Cubo *Figura = new Cubo;

	//Genera el vector lleno de las matrices para puntos y su cadena de nodos interna
	for(int I=0;I<8;I++){
	Figura->Puntos.push_back(new Matriz);
	Figura->Puntos[I]->Filas = 3;
	Figura->Puntos[I]->Columnas = 0;
	Figura->Puntos[I]->ID[0] = 'P';
	Figura->Puntos[I]->Matrix[3][0] = 1;
	AddToList(Figura->Puntos[I],&Figura->Puntos[0]);
	}
	//Punto 1
	Figura->Puntos[0]->Matrix[0][0] = 40;
	Figura->Puntos[0]->Matrix[1][0] = 40;
	Figura->Puntos[0]->Matrix[2][0] = 40;
	Figura->Puntos[0]->conect.push_back(1);
	Figura->Puntos[0]->conect.push_back(2);
	Figura->Puntos[0]->conect.push_back(3);
	//Punto 2
	Figura->Puntos[1]->Matrix[0][0] = 40;
	Figura->Puntos[1]->Matrix[1][0] = -40;
	Figura->Puntos[1]->Matrix[2][0] = 40;
	Figura->Puntos[1]->conect.push_back(0);
	Figura->Puntos[1]->conect.push_back(4);
	Figura->Puntos[1]->conect.push_back(5);
	//Punto 3
	Figura->Puntos[2]->Matrix[0][0] = 40;
	Figura->Puntos[2]->Matrix[1][0] = 40;
	Figura->Puntos[2]->Matrix[2][0] = -40;
	Figura->Puntos[2]->conect.push_back(0);
	Figura->Puntos[2]->conect.push_back(5);
	Figura->Puntos[2]->conect.push_back(6);
	//Punto 4
	Figura->Puntos[3]->Matrix[0][0] = -40;
	Figura->Puntos[3]->Matrix[1][0] = 40;
	Figura->Puntos[3]->Matrix[2][0] = 40;
	Figura->Puntos[3]->conect.push_back(0);
	Figura->Puntos[3]->conect.push_back(4);
	Figura->Puntos[3]->conect.push_back(6);
	//Punto 5
	Figura->Puntos[4]->Matrix[0][0] = -40;
	Figura->Puntos[4]->Matrix[1][0] = -40;
	Figura->Puntos[4]->Matrix[2][0] = 40;
	Figura->Puntos[4]->conect.push_back(1);
	Figura->Puntos[4]->conect.push_back(3);
	Figura->Puntos[4]->conect.push_back(7);
	//Punto 6
	Figura->Puntos[5]->Matrix[0][0] = 40;
	Figura->Puntos[5]->Matrix[1][0] = -40;
	Figura->Puntos[5]->Matrix[2][0] = -40;
	Figura->Puntos[5]->conect.push_back(1);
	Figura->Puntos[5]->conect.push_back(2);
	Figura->Puntos[5]->conect.push_back(7);
	//Punto 7
	Figura->Puntos[6]->Matrix[0][0] = -40;
	Figura->Puntos[6]->Matrix[1][0] = 40;
	Figura->Puntos[6]->Matrix[2][0] = -40;
	Figura->Puntos[6]->conect.push_back(3);
	Figura->Puntos[6]->conect.push_back(7);
	Figura->Puntos[6]->conect.push_back(2);
	//Punto 8
	Figura->Puntos[7]->Matrix[0][0] = -40;
	Figura->Puntos[7]->Matrix[1][0] = -40;
	Figura->Puntos[7]->Matrix[2][0] = -40;
	Figura->Puntos[7]->conect.push_back(6);
	Figura->Puntos[7]->conect.push_back(5);
	Figura->Puntos[7]->conect.push_back(4);

	AddToShapeList(Figura, &RaizFiguras);

	return Figura;
};
Cubo *HacerPiramide(Cubo **Raiz) {
	Cubo *Figura = new Cubo;

	//Genera el vector lleno de las matrices para puntos y su cadena de nodos interna
	for (int I = 0; I < 5; I++) {
		Figura->Puntos.push_back(new Matriz);
		Figura->Puntos[I]->Filas = 3;
		Figura->Puntos[I]->Columnas = 0;
		Figura->Puntos[I]->ID[0] = 'P';
		Figura->Puntos[I]->Matrix[3][0] = 1;
		AddToList(Figura->Puntos[I], &Figura->Puntos[0]);
	}
	//Punto 1
	Figura->Puntos[0]->Matrix[0][0] = 0;
	Figura->Puntos[0]->Matrix[1][0] = 0;
	Figura->Puntos[0]->Matrix[2][0] = 40;
	Figura->Puntos[0]->conect.push_back(1);
	Figura->Puntos[0]->conect.push_back(2);
	Figura->Puntos[0]->conect.push_back(3);
	Figura->Puntos[0]->conect.push_back(4);
	//Punto 2
	Figura->Puntos[1]->Matrix[0][0] = 40;
	Figura->Puntos[1]->Matrix[1][0] = 40;
	Figura->Puntos[1]->Matrix[2][0] = -40;
	Figura->Puntos[1]->conect.push_back(0);
	Figura->Puntos[1]->conect.push_back(2);
	Figura->Puntos[1]->conect.push_back(4);
	//Punto 3
	Figura->Puntos[2]->Matrix[0][0] = 40;
	Figura->Puntos[2]->Matrix[1][0] = -40;
	Figura->Puntos[2]->Matrix[2][0] = -40;
	Figura->Puntos[2]->conect.push_back(0);
	Figura->Puntos[2]->conect.push_back(3);
	Figura->Puntos[2]->conect.push_back(1);
	//Punto 4
	Figura->Puntos[3]->Matrix[0][0] = -40;
	Figura->Puntos[3]->Matrix[1][0] = -40;
	Figura->Puntos[3]->Matrix[2][0] = -40;
	Figura->Puntos[3]->conect.push_back(0);
	Figura->Puntos[3]->conect.push_back(4);
	Figura->Puntos[3]->conect.push_back(2);
	//Punto 5
	Figura->Puntos[4]->Matrix[0][0] = -40;
	Figura->Puntos[4]->Matrix[1][0] = 40;
	Figura->Puntos[4]->Matrix[2][0] = -40;
	Figura->Puntos[4]->conect.push_back(0);
	Figura->Puntos[4]->conect.push_back(1);
	Figura->Puntos[4]->conect.push_back(3);

	AddToShapeList(Figura, &RaizFiguras);

	return Figura;
};
Cubo *HacerTetra(Cubo **Raiz) {
	Cubo *Figura = new Cubo;

	//Genera el vector lleno de las matrices para puntos y su cadena de nodos interna
	for (int I = 0; I < 4; I++) {
		Figura->Puntos.push_back(new Matriz);
		Figura->Puntos[I]->Filas = 3;
		Figura->Puntos[I]->Columnas = 0;
		Figura->Puntos[I]->ID[0] = 'P';
		Figura->Puntos[I]->Matrix[3][0] = 1;
		AddToList(Figura->Puntos[I], &Figura->Puntos[0]);
	}
	//Punto 1
	Figura->Puntos[0]->Matrix[0][0] = 40;
	Figura->Puntos[0]->Matrix[1][0] = 40;
	Figura->Puntos[0]->Matrix[2][0] = 40;
	Figura->Puntos[0]->conect.push_back(1);
	Figura->Puntos[0]->conect.push_back(2);
	Figura->Puntos[0]->conect.push_back(3);
	//Punto 2
	Figura->Puntos[1]->Matrix[0][0] = -40;
	Figura->Puntos[1]->Matrix[1][0] = -40;
	Figura->Puntos[1]->Matrix[2][0] = 40;
	Figura->Puntos[1]->conect.push_back(0);
	Figura->Puntos[1]->conect.push_back(2);
	Figura->Puntos[1]->conect.push_back(3);
	//Punto 3
	Figura->Puntos[2]->Matrix[0][0] = 40;
	Figura->Puntos[2]->Matrix[1][0] = -40;
	Figura->Puntos[2]->Matrix[2][0] = -40;
	Figura->Puntos[2]->conect.push_back(1);
	Figura->Puntos[2]->conect.push_back(0);
	Figura->Puntos[2]->conect.push_back(3);
	//Punto 4
	Figura->Puntos[3]->Matrix[0][0] = -40;
	Figura->Puntos[3]->Matrix[1][0] = 40;
	Figura->Puntos[3]->Matrix[2][0] = -40;
	Figura->Puntos[3]->conect.push_back(1);
	Figura->Puntos[3]->conect.push_back(2);
	Figura->Puntos[3]->conect.push_back(0);

	AddToShapeList(Figura, &RaizFiguras);

	return Figura;

};
Cubo *HacerDiamante(Cubo **Raiz) {
	Cubo *Figura = new Cubo;

	//Genera el vector lleno de las matrices para puntos y su cadena de nodos interna
	for (int I = 0; I < 6; I++) {
		Figura->Puntos.push_back(new Matriz);
		Figura->Puntos[I]->Filas = 3;
		Figura->Puntos[I]->Columnas = 0;
		Figura->Puntos[I]->ID[0] = 'P';
		Figura->Puntos[I]->Matrix[3][0] = 1;
		AddToList(Figura->Puntos[I], &Figura->Puntos[0]);
	}
	//Punto 1
	Figura->Puntos[0]->Matrix[0][0] = 0;
	Figura->Puntos[0]->Matrix[1][0] = 0;
	Figura->Puntos[0]->Matrix[2][0] = 40;
	Figura->Puntos[0]->conect.push_back(1);
	Figura->Puntos[0]->conect.push_back(2);
	Figura->Puntos[0]->conect.push_back(3);
	Figura->Puntos[0]->conect.push_back(4);
	//Punto 2
	Figura->Puntos[1]->Matrix[0][0] = 40;
	Figura->Puntos[1]->Matrix[1][0] = 40;
	Figura->Puntos[1]->Matrix[2][0] = -40;
	Figura->Puntos[1]->conect.push_back(0);
	Figura->Puntos[1]->conect.push_back(2);
	Figura->Puntos[1]->conect.push_back(4);
	Figura->Puntos[1]->conect.push_back(5);
	//Punto 3
	Figura->Puntos[2]->Matrix[0][0] = 40;
	Figura->Puntos[2]->Matrix[1][0] = -40;
	Figura->Puntos[2]->Matrix[2][0] = -40;
	Figura->Puntos[2]->conect.push_back(0);
	Figura->Puntos[2]->conect.push_back(3);
	Figura->Puntos[2]->conect.push_back(1);
	Figura->Puntos[2]->conect.push_back(5);
	//Punto 4
	Figura->Puntos[3]->Matrix[0][0] = -40;
	Figura->Puntos[3]->Matrix[1][0] = -40;
	Figura->Puntos[3]->Matrix[2][0] = -40;
	Figura->Puntos[3]->conect.push_back(0);
	Figura->Puntos[3]->conect.push_back(4);
	Figura->Puntos[3]->conect.push_back(2);
	Figura->Puntos[3]->conect.push_back(5);
	//Punto 5
	Figura->Puntos[4]->Matrix[0][0] = -40;
	Figura->Puntos[4]->Matrix[1][0] = 40;
	Figura->Puntos[4]->Matrix[2][0] = -40;
	Figura->Puntos[4]->conect.push_back(0);
	Figura->Puntos[4]->conect.push_back(1);
	Figura->Puntos[4]->conect.push_back(3);
	Figura->Puntos[4]->conect.push_back(5);
	//Punto 6
	Figura->Puntos[5]->Matrix[0][0] = 0;
	Figura->Puntos[5]->Matrix[1][0] = 0;
	Figura->Puntos[5]->Matrix[2][0] = -80;
	Figura->Puntos[5]->conect.push_back(1);
	Figura->Puntos[5]->conect.push_back(2);
	Figura->Puntos[5]->conect.push_back(3);
	Figura->Puntos[5]->conect.push_back(4);

	AddToShapeList(Figura, &RaizFiguras);

	return Figura;
};
//Cubo *HacerIcosa(Cubo **Raiz) {
//	Cubo *Figura = new Cubo;
//
//	//Genera el vector lleno de las matrices para puntos y su cadena de nodos interna
//	for (int I = 0; I < 12; I++) {
//		Figura->Puntos.push_back(new Matriz);
//		Figura->Puntos[I]->Filas = 3;
//		Figura->Puntos[I]->Columnas = 0;
//		Figura->Puntos[I]->ID[0] = 'P';
//		Figura->Puntos[I]->Matrix[3][0] = '1';
//		AddToList(Figura->Puntos[I], &Figura->Puntos[0]);
//	}
//
//}


Cubo *Transforma(Cubo **Raiz, Cubo *Figura, float angulo){

	Matriz *AuxRaiz;
	AuxRaiz = 0;
	Matriz *MC;
	MC = IgualdadMatriz(&AuxRaiz, 3, 3);

	for (int I = 0; I < Figura->MatrizCompuesta.size(); I++) {
		if(Figura->MatrizCompuesta[I]->Columnas==0){
		MC = MultiMatrices(&AuxRaiz, MC, Cuater(&AuxRaiz, Figura->MatrizCompuesta[I] , angulo));
		}
		else {
			if ((Figura->MatrizCompuesta[I]->Matrix[2][1] != 0) && (Figura->MatrizCompuesta[I]->Matrix[1][2] != 0)) {

				MC = MultiMatrices(&AuxRaiz, MC, RotaX(&AuxRaiz, angulo));

			}
			else if ((Figura->MatrizCompuesta[I]->Matrix[2][0] != 0) && (Figura->MatrizCompuesta[I]->Matrix[0][2] != 0)) {

				MC = MultiMatrices(&AuxRaiz, MC, RotaY(&AuxRaiz, angulo));
			}
			else if ((Figura->MatrizCompuesta[I]->Matrix[1][0] != 0) && (Figura->MatrizCompuesta[I]->Matrix[0][1] != 0)) {

				MC = MultiMatrices(&AuxRaiz, MC, RotaZ(&AuxRaiz, angulo));
			}
			else {
				MC = MultiMatrices(&AuxRaiz, MC, Figura->MatrizCompuesta[I]);
			}
		}

	}

	Cubo *FiguraMovida = new Cubo;
	FiguraMovida->R = Figura->R;
	FiguraMovida->G = Figura->G;
	FiguraMovida->B = Figura->B;
	AddToShapeList(FiguraMovida, Raiz);
	FiguraMovida->Puntos.push_back(0);
	for (int I = 0; I < Figura->Puntos.size(); I++) {
		if (I == 0) {
			/*delete FiguraMovida->Puntos[0];
			FiguraMovida->Puntos[0] = 0;*/
			FiguraMovida->Puntos[0] = MultiMatrices(&FiguraMovida->Puntos[0], MC, Figura->Puntos[I]);
			//FiguraMovida->Puntos[I] = *AuxPnto;
			for (int K = 0; K < Figura->Puntos[I]->conect.size(); K++) {
				FiguraMovida->Puntos[I]->conect.push_back(Figura->Puntos[I]->conect[K]);
			}
			continue;
		}
		Matriz *AuxPnto;
		AuxPnto = MultiMatrices(&FiguraMovida->Puntos[0], MC, Figura->Puntos[I]);
		FiguraMovida->Puntos.push_back(AuxPnto);
		for (int K = 0; K < Figura->Puntos[I]->conect.size(); K++) {
			FiguraMovida->Puntos[I]->conect.push_back(Figura->Puntos[I]->conect[K]);
		}
	}

	DelAll(AuxRaiz);


	return FiguraMovida;
}

void Dibuja(HDC hdc,Cubo *Figura, float Zprp , int Height, int Width/*, bool erase*/)
{
	HPEN hPen;
	/*if (erase) {
		SelectObject(hdc, GetStockObject(WHITE_PEN));
		SetDCPenColor(hdc, RGB(0, 0, 255));
	}
	else {*/
	//}
	hPen = CreatePen(PS_SOLID, 1, RGB(Figura->R, Figura->G, Figura->B));
	SelectObject(hdc, hPen);
	float X4center = Width/2; // estas variables son usadas para hacer los calculos desde el centro de la pantalla
	float Y4center = Height/2; 

	float Xpresp; 
	float Ypersp; 

	for (int I = 0; I < Figura->Puntos.size(); I++){
		Xpresp = Figura->Puntos[I]->Matrix[0][0] * (Zprp / (Zprp - Figura->Puntos[I]->Matrix[2][0]));
		Ypersp = Figura->Puntos[I]->Matrix[1][0] * (Zprp / (Zprp - Figura->Puntos[I]->Matrix[2][0]));
		MoveToEx(hdc, (int)(Xpresp + X4center), (int)(Y4center - Ypersp), NULL);
		float XprespAux = NULL;
		float YperspAux = NULL;
		for (int J = 0; J < Figura->Puntos[I]->conect.size() ; J++) {
			MoveToEx(hdc, (int)(Xpresp + X4center), (int)(Y4center - Ypersp), NULL);
			XprespAux = Figura->Puntos[Figura->Puntos[I]->conect[J]]->Matrix[0][0] * (Zprp / (Zprp - Figura->Puntos[Figura->Puntos[I]->conect[J]]->Matrix[2][0]));
			YperspAux = Figura->Puntos[Figura->Puntos[I]->conect[J]]->Matrix[1][0] * (Zprp / (Zprp - Figura->Puntos[Figura->Puntos[I]->conect[J]]->Matrix[2][0]));
			LineTo(hdc, (int)(XprespAux + X4center), (int)(Y4center - YperspAux));
		}
		//LineTo(hdc, (int)(Xpresp + X4center), (int)(Y4center - Ypersp));
	}
	DeleteObject(hPen);
}

struct ButtonList {
	HWND hwnd;
	Matriz *Matriz = 0;
	Cubo *Figura = 0;
	bool Graficado = false;
	int IDC = 0;
	int Pos = 10;
};

void CrearBoton(ButtonList *Boton,int Xpos,int Ypos, int bSize, HWND Padre) {

	Boton->hwnd = CreateWindow(
		"BUTTON",  // Predefined class; Unicode assumed 
		"",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_MULTILINE,  // Styles 
		Xpos,         // x position 
		Ypos,         // y position 
		bSize,        // Button width
		bSize,        // Button height
		Padre,     // Parent window
		(HMENU)Boton->IDC,       // No menu.
		hInst,
		NULL);      // Pointer not needed.
	if (Boton->Matriz != 0) {
		char BUFF[2500] = { NULL };
		char MatrixBuff[10][10][26] = { NULL };
		for (int K = 0; K < Boton->Matriz->Filas + 1; K++) {
			for (int H = 0; H < Boton->Matriz->Columnas + 1; H++) {
				char Fbuff[25] = { NULL };
				sprintf_s(Fbuff, "%f", Boton->Matriz->Matrix[K][H]);
				for (int L = strlen(Fbuff) - 1; L >= 0; L--) {
					if (Fbuff[L] != '0') {
						if (Fbuff[L] == '.') {
							Fbuff[L] = NULL;
						}
						break;
					}
					else {
						Fbuff[L] = NULL;
					}
				}
				strncat_s(MatrixBuff[K][H], Fbuff, sizeof(MatrixBuff[K][H]));
			}
		}
		char Space[9][25] = { "" };
		for (int I = 0; I < Boton->Matriz->Columnas + 1; I++) {
			int SizeBuffer = 0;
			int FinalSize = 0;
			for (int J = 0; J < Boton->Matriz->Filas + 1; J++) {
				SizeBuffer = strlen(MatrixBuff[J][I]);
				if (SizeBuffer > FinalSize) {
					FinalSize = SizeBuffer;
				}
			}
			for (int J = 0; J < FinalSize + 2; J++) {
				strcat_s(Space[I], " ");
				if (J % 3 == 0) {
					strcat_s(Space[I], " ");
				}
			}
		}
		for (int K = 0; K < Boton->Matriz->Filas + 1; K++) {
			for (int H = 0; H < Boton->Matriz->Columnas + 1; H++) {
				char SpaceBuff[25] = { NULL };
				strcat_s(SpaceBuff, Space[H]);
				for (int I = 0; I < strlen(MatrixBuff[K][H]); I++) {
					Space[H][I] = MatrixBuff[K][H][I];
				}
				strcat_s(BUFF, Space[H]);
				for (int I = 0; I < strlen(Space[K]); I++) {
					Space[H][I] = SpaceBuff[I];
				}
			}
			strcat_s(BUFF, "\n");
		}
		HFONT hFont = CreateFontA(10, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
		SendMessage(Boton->hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
		SetWindowText(Boton->hwnd, BUFF);
	}
	else {
		SetWindowText(Boton->hwnd, Boton->Figura->ID);
	}
}

HWND GraphWindow;
HWND HistorialWindow;
HWND OperationWindow;
HWND CreatorWindow;
HWND ChoiceWindow;
HWND SelectWindow;
HWND CalculatorWindow;



//Ventanas hijas de la calculadora
BOOL CALLBACK CreatorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int LastButtonPos[2] = { 10,10 };
int LastButtonHisto = 0;
int LastButtonIDCHisto = 6480;
int buttonSize = 120;
int Scroll;
HWND ADD;
vector <ButtonList> HistorialButton;

ButtonList NuevoBotonChoice;
ButtonList NuevoBotonSelect;
bool EsperandoChoice = false;
bool GraphMode = false;
bool EsperandoSelect = false;
bool DetOrEsc = false;
bool Atar = false;
vector <ButtonList> OperationButton;
int LastButtonIDCOper = 9777;
int LastButtonOper = 0;
int LastButtonPosOper = 10;
bool EsperandoEstructura = true;
bool ColorSelected = false;
SCROLLINFO BarraHisto;
SCROLLINFO BarraOper;
unsigned char Red = 255;
unsigned char Green = 255;
unsigned char Blue = 255;

BOOL CALLBACK SelectProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// checa los mensajes
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (!Atar){
				if (DetOrEsc) {
					//Determinante
					float Aux = 0.0;
					char BUFF[50];
					Aux = Determ(NuevoBotonSelect.Matriz);
					if (Aux == 0) {
						MessageBoxA(hWnd, "No tiene determinante", "Determinante", MB_OK | MB_ICONSTOP);
						break;
					}
					sprintf_s(BUFF, "%f", Aux);
					MessageBoxA(hWnd, BUFF, "Determinante", MB_OK);
					for (int K = 0; K < HistorialButton.size(); K++) {
						if (HistorialButton[K].Figura != 0) {
							EnableWindow(HistorialButton[K].hwnd, TRUE);
						}
						if (HistorialButton[K].Matriz != 0) {
							if (!GraphMode) {
								if (HistorialButton[K].Matriz->Columnas != HistorialButton[K].Matriz->Filas) {
									EnableWindow(HistorialButton[K].hwnd, TRUE);
								}
							}
							else {
								if (HistorialButton[K].Matriz->Filas==3) {
									EnableWindow(HistorialButton[K].hwnd, TRUE);
								}
							}
						}
					}

				}
				else {
					//Escalar
					Matriz *Aux;
					char BUFF[50];
					GetWindowText(GetDlgItem(CalculatorWindow, IDC_EDITESC), BUFF, sizeof(BUFF));
					Aux = MultMatrXEsc(&RaizMatrices, NuevoBotonSelect.Matriz, atof(BUFF));
					delete NuevoBotonSelect.Matriz;
					NuevoBotonSelect.Matriz = Aux;
					HistorialButton.push_back(NuevoBotonSelect);
					HistorialButton[LastButtonHisto].IDC = LastButtonIDCHisto;
					HistorialButton[LastButtonHisto].Pos = LastButtonPos[1];
					CrearBoton(&HistorialButton[LastButtonHisto],LastButtonPos[0],LastButtonPos[1],buttonSize,HistorialWindow);
					LastButtonIDCHisto++;
					LastButtonPos[1] += buttonSize + 20;
					SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1], 0, 0, SWP_NOSIZE);
					LastButtonHisto++;
					MessageBoxA(hWnd, "Se generó la Matriz escalada exitosamente", "Matriz multiplicada por Escalar", MB_OK);
					for (int K = 0; K < HistorialButton.size(); K++) {
						if (HistorialButton[K].Figura != 0) {
							EnableWindow(HistorialButton[K].hwnd, TRUE);
						}
					}

				}
			}
			else{
				if(NuevoBotonSelect.Figura->MatrizCompuesta.size()>0){
					int MsBoxID = MessageBoxA(hWnd, "Esta figura ya contiene una matriz compuesta: ¿Desea sobreescribirla? (El seleccionar NO va a concatenar ambas matrices siendo la ultima)", "", MB_YESNO | MB_ICONQUESTION);
					switch (MsBoxID) {
					case IDNO:
						break;
					case IDYES:
						for (int K = NuevoBotonSelect.Figura->MatrizCompuesta.size()-1; K >= 0 ; K--) {
							NuevoBotonSelect.Figura->MatrizCompuesta.erase(NuevoBotonSelect.Figura->MatrizCompuesta.begin()+K);
						}
						break;
					}
				}
				for (int K = 0; K < OperationButton.size(); K+=2) {
					NuevoBotonSelect.Figura->MatrizCompuesta.push_back(OperationButton[K].Matriz);
				}
				Atar = false;
				for (int K = 0; K < HistorialButton.size(); K++) {
					if (HistorialButton[K].Matriz != 0) {
						if (HistorialButton[K].Matriz->Filas == 3) {
							EnableWindow(HistorialButton[K].hwnd, TRUE);
						}
					}
				}
				MessageBoxA(hWnd, "Se ató la Matriz Compuesta a la Figura exitosamente", "Matriz Compuesta", MB_OK);
			}
			DestroyWindow(hWnd);
			return 1;
			break;
		case IDCLOSE:
			for (int K = 0; K < HistorialButton.size(); K++) {
				if (HistorialButton[K].Figura != 0) {
					EnableWindow(HistorialButton[K].hwnd, TRUE);
				}
				if (HistorialButton[K].Matriz != 0) {
					if (!GraphMode) {
						if (HistorialButton[K].Matriz->Columnas != HistorialButton[K].Matriz->Filas) {
							EnableWindow(HistorialButton[K].hwnd, TRUE);
						}
					}
					else {
						if (HistorialButton[K].Matriz->Filas == 3) {
							EnableWindow(HistorialButton[K].hwnd, TRUE);
						}
					}
				}
			}
			DestroyWindow(hWnd);
			return 0;
			break;
		case IDCANCEL:
			for (int K = 0; K < HistorialButton.size(); K++) {
				if (HistorialButton[K].Figura != 0) {
					EnableWindow(HistorialButton[K].hwnd, TRUE);
				}
				if (HistorialButton[K].Matriz != 0) {
					if (!GraphMode) {
						if (HistorialButton[K].Matriz->Columnas != HistorialButton[K].Matriz->Filas) {
							EnableWindow(HistorialButton[K].hwnd, TRUE);
						}
					}
					else {
						if (HistorialButton[K].Matriz->Filas == 3) {
							EnableWindow(HistorialButton[K].hwnd, TRUE);
						}
					}
				}
			}
			DestroyWindow(hWnd);
			return 0;
			break;
		}

		break;
	case WM_INITDIALOG:
	{
		EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);
		NuevoBotonSelect.Figura = 0;
		NuevoBotonSelect.Matriz = 0;
		NuevoBotonSelect.hwnd = 0;
		NuevoBotonSelect.IDC = 0;
		NuevoBotonSelect.Pos = 0;
	}
	break;
	case WM_CLOSE:
		for (int K = 0; K < HistorialButton.size(); K++) {
			if (HistorialButton[K].Figura != 0) {
				EnableWindow(HistorialButton[K].hwnd, TRUE);
			}
			if (HistorialButton[K].Matriz != 0) {
				if (!GraphMode) {
					if (HistorialButton[K].Matriz->Columnas != HistorialButton[K].Matriz->Filas) {
						EnableWindow(HistorialButton[K].hwnd, TRUE);
					}
				}
				else {
					if (HistorialButton[K].Matriz->Filas == 3) {
						EnableWindow(HistorialButton[K].hwnd, TRUE);
					}
				}
			}
		}
		DestroyWindow(hWnd);
		return 0;
		break;
	default:

		return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK ChoiceProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		NuevoBotonChoice.Figura = 0;
		NuevoBotonChoice.Matriz = 0;
		NuevoBotonChoice.hwnd = 0;
		NuevoBotonChoice.IDC = 0;
		NuevoBotonChoice.Pos = 0;
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hWnd, FALSE);
			EsperandoChoice = false;
			break;
		case IDOK:
		{
			if (EsperandoChoice) {
				if (IsDlgButtonChecked(hWnd, IDC_RADIOTRANS)) {
					NuevoBotonChoice.Matriz = Transpuesta(&RaizMatrices, NuevoBotonChoice.Matriz);
				}
				if (IsDlgButtonChecked(hWnd, IDC_RADIOADJE)) {
					NuevoBotonChoice.Matriz = MatrizAdj(&RaizMatrices, NuevoBotonChoice.Matriz);
				}
				if (IsDlgButtonChecked(hWnd, IDC_RADIOINVERSA)) {
					NuevoBotonChoice.Matriz = Inversa(&RaizMatrices, NuevoBotonChoice.Matriz);
				}
				if ((NuevoBotonChoice.Matriz==0)) {
					MessageBoxA(hWnd,"Error: Para aplicar esta funcion es necesario que la Matriz sea simetrica y su determinante sea diferente de 0", "Matriz de entrada equivocada", MB_OK|MB_ICONSTOP);
					DestroyWindow(NuevoBotonChoice.hwnd);
					EnableWindow(GetDlgItem(hWnd,IDOK),FALSE);
					break;
				}
				HistorialButton.push_back(NuevoBotonChoice);
				HistorialButton[LastButtonHisto].IDC = LastButtonIDCHisto;
				HistorialButton[LastButtonHisto].Pos = LastButtonPos[1];
				CrearBoton(&HistorialButton[LastButtonHisto], LastButtonPos[0], LastButtonPos[1], buttonSize, HistorialWindow);
				LastButtonIDCHisto++;
				LastButtonPos[1] += buttonSize + 20;
				SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1], 0, 0, SWP_NOSIZE);
				LastButtonHisto++;
			}
			else {
				if (IsDlgButtonChecked(hWnd, IDC_RADIONUEVA)) {
					CreatorWindow = CreateDialog(0, MAKEINTRESOURCE(IDD_CREARNUEVO), 0, (DLGPROC)CreatorProc);
					SetTimer(CreatorWindow,             // handle to main window 
						102,            // timer identifier 
						50,                 // 10-second interval 
						(TIMERPROC)NULL);     // no timer callback 
					ShowWindow(CreatorWindow, SW_SHOW);
				}
				if (IsDlgButtonChecked(hWnd, IDC_RADIOFIGURA)) {
					char FIG[10] = {NULL};
					if (IsDlgButtonChecked(hWnd, IDC_RADIOCUBO)) {
						NuevoBotonChoice.Figura = HacerCubo(&RaizFiguras);
						strcpy_s(FIG,"CUBO");
					}
					if (IsDlgButtonChecked(hWnd, IDC_RADIODIAMANTE)) {
						NuevoBotonChoice.Figura = HacerDiamante(&RaizFiguras);
						strcpy_s(FIG, "DIAMANTE");
					}
					if (IsDlgButtonChecked(hWnd, IDC_RADIOPIRAMIDE)) {
						NuevoBotonChoice.Figura = HacerPiramide(&RaizFiguras);
						strcpy_s(FIG, "PIRAMIDE");
					}
					if (IsDlgButtonChecked(hWnd, IDC_RADIOTETRA)) {
						NuevoBotonChoice.Figura = HacerTetra(&RaizFiguras);
						strcpy_s(FIG, "TETRAEDRO");
					}
					//ESTA ES LA GENERACION DE MATRICES DE FIGURAS
					NuevoBotonChoice.Figura->R = Red;
					NuevoBotonChoice.Figura->G = Green;
					NuevoBotonChoice.Figura->B = Blue;
					HistorialButton.push_back(NuevoBotonChoice);
					HistorialButton[LastButtonHisto].hwnd = CreateWindow(
						"BUTTON",  // Predefined class; Unicode assumed 
						FIG,      // Button text 
						WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
						LastButtonPos[0],         // x position 
						LastButtonPos[1],         // y position 
						buttonSize,        // Button width
						buttonSize,        // Button height
						HistorialWindow,     // Parent window
						(HMENU)LastButtonIDCHisto,       // No menu.
						hInst,
						NULL);      // Pointer not needed.
					HistorialButton[LastButtonHisto].IDC = LastButtonIDCHisto;
					HistorialButton[LastButtonHisto].Pos = LastButtonPos[1];
					LastButtonIDCHisto++;
					LastButtonPos[1] += buttonSize + 20;
					SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1], 0, 0, SWP_NOSIZE);
					LastButtonHisto++;
					

				}
			}
			EsperandoChoice = false;
			Red = 255;
			Green = 255;
			Blue = 255;
			EndDialog(hWnd, TRUE);
		}
			break;
		case IDC_RADIOINVERSA:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICPLS), "Seleccione la matriz a la cual desea generar su MATRIZ INVERSA"); 
			EsperandoChoice = true;
			break;
		case IDC_RADIOADJE:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICPLS), "Seleccione la matriz a la cual desea generar su MATRIZ ADJUNTA");
			EsperandoChoice = true;
			break;
		case IDC_RADIOTRANS:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICPLS), "Seleccione la matriz a la cual desea generar su MATRIZ TRANSPUESTA");
			EsperandoChoice = true;
			break;
		case IDC_RADIONUEVA:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICPLS), "Presione -Aceptar- para abrir la ventana de creacion de Matrices");
			EsperandoChoice = false;
			EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
			break;
		case IDC_RADIOCUBO:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICPLS), "Presione -Aceptar- para crear la figura seleccionada");
			EsperandoChoice = false;
			if (ColorSelected) {
				EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
			}
			break;
		case IDC_RADIODIAMANTE:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICPLS), "Presione -Aceptar- para crear la figura seleccionada");
			EsperandoChoice = false;
			if (ColorSelected) {
				EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
			}
			break;
		case IDC_RADIOPIRAMIDE:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICPLS), "Presione -Aceptar- para crear la figura seleccionada");
			EsperandoChoice = false;
			if (ColorSelected) {
				EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
			}
			break;
		case IDC_RADIOTETRA:
			SetWindowText(GetDlgItem(hWnd, IDC_STATICPLS), "Presione -Aceptar- para crear la figura seleccionada");
			EsperandoChoice = false;
			if (ColorSelected) {
				EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
			}
			break;
		case IDC_BUTTONCOLOR:
		{
			HBRUSH hBrush;
			hdc = GetDC(hWnd);
			gColor = ShowColorDialog(hWnd);
			Red = (unsigned char)gColor;
			Green = (unsigned char)(gColor >> 8);
			Blue = (unsigned char)(gColor >> 16);
			hBrush = CreateSolidBrush(RGB(Red, Green, Blue));
			SelectObject(hdc,hBrush);
			Rectangle(hdc, 190, 170, 210, 190);
			DeleteObject(hBrush);
			if (!ColorSelected) {
				ColorSelected = true;
			}
		}
			break;
		default:
			break;
		}
		if (!IsDlgButtonChecked(hWnd,IDC_RADIOFIGURA)) {
//			EsperandoChoice = false;
			EnableWindow(GetDlgItem(hWnd,IDC_RADIOCUBO),FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOPIRAMIDE), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOTETRA), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIODIAMANTE), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIODODECAEDRO), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOICOSAHEDRO), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTONCOLOR), FALSE);
		}
		else {
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOCUBO), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOPIRAMIDE), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOTETRA), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIODIAMANTE), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTONCOLOR), TRUE);
			//EnableWindow(GetDlgItem(hWnd, IDC_RADIODODECAEDRO), TRUE);
			//EnableWindow(GetDlgItem(hWnd, IDC_RADIOICOSAHEDRO), TRUE);
		}
		if (!IsDlgButtonChecked(hWnd, IDC_RADIOMATRIZ)) {
			EnableWindow(GetDlgItem(hWnd, IDC_RADIONUEVA), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOINVERSA), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOADJE), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOTRANS), FALSE);
		}
		else {
			EnableWindow(GetDlgItem(hWnd, IDC_RADIONUEVA), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOINVERSA), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOADJE), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_RADIOTRANS), TRUE);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	default:

		return FALSE;
	}

	return TRUE;
}
//Variables de HistorialProc

vector <Cubo*> MultiFigura;

BOOL CALLBACK HistorialProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 6480: {
			ChoiceWindow = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOGCHOOSE), 0, (DLGPROC)ChoiceProc, 0);
			ShowWindow(ChoiceWindow, SW_SHOW);
			SetScrollRange(GetDlgItem(hWnd, WM_VSCROLL), SB_VERT, 0, LastButtonPos[1] + buttonSize + 10, TRUE);
		}
			break;
		}

		if ((int)wParam > 6480) {
			if (EsperandoSelect) {
				NuevoBotonSelect.Matriz = HistorialButton[(int)wParam - 6481].Matriz;
				NuevoBotonSelect.Figura = HistorialButton[(int)wParam - 6481].Figura;
				NuevoBotonSelect.IDC = 999969;
				CrearBoton(&NuevoBotonSelect, 20, 20, buttonSize, SelectWindow);
				EnableWindow(GetDlgItem(SelectWindow, IDOK), TRUE);
				EsperandoSelect = false;
				break;
			}
			if (EsperandoChoice) {
				NuevoBotonChoice.Matriz = HistorialButton[(int)wParam - 6481].Matriz;
				NuevoBotonChoice.Figura = HistorialButton[(int)wParam - 6481].Figura;
				NuevoBotonChoice.IDC = 999969;
				CrearBoton(&NuevoBotonChoice, 290, 30, buttonSize, ChoiceWindow);
				EnableWindow(GetDlgItem(ChoiceWindow, IDOK), TRUE);
				break;
			}
			if ((EsperandoEstructura)&&(HistorialButton[(int)wParam - 6481].Figura == 0)) {
				ButtonList NuevoBoton;
				NuevoBoton.Matriz = HistorialButton[(int)wParam - 6481].Matriz;
				OperationButton.push_back(NuevoBoton);
				OperationButton[LastButtonOper].IDC = LastButtonIDCOper;
				OperationButton[LastButtonOper].Pos = LastButtonPosOper;
				CrearBoton(&OperationButton[LastButtonOper], LastButtonPosOper, 10, buttonSize, OperationWindow);
				LastButtonIDCOper++;
				LastButtonPosOper += buttonSize + 20;
				LastButtonOper++;
				EsperandoEstructura = false;
				break;
			}
			else {
				if (HistorialButton[(int)wParam - 6481].Figura == 0) {
					DestroyWindow(OperationButton[OperationButton.size() - 1].hwnd);
					OperationButton.erase(OperationButton.begin() + OperationButton.size() - 1);
					LastButtonIDCOper--;
					LastButtonOper--;
					LastButtonPosOper -= buttonSize + 20;
					ButtonList NuevoBoton;
					NuevoBoton.Matriz = HistorialButton[(int)wParam - 6481].Matriz;
					NuevoBoton.Figura = HistorialButton[(int)wParam - 6481].Figura;
					OperationButton.push_back(NuevoBoton);
					OperationButton[LastButtonOper].IDC = LastButtonIDCOper;
					OperationButton[LastButtonOper].Pos = LastButtonPosOper;
					CrearBoton(&OperationButton[LastButtonOper], LastButtonPosOper, 10, buttonSize, OperationWindow);
					LastButtonIDCOper++;
					LastButtonPosOper += buttonSize + 20;
					LastButtonOper++;
				}
				else if (IsDlgButtonChecked(CalculatorWindow, IDC_CHECKGRAFICA)) {
					int I = 0;
					for (I = 0; I < MultiFigura.size(); I++) {
						if (HistorialButton[(int)wParam - 6481].Figura == MultiFigura[I]) {
							HistorialButton[(int)wParam - 6481].Graficado = true;
							break;
						}
					}
					if (!HistorialButton[(int)wParam - 6481].Graficado) {
						MultiFigura.push_back(HistorialButton[(int)wParam - 6481].Figura);
					}
					else {
						MultiFigura.erase(MultiFigura.begin() + I);
						HistorialButton[(int)wParam - 6481].Graficado = false;
					}
				}
				break;
			}
		}
		break;
	case WM_INITDIALOG:
		//ScrollBarShit
		BarraHisto.cbSize = sizeof(SCROLLBARINFO);
		BarraHisto.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &BarraHisto);

		//--------------------//
	 
		SetWindowPos(hWnd, HWND_TOP,11,17,0,0, SWP_NOSIZE);
		ADD = CreateWindow(
			"BUTTON",  // Predefined class; Unicode assumed 
			"Nueva Estructura",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			LastButtonPos[0],         // x position 
			LastButtonPos[1],         // y position 
			buttonSize,        // Button width
			buttonSize,        // Button height
			hWnd,     // Parent window
			(HMENU)6480,       // No menu.
			hInst,
			NULL);      // Pointer not needed.
		LastButtonIDCHisto++;
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
		break;
	case WM_VSCROLL:
	{
		GetScrollInfo(hWnd, SB_VERT, &BarraHisto);
		BarraHisto.cbSize = sizeof(BarraHisto);
		BarraHisto.nMax = LastButtonPos[1] + buttonSize +10;
		BarraHisto.nMin = 0;
		BarraHisto.fMask = SIF_ALL;
		BarraHisto.nPage = 50;
		ShowScrollBar(hWnd, SB_VERT, TRUE);
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			BarraHisto.nPos = BarraHisto.nMin;
			break;

		case SB_BOTTOM:
			BarraHisto.nPos = BarraHisto.nMax;
			break;

		case SB_LINEUP:
			BarraHisto.nPos -= 1;
			break;

		case SB_LINEDOWN:
			BarraHisto.nPos += 1;
			break;

		case SB_PAGEUP:
			BarraHisto.nPos -= BarraHisto.nPage;
			break;

		case SB_PAGEDOWN:
			BarraHisto.nPos += BarraHisto.nPage;
			break;

		case SB_THUMBTRACK:
			BarraHisto.nPos = BarraHisto.nTrackPos;
			break;

		default:
			break;
		}
		SetScrollInfo(hWnd, SB_VERT, &BarraHisto, TRUE);
		ShowScrollBar(hWnd, SB_VERT, TRUE);
		for (int K = 0; K < HistorialButton.size(); K++) {
			SetWindowPos(HistorialButton[K].hwnd, HWND_TOP, LastButtonPos[0], HistorialButton[K].Pos - BarraHisto.nPos, 0, 0, SWP_NOSIZE);
		}
		SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1] - BarraHisto.nPos, 0, 0, SWP_NOSIZE);
	}
		break;

	default:

		return FALSE;
	}

	return TRUE;
}


BOOL CALLBACK OperationProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// checa los mensajes
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
	
		}
		break;
	case WM_INITDIALOG:
	{
		SetWindowPos(hWnd, HWND_TOP, 180, 17, 0, 0, SWP_NOSIZE);
		BarraOper.cbSize = sizeof(SCROLLBARINFO);
		BarraOper.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_HORZ, &BarraOper);
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
		break;
	case WM_HSCROLL:
	{
		GetScrollInfo(hWnd, SB_HORZ, &BarraOper);
		BarraOper.cbSize = sizeof(BarraOper);
		BarraOper.nMax = LastButtonPosOper + buttonSize + 10;
		BarraOper.nMin = 0;
		BarraOper.fMask = SIF_ALL;
		BarraOper.nPage = 50;
		ShowScrollBar(hWnd, SB_HORZ, TRUE);
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			BarraOper.nPos = BarraOper.nMin;
			break;

		case SB_BOTTOM:
			BarraOper.nPos = BarraOper.nMax;
			break;

		case SB_LINEUP:
			BarraOper.nPos -= 1;
			break;

		case SB_LINEDOWN:
			BarraOper.nPos += 1;
			break;

		case SB_PAGEUP:
			BarraOper.nPos -= BarraOper.nPage;
			break;

		case SB_PAGEDOWN:
			BarraOper.nPos += BarraOper.nPage;
			break;

		case SB_THUMBTRACK:
			BarraOper.nPos = BarraOper.nTrackPos;
			break;

		default:
			break;
		}
		SetScrollInfo(hWnd, SB_HORZ, &BarraOper, TRUE);
		ShowScrollBar(hWnd, SB_HORZ, TRUE);
		for (int K = 0; K < OperationButton.size(); K++) {
			RECT Rect;
			GetWindowRect(OperationButton[K].hwnd, &Rect);
			MapWindowPoints(HWND_DESKTOP, GetParent(OperationButton[K].hwnd), (LPPOINT)&Rect, 2);
			SetWindowPos(OperationButton[K].hwnd, HWND_TOP, OperationButton[K].Pos - BarraOper.nPos, Rect.top, 0, 0, SWP_NOSIZE);
		}
	}
	break;

	default:

		return FALSE;
	}

	return TRUE;
}

//Variables de el Graficador

float Zprp = 0;
float angulo = 0;
bool Animar = false;
HWND EditZprp;
HDC hdc;
RECT rect{ 0,0,0,0 };
POINT Aux{0,0};

BOOL CALLBACK GraphProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	// checa los mensajes
	switch (uMsg)
	{
	case WM_TIMER:
	{
		char txt[10];
		HDC hdc = GetDC(hWnd);
		
		GetWindowText(EditZprp, txt, sizeof(txt));
		Zprp = atoi(txt);
		//SetDCBrushColor(hdc,COLOR_WINDOW + 1);
		Rectangle(hdc,0,0,rect.right,rect.bottom);

		for (int J = 0; J < MultiFigura.size(); J++) {
			Cubo *AuxFig = new Cubo;
			if (Animar) {
				AuxFig = Transforma(&RaizFiguras, MultiFigura[J], angulo);
			}
			else {
				Matriz *AuxMC;
				Matriz *AuxRaiz;
				AuxRaiz = 0;
				AuxMC = IgualdadMatriz(&AuxRaiz,3,3);
				for (int I = 0; I < MultiFigura[J]->MatrizCompuesta.size(); I++) {
					if (MultiFigura[J]->MatrizCompuesta[I]->Columnas == 0) {
						AuxMC = MultiMatrices(&AuxRaiz, AuxMC, Cuater(&AuxRaiz  ,MultiFigura[J]->MatrizCompuesta[I],360));
					}
					else {
						AuxMC = MultiMatrices(&AuxRaiz, AuxMC, MultiFigura[J]->MatrizCompuesta[I]);
					}
				}
				AuxFig->R = MultiFigura[J]->R;
				AuxFig->G = MultiFigura[J]->G;
				AuxFig->B = MultiFigura[J]->B;
				AddToShapeList(AuxFig, &RaizFiguras);
				AuxFig->Puntos.push_back(0);
				for (int I = 0; I < MultiFigura[J]->Puntos.size(); I++) {
					if (I == 0) {
						AuxFig->Puntos[0] = MultiMatrices(&AuxFig->Puntos[0], AuxMC, MultiFigura[J]->Puntos[I]);
						//*AuxFig->Puntos[I] = *AuxPnto;
						for (int K = 0; K < MultiFigura[J]->Puntos[I]->conect.size(); K++) {
							AuxFig->Puntos[I]->conect.push_back(MultiFigura[J]->Puntos[I]->conect[K]);
						}
						continue;
					}
					Matriz *AuxPnto;
					AuxPnto = MultiMatrices(&AuxFig->Puntos[0], AuxMC, MultiFigura[J]->Puntos[I]);
					AuxFig->Puntos.push_back(AuxPnto);
					for (int K = 0; K < MultiFigura[J]->Puntos[I]->conect.size(); K++) {
						AuxFig->Puntos[I]->conect.push_back(MultiFigura[J]->Puntos[I]->conect[K]);
					}
				}
				DelAll(AuxRaiz);
			}
			Dibuja(hdc, AuxFig, Zprp, Aux.x , Aux.y);
			DeleteShape(&RaizFiguras,AuxFig);
		}
		if (angulo >= 360) {
			angulo = 0;
		}
		angulo++;
		//Ejes Coordenados
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		MoveToEx(hdc, 0, (int)(Aux.x/2)-1, NULL);
		LineTo(hdc, Aux.y-1, (int)(Aux.x / 2) - 1);
		MoveToEx(hdc, (int)(Aux.y / 2)-1, 0, NULL);
		LineTo(hdc, (int)(Aux.y / 2)-1, Aux.x-1);
		ReleaseDC(hWnd,hdc);
	}
	break;
	case WM_INITDIALOG:
		SetWindowPos(hWnd, HWND_TOP, 370, 175, 0, 0, SWP_NOSIZE);
		SetTimer(hWnd,             // handle to main window 
			104,            // timer identifier 
			25,                 // 10-second interval 
			(TIMERPROC)NULL);     // no timer callback 
		if (GetWindowRect(hWnd, &rect))
		{
			Aux.y = rect.right - rect.left;
			Aux.x = rect.bottom - rect.top;
		}
		break;
	case WM_CLOSE:
		KillTimer(GraphWindow, 104);
		DestroyWindow(hWnd);
		return 0;
		break;
	case WM_DESTROY:
		KillTimer(GraphWindow, 104);
		DestroyWindow(hWnd);
		return 0;
		break;
	default:

		return FALSE;
	}

	return TRUE;
}

//La principal

BOOL CALLBACK CalculatorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	

	// checar el tipo de mensaje
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDC_CHECKANIMA:
		{
			if (Animar == false) {
				Animar = true;
			}
			else {
				Animar = false;
			}
		}
		break;
		case IDC_BUTTMATSUM:
			if (!EsperandoEstructura) {
				ButtonList NuevoBoton;
				NuevoBoton.Matriz = 0;
				NuevoBoton.Figura = 0;
				OperationButton.push_back(NuevoBoton);
				OperationButton[LastButtonOper].hwnd = CreateWindow(
					"BUTTON",  // Predefined class; Unicode assumed 
					"+",      // Button text 
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
					LastButtonPosOper,         // x position 
					(buttonSize / 2) - 5,         // y position 
					buttonSize / 4,        // Button width
					buttonSize / 4,        // Button height
					OperationWindow,     // Parent window
					(HMENU)LastButtonIDCOper,       // No menu.
					hInst,
					NULL);      // Pointer not needed.
				OperationButton[LastButtonOper].IDC = LastButtonIDCOper;
				OperationButton[LastButtonOper].Pos = LastButtonPosOper;
				LastButtonIDCOper++;
				LastButtonPosOper += buttonSize / 4 + 20;
				LastButtonOper++;
				EsperandoEstructura = true;
			}
			else {
				if (LastButtonIDCOper > 9777) {
					SetWindowText(OperationButton[LastButtonOper - 1].hwnd, "+");
				}
			}
			break;
		case IDC_BUTTMATRES:
			if (!EsperandoEstructura) {
				ButtonList NuevoBoton;
				NuevoBoton.Matriz = 0;
				NuevoBoton.Figura = 0;
				OperationButton.push_back(NuevoBoton);
				OperationButton[LastButtonOper].hwnd = CreateWindow(
					"BUTTON",  // Predefined class; Unicode assumed 
					"-",      // Button text 
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
					LastButtonPosOper,         // x position 
					(buttonSize / 2) - 5,         // y position 
					buttonSize / 4,        // Button width
					buttonSize / 4,        // Button height
					OperationWindow,     // Parent window
					(HMENU)LastButtonIDCOper,       // No menu.
					hInst,
					NULL);      // Pointer not needed.
				OperationButton[LastButtonOper].IDC = LastButtonIDCOper;
				OperationButton[LastButtonOper].Pos = LastButtonPosOper;
				LastButtonIDCOper++;
				LastButtonPosOper += buttonSize / 4 + 20;
				LastButtonOper++;
				EsperandoEstructura = true;
			}
			else {
				if (LastButtonIDCOper > 9777) {
					SetWindowText(OperationButton[LastButtonOper - 1].hwnd, "-");
				}
			}
			break;
		case IDC_BUTTMATMUL:
			if (!EsperandoEstructura) {
				ButtonList NuevoBoton;
				NuevoBoton.Matriz = 0;
				NuevoBoton.Figura = 0;
				OperationButton.push_back(NuevoBoton);
				OperationButton[LastButtonOper].hwnd = CreateWindow(
					"BUTTON",  // Predefined class; Unicode assumed 
					"*",      // Button text 
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
					LastButtonPosOper,         // x position 
					(buttonSize / 2) - 5,         // y position 
					buttonSize / 4,        // Button width
					buttonSize / 4,        // Button height
					OperationWindow,     // Parent window
					(HMENU)LastButtonIDCOper,       // No menu.
					hInst,
					NULL);      // Pointer not needed.
				OperationButton[LastButtonOper].IDC = LastButtonIDCOper;
				OperationButton[LastButtonOper].Pos = LastButtonPosOper;
				LastButtonIDCOper++;
				LastButtonPosOper += buttonSize / 4 + 20;
				LastButtonOper++;
				EsperandoEstructura = true;
			}
			else {
				if (LastButtonIDCOper > 9777) {
					SetWindowText(OperationButton[LastButtonOper - 1].hwnd, "*");
				}
			}
			break;
		case IDC_BUTTMATDIV:
			if (!EsperandoEstructura) {
				ButtonList NuevoBoton;
				NuevoBoton.Matriz = 0;
				NuevoBoton.Figura = 0;
				OperationButton.push_back(NuevoBoton);
				OperationButton[LastButtonOper].hwnd = CreateWindow(
					"BUTTON",  // Predefined class; Unicode assumed 
					"/",      // Button text 
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
					LastButtonPosOper,         // x position 
					(buttonSize / 2) - 5,         // y position 
					buttonSize / 4,        // Button width
					buttonSize / 4,        // Button height
					OperationWindow,     // Parent window
					(HMENU)LastButtonIDCOper,       // No menu.
					hInst,
					NULL);      // Pointer not needed.
				OperationButton[LastButtonOper].IDC = LastButtonIDCOper;
				OperationButton[LastButtonOper].Pos = LastButtonPosOper;
				LastButtonIDCOper++;
				LastButtonPosOper += buttonSize / 4 + 20;
				LastButtonOper++;
				EsperandoEstructura = true;
			}
			else {
				if (LastButtonIDCOper > 9777) {
					SetWindowText(OperationButton[LastButtonOper - 1].hwnd, "/");
				}
			}
			break;
		case IDC_BUTTMATESC:
		{
			char BUFFB[50];
			GetWindowText(GetDlgItem(hWnd,IDC_EDITESC),BUFFB,sizeof(BUFFB));
			if (atof(BUFFB) != 0) {
				for (int K = 0; K < HistorialButton.size(); K++) {
					if (HistorialButton[K].Figura != 0) {
						EnableWindow(HistorialButton[K].hwnd, FALSE);
					}
				}
				SelectWindow = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOGSELECT), 0, (DLGPROC)SelectProc, 0);
				ShowWindow(SelectWindow, SW_SHOW);
				EsperandoSelect = true;
				DetOrEsc = false;
				Atar = false;
			}
		}
			break;
		case IDC_BUTTMATDETER:
			for (int K = 0; K < HistorialButton.size(); K++) {
				if (HistorialButton[K].Figura != 0) {
						EnableWindow(HistorialButton[K].hwnd, FALSE);
				}
				if (HistorialButton[K].Matriz != 0) {
					if (HistorialButton[K].Matriz->Columnas != HistorialButton[K].Matriz->Filas) {
						EnableWindow(HistorialButton[K].hwnd, FALSE);
					}
				}
			}
			SelectWindow = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOGSELECT), 0, (DLGPROC)SelectProc, 0);
			ShowWindow(SelectWindow, SW_SHOW);
			EsperandoSelect = true;
			DetOrEsc = true;
			Atar = false;
		break;
		case IDC_BUTTONCLEAR:
		{
			for (int K = OperationButton.size() - 1; K >= 0; K--) {
				DestroyWindow(OperationButton[K].hwnd);
				OperationButton.erase(OperationButton.begin() + K);
				LastButtonIDCOper--;
				LastButtonOper--;
				if (K % 2 != 0) {
					LastButtonPosOper -= buttonSize / 4 + 20;
				}
				else {
					LastButtonPosOper -= buttonSize + 20;
				}
			}
			if (!IsDlgButtonChecked(hWnd, IDC_CHECKGRAFICA)) {
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATDIV), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATSUM), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATRES), TRUE);
			}
			EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATMUL), TRUE);
			EsperandoEstructura = true;
			
		}
		break;
		case IDC_BUTTONBORRAR:
			if (LastButtonIDCOper > 9777) {
				char BUFF[3];
				GetWindowText(OperationButton[OperationButton.size() - 1].hwnd, BUFF, sizeof(BUFF));
				if (BUFF[0] == '=') {
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATSUM), TRUE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATRES), TRUE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATMUL), TRUE);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATDIV), TRUE);
				}
				DestroyWindow(OperationButton[OperationButton.size() - 1].hwnd);
				OperationButton.erase(OperationButton.begin() + OperationButton.size() - 1);
				LastButtonIDCOper--;
				LastButtonOper--;
				if ((OperationButton.size()) % 2 != 0) {
					LastButtonPosOper -= buttonSize / 4 + 20;
					EsperandoEstructura = false;
				}
				else {
					LastButtonPosOper -= buttonSize + 20;
					EsperandoEstructura = true;
				}
			}
			break;
		case IDC_BUTTONOPERAR:
		    {
			    if (!EsperandoEstructura) {
				ButtonList NuevoBoton;
				NuevoBoton.Matriz = 0;
				NuevoBoton.Figura = 0;
				OperationButton.push_back(NuevoBoton);
				OperationButton[LastButtonOper].hwnd = CreateWindow(
					"BUTTON",  // Predefined class; Unicode assumed 
					"=",      // Button text 
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
					LastButtonPosOper,         // x position 
					(buttonSize / 2) - 5,         // y position 
					buttonSize / 4,        // Button width
					buttonSize / 4,        // Button height
					OperationWindow,     // Parent window
					(HMENU)LastButtonIDCOper,       // No menu.
					hInst,
					NULL);      // Pointer not needed
				OperationButton[LastButtonOper].IDC = LastButtonIDCOper;
				OperationButton[LastButtonOper].Pos = LastButtonPosOper;
				LastButtonIDCOper++;
				LastButtonPosOper += buttonSize / 4 + 20;
				LastButtonOper++;
			}
				else if (LastButtonIDCOper > 9777) {
					SetWindowText(OperationButton[LastButtonOper - 1].hwnd, "=");

				}
				if ((OperationButton.size() != 0) && (!EsperandoEstructura)) {
					char BUFF[2];
					Matriz *MatCom = new Matriz;
					for (int K = 0; K < OperationButton.size()-1; K++) {
						Matriz *Aux = 0;
						if (K == 0) {
							*MatCom = *OperationButton[0].Matriz;
							continue;
						}
						GetWindowText(OperationButton[K].hwnd, BUFF, sizeof(BUFF));
						switch (BUFF[0]) {
						case '+':
							Aux = SumResMatrices(&RaizMatrices, MatCom, OperationButton[K + 1].Matriz, true);
							break;
						case '-':
							Aux = SumResMatrices(&RaizMatrices, MatCom, OperationButton[K + 1].Matriz, false);
							break;
						case '*':
							Aux = MultiMatrices(&RaizMatrices, MatCom, OperationButton[K + 1].Matriz);
							break;
						case '/':
						{
							Matriz *InvAux;
							InvAux = Inversa(&RaizMatrices, OperationButton[K + 1].Matriz);
							Aux = MultiMatrices(&RaizMatrices, MatCom, InvAux);
							Delete(&RaizMatrices, InvAux);
						}
						break;
						}
						delete MatCom;
						MatCom = Aux;
						
						K++;
						if (MatCom==0) {
							MessageBoxA(hWnd, "Error: Las matrices no son compatibles", "Matriz de entrada equivocada", MB_OK | MB_ICONSTOP);
							break;
						}
					}
					if (MatCom != 0) {
						ButtonList NuevoBoton;
						HistorialButton.push_back(NuevoBoton);
						HistorialButton[LastButtonHisto].Matriz = MatCom;
						HistorialButton[LastButtonHisto].IDC = LastButtonIDCHisto;
						HistorialButton[LastButtonHisto].Pos = LastButtonPos[1];
						CrearBoton(&HistorialButton[LastButtonHisto], LastButtonPos[0], LastButtonPos[1], buttonSize, HistorialWindow);
						LastButtonIDCHisto++;
						LastButtonPos[1] += buttonSize + 20;
						SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1], 0, 0, SWP_NOSIZE);
						MessageBoxA(hWnd, "Se generó la Matriz exitosamente", "Operar", MB_OK);

						NuevoBoton.Matriz = HistorialButton[LastButtonHisto].Matriz;
						OperationButton.push_back(NuevoBoton);
						OperationButton[LastButtonOper].IDC = LastButtonIDCOper;
						OperationButton[LastButtonOper].Pos = LastButtonPosOper;
						CrearBoton(&OperationButton[LastButtonOper], LastButtonPosOper, 10, buttonSize, OperationWindow);
						LastButtonIDCOper++;
						LastButtonPosOper += buttonSize + 20;
						LastButtonOper++;
						LastButtonHisto++;
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATSUM), FALSE);
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATRES), FALSE);
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATMUL), FALSE);
						EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATDIV), FALSE);
					}

				}
		    }
			break;
		case IDC_CHECKGRAFICA:
			if (IsDlgButtonChecked(hWnd, IDC_CHECKGRAFICA)) {
				EnableWindow(GraphWindow,TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_STATICGRAF),TRUE);
				EnableWindow(GetDlgItem(hWnd,IDC_BUTTONATAR), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_CHECKANIMA), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_STATICZPRP), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_EDITZPRP), TRUE);

				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATSUM), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATRES), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATDIV), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATESC), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATDETER), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_STATICMATRI), FALSE);

				for (int K = 0; K < HistorialButton.size(); K++) {
					if (HistorialButton[K].Matriz != 0) {
						if (HistorialButton[K].Matriz->Filas != 3) {
							EnableWindow(HistorialButton[K].hwnd, FALSE);
						}
					}
				}
				GraphMode = true;
				GraphWindow = CreateDialog(0, MAKEINTRESOURCE(IDD_GRAFICADOR), hWnd, (DLGPROC)GraphProc);
				ShowWindow(GraphWindow, SW_SHOW);

			}
			else {
				EnableWindow(GraphWindow, FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_STATICGRAF), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTONATAR), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_CHECKANIMA), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_STATICZPRP), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_EDITZPRP), FALSE);

				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATSUM), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATRES), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATDIV), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATESC), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTMATDETER), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_STATICMATRI), TRUE);
				CheckDlgButton(hWnd, IDC_CHECKANIMA, BST_UNCHECKED);

				for (int K = 0; K < HistorialButton.size(); K++) {
					EnableWindow(HistorialButton[K].hwnd, TRUE);
					HistorialButton[K].Graficado = false;
				}
				for (int I = MultiFigura.size() -1 ; I >= 0; I--) {
					MultiFigura.erase(MultiFigura.begin()+I);
				}
				GraphMode = false;
				DestroyWindow(GraphWindow);
			}
			if (LastButtonIDCOper > 9777) {
				for (int K = OperationButton.size() - 1; K >= 0; K--) {
					DestroyWindow(OperationButton[K].hwnd);
					OperationButton.erase(OperationButton.begin() + K);
					LastButtonIDCOper--;
					LastButtonOper--;
					if (K % 2 != 0) {
						LastButtonPosOper -= buttonSize / 4 + 20;
					}
					else {
						LastButtonPosOper -= buttonSize + 20;
					}
				}
			}
			break;
		case IDC_BUTTTRAS:
		{
			char Aux[4], Auy[4], Auz[4];
			GetWindowText(GetDlgItem(hWnd,IDC_EDITTRASX),Aux,sizeof(Aux));
			GetWindowText(GetDlgItem(hWnd, IDC_EDITTRASY), Auy, sizeof(Auy));
			GetWindowText(GetDlgItem(hWnd, IDC_EDITTRASZ), Auz, sizeof(Auz));
			Matriz *Creada = Traslacion(&RaizMatrices, atof(Aux), atof(Auy), atof(Auz));
			ButtonList NuevoBoton;
			NuevoBoton.Matriz = Creada;
			HistorialButton.push_back(NuevoBoton);
			HistorialButton[LastButtonHisto].IDC = LastButtonIDCHisto;
			HistorialButton[LastButtonHisto].Pos = LastButtonPos[1];
			CrearBoton(&HistorialButton[LastButtonHisto], LastButtonPos[0], LastButtonPos[1], buttonSize, HistorialWindow);
			LastButtonIDCHisto++;
			LastButtonPos[1] += buttonSize + 20;
			SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1], 0, 0, SWP_NOSIZE);
			LastButtonHisto++;
		}
			break;
		case IDC_BUTTESCALA:
		{
			char Aux[4], Auy[4], Auz[4];
			GetWindowText(GetDlgItem(hWnd, IDC_EDITTRASX), Aux, sizeof(Aux));
			GetWindowText(GetDlgItem(hWnd, IDC_EDITTRASY), Auy, sizeof(Auy));
			GetWindowText(GetDlgItem(hWnd, IDC_EDITTRASZ), Auz, sizeof(Auz));
			Matriz *Creada = Escalar(&RaizMatrices, atof(Aux), atof(Auy), atof(Auz));
			ButtonList NuevoBoton;
			NuevoBoton.Matriz = Creada;
			HistorialButton.push_back(NuevoBoton);
			HistorialButton[LastButtonHisto].IDC = LastButtonIDCHisto;
			HistorialButton[LastButtonHisto].Pos = LastButtonPos[1];
			CrearBoton(&HistorialButton[LastButtonHisto], LastButtonPos[0], LastButtonPos[1], buttonSize, HistorialWindow);
			LastButtonIDCHisto++;
			LastButtonPos[1] += buttonSize + 20;
			SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1], 0, 0, SWP_NOSIZE);
			LastButtonHisto++;
		}
		break;
		case IDC_BUTTROTA:
		{
			char Aux[4], Auy[4], Auz[4];
			GetWindowText(GetDlgItem(hWnd, IDC_EDITTRASX), Aux, sizeof(Aux));
			GetWindowText(GetDlgItem(hWnd, IDC_EDITTRASY), Auy, sizeof(Auy));
			GetWindowText(GetDlgItem(hWnd, IDC_EDITTRASZ), Auz, sizeof(Auz));
			if ((atof(Aux) != 0) || (atof(Auy) != 0)|| (atof(Auz) != 0)) {
				if ((atof(Aux) != 0) && (atof(Auy) != 0) || (atof(Aux) != 0) && (atof(Auz) != 0) || (atof(Auz) != 0) && (atof(Auy) != 0)) {
					MessageBoxA(hWnd, "Error: La matriz de rotacion solo puede ser generada usando un eje a la vez", "Incapaz de continuar", MB_OK | MB_ICONSTOP);
					break;
				}
				Matriz *Creada = 0;
				if (atof(Aux) != 0) {
					Creada = RotaX(&RaizMatrices, atof(Aux));
				}
				if (atof(Auy) != 0) {
					Creada = RotaY(&RaizMatrices, atof(Auy));
				}
				if (atof(Auz) != 0) {
					Creada = RotaZ(&RaizMatrices, atof(Auz));
				}
				ButtonList NuevoBoton;
				NuevoBoton.Matriz = Creada;
				HistorialButton.push_back(NuevoBoton);
				HistorialButton[LastButtonHisto].IDC = LastButtonIDCHisto;
				HistorialButton[LastButtonHisto].Pos = LastButtonPos[1];
				CrearBoton(&HistorialButton[LastButtonHisto], LastButtonPos[0], LastButtonPos[1], buttonSize, HistorialWindow);
				LastButtonIDCHisto++;
				LastButtonPos[1] += buttonSize + 20;
				SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1], 0, 0, SWP_NOSIZE);
				LastButtonHisto++;
			}
		}
		break;
		case IDC_BUTTONATAR:
			if ((OperationButton.size() != 0)) {
				for (int K = 0; K < HistorialButton.size(); K++) {
					if (HistorialButton[K].Figura == 0) {
						EnableWindow(HistorialButton[K].hwnd, FALSE);
					}
				}
				SelectWindow = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOGSELECT), 0, (DLGPROC)SelectProc, 0);
				ShowWindow(SelectWindow, SW_SHOW);
				EsperandoSelect = true;
				DetOrEsc = false;
				Atar = true;
			}
			break;
		}
		break;

	case WM_INITDIALOG:
	{
		HistorialWindow = CreateDialog(0, MAKEINTRESOURCE(IDD_HISTORIAL), hWnd, (DLGPROC)HistorialProc);
		ShowWindow(HistorialWindow, SW_SHOW);

		OperationWindow = CreateDialog(0, MAKEINTRESOURCE(IDD_OPERACIONES), hWnd, (DLGPROC)OperationProc);
		ShowWindow(OperationWindow, SW_SHOW);

		

		EditZprp = GetDlgItem(hWnd,IDC_EDITZPRP);
		SetWindowText(EditZprp, "1000");
		SetWindowText(GetDlgItem(hWnd, IDC_EDITTRASX), "0");
		SetWindowText(GetDlgItem(hWnd, IDC_EDITTRASY), "0");
		SetWindowText(GetDlgItem(hWnd, IDC_EDITTRASZ), "0");

		EnableWindow(GraphWindow, FALSE);
		EnableWindow(GetDlgItem(hWnd, IDC_STATICGRAF), FALSE);

		RaizMatrices = 0;
		RaizFiguras = 0;

	}
	break;
	case WM_DESTROY:
		//GdiplusShutdown(gdiplusToken);
	    //el postquitmessage termina a toda la aplicacion
		DelAll(RaizMatrices);
		DelAllShapes(RaizFiguras);
		KillTimer(CreatorWindow, 102);
		PostQuitMessage(0);
		return 0;
		break;
	case WM_CLOSE:
		//GdiplusShutdown(gdiplusToken);
	    //el postquitmessage termina a toda la aplicacion
		DelAll(RaizMatrices);
		DelAllShapes(RaizFiguras);
		KillTimer(CreatorWindow, 102);
		PostQuitMessage(0);
		return 0;
		break;
	default:

		return FALSE;
	}

	return TRUE;
}

//Variables de CreatorProc

//HWND
HWND Edit;
HWND EditFilas;
HWND EditColumnas;
HWND EditValorAc;
HWND Static_Op1;
HWND Static_Op2;
HWND Static_Sig;
HWND MatrixGrid[10][10];
HWND ButtonOK;


HWND LastChecked = 0;
float Calcu(float Val1, char Oper, float Val2) {
	float Res;
	switch (Oper) {
	case '+':
		Res = (Val1 + Val2);
		return Res;
		break;
	case '-':
		Res = (Val1 - Val2);
		return Res;
		break;
	case '*':
		Res = (Val1 * Val2);
		return Res;
		break;
	case '/':
		if (Val2 == 0) {
			return ERROR;
		}
		else {
			Res = (Val1 / Val2);
			return Res;
		}
		break;
	case 'v':
		if (Val1 == 0) {
			return ERROR;
		}
		else {
			Res = pow(Val2,1/Val1);
			return Res;
		}
		break;
	case '^':
		Res = pow(Val1, Val2);
		return Res;
		break;
	}
}


char txt[50], Oper = NULL;
int P = 6480, GridIndex[2] = { 0,0 };
bool Pos = false,aux=false;

float Val1=0.0, Val2=0.0, Res=0.0;

char INI[3] = { "0" };

	//Nueva Matriz

Matriz *Creada;
void Acortar(HWND hwnd, char TXT[],int *p) {
	GetWindowText(hwnd, TXT, sizeof(TXT));
	for (int L = strlen(TXT) - 1; L >= 0; L--) {
		if (TXT[L] != '0') {
			if (TXT[L] == '.') {
				TXT[L] = NULL;
				*p = 6480;
			}
			break;
		}
		else {
			TXT[L] = NULL;
		}
	}
	SetWindowText(hwnd, TXT);
}

BOOL CALLBACK CreatorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	

	
	// checar el tipo de mensaje

	switch (uMsg)
	{
	case WM_INITDIALOG:


		Creada = new Matriz;

		Edit = GetDlgItem(hWnd, IDC_MATRIZINTRO);
		EditFilas = GetDlgItem(hWnd, IDC_EDITFILAS);
		EditColumnas = GetDlgItem(hWnd, IDC_EDITCOLUMNAS);
		Static_Op1 = GetDlgItem(hWnd, IDC_STATICVAL1);
		Static_Op2 = GetDlgItem(hWnd, IDC_STATICVAL2);
		Static_Sig = GetDlgItem(hWnd, IDC_STATICSIMB);
		EditValorAc = GetDlgItem(hWnd, IDC_STATICUPDATE);
		ButtonOK = GetDlgItem(hWnd, IDC_OK);
		//Cargado del Grid:
		{
		//FILA 0
		MatrixGrid[0][0] = GetDlgItem(hWnd, IDC_CHECK00);
		MatrixGrid[0][1] = GetDlgItem(hWnd, IDC_CHECK01);
		MatrixGrid[0][2] = GetDlgItem(hWnd, IDC_CHECK02);
		MatrixGrid[0][3] = GetDlgItem(hWnd, IDC_CHECK03);
		MatrixGrid[0][4] = GetDlgItem(hWnd, IDC_CHECK04);
		MatrixGrid[0][5] = GetDlgItem(hWnd, IDC_CHECK05);
		MatrixGrid[0][6] = GetDlgItem(hWnd, IDC_CHECK06);
		MatrixGrid[0][7] = GetDlgItem(hWnd, IDC_CHECK07);
		MatrixGrid[0][8] = GetDlgItem(hWnd, IDC_CHECK08);
		MatrixGrid[0][9] = GetDlgItem(hWnd, IDC_CHECK09);
		//FILA 1
		MatrixGrid[1][0] = GetDlgItem(hWnd, IDC_CHECK10);
		MatrixGrid[1][1] = GetDlgItem(hWnd, IDC_CHECK11);
		MatrixGrid[1][2] = GetDlgItem(hWnd, IDC_CHECK12);
		MatrixGrid[1][3] = GetDlgItem(hWnd, IDC_CHECK13);
		MatrixGrid[1][4] = GetDlgItem(hWnd, IDC_CHECK14);
		MatrixGrid[1][5] = GetDlgItem(hWnd, IDC_CHECK15);
		MatrixGrid[1][6] = GetDlgItem(hWnd, IDC_CHECK16);
		MatrixGrid[1][7] = GetDlgItem(hWnd, IDC_CHECK17);
		MatrixGrid[1][8] = GetDlgItem(hWnd, IDC_CHECK18);
		MatrixGrid[1][9] = GetDlgItem(hWnd, IDC_CHECK19);
		//FILA 2
		MatrixGrid[2][0] = GetDlgItem(hWnd, IDC_CHECK20);
		MatrixGrid[2][1] = GetDlgItem(hWnd, IDC_CHECK21);
		MatrixGrid[2][2] = GetDlgItem(hWnd, IDC_CHECK22);
		MatrixGrid[2][3] = GetDlgItem(hWnd, IDC_CHECK23);
		MatrixGrid[2][4] = GetDlgItem(hWnd, IDC_CHECK24);
		MatrixGrid[2][5] = GetDlgItem(hWnd, IDC_CHECK25);
		MatrixGrid[2][6] = GetDlgItem(hWnd, IDC_CHECK26);
		MatrixGrid[2][7] = GetDlgItem(hWnd, IDC_CHECK27);
		MatrixGrid[2][8] = GetDlgItem(hWnd, IDC_CHECK28);
		MatrixGrid[2][9] = GetDlgItem(hWnd, IDC_CHECK29);
		//FILA 3
		MatrixGrid[3][0] = GetDlgItem(hWnd, IDC_CHECK30);
		MatrixGrid[3][1] = GetDlgItem(hWnd, IDC_CHECK31);
		MatrixGrid[3][2] = GetDlgItem(hWnd, IDC_CHECK32);
		MatrixGrid[3][3] = GetDlgItem(hWnd, IDC_CHECK33);
		MatrixGrid[3][4] = GetDlgItem(hWnd, IDC_CHECK34);
		MatrixGrid[3][5] = GetDlgItem(hWnd, IDC_CHECK35);
		MatrixGrid[3][6] = GetDlgItem(hWnd, IDC_CHECK36);
		MatrixGrid[3][7] = GetDlgItem(hWnd, IDC_CHECK37);
		MatrixGrid[3][8] = GetDlgItem(hWnd, IDC_CHECK38);
		MatrixGrid[3][9] = GetDlgItem(hWnd, IDC_CHECK39);
		//FILA 4
		MatrixGrid[4][0] = GetDlgItem(hWnd, IDC_CHECK40);
		MatrixGrid[4][1] = GetDlgItem(hWnd, IDC_CHECK41);
		MatrixGrid[4][2] = GetDlgItem(hWnd, IDC_CHECK42);
		MatrixGrid[4][3] = GetDlgItem(hWnd, IDC_CHECK43);
		MatrixGrid[4][4] = GetDlgItem(hWnd, IDC_CHECK44);
		MatrixGrid[4][5] = GetDlgItem(hWnd, IDC_CHECK45);
		MatrixGrid[4][6] = GetDlgItem(hWnd, IDC_CHECK46);
		MatrixGrid[4][7] = GetDlgItem(hWnd, IDC_CHECK47);
		MatrixGrid[4][8] = GetDlgItem(hWnd, IDC_CHECK48);
		MatrixGrid[4][9] = GetDlgItem(hWnd, IDC_CHECK49);
		//FILA 5
		MatrixGrid[5][0] = GetDlgItem(hWnd, IDC_CHECK50);
		MatrixGrid[5][1] = GetDlgItem(hWnd, IDC_CHECK51);
		MatrixGrid[5][2] = GetDlgItem(hWnd, IDC_CHECK52);
		MatrixGrid[5][3] = GetDlgItem(hWnd, IDC_CHECK53);
		MatrixGrid[5][4] = GetDlgItem(hWnd, IDC_CHECK54);
		MatrixGrid[5][5] = GetDlgItem(hWnd, IDC_CHECK55);
		MatrixGrid[5][6] = GetDlgItem(hWnd, IDC_CHECK56);
		MatrixGrid[5][7] = GetDlgItem(hWnd, IDC_CHECK57);
		MatrixGrid[5][8] = GetDlgItem(hWnd, IDC_CHECK58);
		MatrixGrid[5][9] = GetDlgItem(hWnd, IDC_CHECK59);
		//FILA 6
		MatrixGrid[6][0] = GetDlgItem(hWnd, IDC_CHECK60);
		MatrixGrid[6][1] = GetDlgItem(hWnd, IDC_CHECK61);
		MatrixGrid[6][2] = GetDlgItem(hWnd, IDC_CHECK62);
		MatrixGrid[6][3] = GetDlgItem(hWnd, IDC_CHECK63);
		MatrixGrid[6][4] = GetDlgItem(hWnd, IDC_CHECK64);
		MatrixGrid[6][5] = GetDlgItem(hWnd, IDC_CHECK65);
		MatrixGrid[6][6] = GetDlgItem(hWnd, IDC_CHECK66);
		MatrixGrid[6][7] = GetDlgItem(hWnd, IDC_CHECK67);
		MatrixGrid[6][8] = GetDlgItem(hWnd, IDC_CHECK68);
		MatrixGrid[6][9] = GetDlgItem(hWnd, IDC_CHECK69);
		//FILA 7
		MatrixGrid[7][0] = GetDlgItem(hWnd, IDC_CHECK70);
		MatrixGrid[7][1] = GetDlgItem(hWnd, IDC_CHECK71);
		MatrixGrid[7][2] = GetDlgItem(hWnd, IDC_CHECK72);
		MatrixGrid[7][3] = GetDlgItem(hWnd, IDC_CHECK73);
		MatrixGrid[7][4] = GetDlgItem(hWnd, IDC_CHECK74);
		MatrixGrid[7][5] = GetDlgItem(hWnd, IDC_CHECK75);
		MatrixGrid[7][6] = GetDlgItem(hWnd, IDC_CHECK76);
		MatrixGrid[7][7] = GetDlgItem(hWnd, IDC_CHECK77);
		MatrixGrid[7][8] = GetDlgItem(hWnd, IDC_CHECK78);
		MatrixGrid[7][9] = GetDlgItem(hWnd, IDC_CHECK79);
		//FILA 8
		MatrixGrid[8][0] = GetDlgItem(hWnd, IDC_CHECK80);
		MatrixGrid[8][1] = GetDlgItem(hWnd, IDC_CHECK81);
		MatrixGrid[8][2] = GetDlgItem(hWnd, IDC_CHECK82);
		MatrixGrid[8][3] = GetDlgItem(hWnd, IDC_CHECK83);
		MatrixGrid[8][4] = GetDlgItem(hWnd, IDC_CHECK84);
		MatrixGrid[8][5] = GetDlgItem(hWnd, IDC_CHECK85);
		MatrixGrid[8][6] = GetDlgItem(hWnd, IDC_CHECK86);
		MatrixGrid[8][7] = GetDlgItem(hWnd, IDC_CHECK87);
		MatrixGrid[8][8] = GetDlgItem(hWnd, IDC_CHECK88);
		MatrixGrid[8][9] = GetDlgItem(hWnd, IDC_CHECK89);
		//FILA 9
		MatrixGrid[9][0] = GetDlgItem(hWnd, IDC_CHECK90);
		MatrixGrid[9][1] = GetDlgItem(hWnd, IDC_CHECK91);
		MatrixGrid[9][2] = GetDlgItem(hWnd, IDC_CHECK92);
		MatrixGrid[9][3] = GetDlgItem(hWnd, IDC_CHECK93);
		MatrixGrid[9][4] = GetDlgItem(hWnd, IDC_CHECK94);
		MatrixGrid[9][5] = GetDlgItem(hWnd, IDC_CHECK95);
		MatrixGrid[9][6] = GetDlgItem(hWnd, IDC_CHECK96);
		MatrixGrid[9][7] = GetDlgItem(hWnd, IDC_CHECK97);
		MatrixGrid[9][8] = GetDlgItem(hWnd, IDC_CHECK98);
		MatrixGrid[9][9] = GetDlgItem(hWnd, IDC_CHECK99);
	}

		SetWindowText(EditFilas, INI);
		SetWindowText(EditColumnas, INI);
			
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		
		case IDC_BUTTUPARR:
			if (GridIndex[0] > 0) {
				if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
					aux = true;
				}
				GridIndex[0]--;
				CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
				if (GetDlgCtrlID(LastChecked) != 0) {
					CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
				}
				if (aux == false) {
					CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
				}
				aux = false;
				LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			};
			break;
		case IDC_BUTTDOWNARR:
			if (GridIndex[0] < Creada->Filas) {
				if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
					aux = true;
				}
				GridIndex[0]++;
				CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
				if (GetDlgCtrlID(LastChecked) != 0) {
					CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
				}
				if (aux == false) {
					CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
				}
				aux = false;
				LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			};
			break;
		case IDC_BUTTLEFTARR:
			if (GridIndex[1] > 0) {
				if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
					aux = true;
				}
				GridIndex[1]--;
				CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
				if (GetDlgCtrlID(LastChecked) != 0) {
					CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
				}
				if (aux == false) {
					CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
				}
				aux = false;
				LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			};
			break;
		case IDC_BUTTRIGHTARR:
			if (GridIndex[1] < Creada->Columnas) {
				if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
					aux = true;
				}
				GridIndex[1]++;
				CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
				if (GetDlgCtrlID(LastChecked) != 0) {
					CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
				}
				if (aux == false) {
					CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
				}
				aux = false;
				LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			};
			break;
		case IDC_UPDATE:
			GetWindowText(Edit, txt, sizeof(txt));
			Creada->Matrix[GridIndex[0]][GridIndex[1]] = atof(txt);
			SetWindowText(EditValorAc, txt);

			break;
		case IDC_OK:
		{
			char buffer[3];
			GetWindowText(EditFilas, buffer, sizeof(buffer));
			Creada->Filas = atoi(buffer) - 1;
			GetWindowText(EditColumnas, buffer, sizeof(buffer));
			Creada->Columnas = atoi(buffer) - 1;
			AddToList(Creada, &RaizMatrices);
			ButtonList NuevoBoton;
			NuevoBoton.Matriz = Creada;
			HistorialButton.push_back(NuevoBoton);
			HistorialButton[LastButtonHisto].IDC = LastButtonIDCHisto;
			HistorialButton[LastButtonHisto].Pos = LastButtonPos[1];
			CrearBoton(&HistorialButton[LastButtonHisto], LastButtonPos[0], LastButtonPos[1], buttonSize, HistorialWindow);
			LastButtonIDCHisto++;
			LastButtonPos[1] += buttonSize + 20;
			SetWindowPos(ADD, HWND_TOP, LastButtonPos[0], LastButtonPos[1], 0, 0, SWP_NOSIZE);
			if (GraphMode && (HistorialButton[LastButtonHisto].Matriz->Filas != 3)) {
				EnableWindow(HistorialButton[LastButtonHisto].hwnd, FALSE);
			}
			LastButtonHisto++;
			EndDialog(hWnd, TRUE); //forma correcta de cerrar solo esta ventana
			//EndDialog(ChoiceWindow, TRUE);
		}
		break;

		case IDCANCEL:
			// 
			Delete(&RaizMatrices, Creada);
			KillTimer(hWnd, 102);
			EndDialog(hWnd, FALSE);
			break;

		//Botones de los numeros de la calculadora
			{
		case IDC_BUTTNUM0:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "0");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM1:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "1");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM2:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "2");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM3:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "3");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM4:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "4");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM5:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "5");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM6:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "6");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM7:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "7");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM8:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "8");
			SetWindowText(Edit, txt);

			break;

		case IDC_BUTTNUM9:

			GetWindowText(Edit, txt, sizeof(txt));
			strcat_s(txt, "9");
			SetWindowText(Edit, txt);

			break;
		}

		case IDC_BUTTPOINT:
			if (P == 6480) {
				GetWindowText(Edit, txt, sizeof(txt));
				strcat_s(txt, ".");
				SetWindowText(Edit, txt);
				P = 777;
			}
			break;
		case IDC_BUTTNUMPI:
		{
			SetWindowText(Edit,"3.141592");
			P = 777;
		}
		break;
		case IDC_RADIOALT:
		case IDC_RADIOINV:
		{
			if (!IsDlgButtonChecked(hWnd, IDC_RADIOALT)) {
				if (IsDlgButtonChecked(hWnd, IDC_RADIOINV)) {
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONROOT), "x^2");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONCOS), "Cos^-1");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONSEN), "Sen^-1");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONTAN), "Tan^-1");
				}
				else {
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONROOT), "v");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONCOS), "Cos");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONSEN), "Sen");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONTAN), "Tan");
				}
			}
			else {
				if (IsDlgButtonChecked(hWnd, IDC_RADIOINV)) {
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONROOT), "x^y");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONCOS), "Sec^-1");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONSEN), "Csc^-1");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONTAN), "Cot^-1");
				}
				else {
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONROOT), "xvy");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONCOS), "Sec");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONSEN), "Csc");
					SetWindowText(GetDlgItem(hWnd, IDC_BUTTONTAN), "Cot");
				}
			}
		}
		break;
		case IDC_BUTTOPERMAS:
			GetWindowText(Edit, txt, sizeof(txt));
			if (Pos == false) {
				Oper = '+';
				SetWindowText(Static_Op1, txt);
				SetWindowText(Static_Sig, "+");
				SetWindowText(Edit, "");
				Pos = true;
			}
			else {
				SetWindowText(Static_Op2, txt);
				Val2 = atof(txt);
				GetWindowText(Static_Op1, txt, sizeof(txt));
				Val1 = atof(txt);
				Res = Calcu(Val1, Oper, Val2);
				sprintf_s(txt, "%f", Res);
				SetWindowText(Edit, txt);
				Acortar(Edit, txt, &P);
				Pos = false;
			}
			P = 6480;
			break;
		case IDC_BUTTOPERMEN:
			GetWindowText(Edit, txt, sizeof(txt));
			if (Pos == false) {
				Oper = '-';
				SetWindowText(Static_Op1, txt);
				SetWindowText(Static_Sig, "-");
				SetWindowText(Edit, "");
				Pos = true;
			}
			else {
				SetWindowText(Static_Op2, txt);
				Val2 = atof(txt);
				GetWindowText(Static_Op1, txt, sizeof(txt));
				Val1 = atof(txt);
				Res = Calcu(Val1, Oper, Val2);
				sprintf_s(txt, "%f", Res);
				SetWindowText(Edit, txt);
				Acortar(Edit, txt, &P);
				Pos = false;
			}
			P = 6480;
			break;

		case IDC_BUTTOPERMUL:
			GetWindowText(Edit, txt, sizeof(txt));
			if (Pos == false) {
				Oper = '*';
				SetWindowText(Static_Op1, txt);
				SetWindowText(Static_Sig, "*");
				SetWindowText(Edit, "");
				Pos = true;
			}
			else {
				SetWindowText(Static_Op2, txt);
				Val2 = atof(txt);
				GetWindowText(Static_Op1, txt, sizeof(txt));
				Val1 = atof(txt);
				Res = Calcu(Val1, Oper, Val2);
				sprintf_s(txt, "%f", Res);
				SetWindowText(Edit, txt);
				Acortar(Edit, txt, &P);
				Pos = false;
			}
			P = 6480;
			break;

		case IDC_BUTTOPERDIV:
			GetWindowText(Edit, txt, sizeof(txt));
			if (Pos == false) {
				Oper = '/';
				SetWindowText(Static_Op1, txt);
				SetWindowText(Static_Sig, "/");
				SetWindowText(Edit, "");
				Pos = true;
			}
			else {
				SetWindowText(Static_Op2, txt);
				Val2 = atof(txt);
				GetWindowText(Static_Op1, txt, sizeof(txt));
				Val1 = atof(txt);
				Res = Calcu(Val1, Oper, Val2);
				sprintf_s(txt, "%f", Res);
				SetWindowText(Edit, txt);
				Acortar(Edit, txt, &P);
				Pos = false;
			}
			P = 6480;
			break;
		case IDC_BUTTPERCEN:
		{
			GetWindowText(Edit, txt, sizeof(txt));
			Val1 = atof(txt);
			Val1 = Val1 / 100;
			sprintf_s(txt, "%f", Val1);
			SetWindowText(Edit, txt);
			Acortar(Edit, txt, &P);
			P = 777;
		}
		break;
		case IDC_BUTTFACTO:
		{
			GetWindowText(Edit, txt, sizeof(txt));
			Val1 = atof(txt);
			for (int I = round(Val1)-1; I > 0; I--) {
				Val1 = Val1 * I;
			}
			sprintf_s(txt, "%f", Val1);
			SetWindowText(Edit, txt);
			Acortar(Edit, txt, &P);
			P = 777;
		
		}
		break;
		case IDC_BUTTONROOT:
		{
			char oper[7] = { NULL };
			GetWindowText(GetDlgItem(hWnd, IDC_BUTTONROOT), oper, sizeof(oper));
			if (strcmp(oper,"v")==0) {
				GetWindowText(Edit,txt,sizeof(txt));
				sprintf_s(txt, "%f", sqrt(atoi(txt)));
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "x^2") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				sprintf_s(txt, "%f", pow(atoi(txt),2));
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "xvy") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (Pos == false) {
					Oper = 'v';
					SetWindowText(Static_Op1, txt);
					SetWindowText(Static_Sig, "v");
					SetWindowText(Edit, "");
					Pos = true;
				}
				else {
					SetWindowText(Static_Op2, txt);
					Val2 = atof(txt);
					GetWindowText(Static_Op1, txt, sizeof(txt));
					Val1 = atof(txt);
					Res = Calcu(Val1, Oper, Val2);
					sprintf_s(txt, "%f", Res);
					SetWindowText(Edit, txt);
					Pos = false;
				}
				P = 6480;
			}
			else if (strcmp(oper, "x^y") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (Pos == false) {
					Oper = '^';
					SetWindowText(Static_Op1, txt);
					SetWindowText(Static_Sig, "^");
					SetWindowText(Edit, "");
					Pos = true;
				}
				else {
					SetWindowText(Static_Op2, txt);
					Val2 = atof(txt);
					GetWindowText(Static_Op1, txt, sizeof(txt));
					Val1 = atof(txt);
					Res = Calcu(Val1, Oper, Val2);
					sprintf_s(txt, "%f", Res);
					SetWindowText(Edit, txt);
					Pos = false;
				}
				P = 6480;
			}
			Acortar(Edit, txt, &P);
		}
		break;
		case IDC_BUTTONCOS:
		{
			char oper[7] = { NULL };
			GetWindowText(GetDlgItem(hWnd, IDC_BUTTONCOS), oper, sizeof(oper));
			if (strcmp(oper, "Cos") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", cos(atoi(txt)));
				}
				else { 
					sprintf_s(txt, "%f", cos(atof(txt)*3.141592 / 180)); 
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Cos^-1") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", acos(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", acos(atof(txt))*(180 / 3.141592));
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Sec") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", 1/cos(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", 1/cos(atof(txt)*3.141592 / 180));
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Sec^-1") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", 1/acos(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", 1/acos(atof(txt))*(180 / 3.141592));
				}
				SetWindowText(Edit, txt);
			}
			Acortar(Edit, txt, &P);
		}
		break;
		case IDC_BUTTONSEN:
		{
			char oper[7] = { NULL };
			GetWindowText(GetDlgItem(hWnd, IDC_BUTTONSEN), oper, sizeof(oper));
			if (strcmp(oper, "Sen") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", sin(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", sin(atof(txt)*3.141592 / 180));
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Sen^-1") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", asin(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", asin(atof(txt))*(180 / 3.141592));
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Csc") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", 1/sin(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", 1/sin(atof(txt)*3.141592 / 180));
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Csc^-1") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", 1/asin(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f",1/asin(atof(txt))*(180 / 3.141592));
				}
				SetWindowText(Edit, txt);
			}
			Acortar(Edit, txt, &P);
		}
		break;
		case IDC_BUTTONTAN:
		{
			char oper[7] = { NULL };
			GetWindowText(GetDlgItem(hWnd, IDC_BUTTONTAN), oper, sizeof(oper));
			if (strcmp(oper, "Tan") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", tan(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", tan(atof(txt)*3.141592 / 180));
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Tan^-1") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", atan(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", atan(atof(txt))*(180 / 3.141592));
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Cot") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", 1/tan(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", 1/tan(atof(txt)*3.141592 / 180));
				}
				SetWindowText(Edit, txt);
			}
			else if (strcmp(oper, "Cot^-1") == 0) {
				GetWindowText(Edit, txt, sizeof(txt));
				if (IsDlgButtonChecked(hWnd, IDC_RADIOIAN)) {
					sprintf_s(txt, "%f", 1/atan(atof(txt)));
				}
				else {
					sprintf_s(txt, "%f", 1/atan(atof(txt))*(180 / 3.141592));
				}
				SetWindowText(Edit, txt);
			}
			Acortar(Edit, txt, &P);
		}
		break;
		case IDC_BUTTONCLE:

			SetWindowText(Static_Op1, "");
			SetWindowText(Static_Op2, "");
			SetWindowText(Static_Sig, "");
			SetWindowText(Edit, "");
			P = 6480;
			Pos = false;
			Oper = NULL;
			break;
		case IDC_BUTTONIGU:
			GetWindowText(Edit, txt, sizeof(txt));
			SetWindowText(Static_Op2, txt);
			GetWindowText(Static_Op2, txt, sizeof(txt));
			Val2 = atof(txt);
			GetWindowText(Static_Op1, txt, sizeof(txt));
			Val1 = atof(txt);
			Res = Calcu(Val1, Oper, Val2);
			sprintf_s(txt, "%f", Res);
			SetWindowText(Edit, txt);
			Acortar(Edit, txt, &P);
			break;
		case IDC_BUTTONBORR:
		{
			GetWindowText(Edit, txt, sizeof(txt));
			int I = strlen(txt);
			if (I != 0) {
				if (txt[I - 1] == '.') {
					P = 6480;
				}

				txt[I - 1] = NULL;
				SetWindowText(Edit, txt);
			}
		}
		break;
			//Casos de TODOS LOS CHECK
			{
		case IDC_CHECK00:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
		}
		break;
		case IDC_CHECK01:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
		}
			break;
		case IDC_CHECK02:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
		}
			break;
		case IDC_CHECK03:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
		}
			break;
		case IDC_CHECK04:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK05:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK06:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK07:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK08:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK09:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 0;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK10:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK11:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK12:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK13:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK14:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK15:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK16:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK17:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK18:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK19:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 1;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK20:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK21:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK22:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK23:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK24:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK25:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK26:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK27:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK28:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK29:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 2;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK30:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK31:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK32:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK33:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK34:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK35:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK36:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK37:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK38:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK39:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 3;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK40:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK41:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK42:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK43:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK44:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK45:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK46:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK47:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK48:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK49:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 4;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK50:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK51:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK52:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK53:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK54:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK55:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK56:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK57:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK58:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK59:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 5;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK60:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK61:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK62:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK63:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK64:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK65:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK66:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK67:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK68:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK69:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 6;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK70:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK71:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK72:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK73:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK74:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK75:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK76:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK77:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK78:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK79:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 7;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK80:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK81:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK82:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK83:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK84:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK85:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK86:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK87:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK88:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK89:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 8;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK90:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 9;
			GridIndex[1] = 0;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK91:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 9;
			GridIndex[1] = 1;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK92:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 9;
			GridIndex[1] = 2;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK93:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 9;
			GridIndex[1] = 3;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK94:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 9;
			GridIndex[1] = 4;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK95:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 9;
			GridIndex[1] = 5;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
		}
		break;
		case IDC_CHECK96:
		{
			GridIndex[0] = 9;
			GridIndex[1] = 6;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK97:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 9;
			GridIndex[1] = 7;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK98:
		{
			if (Creada->Matrix[GridIndex[0]][GridIndex[1]] != 0) {
				aux = true;
			}
			GridIndex[0] = 9;
			GridIndex[1] = 8;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
		case IDC_CHECK99:
		{
			GridIndex[0] = 9;
			GridIndex[1] = 9;
			CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[GridIndex[0]][GridIndex[1]]), BST_CHECKED);
			if (GetDlgCtrlID(LastChecked) != 0) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_INDETERMINATE);
			}
			if (aux == false) {
				CheckDlgButton(hWnd, GetDlgCtrlID(LastChecked), BST_UNCHECKED);
			}
			aux = false;
			LastChecked = MatrixGrid[GridIndex[0]][GridIndex[1]];
			}
			break;
			}

		}
		break;
		case WM_TIMER:
		{
			char NumF[3];
			char NumC[3];
			GetWindowText(EditFilas, NumF, sizeof(NumF));
			GetWindowText(EditColumnas, NumC, sizeof(NumC));
			if (!(atoi(NumF) == (Creada->Filas+1)) || !(atoi(NumC) == (Creada->Columnas+1))) {

				if (atoi(NumF) > 10) {
					strcpy_s(NumF, "10");
					SetWindowText(EditFilas, NumF);
				}
				else if (atoi(NumF) < 0) {
					strcpy_s(NumF, "0");
					SetWindowText(EditFilas, NumF);
				}
				if (atoi(NumC) > 10) {
					strcpy_s(NumC, "10");
					SetWindowText(EditColumnas, NumC);
				}
				else if (atoi(NumC) < 0) {
					strcpy_s(NumC, "0");
					SetWindowText(EditColumnas, NumC);
				}


				Creada->Filas = atoi(NumF) - 1;
				Creada->Columnas = atoi(NumC) - 1;
				for (int I = 0; I < 10; I++) {
					for (int J = 0; J < 10; J++) {
						if ((I < atoi(NumF)) && (J < atoi(NumC))) {
							EnableWindow(MatrixGrid[I][J], true);
						}
						else {
							EnableWindow(MatrixGrid[I][J], false);
							Creada->Matrix[I][J] = 0.0;
							CheckDlgButton(hWnd, GetDlgCtrlID(MatrixGrid[I][J]), BST_UNCHECKED);
						}
					}
				}
			}

			if ((atoi(NumF) > 0) && (atoi(NumC) > 0)){
				EnableWindow(ButtonOK,true);
			}
			else {
				EnableWindow(ButtonOK,false);
			}
		}
		
		sprintf_s(txt, "%f", Creada->Matrix[GridIndex[0]][GridIndex[1]]);
		SetWindowText(EditValorAc, txt);
		break;

	case WM_CLOSE:
		KillTimer(hWnd, 102);
		EndDialog(hWnd, FALSE);
		return 0;
		break;


	default:

		return FALSE;
	}

	return TRUE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL ret;
	hInst = hInstance;
	//GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	//La principal
	HWND hCalculator = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_CALCULADORA), 0, (DLGPROC)CalculatorProc, 0);
	ShowWindow(hCalculator, nCmdShow);
	CalculatorWindow = hCalculator;

	//La de utilidad

	//HWND hCreator = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_CREARNUEVO), 0, (DLGPROC)CreatorProc, 0);
	//ShowWindow(hCreator, nCmdShow);


	//Las hijas

	HWND hHistorial = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_HISTORIAL), 0, (DLGPROC)HistorialProc, 0);
	SetParent(hHistorial, hCalculator);
	//ShowWindow(hHistorial, nCmdShow);

	HWND hOperation = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_OPERACIONES), 0, (DLGPROC)OperationProc, 0);
	SetParent(hOperation, hCalculator);
	//ShowWindow(hOperation, nCmdShow);

	/*HWND hGraficator = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_GRAFICADOR), 0, (DLGPROC)GraphProc, 0);
	SetParent(hGraficator, hCalculator);*/

	//HWND hChoice = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOGCHOOSE), 0, (DLGPROC)ChoiceProc, 0);
	//ShowWindow(hChoice, nCmdShow);

	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret == -1)
			return -1;

		if (!IsDialogMessage(hCalculator, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	
	return 0;
}



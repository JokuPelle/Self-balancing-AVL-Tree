//----------------------------------------------------
// BM40A0301 Tietorakenteet ja algoritmit
// Harjoitustyö
// Juuso Ylikoski 0526228
// 20.11.2019
//----------------------------------------------------
// Tämän harjoitustyön tarkoituksena on luoda AVL-binääripuu,
// johon voidaan manuaalisesti lisätä solmuja. Puu järjestää
// itsensä pitääkseen itsensä tasapainotettuna.
// 
// Projektin käännösnimet tulevat kierron suunnan mukaan:
// LL: Puuta käännetään vasemmalle (syynä oikean puolen ylipituus)
// RL: Alipuuta käännetään oikealle ja puuta vasemmalle (syynä
//     oikean alipuun vasemman puolen ylipituus
// RR: Puuta käännetään oikealle
// LR: Alipuuta käännetään vasemmalle ja puuta oikealle
//----------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

//---Puun solmuna käytetty tietorakenne---
// Puulla arvo, tasapainoindeksi ja osoitteet
// vasempaan ja oikeaan haaraan
//----------------------------------------
typedef struct Node {
  int val;
  short bal;
  struct Node *left, *right;
} nodestruct, *node;

//----------Käytetyt aliohjelmat----------
void addToTree(node *base, int key, int * unbalanced, node *tree);
void printTree(node tree, int level);
void turnLeft(node *base, int * unbalanced);
void turnRight(node *base, int * unbalanced);
void searchKey(node tree, int key);

//----------------------------------------
//---------------Pääohjelma---------------
// Rakentaa alkuperäisen puun "input.txt"-tiedostosta
// Käynnistää valikon
// unbalanced-muuttuja kertoo onko puu tasapainotettu
//----------------------------------------
int main(void) {
	int num, choice, key, unbalanced = 0;
	node tree = NULL;
	//int original[] = {2,4,6,8,10,12,14,30,28};
	//File reading and initial tree building
	FILE* fs;
	fs = fopen("input.txt", "r");
	if (fs == NULL) {
		printf("Failed to open file");
		return -1;
	}
	while (1) {
		fscanf(fs, "%d,", &num);
		if (feof(fs)) break;
		addToTree(&tree, num, &unbalanced, &tree);
		printTree(tree, 0);
		printf("\n");
	}
	fclose(fs);
	//Start loop
	while (1) {
		printf("1) Add a node\n2) Search for a node\n0) End program\n\nYour choice: ");
		scanf("%d", &choice);
		switch (choice) {
			case 1:
				printf("Enter key: ");
				scanf("%d", &key);
				addToTree(&tree, key, &unbalanced, &tree);
				printTree(tree,0);
				break;
			case 2:
				printf("Enter key: ");
				scanf("%d", &key);
				searchKey(tree, key);
				break;
			case 0:
				return 0;
		}
	}
	return 0;
}

//----------Käytetyt aliohjelmat----------

// ---searchKey---
// Etsii halutun avaimen järjestetystä puusta
// Liikkuu vasempaan tai oikeaan lapseen
// kunnes löytää avaimen tai päätyy puun loppuun
//----------------------------------------
void searchKey(node base, int key){
	printf("\n");
	while(1) {
		printf("%d ", base->val);
		if (base->val == key){
			printf("\nKey was found!\n\n");
			return;
		}
		if (key < base->val && base->left != NULL) {
			printf("left, ");
			base = base->left;
		} else if (key >= base->val && base->right != NULL) {
			printf("right, ");
			base = base->right;
		} else {
			printf("\nKey was not found!\n\n");
			return;
		}
	}
}

// ---printTree---
// Tulostaa puun sivusuunnassa rekursiivisesti
//----------------------------------------
void printTree(node base, int level) {
	if (base == NULL) return;
	level++;
	printTree(base->right, level);
	for (int i=0; i<level; i++) {
		printf("      ");
	}
	printf("%d(%d)\n", base->val, base->bal);
	printTree(base->left, level);
}

// ---addToTree---
// Lisää annetun avaimen tämänhetkiseen juureen.
// Rekursiivinen metodi, joka puuta läpikäydessä muokkaa
// solmujen tasapainoindeksiä riippuen avaimen ja solmun
// arvojen erosta.
// Jos huomataan, että puun vasemman ja oikean haaran
// pituusero on enemmän kuin 1, suoritetaan käännös.
//----------------------------------------
void addToTree(node *base, int key, int *unbalanced, node *tree) {
	if (!(*base)){
		//Create a node on the current base (empty child)
		*unbalanced = 1;
		if ((*base = (node)malloc(sizeof(nodestruct))) == NULL) {
		perror("Error: Couldn't allocate memory.");
		exit(0);
		}
		(*base)->left = NULL;
		(*base)->right = NULL;
		(*base)->val = key;
		(*base)->bal = 0;
		printf("New node %d added.\n", key);
	} else if (key < (*base)->val) {
		//Add key to the left child
		addToTree(&(*base)->left, key, unbalanced, &(*tree));
		if (*unbalanced != 0){
			switch((*base)->bal){
				case 0: //Base has no children
					(*base)->bal = -1;
					break;
				case 1: //Base has a right child
					(*base)->bal = 0;
					*unbalanced = 0;
					break;
				case -1: //Base already has a left child !!DANGER!!
					printTree(*tree, 0);
					printf("Left side is too large.\nTurn parent %d. ", (*base)->val);
					turnRight(base, unbalanced);
			}
		}
	} else if (key > (*base)->val) {
		//Add key to the right child
		addToTree(&(*base)->right, key, unbalanced, &(*tree));
		if (*unbalanced != 0){
			switch((*base)->bal){
				case 0: //Base has no children
					(*base)->bal = 1;
					break;
				case -1: //Base has a left child
					(*base)->bal = 0;
					*unbalanced = 0;
					break;
				case 1: //Base already has a right child !!DANGER!!
					printTree(*tree, 0);
					printf("Right side is too large.\nTurn parent %d. ", (*base)->val);
					turnLeft(base, unbalanced);
			}
		}
	} else {
		//Key already in tree
		printf("Key already exists in tree.\n");
		*unbalanced = 0;
	}
}

// ---turnRight---
// Suoritetaan, jos puun vasen puoli on liian pitkä.
// Jos puu on täysin vasemmalle kaartunut, suoritetaan RR,
// muussa tapauksessa LR
//----------------------------------------
void turnRight(node *base, int *unbalanced) {
	node child, grandchild;
	child = (*base)->left;
	if (child->bal == -1) { //RR-Turn (Spin tree to the right)
		printf("Execute RR-turn!\n");
		(*base)->left = child->right;
		child->right = *base;
		(*base)->bal = 0; //Unnecessary??
		(*base) = child;
	} else { //LR-Turn (Spin child tree to the left and main tree to the right)
		printf("Execute LR-turn!\n");
		grandchild = child->right;
		child->right = grandchild->left;
		grandchild->left = child;
		(*base)->left = grandchild->right;
		grandchild->right = *base;
		switch(grandchild->bal) {
			case -1: //Grandchild had left child
				(*base)->bal = 1;
				child->bal = 0;
				break;
			case 0: //Left and tight child
				(*base)->bal = 0;
				child->bal = 0;
				break;
			case 1: //Right child
				(*base)->bal = 0;
				child->bal = -1;
		}
		(*base) = grandchild;
	}
	(*base)->bal = 0;
	*unbalanced = 0;
}

// ---turnLeft---
// Suoritetaan, jos puun oikea puoli on liian pitkä.
// Jos puu on täysin oikealle kaartunut, suoritetaan LL,
// muussa tapauksessa RL
//----------------------------------------
void turnLeft(node *base, int *unbalanced) {
	node child, grandchild;
	child = (*base)->right;
	if (child->bal == 1) { //LL-Turn (Spin tree to the left)
		printf("Execute LL-turn!\n");
		(*base)->right = child->left;
		child->left = *base;
		(*base)->bal = 0; //Unnecessary??
		(*base) = child;
	} else { //RL-Turn (Spin child tree to the right and main tree to the left)
		printf("Execute RL-turn\n");
		grandchild = child->left;
		child->left = grandchild->right;
		grandchild->right = child;
		(*base)->right = grandchild->left;
		grandchild->left = *base;
	switch(grandchild->bal) {
		case 1: //Grandchild had left child
			(*base)->bal = -1;
			child->bal = 0;
			break;
		case 0: //Left and tight child
			(*base)->bal = 0;
			child->bal = 0;
			break;
		case -1: //Right child
			(*base)->bal = 0;
			child->bal = 1;
		}
		(*base) = grandchild;
	}
	(*base)->bal = 0;
	*unbalanced = 0;
}

//----End of File-------------------------------------------------
//----------------------------------------------------------------

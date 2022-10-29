/*
* File: Boggle.cpp
* ----------------
* Name: [TODO: enter name here]
* Section: [TODO: enter section leader here]
* This file is the main starter file for Assignment #4, Boggle.
* [TODO: extend the documentation]
*/

#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"
#include <string>
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;

const string STANDARD_CUBES[16] = {
	"AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
	"AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
	"DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
	"EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const string BIG_BOGGLE_CUBES[25] = {
	"AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
	"AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
	"CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
	"DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
	"FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

const int BOARD_NUM_ROWS = 4;
const int BOARD_NUM_COLS = 4;
const int CUBE_NUM_SIDE = 6;
const int MIN_WORD_SIZE = 4;

/* Function prototypes */

void welcome();
void giveInstructions();
void generateBoard(Grid<char>& boardG);
void selectGenerateCombinationMode(Vector<string>& cubesV);
void userEntersCubesCombination(Vector<string>& cubesV);
void copyConstants(Vector<string>& cubesV);
void randomSwapCubes(Vector<string>& cubesV);
void swapElements(Vector<string>& cubesV, int index1, int index2);
void randomRollCubes(Grid<char>& boardG, Vector<string>& cubesV);
void enterWords(Lexicon& lexiconL, Grid<char>& boardG, Set<string>& foundWordsS, Player player);
bool isValidWord(string word, Lexicon& lexiconL, Grid<char>& boardG, Set<string>& foundWordsS);
bool isWordOnBoard(string word, Grid<char>& boardG);
void fillGridWithBool(Grid<bool>& grid, bool b);
bool recIsWordOnBoard(string word, Grid<char>& boardG, Grid<bool>& usedLettersG, int row, int col);
void highlightCubes(Grid<bool>& usedLettersG);
void clearHighlightedCubes();
void findWords(Grid<char>& boardG, Lexicon& lexiconL, Set<string>& foundWordsS, Player computer);
void recFindWords(Grid<char>& boardG, Lexicon& lexiconL, Set<string>& foundWordsS,
	Grid<bool>& usedLettersG, int row, int col, string soFar, Player computer);
bool playAgain();

/* Main program */

int main()
{
	GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
	initGBoggle(gw);
	welcome();
	giveInstructions();

	Player human = HUMAN;
	Player computer = COMPUTER;

	Grid<char> boardG(BOARD_NUM_ROWS, BOARD_NUM_COLS);
	drawBoard(BOARD_NUM_ROWS, BOARD_NUM_COLS);

	while (true)
	{
		gw.repaint();
		generateBoard(boardG);

		Lexicon lexiconL("EnglishWords.dat");
		Set<string> foundWordsS;
		enterWords(lexiconL, boardG, foundWordsS, human); //user enters found words in consol

		findWords(boardG, lexiconL, foundWordsS, computer); //computer finds remaining words on board

		if (!playAgain()) break;
	}

	cout << endl << "... THE END ..." << endl;
	return 0;
};

/*
* Function: welcome
* Usage: welcome();
* -----------------
* Print out a cheery welcome message.
*/

void welcome()
{
	cout << "Welcome!  You're about to play an intense game ";
	cout << "of mind-numbing Boggle.  The good news is that ";
	cout << "you might improve your vocabulary a bit.  The ";
	cout << "bad news is that you're probably going to lose ";
	cout << "miserably to this little dictionary-toting hunk ";
	cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
* Function: giveInstructions
* Usage: giveInstructions();
* --------------------------
* Print out the instructions for the user.
*/

void giveInstructions()
{
	cout << endl;
	cout << "The boggle board is a grid onto which I ";
	cout << "I will randomly distribute cubes. These ";
	cout << "6-sided cubes have letters rather than ";
	cout << "numbers on the faces, creating a grid of ";
	cout << "letters on which you try to form words. ";
	cout << "You go first, entering all the words you can ";
	cout << "find that are formed by tracing adjoining ";
	cout << "letters. Two letters adjoin if they are next ";
	cout << "to each other horizontally, vertically, or ";
	cout << "diagonally. A letter can only be used once ";
	cout << "in each word. Words must be at least four ";
	cout << "letters long and can be counted only once. ";
	cout << "You score points based on word length: a ";
	cout << "4-letter word is worth 1 point, 5-letters ";
	cout << "earn 2 points, and so on. After your puny ";
	cout << "brain is exhausted, I, the supercomputer, ";
	cout << "will find all the remaining words and double ";
	cout << "or triple your paltry score." << endl << endl;
	cout << "Hit return when you're ready...";
	getLine();
}


//this function generates board with random letters
void generateBoard(Grid<char>& boardG)
{
	Vector<string> cubesV;
	selectGenerateCombinationMode(cubesV);

	randomSwapCubes(cubesV); 
	randomRollCubes(boardG, cubesV);
}


//this function helps user to select board configuration by hand or
//default board configuration
void selectGenerateCombinationMode(Vector<string>& cubesV)
{
	cout << endl << "Do you want to force the board configuration? yes/no: ";

	string answer;
	while (true)
	{
		answer = getLine();
		if (answer == "no" || answer == "yes") break;
		cout << "Please answer yes or no" << endl;
	}

	if (answer == "yes")
	{
		userEntersCubesCombination(cubesV);
	}
	else
	{
		copyConstants(cubesV);
	}

	cout << endl << "Ok, take all the time you want and find all the words you can!";
	cout << "Signal that you're finished by entering an empty line." << endl << endl;
}

//with help of this function user can enters board configuration by hand
void userEntersCubesCombination(Vector<string>& cubesV)
{
	cout << "Enter strings 16 times to identify cubes which cubes you want to on the game board.";
	cout << "The first 4 cubes are on the top row from left to right, ";
	cout << "the next 4 cubes are the second row and so on." << endl;

	for (int i = 0; i < BOARD_NUM_ROWS * BOARD_NUM_COLS; i++)
	{
		cubesV.add(toUpperCase(getLine("Enter the string: ")));
	}
}

//this function copies constants(in this case: letter) in Vector
void copyConstants(Vector<string>& cubesV)
{
	for (int i = 0; i < BOARD_NUM_ROWS * BOARD_NUM_COLS; i++)
	{
		cubesV.add(STANDARD_CUBES[i]);
	}
}

//this function swaps elements in vector randomly
void randomSwapCubes(Vector<string>& cubesV)
{
	int randomNum;
	for (int i = 0; i < cubesV.size(); i++)
	{
		randomNum = randomInteger(i, cubesV.size() - 1);
		swapElements(cubesV, i, randomNum);
	}
}

//function just swaps two elements in vector
void swapElements(Vector<string>& cubesV, int index1, int index2)
{
	string value1 = cubesV[index1];
	cubesV[index1] = cubesV[index2];
	cubesV[index2] = value1;
}

//function makes simulation of randomly rolling cubes
void randomRollCubes(Grid<char>& boardG, Vector<string>& cubesV)
{
	string cube;
	char letter;
	for (int i = 0; i < boardG.nRows; i++)
	{
		for (int j = 0; j < boardG.nCols; j++)
		{
			cube = cubesV[i * boardG.nCols + j];
			letter = cube[randomInteger(0, CUBE_NUM_SIDE - 1)];
			boardG[i][j] = letter;
			labelCube(i, j, letter);
		}
	}
}

//in this function, user can enter found words in consle
void enterWords(Lexicon& lexiconL, Grid<char>& boardG, Set<string>& foundWordsS, Player player)
{
	string word;

	while (true)
	{
		word = getLine("Enter word: ");

		if (word == "") break;

		if (isValidWord(word, lexiconL, boardG, foundWordsS)) //Checks if the entered word is valid
		{
			foundWordsS.add(word); //add word in found words list
			recordWordForPlayer(word, player); //count score
			clearHighlightedCubes(); 
		}
		else
		{
			cout << "You can't make that word!" << endl;
		}
	}
}
//this function checks if entered word from console is valid
bool isValidWord(string word, Lexicon& lexiconL, Grid<char>& boardG, Set<string>& foundWordsS)
{
	//if size of word is less than 4 or word isn't in english lexicon or word is already entered then return false
	if (word.size() < MIN_WORD_SIZE || !lexiconL.contains(word) || foundWordsS.contains(word))
	{
		return false;
	}

	if (isWordOnBoard(word, boardG)) //checking if user can make this word on board
	{
		return true;
	}

	return false;
}

//this function checks all letters on board and recursively checks for all letters one by one to
//find if user can make this word on board
bool isWordOnBoard(string word, Grid<char>& boardG)
{
	Grid<bool> usedLettersG(BOARD_NUM_ROWS, BOARD_NUM_COLS);
	fillGridWithBool(usedLettersG, false);

	for (int i = 0; i < boardG.nRows; i++)
	{
		for (int j = 0; j < boardG.nCols; j++)
		{
			if (boardG[i][j] == toupper(word[0]))
			{
				if (recIsWordOnBoard(word, boardG, usedLettersG, i, j)) //recursive
				{
					highlightCubes(usedLettersG);
					return true;
				}
			}
		}
	}

	return false;
}

void fillGridWithBool(Grid<bool>& grid, bool b)
{
	for (int i = 0; i < grid.nRows; i++)
	{
		for (int j = 0; j < grid.nCols; j++)
		{
			grid[i][j] = b;
		}
	}
}


//recursively sub function of isWordOnBoard
bool recIsWordOnBoard(string word, Grid<char>& boardG, Grid<bool>& usedLettersG, int row, int col)
{
	if (word.size() == 0)
	{
		return true;
	}

	for (int i = row - 1; i <= row + 1; i++)
	{
		for (int j = col - 1; j <= col + 1; j++)
		{
			if (boardG.inBounds(i, j) && usedLettersG[i][j] == false && toupper(word[0]) == boardG[i][j])
			{
				usedLettersG[i][j] = true;

				if (recIsWordOnBoard(word.substr(1), boardG, usedLettersG, i, j))
				{
					return true;
				}

				usedLettersG[i][j] = false;
			}
		}
	}

	return false;
}

void highlightCubes(Grid<bool>& usedLettersG)
{
	for (int i = 0; i < usedLettersG.nRows; i++)
	{
		for (int j = 0; j < usedLettersG.nCols; j++)
		{
			highlightCube(i, j, usedLettersG[i][j]);
		}
	}
}

void clearHighlightedCubes()
{
	for (int i = 0; i < BOARD_NUM_ROWS; i++)
	{
		for (int j = 0; j < BOARD_NUM_COLS; j++)
		{
			highlightCube(i, j, false);
		}
	}
}

//computer finds remaining words on board
void findWords(Grid<char>& boardG, Lexicon& lexiconL, Set<string>& foundWordsS, Player computer)
{
	Grid<bool> usedLettersG(BOARD_NUM_COLS, BOARD_NUM_COLS);
	fillGridWithBool(usedLettersG, false);
	string empty = "";

	for (int i = 0; i < boardG.nRows; i++)
	{
		for (int j = 0; j < boardG.nCols; j++)
		{
			usedLettersG[i][j] = true;
			empty = tolower(boardG[i][j]);
			recFindWords(boardG, lexiconL, foundWordsS, usedLettersG,
				i, j, empty, computer);
			usedLettersG[i][j] = false;
			empty = "";
		}
	}
}

void recFindWords(Grid<char>& boardG, Lexicon& lexiconL, Set<string>& foundWordsS,
	Grid<bool>& usedLettersG, int row, int col, string soFar, Player computer)
{
	if (soFar.size() >= MIN_WORD_SIZE && lexiconL.contains(soFar) && !foundWordsS.contains(soFar))
	{
		foundWordsS.add(soFar);
		recordWordForPlayer(soFar, computer);
	}


	for (int i = row - 1; i <= row + 1; i++)
	{
		for (int j = col - 1; j <= col + 1; j++)
		{
			if (boardG.inBounds(i, j) && usedLettersG[i][j] == false &&
				lexiconL.containsPrefix(soFar + boardG[i][j]))
			{
				usedLettersG[i][j] = true;
				soFar += tolower(boardG[i][j]);

				recFindWords(boardG, lexiconL, foundWordsS, usedLettersG,
					i, j, soFar, computer);

				usedLettersG[i][j] = false;
				soFar = soFar.substr(0, soFar.size() - 1);
			}
		}
	}

}

bool playAgain()
{
	string str;

	while (true)
	{
		cout << endl;
		str = getLine("Would you like to play again? yes/no: ");

		if (str == "yes")
		{
			return true;
		}
		else if (str == "no")
		{
			return false;
		}


		cout << "Please answer yes or no." << endl;
	}
}
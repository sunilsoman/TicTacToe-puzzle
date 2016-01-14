#include<iostream>
#include<cstdlib>
#include<ctype.h>
#include<cstring>
#include<deque>
#include<algorithm>
#include<map>
#include<string>
#include<iterator>
using namespace std;

int result;
int xCount;
int oCount;
long nodesExplored=-1;
bool gameOver;
char winner;
int alphaCut=0;
int betaCut=0;
int abSize=0;
int rotationCount = 0;
bool cutOff;
int *killerPtr;

struct Node
{
		char *gameState;
		int value;
		int level;
		bool terminal;
		deque<Node *> childNodes;		
		int eVal;
		// rotation Invariance states
		string str1;
		string str2;
		string str3;

}*front=NULL;

map<string, int> ticMap;

void printState(char gState[])
{
		if(gState==NULL)
				return;
		cout<<"\n\n";
		for(int i = 0; i<9; i++)
		{
				if(i==3||i==6)
						cout<<"\n";
				cout<<gState[i]<<" ";
		}
}

int findHeuristicValue(char gState[])
{
		int xValue=0,oValue=0;
		//row wise check for X
		if(gState[0]!='X' && gState[1]!='X' && gState[2]!='X')
				++oValue;

		if(gState[3]!='X' && gState[4]!='X' && gState[5]!='X')
				++oValue;

		if(gState[6]!='X' && gState[7]!='X' && gState[8]!='X')
				++oValue;

		//Column wise check for X
		if(gState[0]!='X' && gState[3]!='X' && gState[6]!='X')
				++oValue;

		if(gState[1]!='X' && gState[4]!='X' && gState[7]!='X')
				++oValue;

		if(gState[2]!='X' && gState[5]!='X' && gState[8]!='X')
				++oValue;

		//Diagonal check for X
		if(gState[0]!='X' && gState[4]!='X' && gState[8]!='X')
				++oValue;

		if(gState[2]!='X' && gState[4]!='X' && gState[6]!='X')
				++oValue;

		//Row wise check for O
		if(gState[0]!='O' && gState[1]!='O' && gState[2]!='O')
				++xValue;

		if(gState[3]!='O' && gState[4]!='O' && gState[5]!='O')
				++xValue;

		if(gState[6]!='O' && gState[7]!='O' && gState[8]!='O')
				++xValue;

		//Column check for O
		if(gState[0]!='O' && gState[3]!='O' && gState[6]!='O')
				++xValue;

		if(gState[1]!='O' && gState[4]!='O' && gState[7]!='O')
				++xValue;

		if(gState[2]!='O' && gState[5]!='O' && gState[8]!='O')
				++xValue;

		//Diagonal check for O
		if(gState[0]!='O' && gState[4]!='O' && gState[8]!='O')
				++xValue;

		if(gState[2]!='O' && gState[4]!='O' && gState[6]!='O')
				++xValue;

		return xValue-oValue;
}

void checkForWinner(char gState[])
{
		// function checks if any winning checks if any winning state has been attained by either of the players.
		// it performs a row wise, column wise, & diagonal check.
		gameOver = false;
		winner = '-';
		//row wise check
		if(gState[0] == gState[1] && gState[1] == gState[2] && (gState[0]!='_'))
		{
				gameOver=true;
				winner = gState[0];
		}
		else if(gState[3] == gState[4] && gState[4] == gState[5] && (gState[3]!='_')) 
		{
				gameOver=true;
				winner = gState[3];
		}
		else if (gState[6] == gState[7] && gState[7] == gState[8] && (gState[6]!='_'))
		{
				gameOver=true;
				winner = gState[6];
		}

		// column wise check 
		if(gState[0] == gState[3] && gState[3] == gState[6] && (gState[0]!='_'))
		{
				gameOver=true;
				winner = gState[0];
		}
		else if(gState[1] == gState[4] && gState[4] == gState[7] && (gState[1]!='_')) 
		{
				gameOver=true;
				winner = gState[1];
		}
		else if (gState[2] == gState[5] && gState[5] == gState[8] && (gState[2]!='_'))
		{
				gameOver=true;
				winner = gState[2];
		}

		// diagonal wise check	
		if(gState[0] == gState[4] && gState[4] == gState[8] && (gState[0]!='_'))
		{
				gameOver=true;
				winner = gState[0];
		}
		else if(gState[2] == gState[4] && gState[4] == gState[6] && (gState[2]!='_')) 
		{
				gameOver=true;
				winner = gState[2];
		}
}

int getEmptySpots(char gState[])
{
	//Counts the empty spots in the 3X3 Tic-Tac Toe puzzle board.
		int count = 0;
		xCount=0;
		oCount=0;
		for (int i = 0; i<9;i++)
		{	
				if(gState[i]=='_')
						++count;
				else if(gState[i]=='X')
						++xCount;
				else if(gState[i]=='O')
						++oCount;
		}
		return count;
}

int stateCheck(Node* x)
{
	// identifies the duplicate states in hashmap by rotating input state by 90,180,270 degrees. 
    // This function is used by the rotation invariance algorithm	
		map<string , int>::iterator it;
		char *c1,*c2, *c3;
		c1 = new char[9];
		c2 = new char[9];
		c3 = new char[9];

		int a[4] = {0,6,8,2};
		int b[4] = {1,3,7,5};
		for(int i = 0; i<9; i++)
		{
				if(i==4)
				{
						c1[i]=x->gameState[i];
						c2[i]=x->gameState[i];
						c3[i]=x->gameState[i];
				}
				else if(i%2==0)
				{
						int k = 0;
						while(k<4)
						{
								if(i==a[k])
								{
										c1[i]=x->gameState[a[(k+1)%4]];
										c2[i]=x->gameState[a[(k+2)%4]];
										c3[i]=x->gameState[a[(k+3)%4]];
										break;
								}
								k++;
						}
				}
				else
				{
						int k = 0;
						while(k<4)
						{
								if(i==b[k])
								{
										c1[i]=x->gameState[b[(k+1)%4]];
										c2[i]=x->gameState[b[(k+2)%4]];
										c3[i]=x->gameState[b[(k+3)%4]];
										break;
								}
								k++;
						}
				}
		}

		string str(x->gameState);
		string str1(c1);
		string str2(c2);
		string str3(c3);

		x->str1 = str1;
		x->str2 = str2;
		x->str3 = str3;

		it=ticMap.find(str);
		if(it!=ticMap.end())
				return it->second;
		else
				ticMap[str]=2;
		if(str==str1){
				it=ticMap.find(str1);
				if(it!=ticMap.end())
						return it->second;
				else
						ticMap[str1]=2;
		}

		if(str==str2){
				it=ticMap.find(str2);
				if(it!=ticMap.end())
						return it->second;
				else
						ticMap[str2]=2;
		}

		if(str==str3){
				it=ticMap.find(str3);
				if(it!=ticMap.end())
						return it->second;
				else
						ticMap[str3]=2;
		}
		return 2;
}	

int findMinMax(Node *x)
{
// Calculates the min/max value in a particular tree level by identifying which player's turn is to player
// x->level = X turn to play
// x->level = O turn to play
		int result;
		if(x->level>0)
		{
				result = x->childNodes[0]->value;
				for(int i = 1; i<x->childNodes.size(); i++)
						result = x->childNodes[i]->value>result?x->childNodes[i]->value:result;
		}
		else
		{
				result = x->childNodes[0]->value;
				for(int i = 1; i<x->childNodes.size(); i++)
						result = x->childNodes[i]->value<result?x->childNodes[i]->value:result;
		}

		return result;
}

int DFS(Node *x, char c)
{
	// The main Min-Max algorithm was implemented using depth first search method by calulating either min or max value at each tree level
	// depending on which player's turn is to place a move.
		nodesExplored++;

		int count = getEmptySpots(x->gameState);
		int countX = xCount;
		int countO = oCount;

		checkForWinner(x->gameState);

		if(gameOver||count==0)
		{	
				x->terminal= true;

				if(winner=='X')
						return 1;
				else if(winner=='O')
						return -1;
				else
						return 0;
		}

		x->level = c=='X'?1:0;

		if(count>0)
		{
				x->terminal = false;
				for (int j =0; j<9; j++)
				{
						if(x->gameState[j]=='_')
						{	
								Node *p = new Node();
								p->gameState =new char[9]; 
								for(int i = 0; i<9;i++)
										p->gameState[i] = x->gameState[i];

								p->gameState[j] = c;
								x->childNodes.push_back(p);
						}
				}

				c = c=='X'?'O':'X';
				for(int k=0; k<x->childNodes.size(); k++)
						x->childNodes[k]->value=DFS(x->childNodes[k],c);

		}
// For leaf nodes no need to recurse down to find the minmax value. 
		if(x->terminal)	
				return 0;
		else
				return findMinMax(x);
}

//for sorting elements in ascending order
bool aDecompare(Node *x, Node *y)
{
		return (x->eVal<y->eVal);
}

//for sorting elements in descending order
bool dDecompare(Node *x, Node *y)
{
		return (x->eVal>y->eVal);
}

int alphaBeta(Node *x, int abprune[], int treeLevel, char c, bool rotationInv=false, bool killer=false,bool eHeuristic=false)
{
	//This function implements 4 algorithms based on setting last 4 parameters as either true or false.
	// rotationInv - to activate the rotation invariance algorithm 
	// killer - to activate the killer heuristics
	// ehueristic - to activate a hueristic approach which chooses the path based on the highest difference value of valid empty spots for either players. 
	// setting the above parameters as false, function will output the winner based on alpha-beta algorithm  
		nodesExplored++;

		int count = getEmptySpots(x->gameState);
		int countX = xCount;
		int countO = oCount;

		checkForWinner(x->gameState);

		if(gameOver||count==0)
		{	
				x->terminal= true;
				treeLevel--;
				if(winner=='X')
				{
	//					abprune[treeLevel+1]=1;
						return 1;
				}
				else if(winner=='O')
				{
	//					abprune[treeLevel+1]=-1;
						return -1;
				}
				else
				{
	//					abprune[treeLevel+1]=0;
						return 0;
				}
		}

		x->level = c=='X'?1:0;
		x->terminal = false;

		for (int j =0; j<9; j++)
		{
				if(x->gameState[j]=='_')
				{	
						Node *p = new Node();
						p->gameState =new char[10]; 
						for(int i = 0; i<9;i++)
								p->gameState[i] = x->gameState[i];

						p->gameState[j] = c;
						p->gameState[9] = '\0';
						x->childNodes.push_back(p);
				}
		}

		char ch = c=='X'?'O':'X';
		if(killer)
		{
			//Explore the previous path that resulted in alpha-beta pruning in the same tree level first.
				if(killerPtr[treeLevel]!=0)
				{
						Node *p = new Node();
						p = x->childNodes[killerPtr[treeLevel]];
						deque<Node *>::iterator iter=x->childNodes.begin();
						advance(iter, killerPtr[treeLevel]);
						x->childNodes.erase(iter);	
						x->childNodes.push_front(p);
				}
		}

		if(eHeuristic)
		{
				for (int k=0; k<x->childNodes.size();k++)
						x->childNodes[k]->eVal=findHeuristicValue(x->childNodes[k]->gameState);

				if(x->level==0)
						sort(x->childNodes.begin(),x->childNodes.end(),aDecompare);
				else
						sort(x->childNodes.begin(),x->childNodes.end(),dDecompare);
		}

		for(int k=0; k<x->childNodes.size(); k++)
		{
				cutOff = false;
				int tLevel = treeLevel+1;
				for (int n = tLevel; n<abSize; n++)
						abprune[n]=2;

				if(!rotationInv)
						x->childNodes[k]->value=alphaBeta(x->childNodes[k],abprune,tLevel, ch, rotationInv,killer,eHeuristic);
				else
				{
					
						int nodeValue = stateCheck(x->childNodes[k]);

						if(nodeValue!=2)
						{
								rotationCount++;
								x->childNodes[k]->value = nodeValue;
						}
						else
						{
								x->childNodes[k]->value=alphaBeta(x->childNodes[k],abprune,tLevel,ch, rotationInv,killer,eHeuristic);

								string strg(x->childNodes[k]->gameState);
								if(!cutOff)
								{
									//rotation invariance gets activated only if there is no alpha-beta pruning 
										ticMap[strg]=x->childNodes[k]->value;
										ticMap[x->childNodes[k]->str1]=x->childNodes[k]->value;
										ticMap[x->childNodes[k]->str2]=x->childNodes[k]->value;
										ticMap[x->childNodes[k]->str3]=x->childNodes[k]->value;
								}
						}
				}				
				if(c=='O')
				{
						if(x->childNodes[k]->value<x->value || k==0)
						{
								x->value = x->childNodes[k]->value;
								abprune[treeLevel]=x->value;
								int l= treeLevel-1;
								while(l>=0)
								{
										if(abprune[l]>=x->value && abprune[l]!=2 && k!=x->childNodes.size()-1)
										{
												if(killer)
														killerPtr[treeLevel] = k;
												printState(x->gameState);
												cout<<"\tAlphaCut";
												cutOff = true;
												alphaCut++;
												treeLevel--;
												return x->value;
										}
										l=l-2;
								}
						}
				}
				else
				{
						if(x->childNodes[k]->value>x->value || k==0)
						{
								x->value = x->childNodes[k]->value;
								abprune[treeLevel]=x->value;
								int l= treeLevel-1;
								while(l>=0)
								{
										if(abprune[l]<=x->value && abprune[l]!=2 && k!=x->childNodes.size()-1)
										{
												if(killer)
														killerPtr[treeLevel] = k;
												printState(x->gameState);
												cout<<"\tBetaCut";
												cutOff = true;
												betaCut++;
												treeLevel--;
												return x->value;
										}

										l=l-2;
								}
						}
				}
		}

		treeLevel--;
		if(x->terminal)	
				return 0;
		else
				return x->value;
}

void reInitialize()
{
		front->childNodes.clear();
		gameOver = false;
		winner='-';
		alphaCut = 0;
		betaCut = 0;
		nodesExplored =-1;
		ticMap.clear();
		rotationCount = 0;
		result = 0;	
}

int main (int argc, char* argv[])
{
		if(argc==1)
		{
				std::cout<<"ERROR: Please specify the input state";
				return 1;
		}
		else if(strlen(argv[1])!=9)
		{
				cout<<"ERROR: Incorrect input. Length of the input state should be equal to 9";
				return 1;
		}
		front = new Node();
		front->gameState =new char[10]; 

		for(int i=0;i<9;i++)
		{
				if((char)toupper(argv[1][i])!='X'&& (char)toupper(argv[1][i])!='O'&&(char)toupper(argv[1][i]!='_'))
				{
						cout<<"ERROR: Incorrect input. Input state should only contain X, O or _ ";
						return 1;
				}
				else
				{
						front->gameState[i]=(char)toupper(argv[1][i]);
						if(front->gameState[i]=='X')
								xCount++;
						else if( front->gameState[i]=='O')
								oCount++;
				}
		}

		if(abs(xCount-oCount)>=2)
		{
				cout<<"ERROR: Incorrect input. Difference between number of Xs and Os shouldn't be more than 1";
				return 1;
		}

		char iChar = xCount<=oCount?'X':'O';
		front->gameState[9]='\0';

		result = DFS(front,iChar);

		cout<<"\n\nGame Result: "<<result;
		cout<<"\nMoves considered without alpha-beta pruning: "<<nodesExplored;
		cout<<"\n\n------------------------------------------------------------------";

		abSize =getEmptySpots(front->gameState)+1; 

		int abprune[9]={};
		std::fill_n(abprune,abSize,2);

		reInitialize();

		result = alphaBeta(front, abprune, 0,iChar);

		cout<<"\n\nGame Result: "<<result;
		cout<<"\nMoves considered with alpha-beta pruning: "<<nodesExplored;
		cout<<"\nAlpha cuts: "<<alphaCut;
		cout<<"\nBeta Cuts: "<<betaCut;
		cout<<"\n\n------------------------------------------------------------------";

		std::fill_n(abprune,abSize,2);
		reInitialize();
		int killerArray[9] = {0};
		killerPtr = killerArray;

		result = alphaBeta(front, abprune, 0, iChar,false,true);
		cout<<"\n\nGame Result: "<<result;
		cout<<"\nMoves considered with alpha-beta pruning using killer hueristic: "<<nodesExplored;
		cout<<"\nAlpha cuts: "<<alphaCut;
		cout<<"\nBeta Cuts: "<<betaCut;
		cout<<"\n\n------------------------------------------------------------------";

		std::fill_n(abprune,abSize,2);
		reInitialize();

		result = alphaBeta(front, abprune, 0,iChar, true,true);

		cout<<"\n\nGame Result: "<<result;
		cout<<"\nMoves considered with alpha-beta pruning using killer hueristics & rotation invariance: "<<nodesExplored;
		cout<<"\nAlpha cuts: "<<alphaCut;
		cout<<"\nBeta Cuts: "<<betaCut;
		cout<<"\nrotation Invoked: "<<rotationCount;
		cout<<"\n\n------------------------------------------------------------------";

		std::fill_n(abprune,abSize,2);
		reInitialize();

		result = alphaBeta(front, abprune, 0, iChar,false,false,true);

		cout<<"\n\nGame Result: "<<result;
		cout<<"\nMoves considered with alpha-beta pruning using hueristics : "<<nodesExplored;
		cout<<"\nAlpha cuts: "<<alphaCut;
		cout<<"\nBeta Cuts: "<<betaCut;
		cout<<"\n\n------------------------------------------------------------------";
		return 0;
}

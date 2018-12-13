#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#define maxLevel 6


struct qnode 
{
  int level;
  int flag; //Add an extra integer (int flag) to the quadtree Node data structure.
  double xy[2];
  struct qnode *child[4];
};
typedef struct qnode Node;

// function definitions

Node *makeNode( double x, double y, int level );
void makeChildren( Node *parent );
void writeTree( Node *head );
void writeNode( FILE *fp, Node *node );
void printOut( FILE *fp, Node *node );

double value( double x, double y, double time );
double nodeValue( Node *node, double time );

void destroyTree(Node *boss );
void removeChildren( Node *parent );
void growtree(Node *node);
void setflag(Node *node);
void adjust(Node *head);

int main( int argc, char **argv ) 
{
    Node *head;

	head = makeNode(0.0, 0.0, 0);// the head node

	makeChildren(head);
	
	
	/*destroyTree(head); */ 
	//the first task
	
	/*removeChildren(head);
	growtree(head);
	growtree(head);
	growtree(head);
	growtree(head);
	growtree(head);
	growtree(head);
	growtree(head);
	growtree(head);*/
	// print the tree for Gnuplot  /try to build more trees
	//the second task
	
	growtree(head);
	/*growtree(head);
	growtree(head);
	try to build more trees*/
	adjust(head);
	//the third task

	writeTree( head );

  return 0;
}

// make a node at given location (x,y) and level

Node *makeNode( double x, double y, int level ) 
{

  int i;

  Node *node = (Node *)malloc(sizeof(Node));

  node->level = level;
  node->flag=0;  //set to zero at all nodes
  node->xy[0] = x;
  node->xy[1] = y;

  for( i=0;i<4;++i )
    node->child[i] = NULL;

  return node;
}

// split a leaf nodes into 4 children

void makeChildren( Node *parent ) 
{

  double x = parent->xy[0];
  double y = parent->xy[1];

  int level = parent->level;

  double hChild = pow(2.0,-(level+1));

  parent->child[0] = makeNode( x,y, level+1 );
  parent->child[1] = makeNode( x+hChild,y, level+1 );
  parent->child[2] = makeNode( x+hChild,y+hChild, level+1 );
  parent->child[3] = makeNode( x,y+hChild, level+1 );

  return;
}

// write out the tree to file 'quad.out'

void writeTree( Node *head ) 
{

  FILE *fp = fopen("quad.out","w");

  writeNode(fp,head);

  fclose(fp);

  return;
}

// recursively visit the leaf nodes

void writeNode( FILE *fp, Node *node ) 
{

  int i;

  if( node->child[0] == NULL )
    printOut( fp, node );
  else {
    for ( i=0; i<4; ++i ) {
      writeNode( fp, node->child[i] );
    }
  }
  return;
}

// write out the (x,y) corners of the node

void printOut( FILE *fp, Node *node ) 
{
  double x = node->xy[0];
  double y = node->xy[1];
  int level = node->level;
  double h = pow(2.0,-level);

  fprintf(fp, " %g %g\n",x,y);
  fprintf(fp, " %g %g\n",x+h,y);
  fprintf(fp, " %g %g\n",x+h,y+h);
  fprintf(fp, " %g %g\n",x,y+h);
  fprintf(fp, " %g %g\n\n",x,y);

  return;
}


void destroyTree(Node* boss)
{
	int i = 0;

	if (boss->child[0] == NULL)
	{
		return;
	}

	else
	{
		for (i = 0; i < 4; i++)
		{
			destroyTree(boss->child[i]);
		}
		free(boss);
		boss = NULL;
	}
	boss = NULL;
}

void growtree(Node *node)
{
	if(node->child[0]==NULL)
		makeChildren(node);
	else
	{
		int c;
		for(c=0;c<4;c++)
			growtree(node->child[c]);
	}
}

void removeChildren( Node *parent )
{
    int k=0;
    while ( k<4 )
    {
        if(parent->child[k]->child[0]==NULL&&parent->child[k]!=NULL){
            {
                free(parent->child[k]);
                parent->child[k]=NULL;
            }
        }
        else
            return;
        k++;
    }
}


// Evaluate function at centre of quadtree node

double nodeValue( Node *node, double time ) 
{

  int level = node->level;
  double x = node->xy[0];
  double y = node->xy[1];

  double h = pow(2.0,-level);

  return( value( x+0.5*h, y+0.5*h, time ) );
}

// Data function

double value( double x, double y, double time ) 
{
  return( 2.0*exp(-8.0*(x-time)*(x-time)) - 1.0 ) ;
}

void adjust( Node *node )
{
    if(node->child[0]==NULL)
    {
        setflag(node);
        if(node->level<maxLevel&&node->flag==1)
            {
                makeChildren(node);
                int i;
                for (i=0;i<4;i++)
                {
                    adjust(node->child[i]);
                }
            }
    }
    else if(node->child[0]!=NULL&&node->child[0]->child[0]==NULL)
    {
        int i,j=0;
        for(i=0;i<4;i++)
            setflag(node->child[i]);
        for(i=0;i<4;i++)
           {
               if(node->child[i]->flag==-1)
                j+=1;
           }

        if(j==4)
            {
                removeChildren(node);
                //deliver d duixang
                adjust(node);
            }
        else
            {
                for(i=0;i<4;i++)
                adjust(node->child[i]);
            }
    }
    else
    {
        int i;
        for(i=0;i<4;i++)
            adjust(node->child[i]);
    }
}
  
  
void setflag(Node *node)
{
    if((node->child[0])==NULL)
    {
        if (nodeValue(node,0)>0.5)
                node->flag=1;
        if(nodeValue(node,0 )<-0.5)
                node->flag=-1;
    }
}

  

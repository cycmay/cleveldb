#pragma once

#include<iostream>
#include<iomanip>

 
namespace leveldb{

    class Interval{
        public:
            int low;
            int high;

            Interval(){};
            Interval(int low, int high){
                this->low=low;
                this->high=high;
            };
            Interval & operator=(const Interval &inte){
                this->low = inte.low;
                this->high = inte.high;
                return *this;
            };
    };

    enum RBTColor{RED, BLACK};

    // typedef struct interval{
    //     int low;
    //     int high;
	// }interval;

    template <class TC>
    class IntervalTNode{
        public:
            int key;
            bool color;
            IntervalTNode<TC> *parent;
            IntervalTNode<TC> *left;
            IntervalTNode<TC> *right;

            bool left_flag = false; // false: left-child 
                                    // true: previous-pointer
            bool right_flag = false; // false: right-child
                                    // true: successor-pointer

            typedef TC interval;
        
            interval inte;  //additional information
            int max;        //additional information

            IntervalTNode<TC>(){};
            ~IntervalTNode<TC>(){};
            IntervalTNode<TC>(int value, RBTColor c, 
                        IntervalTNode<TC> *p, 
                        IntervalTNode<TC> *l, 
                        IntervalTNode<TC> *r,
                        interval inte,
                        int max):
                key(value),color(c),parent(),left(l),right(r),inte(inte), max(max) {};

    };

    template<class TC>
    class IntervalTree{
        private:
            typedef TC interval;
            IntervalTNode<TC> * prev;
            interval interval0;
            
        public:
            //init sentine NIL
            IntervalTNode<TC> *NIL=nullptr;
            IntervalTNode<TC> * root;

            IntervalTree(){
                this->interval0 = interval();
                this->NIL = new IntervalTNode<TC>(-1,BLACK,nullptr,nullptr,nullptr,interval0,-1);
                this->prev = this->NIL;     

            }
        private:
            /*-----------------------------------------------------------------------*/
            int Max(int a,int b,int c)
            {
                if(a>b)
                    return a>c ? a:c;
                else
                    return b>c ? b:c;
            }
            
            void inorderWalkThreadingRecursive(IntervalTNode<TC> *x){
                if(x==NIL){
                    return;
                }

                inorderWalkThreadingRecursive(x->left);
                
                if (x->left == NIL) {
                    x->left_flag = true;
                    x->left = prev;
                }
                if (prev != NIL && prev->right == NIL) {
                    prev->right_flag = true;
                    prev->right = x;
                }
                prev = x;
        
                inorderWalkThreadingRecursive(x->right);
            }

        public:
            static bool Overlap(interval &a,interval &b)
            {
                if(a.high < b.low || a.low > b.high)     // a & b do not overlap
                    return 0;
                return 1;
            }
            
            IntervalTNode<TC> *IntervalT_Search(interval &i)
            {
                IntervalTNode<TC> *x=this->root;
                IntervalTNode<TC> *res = NIL;
                while(x!=NIL && !Overlap(i,x->inte))
                { 
                    if(x->left !=NIL && x->left->max>= i.low)
                        x=x->left;
                    else
                        x=x->right;
                }
                while(x!=NIL && Overlap(i, x->inte)){
                    res = x;
                    x = x->left;
                }
                return res;
            }

            std::vector<IntervalTNode<TC> *> IntervalT_Search_All(interval &i)
            {
                std::vector<IntervalTNode<TC> *> res;
                IntervalTNode<TC> *x=this->IntervalT_Search(i);
                if(x==NIL){
                    return res;
                }
                
                while(x!=NIL && Overlap(i, x->inte)){
                    res.push_back(x);
                    IntervalTNode<TC> * nextNode;
                    if(x->right_flag && x->right!=NIL){
                        nextNode = x->right;
                    }else{
                        for(nextNode=x->right;!nextNode->left_flag; nextNode=nextNode->left)
                        ;
                    }
                    x = nextNode;
                }
                return res;
            }
            /*-----------------------------------------------------------------------*/

            void inorderWalkThreading(){
                this->inorderWalkThreadingRecursive(this->root);
            }
            
            void IntervalT_InorderWalk(IntervalTNode<TC> *x)
            {
                if(x==NIL){
                    return;
                }

                IntervalT_InorderWalk(x->left);
                cout<<"["<<setw(3)<<x->inte.low<<setw(3)<<x->inte.high<<"  ]";
                if(x->color==1)
                    cout<<"     Red       "<<x->max<<endl;
                else
                    cout<<"     Black     "<<x->max<<endl;
                IntervalT_InorderWalk(x->right);
            
            }
            
            IntervalTNode<TC> *IntervalT_Minimum(IntervalTNode<TC> *x)
            {
                while(x->left != NIL)
                    x=x->left;
                return x;
                }
            
            IntervalTNode<TC> *IntervalT_Successor(IntervalTNode<TC> *x)
            {
                if(x->right != NIL)
                    return IntervalT_Minimum(x->right);
                IntervalTNode<TC> *y = x->parent;
                while(y != NIL && x  == y->right){
                    x = y;
                    y = y->parent;
                    }
                return y;
                }
            
            void Left_Rotate(IntervalTNode<TC> *x)
            {
                IntervalTNode<TC> *y=x->right;    //set y
            
                x->right=y->left;       //turn y's left subtree into x's right subtree
                if(y->left!=NIL)
                    y->left->parent=x;
            
                y->parent=x->parent;     //link x's parent to y;
                if(x->parent == NIL)
                    this->root=y;
                else if(x==x->parent->left)
                    x->parent->left=y;
                else
                    x->parent->right=y;
            
                y->left=x;               //put x on y's left
                x->parent=y;
                
                //maitaining additional information
                y->max=x->max;
                x->max=Max(x->inte.high,x->left->max,x->right->max);
                }
            
            void Right_Rotate(IntervalTNode<TC> *x)
            {
                IntervalTNode<TC> *y=x->left;      //set y
                
                x->left=y->right;   //link x's left tree into y's right subtree;
                if(y->right !=NIL)
                    y->right->parent=x;
                
                y->parent=x->parent;    //link x's parent to y
                if(x->parent == NIL)
                    this->root=y;
                else if(x == x->parent->left)
                    x->parent->left=y;
                else
                    x->parent->right=y;
                
                y->right=x;         //put x on y's right
                x->parent=y;
            
                //Maintaining additional information
                y->max=x->max;
                x->max=Max(x->inte.high,x->left->max,x->right->max);
            
                }
            
            void IntervalT_InsertFixup(IntervalTNode<TC> *z)
            {
                while(z->parent->color==RED)
                {
                    if(z->parent == z->parent->parent->left)    
                    {
                        IntervalTNode<TC> *y=z->parent->parent->right;   
                        if(y->color==RED)
                        {  
                            z->parent->color=BLACK;            //case 1
                            y->color=BLACK;                    //case 1
                            z->parent->parent->color=RED;      //case 1
                            z=z->parent->parent;               //case 1
                            }
                        else
                        {
                            if(z==z->parent->right)
                            { 
                                z=z->parent;                    //case 2
                                Left_Rotate(z);               //case 2
                                }
                            z->parent->color=BLACK;             //case 3
                            z->parent->parent->color=RED;       //case 3
                            Right_Rotate(z->parent->parent);  //case 3
                            } 
                        }
                    else
                    {//a me as then clause with "right" and "left" exchanged  
                        IntervalTNode<TC> *y=z->parent->parent->left;
                        if(y->color==RED)
                        {
                            z->parent->color=BLACK;
                            y->color=BLACK;
                            z->parent->parent->color=RED;
                            z=z->parent->parent;
                            }
                        else
                        {
                            if(z==z->parent->left)
                            {
                                z=z->parent;
                                Right_Rotate(z);
                                }
                            z->parent->color=BLACK;
                            z->parent->parent->color=RED;
                            Left_Rotate(z->parent->parent);
                            } 
                        } 
                    }   
                this->root->color=BLACK;      //turn the root to BLACK
                }
            
            void IntervalT_Insert(interval inte)
            {
                IntervalTNode<TC> *z=new IntervalTNode<TC>();
                z->key=inte.low;
                z->max=inte.high;
                z->inte=inte;
                z->color =RED;   
                z->parent=NIL;
                z->left=NIL;
                z->right=NIL;
            
                IntervalTNode<TC> *y=NIL;        //y is the parent of x
                IntervalTNode<TC> *x=this->root;
                while(x != NIL)
                { 
                    x->max=max(x->max,z->max);     //Maintaining the max value of each node from z up to root
                    y=x;
                    if(z->key < x->key)
                        x=x->left;
                    else
                        x=x->right;
                    }   
                z->parent=y;   //link new node's parent node to y(y's child is NIL)
                if(y==NIL)
                    this->root=z;
                else if(z->key < y->key)
                    y->left=z;
                else
                    y->right =z;
                IntervalT_InsertFixup(z);
                }
            
            void IntervalT_DeleteFixup(IntervalTNode<TC> *x)
            {
                IntervalTNode<TC> *w;
                while(x!=this->root && x->color==BLACK)
                {
                    if(x==x->parent->left)
                    {
                        w=x->parent->right;  //set w to x's sibling
                        if(w->color==RED)      //case 1:x's sibling w is red
                        {
                            w->color=BLACK;
                            x->parent->color=RED;
                            Left_Rotate(x->parent);
                            w=x->parent->right;
                            }
                        if(w->left->color==BLACK && w->right->color==BLACK)  
                        { //case 2:x's sibling w is black and both of w's children are black
                            w->color=RED;
                            x=x->parent;
                            }
                        else
                        { 
                            if(w->right->color==BLACK)     
                            {//case 3:x's sibling w is black,w's left child is red, and w's right child is black
                                w->left->color=BLACK;
                                w->color=RED;
                                Right_Rotate(w);
                                w=x->parent->right;
                                }
                            w->color=x->parent->color;      //case 4: x's sibling w is black,and w's right child is red
                            x->parent->color=BLACK;         //.
                            w->right->color=BLACK;         // .
                            Left_Rotate(x->parent);      // .
                            x=this->root;                     //case 4
                            }
                        }
                    else
                    {//Same as then clause with "right" and "left" exchanged
                        w=x->parent->left;
                        if(w->color==RED)
                        {
                            w->color=BLACK;
                            x->parent->color=RED;
                            Right_Rotate(x->parent);
                            w=x->parent->left;
                            } 
                        if(w->left->color==BLACK && w->right->color==BLACK)
                        { 
                            w->color=RED;
                            x=x->parent;
                            }
                        else
                        {
                            if(w->left->color==BLACK)
                            {
                                w->right->color=BLACK;
                                w->color=RED;
                                Left_Rotate(w);
                                w=x->parent->left;
                                }
                            w->color=x->parent->color;
                            x->parent->color=BLACK;
                            w->left->color=BLACK;
                            Right_Rotate(x->parent);
                            x=this->root;
                            } 
                        }
                    }
                x->color=BLACK;
                }
            void IntervalT_Delete(IntervalTNode<TC> *z)
            {
                IntervalTNode<TC> *x=nullptr,*y=nullptr,*g=nullptr;
                
                if(z->left == NIL || z->right==NIL)
                    y=z;
                else
                    y=IntervalT_Successor(z);
            
                //maintaining additional information
                g=y->parent;
                g->max=g->inte.high;
                g=g->parent;
                while(g->max==y->max)
                {            
                    g->max=Max(g->max,g->left->max,g->right->max);
                    g=g->parent;
                    }
                //delete y node
                if(y->left !=NIL)
                    x=y->left;
                else
                    x=y->right;
                x->parent=y->parent;
            
                if(y->parent==NIL)
                    this->root=x;
                else if(y==y->parent->left)
                    y->parent->left=x;
                else
                    y->parent->right=x;
            
                if(y != z)
                    z->key=y->key;
            
                if(y->color==BLACK)
                    IntervalT_DeleteFixup(x);
                }
    
    
    };
}

 
 
// int main()
// {
// 	interval A[]={{16,21},{8,9},{25,30},{5,8},{15,23},{17,19},{26,26},{0,3},{6,10},{19,20}};
// 	int n=sizeof(A)/sizeof(interval);
	
// 	cout<<"/*---------------------Create Interval Tree-------------------*/"<<endl;
// 	IntervalTree *T=new IntervalTree();
// 	T->root=NIL;
// 	for(int i=0;i<n;i++)
// 		IntervalT_Insert(T,A[i]);
// 	cout<<"The interval tree is:"<<endl;
// 	IntervalT_InorderWalk(T->root);
// 	cout<<"The root of the tree is:"<<T->root->inte.low<<"   "<<T->root->inte.high<<endl;
// 	cout<<"/*-------------------------------------------------------------*/"<<endl;
 
// 	cout<<"/*--------------------Searching Interval Tree------------------*/"<<endl;
// 	interval sInt;
// 	cout<<"Please input the searching interval:";
// 	cin>>sInt.low>>sInt.high;
// 	IntervalTNode<TC> *sITNode=NIL;
// 	sITNode=IntervalT_Search(T,sInt);
// 	if(sITNode==NIL)
// 		cout<<"The searching interval doesn't exist in the tree."<<endl;
// 	else{
// 		cout<<"The overlap interval is:"<<endl;
// 		cout<<"["<<sITNode->inte.low<<"  "<<sITNode->inte.high<<"]";
// 		if(sITNode->color==0)
// 			cout<<"   color:RED     ";
// 		else
// 			cout<<"   color:BLACK   ";
// 		cout<<"Max:"<<sITNode->max<<endl;
// 		}
// 	cout<<"/*------------------Deleting INterval Tree--------------------*/"<<endl;
// 	interval dInt;
// 	cout<<"Please input the deleting interval:";
// 	cin>>dInt.low>>dInt.high;
// 	IntervalTNode<TC>  *dITNode=NIL;
// 	dITNode=IntervalT_Search(T,dInt);
// 	if(dITNode==NIL)
// 		cout<<"The deleting interval doesn't exist in the tree."<<endl;
// 	else
// 	{ 
// 		IntervalT_Delete(T,dITNode);
// 		cout<<"After deleting ,the interval tree is:"<<endl;
// 		IntervalT_InorderWalk(T->root);
// 		cout<<"The root of the tree is:"<<T->root->inte.low<<"   "<<T->root->inte.high<<endl;
// 		}
// 	cout<<"/*------------------------------------------------------------*/"<<endl;
 
 
// 	return 0;
// 	}
 
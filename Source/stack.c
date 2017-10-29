
#define MAXSIZE 15

 
struct stack
{
    int stk[MAXSIZE];
    int top = -1;
};


stack stackdir;

void pushdir (int num)
{
    if (stackdir.top == (MAXSIZE - 1))
    {
        return;
    }
    else
    {
        stackdir.top = stackdir.top + 1;
        stackdir.stk[stackdir.top] = num;
    }
    return;
}

/*  Function to delete an element from the stack */
int popdir ()
{
    int num;
    if (stackdir.top == - 1)
    {
        printf ("In root directory of File system\n");
        return (stackdir.top);
    }
    else
    {
        num = stackdir.stk[stackdir.top];
        stackdir.top = stackdir.top - 1;
    }
    return(num);
}
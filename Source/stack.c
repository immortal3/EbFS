
#define MAXSIZE 15

// for managing directory stack
struct stack
{
    int stk[MAXSIZE];
    int top = -1;
};


stack stackdir;

// Function info : pushing directory means going into child directory
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

// Function info : poping directory means going back to parent directory
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

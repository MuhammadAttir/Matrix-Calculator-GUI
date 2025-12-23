#include <windows.h>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

#define MAX 3

//-------------------------------------- Class Names ----------------------------------------------------------
 
const char g_mainClass[]  = "MainWindow";
const char g_ops1DClass[] = "Ops1DWindow";
const char g_ops2DClass[] = "Ops2DWindow";
const char g_adv2DClass[] = "Adv2DWindow";   

// -------------------------------------- 1D Arrays ----------------------------------------------------------

int arr1[10], arr2[10], arraySize = 0;
HWND hSizeBox, hArr1Boxes[10], hArr2Boxes[10], h1DResultLabel, h1DResultTitle;

// -------------------------------------- 2D Matrices -------------------------------------------------------
 
int matA[MAX][MAX], matB[MAX][MAX], matRes[MAX][MAX], matSize = 2;
HWND hSize2, hSize3, hMatABoxes[MAX][MAX], hMatBBoxes[MAX][MAX], h2DResultOutput;

// Advanced 2D
HWND hAdvSize2, hAdvSize3, hAdvMatA[MAX][MAX], hAdv2DResultOutput;

// To Center the Things
int CenterX(int w, int win) { return (win - w)/2; }

// ----------------------------------------- MATRIX HELPERS -------------------------------------------------------
// 
int determinant2x2(int M[MAX][MAX])
{
    return M[0][0]*M[1][1] - M[0][1]*M[1][0];
}

int determinant3x3(int M[MAX][MAX])
{
    return M[0][0]*(M[1][1]*M[2][2] - M[1][2]*M[2][1])
         - M[0][1]*(M[1][0]*M[2][2] - M[1][2]*M[2][0])
         + M[0][2]*(M[1][0]*M[2][1] - M[1][1]*M[2][0]);
}

void adjoint2x2(int M[MAX][MAX], int R[MAX][MAX])
{
    R[0][0] =  M[1][1];
    R[0][1] = -M[0][1];
    R[1][0] = -M[1][0];
    R[1][1] =  M[0][0];
}

void adjoint3x3(int M[MAX][MAX], int R[MAX][MAX])
{
    int C[3][3];
    C[0][0] =  (M[1][1]*M[2][2] - M[1][2]*M[2][1]);
    C[0][1] = -(M[1][0]*M[2][2] - M[1][2]*M[2][0]);
    C[0][2] =  (M[1][0]*M[2][1] - M[1][1]*M[2][0]);

    C[1][0] = -(M[0][1]*M[2][2] - M[0][2]*M[2][1]);
    C[1][1] =  (M[0][0]*M[2][2] - M[0][2]*M[2][0]);
    C[1][2] = -(M[0][0]*M[2][1] - M[0][1]*M[2][0]);

    C[2][0] =  (M[0][1]*M[1][2] - M[0][2]*M[1][1]);
    C[2][1] = -(M[0][0]*M[1][2] - M[0][2]*M[1][0]);
    C[2][2] =  (M[0][0]*M[1][1] - M[0][1]*M[1][0]);

    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            R[i][j] = C[j][i];
}

void transpose(int M[MAX][MAX], int R[MAX][MAX], int n)
{
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            R[j][i] = M[i][j];
}

void add2D(int A[MAX][MAX], int B[MAX][MAX], int C[MAX][MAX], int n)
{
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            C[i][j] = A[i][j] + B[i][j];
}

void sub2D(int A[MAX][MAX], int B[MAX][MAX], int C[MAX][MAX], int n)
{
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            C[i][j] = A[i][j] - B[i][j];
}

void mul2D(int A[MAX][MAX], int B[MAX][MAX], int C[MAX][MAX], int n)
{
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++){
            C[i][j] = 0;
            for(int k=0;k<n;k++)
                C[i][j] += A[i][k] * B[k][j];
        }
}

// ------------------------------------------- UPDATE 2D LAYOUTS ----------------------------------------------------

void Update2DLayout()
{
    for(int i=0;i<MAX;i++)
        for(int j=0;j<MAX;j++)
        {
            ShowWindow(hMatABoxes[i][j], (i<matSize&&j<matSize)?SW_SHOW:SW_HIDE);
            ShowWindow(hMatBBoxes[i][j], (i<matSize&&j<matSize)?SW_SHOW:SW_HIDE);
        }
}

void UpdateAdv2DLayout()
{
    for(int i=0;i<MAX;i++)
        for(int j=0;j<MAX;j++)
            ShowWindow(hAdvMatA[i][j], (i<matSize&&j<matSize)?SW_SHOW:SW_HIDE);
}

// -------------------------------------- 1D Operations Window ---------------------------------------------------

LRESULT CALLBACK Ops1DWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
    {
        HINSTANCE hInst=(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE);

        CreateWindow("STATIC","Enter Size (max 10):",WS_VISIBLE|WS_CHILD,
                     20,20,200,25,hwnd,NULL,hInst,NULL);

        hSizeBox = CreateWindow("EDIT","",WS_VISIBLE|WS_CHILD|WS_BORDER,
                                220,20,120,28,hwnd,NULL,hInst,NULL);

        CreateWindow("BUTTON","Set Size",WS_VISIBLE|WS_CHILD,
                     360,20,120,30,hwnd,(HMENU)10,hInst,NULL);

        h1DResultTitle = CreateWindow("STATIC","Result:",WS_VISIBLE|WS_CHILD,
                                      20,420,200,30,hwnd,NULL,hInst,NULL);

        h1DResultLabel = CreateWindow("STATIC","",WS_VISIBLE|WS_CHILD|SS_LEFT,
                                      20,450,600,80,hwnd,NULL,hInst,NULL);

        CreateWindow("BUTTON","Add",WS_VISIBLE|WS_CHILD,20,550,150,35,hwnd,(HMENU)20,hInst,NULL);
        CreateWindow("BUTTON","Subtract",WS_VISIBLE|WS_CHILD,200,550,150,35,hwnd,(HMENU)21,hInst,NULL);
        CreateWindow("BUTTON","Multiply",WS_VISIBLE|WS_CHILD,380,550,150,35,hwnd,(HMENU)22,hInst,NULL);
        break;
    }

    case WM_COMMAND:{
        if(LOWORD(wParam)==10)
        {
            char b[10];
            GetWindowText(hSizeBox,b,10);
            int newSize = atoi(b);

            if(newSize<1 || newSize>10){
                MessageBox(hwnd,"Invalid size!","Error",MB_OK);
                return 0;
            }

            for(int i=0;i<arraySize;i++){
                if(hArr1Boxes[i]) { DestroyWindow(hArr1Boxes[i]); hArr1Boxes[i]=NULL; }
                if(hArr2Boxes[i]) { DestroyWindow(hArr2Boxes[i]); hArr2Boxes[i]=NULL; }
            }

            arraySize = newSize;
            HINSTANCE hInst=(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE);

            for(int i=0;i<arraySize;i++){
                hArr1Boxes[i]=CreateWindow("EDIT","",WS_VISIBLE|WS_CHILD|WS_BORDER,
                                           20,70+i*35,120,30,hwnd,NULL,hInst,NULL);

                hArr2Boxes[i]=CreateWindow("EDIT","",WS_VISIBLE|WS_CHILD|WS_BORDER,
                                           160,70+i*35,120,30,hwnd,NULL,hInst,NULL);
            }
        }

        if(LOWORD(wParam)>=20 && LOWORD(wParam)<=22)
        {
            if(arraySize==0){
                MessageBox(hwnd,"Set Size First!","Error",MB_OK);
                return 0;
            }

            int res[10];
            string out="";

            for(int i=0;i<arraySize;i++)
            {
                char t[20];
                GetWindowText(hArr1Boxes[i],t,20); arr1[i]=atoi(t);
                GetWindowText(hArr2Boxes[i],t,20); arr2[i]=atoi(t);

                if(wParam==20) res[i]=arr1[i]+arr2[i];
                if(wParam==21) res[i]=arr1[i]-arr2[i];
                if(wParam==22) res[i]=arr1[i]*arr2[i];

                out += to_string(res[i]) + " ";
            }
            SetWindowText(h1DResultLabel,out.c_str());
        }
        break;
    }

    case WM_CLOSE: DestroyWindow(hwnd); break;
    default:
        return DefWindowProc(hwnd,msg,wParam,lParam);
    }
    return 0;
}

// --------------------------------------- 2D Operations Window --------------------------------------------

LRESULT CALLBACK Ops2DWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
    {
        HINSTANCE hInst=(HINSTANCE)GetWindowLongPtr(hwnd,GWLP_HINSTANCE);

        CreateWindow("STATIC","Select Size:",WS_CHILD|WS_VISIBLE,
                     20,20,150,25,hwnd,NULL,hInst,NULL);

        hSize2=CreateWindow("BUTTON","2x2",WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
                            180,20,70,25,hwnd,(HMENU)1,hInst,NULL);

        hSize3=CreateWindow("BUTTON","3x3",WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
                            260,20,70,25,hwnd,(HMENU)2,hInst,NULL);

        SendMessage(hSize2,BM_SETCHECK,BST_CHECKED,0);
        matSize=2;

        int x=20, y=70;

        for(int i=0;i<MAX;i++)
            for(int j=0;j<MAX;j++)
            {
                hMatABoxes[i][j]=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|WS_BORDER,
                                              x+j*70, y+i*45, 60,35,hwnd,NULL,hInst,NULL);

                hMatBBoxes[i][j]=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|WS_BORDER,
                                              x+260+j*70, y+i*45, 60,35,hwnd,NULL,hInst,NULL);
            }

        Update2DLayout();

        CreateWindow("STATIC","Result:",WS_CHILD|WS_VISIBLE,
                     20,260,200,30,hwnd,NULL,hInst,NULL);

        h2DResultOutput=CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE|SS_LEFT,
                                     20,290,650,200,hwnd,NULL,hInst,NULL);

        CreateWindow("BUTTON","Add",WS_CHILD|WS_VISIBLE,
                     20,520,120,40,hwnd,(HMENU)10,hInst,NULL);

        CreateWindow("BUTTON","Subtract",WS_CHILD|WS_VISIBLE,
                     160,520,120,40,hwnd,(HMENU)11,hInst,NULL);

        CreateWindow("BUTTON","Multiply",WS_CHILD|WS_VISIBLE,
                     300,520,120,40,hwnd,(HMENU)12,hInst,NULL);

        break;
    }

    case WM_COMMAND:{

        if(wParam==1)
        {
            matSize=2;
            SendMessage(hSize2,BM_SETCHECK,BST_CHECKED,0);
            SendMessage(hSize3,BM_SETCHECK,BST_UNCHECKED,0);
            Update2DLayout();
        }
        if(wParam==2)
        {
            matSize=3;
            SendMessage(hSize3,BM_SETCHECK,BST_CHECKED,0);
            SendMessage(hSize2,BM_SETCHECK,BST_UNCHECKED,0);
            Update2DLayout();
        }

        if(LOWORD(wParam)>=10 && LOWORD(wParam)<=12)
        {
            for(int i=0;i<matSize;i++)
                for(int j=0;j<matSize;j++)
                {
                    char t[20];
                    GetWindowText(hMatABoxes[i][j],t,20); matA[i][j]=atoi(t);
                    GetWindowText(hMatBBoxes[i][j],t,20); matB[i][j]=atoi(t);
                }

            if(wParam==10) add2D(matA,matB,matRes,matSize);
            if(wParam==11) sub2D(matA,matB,matRes,matSize);
            if(wParam==12) mul2D(matA,matB,matRes,matSize);

            string out="";
            for(int i=0;i<matSize;i++){
                for(int j=0;j<matSize;j++)
                    out+=to_string(matRes[i][j])+" ";
                out+="\n";
            }
            SetWindowText(h2DResultOutput,out.c_str());
        }
        break;
    }

    case WM_CLOSE: DestroyWindow(hwnd); break;
    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

// --------------------------------------- ADVANCED 2D WINDOW -------------------------------------------------

LRESULT CALLBACK Adv2DWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
    {
        HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

        CreateWindow("STATIC", "Select Size:", WS_VISIBLE | WS_CHILD,
            20, 20, 150, 25, hwnd, NULL, hInst, NULL);

        hAdvSize2 = CreateWindow("BUTTON", "2x2",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            180, 20, 70, 25, hwnd, (HMENU)201, hInst, NULL);

        hAdvSize3 = CreateWindow("BUTTON", "3x3",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            260, 20, 70, 25, hwnd, (HMENU)202, hInst, NULL);

        SendMessage(hAdvSize2, BM_SETCHECK, BST_CHECKED, 0);

        int startX = 20, startY = 70;

        for (int i = 0; i < MAX; i++)
            for (int j = 0; j < MAX; j++)
            {
                hAdvMatA[i][j] = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                    startX + j * 70, startY + i * 45, 60, 35,
                    hwnd, NULL, hInst, NULL);
            }

        UpdateAdv2DLayout();

        CreateWindow("STATIC", "Result:", WS_CHILD | WS_VISIBLE,
            20, 260, 200, 25, hwnd, NULL, hInst, NULL);

        hAdv2DResultOutput = CreateWindow("STATIC", "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            20, 290, 650, 200, hwnd, NULL, hInst, NULL);

        CreateWindow("BUTTON", "Determinant", WS_VISIBLE | WS_CHILD,
            20, 520, 150, 40, hwnd, (HMENU)210, hInst, NULL);

        CreateWindow("BUTTON", "Adjoint", WS_VISIBLE | WS_CHILD,
            200, 520, 150, 40, hwnd, (HMENU)211, hInst, NULL);

        CreateWindow("BUTTON", "Transpose", WS_VISIBLE | WS_CHILD,
            380, 520, 150, 40, hwnd, (HMENU)212, hInst, NULL);

        break;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 201)
        {
            matSize = 2;
            SendMessage(hAdvSize2, BM_SETCHECK, BST_CHECKED, 0);
            SendMessage(hAdvSize3, BM_SETCHECK, BST_UNCHECKED, 0);
            UpdateAdv2DLayout();
        }
        if (LOWORD(wParam) == 202)
        {
            matSize = 3;
            SendMessage(hAdvSize3, BM_SETCHECK, BST_CHECKED, 0);
            SendMessage(hAdvSize2, BM_SETCHECK, BST_UNCHECKED, 0);
            UpdateAdv2DLayout();
        }

        if (LOWORD(wParam) >= 210 && LOWORD(wParam) <= 212)
        {
            for (int i = 0; i < matSize; i++)
                for (int j = 0; j < matSize; j++)
                {
                    char t[20];
                    GetWindowText(hAdvMatA[i][j], t, 20);
                    matA[i][j] = atoi(t);
                }

            string out = "";

            if (LOWORD(wParam) == 210)  // determinant
            {
                int det = (matSize == 2)
                    ? determinant2x2(matA)
                    : determinant3x3(matA);

                out = "Determinant = " + to_string(det);
            }

            else if (LOWORD(wParam) == 211) // adjoint
            {
                if (matSize == 2)
                    adjoint2x2(matA, matRes);
                else
                    adjoint3x3(matA, matRes);

                out = "Adjoint:\n";
                for (int i = 0; i < matSize; i++)
                {
                    for (int j = 0; j < matSize; j++)
                        out += to_string(matRes[i][j]) + " ";
                    out += "\n";
                }
            }

            else if (LOWORD(wParam) == 212) // transpose
            {
                transpose(matA, matRes, matSize);

                out = "Transpose:\n";
                for (int i = 0; i < matSize; i++)
                {
                    for (int j = 0; j < matSize; j++)
                        out += to_string(matRes[i][j]) + " ";
                    out += "\n";
                }
            }

            SetWindowText(hAdv2DResultOutput, out.c_str());
        }

        break;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ----------------------------------------- MAIN WINDOW ---------------------------------------------------

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
    {
        HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

        CreateWindow("STATIC", "Matrix Calculator", WS_CHILD | WS_VISIBLE | SS_CENTER,
            0, 20, 900, 40, hwnd, NULL, hInst, NULL);

        CreateWindow("BUTTON", "1D Operations", WS_CHILD | WS_VISIBLE,
            CenterX(250, 900), 100, 250, 45, hwnd, (HMENU)1, hInst, NULL);

        CreateWindow("BUTTON", "2D Operations", WS_CHILD | WS_VISIBLE,
            CenterX(250, 900), 160, 250, 45, hwnd, (HMENU)2, hInst, NULL);

        CreateWindow("BUTTON", "Advanced 2D Operations", WS_CHILD | WS_VISIBLE,
            CenterX(300, 900), 220, 300, 45, hwnd, (HMENU)3, hInst, NULL);

        CreateWindow("BUTTON", "Exit", WS_CHILD | WS_VISIBLE,
            CenterX(180, 900), 280, 180, 45, hwnd, (HMENU)9, hInst, NULL);

        break;
    }

    case WM_COMMAND:
    {
        if (wParam == 1)
            CreateWindow(g_ops1DClass, "1D Operations",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                150, 150, 750, 650, hwnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        if (wParam == 2)
            CreateWindow(g_ops2DClass, "2D Operations",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                150, 150, 850, 700, hwnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        if (wParam == 3)
            CreateWindow(g_adv2DClass, "Advanced 2D Operations",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                150, 150, 850, 700, hwnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

        if (wParam == 9) PostQuitMessage(0);
        break;
    }

    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ------------------------------------------- Win Main ---------------------------------------------------

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int ncmd)
{
    WNDCLASS wc = {0};  // Initialize 
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;  //Set BG Color
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // Set Cursor
    wc.hInstance = hInst;        //Stores program instance
    
    // Register Mian Window 
    wc.lpfnWndProc = MainWndProc;  //tells Windows that the Main Window will use the MainWndProc() function to handle its messages.
    wc.lpszClassName = g_mainClass;
    RegisterClass(&wc);

    // Register 1D Window
    wc.lpfnWndProc = Ops1DWndProc;
    wc.lpszClassName = g_ops1DClass;
    RegisterClass(&wc);

    // Register 2D
    wc.lpfnWndProc = Ops2DWndProc;
    wc.lpszClassName = g_ops2DClass;
    RegisterClass(&wc);

    // Register Advanced 2D
    wc.lpfnWndProc = Adv2DWndProc;
    wc.lpszClassName = g_adv2DClass;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(g_mainClass, "Matrix Calculator",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, 900, 700, NULL, NULL, hInst, NULL);

    MSG msg = {0};
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

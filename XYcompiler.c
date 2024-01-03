#include "FPToolkit.c"
#include "M2d_matrix_tools.c"
#include "intersect_two_linesS.c"

#define MAXOBJS 50
#define MAXPTS 59000
#define MAXPOLYS 57500

int numobjects;
int numpoints[MAXOBJS] ;
int numpolys[MAXOBJS] ;
double x[MAXOBJS][MAXPTS] ;
double y[MAXOBJS][MAXPTS] ;
int psize[MAXOBJS][MAXPOLYS] ;
int con[MAXOBJS][MAXPOLYS][20] ;
double red[MAXOBJS][MAXPOLYS],grn[MAXOBJS][MAXPOLYS],blu[MAXOBJS][MAXPOLYS] ;

int read_object(FILE *f, int onum)
{
  int i,j ;
    // point info
    fscanf(f,"%d",&numpoints[onum]) ;
    if (numpoints[onum] >= MAXPTS) {
      // need an extra for object centering
      printf("MAXPTS = %d :  exceeded.\n",MAXPTS) ;
      exit(1) ;
    }
    for (i = 0 ; i < numpoints[onum] ; i++) {
      fscanf(f,"%lf %lf",&x[onum][i],&y[onum][i]) ;
    }
    // connectivity info
    fscanf(f,"%d",&numpolys[onum]) ;
    if (numpolys[onum] > MAXPOLYS) {
      printf("MAXPOLYS = %d :  exceeded.\n",MAXPOLYS) ;
      exit(1) ;
    }
    for (i = 0 ; i < numpolys[onum] ; i++) {
      fscanf(f,"%d",&psize[onum][i]) ;
      for (j = 0 ; j < psize[onum][i] ; j++) {
        fscanf(f,"%d",&con[onum][i][j]) ;
      } // end for j
    } // end for i
    // color info :
    for (i = 0 ; i < numpolys[onum] ; i++) {
      fscanf(f,"%lf %lf %lf",&red[onum][i],&grn[onum][i],&blu[onum][i]) ;
    }    
}

int print_object (FILE *fout, int onum)
{
  int i,j ;
  fprintf(fout, "%d\n",numpoints[onum]) ;
  for (i = 0 ; i < numpoints[onum] ; i++) {
    fprintf(fout, "%12.6lf %12.6lf\n",x[onum][i],y[onum][i]) ;
  }
  for (i = 0 ; i < numpolys[onum] ; i++) {
    fprintf(fout, "%3d    ",psize[onum][i]) ;
    for (j = 0 ; j < psize[onum][i] ; j++) {
      fprintf(fout, "%2d ", con[onum][i][j]) ;
    }
    fprintf(fout, "\n") ;
  }
  for (i = 0 ; i < numpolys[onum] ; i++) {
    fprintf(fout,"%lf %lf %lf\n",red[onum][i],grn[onum][i],blu[onum][i]) ;
  }      
}

int click_and_save(double x[], double y[])
{
  double xy[2], xr, yr ;
  int numpoints ;
  
  numpoints = 0 ;
  while (0 == 0) {
     G_wait_click(xy) ;
     if ((xy[0] >= 0) && (xy[0] <= 50) && 
         (xy[1] >= 0) && (xy[1] <= 50))  { break ; }

     xr=fmod(xy[0], 20);
     yr=fmod(xy[1], 20);
     if (xr<10) { xy[0]-=xr; }
     else { xy[0]=xy[0]-xr+20; }
     if (yr<10) { xy[1]-=yr; }
     else {xy[1]=xy[1]-yr+20; }

     G_rgb(1, 0, 0) ; // red

     G_circle(xy[0], xy[1], 3) ;     

     x[numpoints] = xy[0] ; y[numpoints] = xy[1] ;

     if (numpoints > 0) {
       G_line (x[numpoints-1],y[numpoints-1], x[numpoints],y[numpoints]) ;
     }
     numpoints++ ;
  }
  G_line (x[numpoints-1],y[numpoints-1], x[0],y[0]) ;  
  return numpoints ;
}

int crop(double xp[], double yp[], int np, int onum)
{
  double A[2], B[2], C[2], D[2];
  double cutx[100], cuty[100], cutpoints, intersect[2], yes, keys;
  cutpoints=click_and_save(cutx,cuty);
  
  for(int i =0; i<cutpoints; i++){
    int j=i+1; if(j==cutpoints){j=0;}
    
    for(int k=0; k<numpoints[onum]; k++){
      int t = k+1; if(t==numpoints[onum]){t=0;}
      A[0]=x[onum][k];   A[1]=y[onum][k];
      B[0]=x[onum][t];   B[1]=y[onum][t];
      C[0]=cutx[i];      C[1]=cuty[i];
      D[0]=cutx[j];      D[1]=cuty[j];
      intersect_2_lines(A, B, C, D, intersect);
      /*
      double dx=A[0]-intersect[0], dy=A[1]-intersect[1];
      double a=-dy, b=dx;
      double c=(-(dy*A[0])+(dx*A[1]));
      
      double dx2=intersect[0]-D[0], dy2=intersect[1]-D[1];    
      double d=-dy2, e=dx2;
      double f=(-(dy2*intersect[0])+(dx2*intersect[1]));

      if(f<0){
      } else if(c<0){
      } else {

      }
      */
    }
  }
  keys=G_wait_key();
  return 1;
}

int draw_object (int onum)
{
  int h,i,j ;
  double xp[100],yp[100] ;
  int np, tnp ;

  for (i = 0 ; i < numpolys[onum] ; i++) {
    np = psize[onum][i] ;
    for (j = 0 ; j < np ; j++) {
      h = con[onum][i][j] ;
      xp[j] = x[onum][h] ;
      yp[j] = y[onum][h] ;
    }
    
    tnp  = crop(xp, yp, np, onum);
      
    G_rgb(red[onum][i], grn[onum][i], blu[onum][i]) ;
    G_fill_polygon(xp,yp,np) ;
  }
}


void center (int onum) {
  int lx=5000, hx=0, mx, ly=5000, hy, my, size, ax, ay;
  double s, temp[3][3];

  lx = hx = x[onum][0] ;
  ly = hy = y[onum][0] ;  
  for (int i=0; i<numpoints[onum]; i++) {
    if (lx>x[onum][i]) {lx=x[onum][i];}
    if (hx<x[onum][i]) {hx=x[onum][i];}
    if (ly>y[onum][i]) {ly=y[onum][i];}
    if (hy<y[onum][i]) {hy=y[onum][i];}
  }
    ax=((hx-lx));
    ay=((hy-ly));
    mx=ax/2+lx;
    my=ay/2+ly;


    if (ax>ay) {s=ax;}
    else {s=ay;}

    s=800/s;
    M2d_make_translation(temp, -mx, -my);
    M2d_mat_mult_points(x[onum], y[onum], temp, x[onum], y[onum], numpoints[onum]);
    M2d_make_scaling(temp, s, s);
    M2d_mat_mult_points(x[onum], y[onum], temp, x[onum], y[onum], numpoints[onum]);
    M2d_make_translation(temp, 400, 400);
    M2d_mat_mult_points(x[onum], y[onum], temp, x[onum], y[onum], numpoints[onum]);
  
}

int main(int argc, char **argv)
{
  FILE *fln ;
  int key,w ;
  char fname[100] ;
  int onum;
  int ke;

  //  scanf("%d", &numobjects);
  numobjects = argc - 1 ;
  
  if (numobjects>MAXOBJS) {
    printf("MAXOBJS= %d : exceeded.\n", MAXOBJS);
    exit(1);
  }
  for (onum=0; onum < numobjects; onum++) { 
    //    printf("enter name of xy file ") ;
    //    scanf("%s",fname) ;
    //    fln = fopen(fname,"r") ;
    fln = fopen(argv[onum+1],"r") ;    
    if (fln == NULL) {
      printf("can't read file, %s\n",fname) ;
      exit(1) ;
    }
    read_object(fln, onum) ;
    center(onum);
    }
  print_object(stdout, 0) ;

  double m[3][3] ;
  
  M2d_make_identity(m);
 

  onum = 0 ;
  G_init_graphics(800,800) ;
  while (0==0) {
    G_rgb(0, 0, 0);
    G_clear();
    draw_object(onum);
    key=G_wait_key();
    if (key=='q') {break;}
    
    else if (key=='t') {

      M2d_make_translation(m, 10, 5);
      M2d_mat_mult_points(x[onum], y[onum], m, x[onum], y[onum], numpoints[onum]);
      
    } else if (key=='s') {
      
      M2d_make_translation(m, -400,-400);
      M2d_mat_mult_points(x[onum], y[onum], m, x[onum], y[onum], numpoints[onum]);
      M2d_make_scaling(m, 0.98, 0.98);
      M2d_mat_mult_points(x[onum], y[onum], m, x[onum], y[onum], numpoints[onum]);
      M2d_make_translation(m, 400, 400);
      M2d_mat_mult_points(x[onum], y[onum], m, x[onum], y[onum], numpoints[onum]);

    } else if (key=='r') {
      double tm1[3][3], tm2[3][3], tm3[3][3];

      M2d_make_translation(tm1, -400,-400);
      M2d_make_rotation(tm2, 2*M_PI/180);
      M2d_make_translation(tm3, 400, 400);
      
      M2d_mat_mult_points(x[onum], y[onum], tm1, x[onum], y[onum], numpoints[onum]);
      M2d_mat_mult_points(x[onum], y[onum], tm2, x[onum], y[onum], numpoints[onum]);
      M2d_mat_mult_points(x[onum], y[onum], tm3, x[onum], y[onum], numpoints[onum]);
           
    }
    else if ('0'<=key && key<='9') {
      w=key-'0';
      if (w<numobjects) {
        onum=w;
      }
    }
    draw_object(onum) ;
  }  
}



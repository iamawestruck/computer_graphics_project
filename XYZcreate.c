#include "FPToolkit.c"
#include "M3d_matrix_tools.c"
#include <stdio.h>
#include <stdlib.h>

int click_and_save(double x[], double y[])
{
  double xy[2], xr, yr;
  int numpoints=0;
  while (0 == 0) {
     G_wait_click(xy) ;
     if ((xy[0] >= 0) && (xy[0] <= 10) && 
         (xy[1] >= 0) && (xy[1] <= 10))  { break ; }
     G_rgb(0, 0, 0) ; // black
     G_circle(xy[0], xy[1], 3) ;     
     x[numpoints] = xy[0] ; y[numpoints] = xy[1] ;
     if (numpoints > 0) {
       G_line (x[numpoints-1],y[numpoints-1], x[numpoints],y[numpoints]) ;
     }
     numpoints++ ;
  }
  //G_line (x[numpoints-1],y[numpoints-1], x[0],y[0]) ;  
  return numpoints ;
}



int main()
{
  char *filename = "test.xyz";
  int key, np, faces=0, slices, counter=0, tcounter=0, polys=0, point=0, total;
  double x[100], y[100], z[100];
  double temp[4][4];
  FILE *fp = fopen(filename, "w");

  printf("How many slices?\n");
  scanf("%d", &slices);
  
  G_init_graphics(200,200);
  G_rgb(1,1,1);
  G_clear();
  G_rgb(0,0,0);
  G_fill_rectangle(0,0,10,10);
  np = click_and_save(x,y);

  

  
  for(int i=0; i<np; i++){ z[i]=0; }
  
  total = np*slices;
  fprintf(fp,"%d\n", total); 
  
    for(int t=0; t<slices; t++){
      M3d_make_x_rotation_cs(temp, cos(2*M_PI/slices), sin(2*M_PI/slices)); 
      M3d_mat_mult_points(x, y, z, temp, x, y, z, np+1);
      for(int k=0; k<np; k++){
	fprintf(fp,"   %lf    %lf    %lf\n", x[k], y[k], z[k]); 
      }
    }
  
  
    polys=slices*(np-1); //works
  

  fprintf(fp,"%d\n", polys);
  for(counter=0; counter<total; counter++){
   if(counter<(total-np)){
    for(int i=0; i<(np-1);i++){
      point=counter;
      fprintf(fp, "4   %d ", point);
      point++;
      fprintf(fp, "%d ", point);
      point=point+np;
      fprintf(fp, "%d ", point);
      point--;
      fprintf(fp, "%d\n", point);
      counter++;
      }
   } else {
     tcounter=counter;
      for(int i=0; i<(np-1);i++){
        point=counter;
        fprintf(fp, "4   %d ", point);
        point++;
        fprintf(fp, "%d ", point);
        point=point-tcounter;
        fprintf(fp, "%d ", point);
        point--;
        fprintf(fp, "%d\n", point);
        counter++;
      }
    }
  }
  /*
  for(int i=0; i<(total-1);i+=np){
    for (int j=0; j<op-1; j++) {
      points=i+j;
      fprintf(fp, "4   %d ", points);
      points++;
      fprintf(fp, "%d ", points);
      points+=np;
      if (points>total) {points=points-total;}
      fprintf(fp, "%d ", points);
      points--;
      fprintf(fp, "%d\n", points);
    }
  }
  */
  fclose(fp);
}

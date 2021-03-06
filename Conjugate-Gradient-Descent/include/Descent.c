//This is now general for all sets

#include "Energy.h"

//this updates the points in the ball by using the gradient to move each point
void updateBall(pt *newpts, pt *oldpts, int numpts, double *pvec, double t,int dim,double inner){
  int i, j;
  double distSQ, newcomponent, scalefac;
  for(i=0;i<numpts;i++){
    newpts[i].index=oldpts[i].index;
    distSQ=0;
    for(j=0;j<dim;j++){
      newcomponent=oldpts[i].components[j]-t*pvec[i*dim+j];
      newpts[i].components[j]=newcomponent;
      distSQ+=newcomponent*newcomponent;
    }
    if (distSQ>1){
      scalefac=1/sqrt(distSQ);     
      for(j=0;j<dim;j++){
        newpts[i].components[j]=scalefac*newpts[i].components[j];
      }
    }
  }     
}

//updates the points in the shell by using the gradient to move each point
void updateShell(pt *newpts, pt *oldpts, int numpts, double *pvec, double t,int dim,double inner_rad){
    int i, j;
    double distSQ, newcomponent, scalefac;
    
    for(i=0;i<numpts;i++){
        newpts[i].index=oldpts[i].index;
        distSQ=0;
        for(j=0;j<dim;j++){
            newcomponent=oldpts[i].components[j]-t*pvec[i*dim+j];
            newpts[i].components[j]=newcomponent;
            distSQ+=newcomponent*newcomponent;
        }
        if (distSQ>1){
            scalefac=1/sqrt(distSQ);     
            for(j=0;j<dim;j++){
                newpts[i].components[j]=scalefac*newpts[i].components[j];
            }
        }
        if (distSQ<inner_rad*inner_rad){
						 
            scalefac=inner_rad/sqrt(distSQ);
            for(j=0;j<dim;j++){
                newpts[i].components[j]=scalefac*newpts[i].components[j];
            }
        }
    }     
}

//updates the points in the sphere by using the gradient to move each point
void updateSphere(pt *newpts, pt *oldpts, int numpts, double *pvec, double t,int dim,double inner){
    int i, j;
    double distSQ, newcomponent, scalefac;
    
    for(i=0;i<numpts;i++){
        newpts[i].index=oldpts[i].index;
        distSQ=0;
        for(j=0;j<dim;j++){
            newcomponent=oldpts[i].components[j]-t*pvec[i*dim+j];
            newpts[i].components[j]=newcomponent;
            distSQ+=newcomponent*newcomponent;
        }
        scalefac=1/sqrt(distSQ);     
        for(j=0;j<dim;j++){
            newpts[i].components[j]=scalefac*newpts[i].components[j];
        }
    }     
}

//performs gradient descent by finding a value of t, finding the gradient, updating the points, testing if the energy has decreased, if not, a different value of t is chosen
//and the energy is then checked again, this process repeats until a lower energy is found

double linesearch (pt *points, pt *newpoints, int numpts, double initEnergy, double *theDir, double t0, double radius, double* cornera, double* cornerb, cube *cubes, int numCubes, int *neighborArray, int maxNbrs, double s, cube *neighborhood, nbhd *nbhds, FILE *writeTo, int *trinaryList, nbhd* cubeNbhd, int* testIndex, int* cubePtsArray,int dim,double inner,void(*updateMethod)(pt *,pt *,int,double *,double,int,double), double *minDistance, double *maxGradient){
  double en0, en1, en2;
  int count = 0; 
  double t1,t_cal;
	done=0;

  en0=initEnergy;


////////////////////////NEW CODE STARTS HERE//////////////////////////////////////////////
	//I am inputting a method that finds t0 by finding the min distance and the max gradient
	double t00;
	int i,j,d;
    double maxV = *maxGradient;
//	double delta=100000.0;
//	double maxV=0.0;
//	double p_dist;
//	double g_dist;
//	for(i=0;i<numpts;i++){
//		for(j=i+1;j<numpts;j++){
//			p_dist=dist(points[i],points[j],dim);
//			g_dist=0.0;
//			for(d=0;d<dim;d++){
//				g_dist+=(pow((theDir[i*dim+d]-theDir[j*dim+d]),2));
//			}
//			g_dist=pow(g_dist,0.5);
//			if(p_dist<delta){
//				delta=p_dist;
//			}
//			if(g_dist>maxV){
//				maxV=g_dist;
//			}
//		}
//	}
	t00=0.5*(*minDistance)/maxV;
//	printf("Old t0 = %lf\n",t0);
//	printf("New t0 = %lf\n",t00);
	t0=t00;
    
//    printf("t00: %f\n", t0);

////////////////////////////END OF NEW SEGMENT OF CODE//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


	t1=t0;
  updateMethod(newpoints, points, numpts, theDir, t0,dim,inner);
  en1 = sEnergy(newpoints, s, radius, cornera, cornerb, numpts, cubes, numCubes, nbhds, neighborArray, neighborhood, maxNbrs, trinaryList, cubeNbhd, testIndex, cubePtsArray,dim, minDistance, maxGradient);

  while(en1 > en0 && count < 20){
    t1 = 0.1* t1;
    updateMethod(newpoints, points, numpts, theDir, t1,dim,inner);
    en1 = sEnergy(newpoints, s, radius, cornera, cornerb, numpts, cubes, numCubes, nbhds, neighborArray, neighborhood, maxNbrs, trinaryList, cubeNbhd, testIndex,cubePtsArray,dim,minDistance, maxGradient);
    count++;
  }   
  if(en1>en0){
    perror("choose smaller t0");
    return 0;
  }
  if(t1 != t0){
    updateMethod(newpoints, points, numpts, theDir, t1,dim,inner);
    en1 = sEnergy(newpoints, s, radius, cornera, cornerb, numpts, cubes, numCubes, nbhds, neighborArray, neighborhood, maxNbrs, trinaryList, cubeNbhd, testIndex,cubePtsArray,dim,minDistance, maxGradient);
  }
  updateMethod(newpoints, points, numpts, theDir, 2*t1,dim,inner);
  en2 = sEnergy(newpoints, s, radius, cornera, cornerb, numpts, cubes, numCubes, nbhds, neighborArray, neighborhood, maxNbrs, trinaryList, cubeNbhd, testIndex,cubePtsArray,dim,minDistance, maxGradient);
  int k = 1;
  while(en2 < en1 && k < 10){
    en0 = en1;
    en1 = en2;
    updateMethod(newpoints, points, numpts, theDir, t1 * (k+2),dim,inner);
    en2 = sEnergy(newpoints, s, radius, cornera, cornerb, numpts, cubes, numCubes, nbhds, neighborArray, neighborhood, maxNbrs, trinaryList, cubeNbhd, testIndex,cubePtsArray,dim,minDistance, maxGradient);
    k++;
  }
	if(en2>en1){
    t_cal = t1*(k + (en0 - en2)/(2*(en0+en2-2*en1)));
    updateMethod(newpoints, points, numpts, theDir, t_cal,dim,inner);
		en2 = sEnergy(newpoints, s, radius, cornera, cornerb, numpts, cubes, numCubes, nbhds, neighborArray, neighborhood, maxNbrs, trinaryList, cubeNbhd, testIndex,cubePtsArray,dim,minDistance, maxGradient);
		if(en2>en1){
		//this is one step between the high en2 and low en1, its not (k+1) because of how k increments
			t_cal=t1*k;
			updateMethod(newpoints,points,numpts,theDir,t_cal,dim,inner);
			en2 = sEnergy(newpoints, s, radius, cornera, cornerb, numpts, cubes, numCubes, nbhds, neighborArray, neighborhood, maxNbrs, trinaryList, cubeNbhd, testIndex,cubePtsArray,dim,minDistance, maxGradient);
		}
  }
	else{
		t_cal=(k+1)*t1;
	}

/////////////////NEW CODE THAT PRINTS OUT IF t_n*v_n///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

	FILE *epsFile = fopen("epsFile.txt","a+");
	if(roundcount==0){
		fprintf(epsFile,"n\tt\t\tmaxV\t\tt*maxV\t\tenergy\n");
	}
//	maxV=0;
//	for(i=0;i<numpts;i++){
//		for(j=i+1;j<numpts;j++){
//			p_dist=dist(newpoints[i],newpoints[j],dim);
//			g_dist=0.0;
//			for(d=0;d<dim;d++){
//				g_dist+=(pow((theDir[i*dim+d]-theDir[j*dim+d]),2));
//			}
//			g_dist=pow(g_dist,0.5);
//			if(g_dist>maxV){
//				maxV=g_dist;
//			}
//		}
//	}
    fprintf(epsFile,"%d\t%lf\t%lf\t%lf\t%lf\n",roundcount,t_cal,maxV,t_cal*maxV,en2);
		
	fclose(epsFile);

	//checking to see if the program has converged	
	if(t1*maxV<=0.0000009){
		done=1;
	}

////////////////////////////END OF NEW CODE//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

	//why is t_calE14 output?
  fprintf(writeTo, "%d\t%lf\t%lf\t", roundcount, t_cal*pow(10, 14), en1);
  fprintf(writeTo, "\t%lf\n", en2);
  roundcount++;
  return t_cal;
}

double ConjGrad(pt *points, pt *newpoints, int numpts, double currEnergy, double *theDir, double *newGrad, double *oldGrad, double t0, double radius, double* cornera, double* cornerb, cube *cubes, int numCubes, int *neighborArray, int maxNbrs, double s, cube *neighborhood, nbhd *nbhds, FILE *writeTo, int *trinaryList, nbhd* cubeNbhd, int* testIndex, int* cubePtsArray,int dim,double inner,void(*updateMethod)(pt *,pt *,int,double *,double,int,double),double(*gradMethod)(pt *,double *,double,double,double *,double *,int,cube *,int,nbhd *,int *,cube *,int,int *,nbhd *,int *,int *,int,double,double*, double*), double *minDistance, double *maxGradient){
  int count = 0; 
  double t_cal;
	done=0;


t_cal=linesearch(points, newpoints, numpts, currEnergy, theDir, t0, radius, cornera, cornerb, cubes, numCubes, neighborArray,  maxNbrs, s, neighborhood, nbhds, writeTo, trinaryList, cubeNbhd, testIndex, cubePtsArray, dim, inner, updateMethod, minDistance, maxGradient);

currEnergy=gradMethod(newpoints, newGrad, s, radius, cornera, cornerb, numpts, cubes, numCubes, nbhds, neighborArray, neighborhood, maxNbrs, trinaryList, cubeNbhd, testIndex,cubePtsArray,dim,inner, minDistance, maxGradient);


double beta_num=0;
double beta_den=0;
double beta=0;

int i;

for(i=0; i<numpts*dim; i++){
beta_num+=newGrad[i]*(newGrad[i]-oldGrad[i]);
beta_den+=(oldGrad[i]*oldGrad[i]);
}

if (beta_den>0) beta=beta_num/beta_den;
 
if (beta<0) {beta=0;}

for(i=0; i<numpts*dim; i++){
theDir[i]=newGrad[i]+beta*theDir[i];
}

	printf("en = %lf \t ",currEnergy); printf("beta = %lf \n",beta);


  return t_cal;
}


double GradDescent(pt *points, pt *newpoints, int numpts, double *theGrad, double t0, double radius, double* cornera, double* cornerb, cube *cubes, int numCubes, int *neighborArray, int maxNbrs, double s, cube *neighborhood, nbhd *nbhds, FILE *writeTo, int *trinaryList, nbhd* cubeNbhd, int* testIndex, int* cubePtsArray,int dim,double inner,void(*updateMethod)(pt *,pt *,int,double *,double,int,double),double(*gradMethod)(pt *,double *,double,double,double *,double *,int,cube *,int,nbhd *,int *,cube *,int,int *,nbhd *,int *,int *,int,double), double *minDistance, double *maxGradient){
  double en0, en1, en2;
  int count = 0; 
  double t_cal;
	done=0;

     
t_cal=linesearch(points, newpoints, numpts, en0, theGrad, t0, radius, cornera, cornerb, cubes, numCubes, neighborArray,  maxNbrs, s, neighborhood, nbhds, writeTo, trinaryList, cubeNbhd, testIndex, cubePtsArray, dim, inner, updateMethod, minDistance, maxGradient);

  return t_cal;
}



/*
	Multithreaded Select Landing Spot Algorithm

*/


#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <vector>
#include "pthread.h"

#define THREAD_COUNT 4
#define RESOLUTION 0.1
#define LIMITS 20.0
#define THRESHOLD 0.075
#define PERCENTAGE 0.7
#define TAM 200

using namespace cv;
using namespace std;
Mat mapcv, thResult;
string filename;
long rowsPerThread;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
     uchar* destination;
     if  ( event == EVENT_LBUTTONDOWN ) {
          cout << "Left button (" << y << ", " << x << ")" << endl;
          cout << "val:"<< mapcv.at<float>(y,x) <<endl;
     }
     else if  ( event == EVENT_RBUTTONDOWN ) {
          cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if  ( event == EVENT_MBUTTONDOWN ) {
          cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
}

Mat tab_to_mat() {
	Mat mapcv(TAM, TAM, CV_32F, Scalar(0.0));
    std::ifstream file("./maps/"+filename+".txt");
    std::string str;

    for(int i=0; i<TAM; i++){
    	for(int j=0; j<TAM; j++){
    		getline(file, str,'\t');
    		mapcv.at<float>(i,j) = std::stof(str);
    	}
    }

    cout <<"end tab to mat" << endl;
    file.close();
	return mapcv;
}

struct Spot {
	Point coordinate;
	int stability;
};

struct param{
	Mat map;
	Mat res;
	int radius;
	float slope;
	int thNum;
};

void *threadRoutine(void *p){
	struct param *data = (struct param*)p;
	int radius = data->radius;
	float slope = data->slope;
	int thNum = data->thNum;
	int cindex=0;

	int firstRow = rowsPerThread * data->thNum;

	Mat localMap(rowsPerThread, TAM, CV_8UC3, Scalar(255,255,255) );


	for(int row = firstRow+radius, lr = radius; row < (firstRow+rowsPerThread) ; row+= radius, lr+= radius){
		for(int col = radius; col < mapcv.cols; col+= radius){
			

			if( mapcv.at<float>(row,col) != 0 && 
				abs(mapcv.at<float>(row,col-radius) - mapcv.at<float>(row,col+radius)) < slope &&
				abs(mapcv.at<float>(row-radius,col) - mapcv.at<float>(row+radius,col)) < slope &&
				abs(mapcv.at<float>(row-radius,col-radius) - mapcv.at<float>(row+radius,col+radius)) < slope &&
				abs(mapcv.at<float>(row-radius,col+radius) - mapcv.at<float>(row+radius,col-radius)) < slope) {
					//pthread_mutex_lock( &mutex1 );
					//cout << "("<<col<<","<<row<<")"<<endl; // (x,y)
					//pthread_mutex_unlock( &mutex1 );
					
					for(int pi=lr-radius+1; pi<lr+radius-1; pi++){
						for(int pj=col-radius+1; pj<col+radius-1; pj++){
							localMap.at<Vec3b>(pi,pj)[0] = 0; 	// B
							localMap.at<Vec3b>(pi,pj)[1] = 0; 	// G
							localMap.at<Vec3b>(pi,pj)[2] = 255; // R


						} // For col
					} // For rows

			}

		}

	}
	pthread_mutex_lock( &mutex1 );
	localMap.copyTo( data->res( Rect(0, firstRow, localMap.cols, localMap.rows) ) );
	pthread_mutex_unlock( &mutex1 );

}

Point select_landing_spot(Mat map, int radius, float slope){
	Mat result(map.rows, map.cols, CV_8UC3, Scalar(255,255,255));
	Mat mapClone = map.clone();
	Spot pts[100];
	int cindex=0;

	for(int row=radius; row<map.rows-radius; row+=radius){
		for(int col=radius; col<map.cols-radius; col+=radius){
			/*
			Check slope:
			1) from left to right
			map.at<float>(row,col-diameter) - map.at<float>(row,col+diameter)
			2) from top to bottom
			map.at<float>(row-diameter,col) - map.at<float>(row+diameter,col)
			3) from left-upper corner to right-lower corner
			map.at<float>(row-diameter,col-diameter) - map.at<float>(row+diameter,col+diameter)
			map.at<float>(row-diameter,col+diameter) - map.at<float>(row+diameter,col-diameter)

			should be less than the given maximum slope.
			*/
				/*
				// Print condition
				cout << "("<< row << ","<< col<< ") "<<endl;
				cout << ( abs(map.at<float>(row,col-radius) - map.at<float>(row,col+radius)) < slope ) << " ";
				cout << ( abs(map.at<float>(row-radius,col) - map.at<float>(row+radius,col)) < slope ) << " ";
				cout << ( abs(map.at<float>(row-radius,col-radius) - map.at<float>(row+radius,col+radius)) < slope ) << " ";
				cout << ( abs(map.at<float>(row-radius,col+radius) - map.at<float>(row+radius,col-radius)) < slope ) <<endl;
				*/
				if( map.at<float>(row,col) != 0 && 
					abs(map.at<float>(row,col-radius) - map.at<float>(row,col+radius)) < slope &&
					abs(map.at<float>(row-radius,col) - map.at<float>(row+radius,col)) < slope &&
					abs(map.at<float>(row-radius,col-radius) - map.at<float>(row+radius,col+radius)) < slope &&
					abs(map.at<float>(row-radius,col+radius) - map.at<float>(row+radius,col-radius)) < slope) {

					pts[cindex].coordinate = Point(col, row); // (col, row) (x,y)
					pts[cindex].stability = 0;
					for(int pi=row-radius+1; pi<row+radius-1; pi++){
						for(int pj=col-radius+1; pj<col+radius-1; pj++){
							result.at<Vec3b>(pi,pj)[0] = 0; 	// B
							result.at<Vec3b>(pi,pj)[1] = 0; 	// G
							result.at<Vec3b>(pi,pj)[2] = 255; // R

							//cout << "Paint: (" << pi<<" , "<< pj<<")" << endl;
							//cout << "c";
							// Check neighbours arriba-abajo, izq-der, arrizq-abajder, abajizq-arrder

							int p1 = map.at<float>(pi-1,pj);
							int p2 = map.at<float>(pi+1,pj);
							int p3 = map.at<float>(pi,pj-1);
							int p4 = map.at<float>(pi,pj+1);
							int p5 = map.at<float>(pi-1,pj-1);
							int p6 = map.at<float>(pi+1,pj+1);
							int p7 = map.at<float>(pi+1,pj-1);
							int p8 = map.at<float>(pi-1,pj+1);
							if( p1 != 0 && p2 != 0 && p3 != 0 && p4 != 0 &&
								p5 != 0 && p6 != 0 && p7 != 0 && p8 != 0 ){
								if( abs( p1 - p2 ) < slope && abs( p3 - p4 ) < slope &&
									abs( p5 - p6 ) < slope && abs( p7 - p8 ) < slope ){
									//cout << "   >>BUENO" << endl;
									pts[cindex].stability += 1;
								}
							}
						} // For col
					} // For rows

					cindex++;
					if(cindex == 100){
						row=map.rows-radius;
						col=map.cols-radius;
					}
				}

		} // for col
	} // for row
	cout << "RESULTS:"<<endl;
	Spot bestplacetoland;
	bestplacetoland.stability=-1;
	bestplacetoland.coordinate=Point(0,0);
	for(int k = 0; k < cindex; k++){
		cout << ">" << pts[k].coordinate << "  S:" << pts[k].stability <<endl;
		if(pts[k].stability > bestplacetoland.stability){
			bestplacetoland.coordinate = pts[k].coordinate;
		}
	}
	cout << "Landing spot : "<< bestplacetoland.coordinate<<endl;


	imshow("SLS: Options", result);
	setMouseCallback("SLS: Options", CallBackFunc, NULL);

	return bestplacetoland.coordinate;
}

int main(int argc, char* argv[])
{

	if(argc != 4){
		cout << "USAGE: "<< argv[0] << " <filename> <radius> <slope>"<<endl;
		return -1;
	}
	filename = argv[1];

	// Read input parameters
	clock_t begin, end;
	double ttime;
	char key;


	mapcv = tab_to_mat();

	int rad = atoi(argv[2]);
	float slope = atof(argv[3]);
	Point target;

	begin = clock();
	target = select_landing_spot(mapcv, rad, slope);
	end = clock();
	
	ttime = (double) (end - begin)/CLOCKS_PER_SEC;
	cout << "SLS: Finding spot time = " << ttime << endl << endl;


	//----------------------------------
	
	pthread_t threadHandle[THREAD_COUNT];
	struct param threadParameters[THREAD_COUNT];
	Mat thResult(TAM, TAM, CV_8UC3, Scalar(255,255,255)  );

	begin=clock();
	rowsPerThread = TAM/THREAD_COUNT;

	for(int i=0; i<THREAD_COUNT;i++){
		threadParameters[i].map = mapcv;
		threadParameters[i].res = thResult;
		threadParameters[i].radius = rad;
		threadParameters[i].slope = slope; 
		threadParameters[i].thNum=i;
		pthread_create( &threadHandle[i], NULL, &threadRoutine, &threadParameters[i]);
	}
	for(int i=0; i<THREAD_COUNT;i++){
		pthread_join( threadHandle[i], NULL);
	}
	end=clock();
	ttime = (double) (end - begin)/CLOCKS_PER_SEC;
	cout<<"MULTI SLS: Finding spot time = "<<ttime<<endl<<endl;	

	imshow("TH RESULT", thResult);
	setMouseCallback("TH RESULT", CallBackFunc, NULL);

	cout<<endl<<"Press 'q' to close each windows ... "<<endl;
	while(key != 27) {
		imshow("Opencv Map", mapcv);
		setMouseCallback("Opencv Map", CallBackFunc, NULL);
		key = waitKey(1);
	}

	return 0;
}

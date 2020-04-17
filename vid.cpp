/*
David Dejmal PRL 2020

Odd-even transposition sort

*/
#include <mpi.h>
#include <fstream>

#include <chrono>
#include <unistd.h>

#include <iostream>
#include <string>
#include <sstream>

#include <math.h> 
//#define TIME_COUNT
#define TAG 0
#define BUFSIZE 256    //maximalni mnozstvi hodnot

using namespace std;

int main(int argc, char *argv[])
{
	int id;
	int pocetProcesu;
	
	int vychoziVyska;
	int mojeHodnota;
	double  uhelA;
	double  uhelB;
	MPI_Status stat; 
	
	int pocetPrvku=1;
	int vyskaA;
	int vyskaB;
	int upsweep;

	
	int pocetProcesuD;
	
	double uhly[24];
	double maximap[24];
	
	int helpVar;

  	MPI_Init(&argc,&argv);                       
    MPI_Comm_size(MPI_COMM_WORLD, &pocetProcesu);       
    MPI_Comm_rank(MPI_COMM_WORLD, &id); 	

    if(id == 0)    //hodnoty nacte master
   {
		//cout<<endl<<endl<<endl; //TODO
		cout<<"Ahoj argc:"<<argc<<" argv:"<<argv[1]<<endl;
		
		//upraveno z https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
		string s = argv[1];
		string delimiter = ",";
		size_t pos = 0;
		string token;
		while ((pos = s.find(delimiter)) != string::npos) {
			pocetPrvku++;
			s.erase(0, pos + delimiter.length());
		}
		
		s = argv[1];
		pos = 0;
		int vstup[pocetPrvku];
		int q=0;
		while ((pos = s.find(delimiter)) != string::npos) {
			token = s.substr(0, pos);
			istringstream(token) >> vstup[q];
			q++;
			s.erase(0, pos + delimiter.length());
		}
		istringstream(s) >> vstup[q];
		//konec prevzateho kodu
		
		
		for(int i=0;i<pocetPrvku;i++)
			cout<<vstup[i]<<endl;
		
		cout<<endl<<endl<<endl;	//TODO
		
		
		int helpCounter=1;
		for(int j=0;j<2;j++)	//TODO pocetPrvku>25
		{
			for(int i=0;i<pocetProcesu;i++)	//TODO pocetPrvku>25
			{
			//odeslani ostatnim
			//buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
			//if(helpCounter<pocet)
				if(helpCounter<pocetPrvku)
				{
					//cout<<i<<";"<<vstup[helpCounter]<<endl;
					MPI_Send(&vstup[helpCounter++], 1, MPI_INT, i, j, MPI_COMM_WORLD);				

				}
				else
				{
					int help[] = {0};
					MPI_Send(&help[0], 1, MPI_INT, i, j, MPI_COMM_WORLD);
					//cout<<i<<";"<<help[0]<<endl;
				}
			}
		}
		vychoziVyska = vstup[0];
		
	cout<<"j <"<<log2(pocetProcesu)<<endl;
	cout<<"i <"<<((pocetProcesu)-1)<<endl;
	

		
	}
	
		
	MPI_Bcast(&vychoziVyska,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&pocetPrvku,1,MPI_INT,0,MPI_COMM_WORLD);
	//vsichni cekaji na prijeti
    //buffer,velikost,typ,rank odesilatele,tag, skupina, stat
    MPI_Recv(&vyskaA, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat); 
	MPI_Recv(&vyskaB, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat); 
	uhelA = atan((vyskaA-vychoziVyska)/(double)pocetPrvku);
	uhelB = atan((vyskaB-vychoziVyska)/(double)pocetPrvku);
	//cout<<"ID:"<<id<<" hodnota:"<<vyskaA<<";"<<vyskaB<<endl;
	
	upsweep=(vyskaA>vyskaB)?vyskaA:vyskaB;
	//upsweep=vyskaA+vyskaB;
	
	
	MPI_Barrier( MPI_COMM_WORLD );

	
	//cout<<"ID:"<<id<<" up:"<<upsweep<<" hodnoty"<<vyskaA<<";"<<vyskaB<<endl;
	
	//odeslani uhlu masterovi
	//MPI_Send(&uhel, 1, MPI_DOUBLE, 0, TAG,  MPI_COMM_WORLD); //poslani sveho cilsa masterovi

	for(int j=0;j<log2(pocetProcesu);j++)
	{
		for(int i=0;i<((pocetProcesu)-1);i+=(int)pow(2.0,j+1.0))
		{
			if(id==i+(int)pow(2.0,j)-1)
			{
				//if a
			//a send b
			//cout<<"J:"<<j<<"d:"<<(int)pow(2.0,j+1.0)<<endl;
			//cout<<i+(int)pow(2.0,j+1.0)-1<<"="<<i+(int)pow(2.0,j)-1<<"+"<<i+(int)pow(2.0,j+1.0)-1<<endl;
				//cout<<"A send "<<i+(int)pow(2.0,j)-1<<endl;
				
				//buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
				MPI_Send(&upsweep, 1, MPI_INT,i+(int)pow(2.0,j+1.0)-1 , j, MPI_COMM_WORLD);
			}
			
			if(id==i+(int)pow(2.0,j+1.0)-1)
			{
			//if b
			//recv 
			//update
				//cout<<"B recv nad update "<<i+(int)pow(2.0,j+1.0)-1<<endl;
			//buffer,velikost,typ,rank odesilatele,tag, skupina, stat
			
			helpVar=upsweep;
				MPI_Recv(&upsweep, 1, MPI_INT, i+(int)pow(2.0,j)-1, j, MPI_COMM_WORLD, &stat); 
				if(helpVar>upsweep)
				{
					upsweep=helpVar;
				}
				//upsweep+=helpVar;
			}
		MPI_Barrier( MPI_COMM_WORLD );
		}
		//TODO bariera
		//MPI_Barrier( MPI_COMM_WORLD );
	}
	
	cout<<"END-ID:"<<id<<" upsweep:"<<upsweep<<endl;
	/*
	if(id == 0)
	{
		for(int i=0; i<pocetPrvku; i++) //nacteni hodnot zpet do pole
        {
            MPI_Recv(&uhly[i], 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, &stat);
        }
        for(int i=0; i<pocetPrvku; i++) //vypis pole
        {
            cout<<uhly[i]<<endl;
        }
		
	}
	*/

	
	MPI_Finalize(); 
    return 0;
}


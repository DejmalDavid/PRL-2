/*
David Dejmal PRL-2 2020

Line-of-Sight example using MPI

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
#define NEUTRAL -1
using namespace std;

int main(int argc, char *argv[])
{
	int id;
	int pocetProcesu;
	
	int vychoziVyska;
	int mojeHodnota;
	double  uhelA;
	double  uhelB;
	double  uhelMaxA=NEUTRAL;
	double  uhelMaxB=NEUTRAL;
	MPI_Status stat; 
	
	int pocetPrvku=1;
	int vyskaA;
	int vyskaB;
	double upsweep;

	
	int pocetProcesuD;
	
	char visA='u';
	char visB='u';
	
	double helpVar;

  	MPI_Init(&argc,&argv);                       
    MPI_Comm_size(MPI_COMM_WORLD, &pocetProcesu);       
    MPI_Comm_rank(MPI_COMM_WORLD, &id); 	

 //hodnoty nacte a zparsuje master
    if(id == 0)   
   {
		//cout<<endl<<endl<<endl; //TODO
		//cout<<"Ahoj argc:"<<argc<<" argv:"<<argv[1]<<endl;
		
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
		
		
		//for(int i=0;i<pocetPrvku;i++)
		//	cout<<vstup[i]<<endl;
		
		//cout<<endl<<endl<<endl;	//TODO
		
		//odeslani vysek procesorum
		int helpCounter=1;

			for(int i=0;i<pocetProcesu;i++)	//TODO pocetPrvku>25
			{
			//odeslani ostatnim
			//buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
			//if(helpCounter<pocet)
				if(helpCounter<pocetPrvku)
				{
					//cout<<i<<";"<<vstup[helpCounter]<<endl;
					MPI_Send(&vstup[helpCounter++], 1, MPI_INT, i, 0, MPI_COMM_WORLD);				

				}
				else
				{
					int help[] = {0};
					MPI_Send(&help[0], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
					//cout<<i<<";"<<help[0]<<endl;
				}
				if(helpCounter<pocetPrvku)
				{
					//cout<<i<<";"<<vstup[helpCounter]<<endl;
					MPI_Send(&vstup[helpCounter++], 1, MPI_INT, i, 1, MPI_COMM_WORLD);				

				}
				else
				{
					int help[] = {NEUTRAL};
					MPI_Send(&help[0], 1, MPI_INT, i, 1, MPI_COMM_WORLD);
					//cout<<i<<";"<<help[0]<<endl;
				}
			}

		vychoziVyska = vstup[0];
			
	}
	
	//nasdileni stejnych promenych	
	MPI_Bcast(&vychoziVyska,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&pocetPrvku,1,MPI_INT,0,MPI_COMM_WORLD);
	//vsichni cekaji na prijeti a vypocet uhlu
    //buffer,velikost,typ,rank odesilatele,tag, skupina, stat
    MPI_Recv(&vyskaA, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat); 
	if(vyskaA!=NEUTRAL)
	{
		uhelA = atan((vyskaA-vychoziVyska)/(double)pocetPrvku);	
	}
	else
	{
		uhelA=(double)NEUTRAL;
	}
	
	MPI_Recv(&vyskaB, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat); 
	if(vyskaB!=NEUTRAL)
	{
		uhelB = atan((vyskaB-vychoziVyska)/(double)pocetPrvku);
	}
	else
	{
		uhelB=(double)NEUTRAL;
	}


	//cout<<"ID:"<<id<<" hodnota:"<<vyskaA<<";"<<vyskaB<<endl;
	//lokalni max
	upsweep=(uhelA>uhelB)?uhelA:uhelB;
	//upsweep=(vyskaA>vyskaB)?vyskaA:vyskaB;
	//upsweep=vyskaA+vyskaB;
	
	//doporucene srovnani procesu
	MPI_Barrier( MPI_COMM_WORLD );

	
	//cout<<"ID:"<<id<<" up:"<<upsweep<<" yuhly"<<uhelA<<";"<<uhelB<<endl;

	//upsweep dle prezentace predmetu PRL
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
				MPI_Send(&upsweep, 1, MPI_DOUBLE,i+(int)pow(2.0,j+1.0)-1 , j, MPI_COMM_WORLD);
			}
			
			if(id==i+(int)pow(2.0,j+1.0)-1)
			{
			//if b
			//recv 
			//update
				//cout<<"B recv nad update "<<i+(int)pow(2.0,j+1.0)-1<<endl;
			//buffer,velikost,typ,rank odesilatele,tag, skupina, stat
			
			helpVar=upsweep;
				MPI_Recv(&upsweep, 1, MPI_DOUBLE, i+(int)pow(2.0,j)-1, j, MPI_COMM_WORLD, &stat); 
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
	
	//cout<<"END-DOWN:"<<id<<" upsweep:"<<upsweep<<endl;
	
	//downsweep dle prezentace predmetu PRL
	if(id==(pocetProcesu-1))
	{
		upsweep=NEUTRAL;
		//cout<<"IDdowsn:"<<id<<" upsweep:"<<upsweep<<endl;
	}
	
	

	for(int d=log2(pocetProcesu)-1;d>=0;d--)
	{
		for(int i=0;i<((pocetProcesu)-1);i+=(int)pow(2.0,d+1.0))
		{
			//leva
			//cout<<"t="<<(int)(i+pow(2.0,d)-1)<<endl; //nacteni leveho
			//cout<<"L("<<(int)(i+pow(2.0,d)-1)<<")="<<(int)(i+pow(2.0,d+1.0)-1)<<endl; //root send levy
			//cout<<"P("<<(int)(i+pow(2.0,d+1.0)-1)<<")="<<(int)(i+pow(2.0,d)-1)<<"+"<<(int)(i+pow(2.0,d+1.0)-1)<<endl<<endl; //root send pravy
			
			//levy send root t
			//levy recv sweep
			if(id==(int)(i+pow(2.0,d)-1))
			{
				//cout<<"id:"<<id<<" send to:"<<(int)(i+pow(2.0,d+1.0)-1)<<" + recv"<<endl;
				
				//buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
				MPI_Send(&upsweep, 1, MPI_DOUBLE,(int)(i+pow(2.0,d+1.0)-1) , i, MPI_COMM_WORLD);
				//buffer,velikost,typ,rank odesilatele,tag, skupina, stat
				MPI_Recv(&upsweep, 1, MPI_DOUBLE,(int)(i+pow(2.0,d+1.0)-1), i, MPI_COMM_WORLD, &stat); 
			}
			
			//root recv
			//root send levy sweep

			if(id==(int)(i+pow(2.0,d+1.0)-1))
			{
				//cout<<"id:"<<id<<" send to:"<<(int)(i+pow(2.0,d)-1)<<" + recv + update sweep:"<<endl;
				
				//buffer,velikost,typ,rank odesilatele,tag, skupina, stat
				helpVar=upsweep;
				MPI_Recv(&upsweep, 1, MPI_DOUBLE,(int)(i+pow(2.0,d)-1), i, MPI_COMM_WORLD, &stat);
				//upsweep=upsweep+helpVar;
				upsweep=(upsweep>helpVar)?upsweep:helpVar;
				//buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
				MPI_Send(&helpVar, 1, MPI_DOUBLE,(int)(i+pow(2.0,d)-1) , i, MPI_COMM_WORLD);
			}
			
			MPI_Barrier( MPI_COMM_WORLD );		
		}
		
	}
	//cout<<"END-UP:"<<id<<" upsweep:"<<upsweep<<endl;
	
	
	
	if(uhelB!=NEUTRAL) // obsahuje validni hodnotu
	{					//TODO spuvodni vyska
		uhelMaxB=(upsweep>uhelA)?upsweep:uhelA;
	}
	if(uhelA!=NEUTRAL) // obsahuje validni hodnotu
	{
		uhelMaxA=upsweep;
	}
	
	//cout<<"END-ID:"<<id<<" up:"<<upsweep<<" hodnoty:"<<uhelMaxA<<";"<<uhelMaxB<<endl;
	
	//porovnani viditelnosti
	if(uhelA>uhelMaxA)
	{
		visA='v';
	}
	if(uhelB>uhelMaxB)
	{
		visB='v';
	}
	
	
	//odeslani vysledku masterovi
	if(uhelA!=NEUTRAL) // obsahuje validni hodnotu
	{
		//send
		//cout<<"ID:"<<id<<" A:"<<vyskaA<<visA<<endl;
		//buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
		MPI_Send(&visA, 1, MPI_CHAR,0 , 0, MPI_COMM_WORLD);
	}
	if(uhelB!=NEUTRAL) // obsahuje validni hodnotu
	{					
		//send
		//cout<<"ID:"<<id<<" B:"<<vyskaB<<visB<<endl;
		//buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
		MPI_Send(&visB, 1, MPI_CHAR,0 , 1, MPI_COMM_WORLD);
	}
	

	//prijem a vypis vysledku
	if(id==0)
	{
		char vystup[pocetPrvku];
		for(int i=0; i<pocetPrvku-1; i++) //nacteni hodnot zpet do pole
		{
			MPI_Recv(&vystup[i], 1, MPI_CHAR, i/2, i%2 , MPI_COMM_WORLD, &stat);
		}
		cout<<"_";
		for(int i=0; i<pocetPrvku-1; i++) //vypis pole
		{
			cout<<","<<vystup[i];
		}
			
		cout<<endl;
	}

	//finito
	MPI_Finalize(); 
    return 0;
}


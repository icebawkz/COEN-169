//Alex McAfee
//COEN 169 - Web Information Management
//Project 2 - Movie Recommendation System
//Yi Fang


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// don't want to deal with passing in between functions
int ratings[500][1000];
int ratingResults[100][1000];
int numRated;
int offset;

//standard function declarations
int pearsonCorrelation (int, int);
int pearsonIUF (int, int);
int pearsonCase (int, int);
int cosineSimilarity (int, int);
int itemBasedCosine (int, int);
int custom (int, int);

/*                                                                                           
        CCCCCCCCCCCCC                                    iiii                                       
     CCC::::::::::::C                                   i::::i                                      
   CC:::::::::::::::C                                    iiii                                       
  C:::::CCCCCCCC::::C                                                                               
 C:::::C       CCCCCC   ooooooooooo       ssssssssss   iiiiiiinnnn  nnnnnnnn        eeeeeeeeeeee    
C:::::C               oo:::::::::::oo   ss::::::::::s  i:::::in:::nn::::::::nn    ee::::::::::::ee  
C:::::C              o:::::::::::::::oss:::::::::::::s  i::::in::::::::::::::nn  e::::::eeeee:::::ee
C:::::C              o:::::ooooo:::::os::::::ssss:::::s i::::inn:::::::::::::::ne::::::e     e:::::e
C:::::C              o::::o     o::::o s:::::s  ssssss  i::::i  n:::::nnnn:::::ne:::::::eeeee::::::e
C:::::C              o::::o     o::::o   s::::::s       i::::i  n::::n    n::::ne:::::::::::::::::e 
C:::::C              o::::o     o::::o      s::::::s    i::::i  n::::n    n::::ne::::::eeeeeeeeeee  
 C:::::C       CCCCCCo::::o     o::::ossssss   s:::::s  i::::i  n::::n    n::::ne:::::::e           
  C:::::CCCCCCCC::::Co:::::ooooo:::::os:::::ssss::::::si::::::i n::::n    n::::ne::::::::e          
   CC:::::::::::::::Co:::::::::::::::os::::::::::::::s i::::::i n::::n    n::::n e::::::::eeeeeeee  
     CCC::::::::::::C oo:::::::::::oo  s:::::::::::ss  i::::::i n::::n    n::::n  ee:::::::::::::e  
        CCCCCCCCCCCCC   ooooooooooo     sssssssssss    iiiiiiii nnnnnn    nnnnnn    eeeeeeeeeeeeee   

The cosine similarity algorithm serves as the baseline for this project and is supposed to merit an MAE of ~.8
The basic algorithm for Cosine Similarity is 
Cosine Similarity (d1, d2) =  Dot product(d1, d2) / ||d1|| * ||d2||

Dot product (d1,d2) = d1[0] * d2[0] + d1[1] * d2[1] * … * d1[n] * d2[n]
||d1|| = square root(d1[0]2 + d1[1]2 + ... + d1[n]2)
||d2|| = square root(d2[0]2 + d2[1]2 + ... + d2[n]2)
In this scenario we will treat each group of movie ratings as a vector 
so for instance we might be comparing [5,1,3,4,6] to [2,0,1,4,6]                                                                                                  
*/
int cosineSimilarity (int user, int movie)
{
	int rating = 0;
	int testMovies[numRated];
	float relevantUsers[200][4]; // this 2d array is usd to hold info
	float average = 0;
	float Standard_dev = 0;
	int i, j, count, count2;
	
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
			relevantUsers[i][j] = 0;
	}
    
	// Calculate average rating for user in question
	count = 0;
	average = 0;
	for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9))
		{
			testMovies[count] = i;
			average += ratings[user][i];
			count++;
		}
	}
	//generic average formula
	average = average / (float)count;
	count2 = 0;
	//iterate through train data up to the number rated
	for (i=0; i<200; i++)
	{
		count = 0;
		for (j=0; j<numRated; j++) //up to the nuber rated
		{
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				count++; //if within bounds increment coutner
		}
		if (count == numRated) // if rated the same then we want to check out data
		{
			for (j=0; j<numRated; j++)
			{
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
					relevantUsers[count2][3] = i;
					relevantUsers[count2][0] += ratings[i][j]; //running total
				}
			}
			count2++;
			relevantUsers[count2][0] = relevantUsers[count2][0] / count;
		}
	}
    
	// now we want to get the denom
	for (i=0; i<numRated; i++)
		Standard_dev += (ratings[user][testMovies[i]] - average) * (ratings[user][testMovies[i]] - average); //calculate the weighting
	Standard_dev = sqrt (Standard_dev);

	//iteraating through train values again
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0) //finding relevant users and updating the aptly named relevantUsers
		{
			for (j=0; j<numRated; j++)
                relevantUsers[i][1] += (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
    
	float numerator, denominator;
	numerator = 0;
	denominator = 0;
	//logic catch
	if (count2 == 0)
	{
		rating = average;
		return rating;
	}
	//actually calculating the parts to be used in the Cosine algo
	for (i=0; i<200; i++) //loop because summations
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
				numerator += (ratings[user][testMovies[j]] - average) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);	
			denominator = Standard_dev * relevantUsers[i][1];
			relevantUsers[i][2] = numerator / denominator;
		}
	}
    
	// Compute prediction
	numerator = 0;
	denominator = 0;
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
			{
				numerator += relevantUsers[i][2] * ratings[(int)relevantUsers[i][3]][testMovies[j]];
				denominator += relevantUsers[i][2];
			}
		}
	}

	//Here is your normal Cosine algorithm after all the backend
	rating = (numerator / denominator);
	
	if (rating > 5)
		rating = 5;
	if (rating < 1)
		rating = 1;
	
	// Return final calculated value
	return rating;	
}

/*                                                                                                                            
PPPPPPPPPPPPPPPPP                                                                                                            
P::::::::::::::::P                                                                                                           
P::::::PPPPPP:::::P                                                                                                          
PP:::::P     P:::::P                                                                                                         
  P::::P     P:::::P  eeeeeeeeeeee    aaaaaaaaaaaaa  rrrrr   rrrrrrrrr       ssssssssss      ooooooooooo   nnnn  nnnnnnnn    
  P::::P     P:::::Pee::::::::::::ee  a::::::::::::a r::::rrr:::::::::r    ss::::::::::s   oo:::::::::::oo n:::nn::::::::nn  
  P::::PPPPPP:::::Pe::::::eeeee:::::eeaaaaaaaaa:::::ar:::::::::::::::::r ss:::::::::::::s o:::::::::::::::on::::::::::::::nn 
  P:::::::::::::PPe::::::e     e:::::e         a::::arr::::::rrrrr::::::rs::::::ssss:::::so:::::ooooo:::::onn:::::::::::::::n
  P::::PPPPPPPPP  e:::::::eeeee::::::e  aaaaaaa:::::a r:::::r     r:::::r s:::::s  ssssss o::::o     o::::o  n:::::nnnn:::::n
  P::::P          e:::::::::::::::::e aa::::::::::::a r:::::r     rrrrrrr   s::::::s      o::::o     o::::o  n::::n    n::::n
  P::::P          e::::::eeeeeeeeeee a::::aaaa::::::a r:::::r                  s::::::s   o::::o     o::::o  n::::n    n::::n
  P::::P          e:::::::e         a::::a    a:::::a r:::::r            ssssss   s:::::s o::::o     o::::o  n::::n    n::::n
PP::::::PP        e::::::::e        a::::a    a:::::a r:::::r            s:::::ssss::::::so:::::ooooo:::::o  n::::n    n::::n
P::::::::P         e::::::::eeeeeeeea:::::aaaa::::::a r:::::r            s::::::::::::::s o:::::::::::::::o  n::::n    n::::n
P::::::::P          ee:::::::::::::e a::::::::::aa:::ar:::::r             s:::::::::::ss   oo:::::::::::oo   n::::n    n::::n
PPPPPPPPPP            eeeeeeeeeeeeee  aaaaaaaaaa  aaaarrrrrrr              sssssssssss       ooooooooooo     nnnnnn    nnnnnn
 
 Pearson Correlation is very highly similar to Cosine Similairty.
 In fact in order to compute pearson correlation all you really have to do is calculate the average rating
 for each user, subtract any given score you want to compare from said average and then 
 run that given rating through the geeric cosine similarity algorithm. In this program I used the same algorithm for both
 since they are so similar.                                                                                                                          
*/
int pearsonCorrelation (int user, int movie)
{
	int rating = 0;
	int testMovies[numRated];
	float relevantUsers[200][4]; 
	float average = 0;
	float Standard_dev = 0;
	int i, j, count, count2;
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
			relevantUsers[i][j] = 0;
	}
	count = 0;
	average = 0;
	for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9))
		{
			testMovies[count] = i;
			average += ratings[user][i];
			count++;
		}
	}
	average = average / (float)count;
	count2 = 0;
	for (i=0; i<200; i++)
	{
		count = 0;
		for (j=0; j<numRated; j++) 
		{
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				count++;
		}
		if (count == numRated) // if the user has rated the same movies, then we look at their data
		{
			for (j=0; j<numRated; j++)
			{
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
					relevantUsers[count2][3] = i;
					relevantUsers[count2][0] += ratings[i][j];
				}
			}
			relevantUsers[count2][0] = relevantUsers[count2][0] / count;
			count2++;
		}
	}
	for (i=0; i<numRated; i++)
		Standard_dev += (ratings[user][testMovies[i]] - average) * (ratings[user][testMovies[i]] - average);
	Standard_dev = sqrt (Standard_dev);
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
					relevantUsers[i][1] += (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
	float numerator, denominator;
	numerator = 0;
	denominator = 0;
	if (count2 == 0)
	{
		rating = average;
		return rating;
	}
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
				numerator += (ratings[user][testMovies[j]] - average) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			denominator = Standard_dev * relevantUsers[i][1];
			relevantUsers[i][2] = numerator / denominator;
		}
	}
	numerator = 0;
	denominator = 0;
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
			{

				numerator += relevantUsers[i][2] * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
			}
		}
	}
	rating = average + (numerator / denominator) + 0.5;
	if (rating > 5)
		rating = 5;
	if (rating < 1)
		rating = 1;	
	return rating;	
}
/*                                                   
IIIIIIIIIIUUUUUUUU     UUUUUUUUFFFFFFFFFFFFFFFFFFFFFF
I::::::::IU::::::U     U::::::UF::::::::::::::::::::F
I::::::::IU::::::U     U::::::UF::::::::::::::::::::F
II::::::IIUU:::::U     U:::::UUFF::::::FFFFFFFFF::::F
  I::::I   U:::::U     U:::::U   F:::::F       FFFFFF
  I::::I   U:::::D     D:::::U   F:::::F             
  I::::I   U:::::D     D:::::U   F::::::FFFFFFFFFF   
  I::::I   U:::::D     D:::::U   F:::::::::::::::F   
  I::::I   U:::::D     D:::::U   F:::::::::::::::F   
  I::::I   U:::::D     D:::::U   F::::::FFFFFFFFFF   
  I::::I   U:::::D     D:::::U   F:::::F             
  I::::I   U::::::U   U::::::U   F:::::F             
II::::::II U:::::::UUU:::::::U FF:::::::FF           
I::::::::I  UU:::::::::::::UU  F::::::::FF           
I::::::::I    UU:::::::::UU    F::::::::FF           
IIIIIIIIII      UUUUUUUUU      FFFFFFFFFFF                                                            

PearsonIUF is a way of further weighting the calculated averages. Inverse Document Frequency
The basi idea is that movies which are popularly rated by lots of people are less likely to be
indicative of trends then movies which are very rarely rated. for example if two people watch the most
popular movie of the year it really doest mean anything especially when compared to wo people watching 
a smaller, niche film. A popular and easy way to calculate IUF is 
        IUFweights[i] = log ((float)totalUsers / (float)usersRated);
because in this formula when all users have seen a movie it gets a wight of 0

almost all of this code is the same as PEearson which is almost the sam as Cosine. The only major difference being the IUF weights
*/
int pearsonIUF (int user, int movie)
{
	int rating = 0;
	int testMovies[numRated];
    float IUFweights[numRated];
    int usersRated;
    int totalUsers = 200;
	float relevantUsers[200][4]; 
	float average = 0;
	float Standard_dev = 0;
	int i, j, count, count2;
	
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
		{
			relevantUsers[i][j] = 0;
		}
	}
    
    count = 0;
    for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9))
		{
			testMovies[count] = i;
            count++;
        }
    }
    
    // we waant to create an IUF weight matrix of sorts by alculating the IUF for each movie and storing that in
    //a 2d array which we can then iterate through and multiply as weights.
    for (i=0; i<numRated; i++)
    {
        usersRated = 0;
        for (j=0; j<200; j++)
        {
            if ((ratings[j][testMovies[i]] != 0) && (ratings[j][testMovies[i]] != 9))
                usersRated++;
        }
        //Here is where we actually calculate the IUF for each movie
        IUFweights[i] = log ((float)totalUsers / (float)usersRated);
    }
    
	count = 0;
	average = 0;

	for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9))
		{
			average += ratings[user][i] * IUFweights[count];
			count++;
		}
	}

	average = average / (float)count;
	count2 = 0;

	for (i=0; i<200; i++)
	{
		count = 0;
		for (j=0; j<numRated; j++) 
		{
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				count++;
		}

		if (count == numRated)
		{
			for (j=0; j<numRated; j++)
			{
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
					relevantUsers[count2][3] = i;
					relevantUsers[count2][0] += ratings[i][j] * IUFweights[count2];
				}
			}
			relevantUsers[count2][0] = relevantUsers[count2][0] / count;
			count2++;
		}
	}
    
	for (i=0; i<numRated; i++)
		Standard_dev += ((ratings[user][testMovies[i]] * IUFweights[i]) - average) * ((ratings[user][testMovies[i]] * IUFweights[i]) - average);
	Standard_dev = sqrt (Standard_dev);
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
                relevantUsers[i][1] += ((ratings[(int)relevantUsers[i][3]][testMovies[j]] * IUFweights[j]) - relevantUsers[i][0]) * ((ratings[(int)relevantUsers[i][3]][testMovies[j]] * IUFweights[j]) - relevantUsers[i][0]);
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
    
	float numerator, denominator;
	numerator = 0;
	denominator = 0;
	
	if (count2 == 0)
	{
		//logi catches
		rating = average + 0.5;
        if (rating > 5)
            rating = 5;
        if (rating < 1)
            rating = 1;      
		return rating;
	}
	
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++) //here you can see the implementation of the IUF weights
				numerator += ((ratings[user][testMovies[j]] * IUFweights[j]) - average) * ((ratings[(int)relevantUsers[i][3]][testMovies[j]] * IUFweights[j]) - relevantUsers[i][0]);
			denominator = Standard_dev * relevantUsers[i][1];
			relevantUsers[i][2] = numerator / denominator;
		}
	}
    
	numerator = 0;
	denominator = 0;
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++) //agan here you can see how the IUF weights are being implemented
			{
                
				numerator += relevantUsers[i][2] * ((ratings[(int)relevantUsers[i][3]][testMovies[j]] * IUFweights[j]) - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
			}
		}
	}

	rating = average + (numerator / denominator) + 0.5;
	
	//more logic catches
	if (rating > 5)
		rating = 5;
	if (rating < 1)
		rating = 1;
	
	return rating;
}
/*                                                                                                                                      dddddddd
        CCCCCCCCCCCCC                                                      MMMMMMMM               MMMMMMMM                             d::::::d
     CCC::::::::::::C                                                      M:::::::M             M:::::::M                             d::::::d
   CC:::::::::::::::C                                                      M::::::::M           M::::::::M                             d::::::d
  C:::::CCCCCCCC::::C                                                      M:::::::::M         M:::::::::M                             d:::::d 
 C:::::C       CCCCCC  aaaaaaaaaaaaa      ssssssssss       eeeeeeeeeeee    M::::::::::M       M::::::::::M   ooooooooooo       ddddddddd:::::d 
C:::::C                a::::::::::::a   ss::::::::::s    ee::::::::::::ee  M:::::::::::M     M:::::::::::M oo:::::::::::oo   dd::::::::::::::d 
C:::::C                aaaaaaaaa:::::ass:::::::::::::s  e::::::eeeee:::::eeM:::::::M::::M   M::::M:::::::Mo:::::::::::::::o d::::::::::::::::d 
C:::::C                         a::::as::::::ssss:::::se::::::e     e:::::eM::::::M M::::M M::::M M::::::Mo:::::ooooo:::::od:::::::ddddd:::::d 
C:::::C                  aaaaaaa:::::a s:::::s  ssssss e:::::::eeeee::::::eM::::::M  M::::M::::M  M::::::Mo::::o     o::::od::::::d    d:::::d 
C:::::C                aa::::::::::::a   s::::::s      e:::::::::::::::::e M::::::M   M:::::::M   M::::::Mo::::o     o::::od:::::d     d:::::d 
C:::::C               a::::aaaa::::::a      s::::::s   e::::::eeeeeeeeeee  M::::::M    M:::::M    M::::::Mo::::o     o::::od:::::d     d:::::d 
 C:::::C       CCCCCCa::::a    a:::::assssss   s:::::s e:::::::e           M::::::M     MMMMM     M::::::Mo::::o     o::::od:::::d     d:::::d 
  C:::::CCCCCCCC::::Ca::::a    a:::::as:::::ssss::::::se::::::::e          M::::::M               M::::::Mo:::::ooooo:::::od::::::ddddd::::::dd
   CC:::::::::::::::Ca:::::aaaa::::::as::::::::::::::s  e::::::::eeeeeeee  M::::::M               M::::::Mo:::::::::::::::o d:::::::::::::::::d
     CCC::::::::::::C a::::::::::aa:::as:::::::::::ss    ee:::::::::::::e  M::::::M               M::::::M oo:::::::::::oo   d:::::::::ddd::::d
        CCCCCCCCCCCCC  aaaaaaaaaa  aaaa sssssssssss        eeeeeeeeeeeeee  MMMMMMMM               MMMMMMMM   ooooooooooo      ddddddddd   ddddd                                                                                                                                             

Pearson with Case Modification is very simple to udnerstand and implement. 
the basic idea is simply that you apply a transformation in the form of
raising all ratings to a certain power and this increases the differences between ratings
which in turn increases accuracy
Again almost all of this code is the same as other Pearson code so the comments are sparing
*/
int pearsonCase (int user, int movie)
{
	int rating = 0;
	int testMovies[numRated];
	float relevantUsers[200][4]; 
	float average = 0;
	float Standard_dev = 0;
	int i, j, count, count2;
	
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
			relevantUsers[i][j] = 0;
	}
    
	count = 0;
	average = 0;
	for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9)) 
		{
			testMovies[count] = i;
			average += ratings[user][i];
			count++;
		}
	}


	average = average / (float)count;

	count2 = 0;



	for (i=0; i<200; i++)
	{
		count = 0;
		for (j=0; j<numRated; j++) 
		{
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				count++;
		}




		if (count == numRated) 
		{
			for (j=0; j<numRated; j++)
			{
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
					relevantUsers[count2][3] = i;
					relevantUsers[count2][0] += ratings[i][j];
				}
			}

			relevantUsers[count2][0] = relevantUsers[count2][0] / count;
			count2++;


		}
	}
    
	for (i=0; i<numRated; i++)
		Standard_dev += (ratings[user][testMovies[i]] - average) * (ratings[user][testMovies[i]] - average);
	Standard_dev = sqrt (Standard_dev);
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
                relevantUsers[i][1] += (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
    
	// Compute similarity weight
	float numerator, denominator;
	numerator = 0;
	denominator = 0;
	
	if (count2 == 0)
	{
		rating = average;
		return rating;
	}
	
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
				numerator += (ratings[user][testMovies[j]] - average) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			denominator = Standard_dev * relevantUsers[i][1];
			relevantUsers[i][2] = numerator / denominator;
            relevantUsers[i][2] = relevantUsers[i][2] * abs(pow (relevantUsers[i][2], 1.5));
		}
	}
    
	numerator = 0;
	denominator = 0;
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<numRated; j++)
			{
                
				numerator += relevantUsers[i][2] * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
			}
		}
	}
	rating = average + (numerator / denominator) + 0.5;
	
	if (rating > 5)
	{
		rating = 5;
	}
	if (rating < 1)
	{
		rating = 1;
	}
	
	return rating;	
}
/*                                                                                               
IIIIIIIIIIBBBBBBBBBBBBBBBBB           CCCCCCCCCCCCCFFFFFFFFFFFFFFFFFFFFFF      AAA               
I::::::::IB::::::::::::::::B       CCC::::::::::::CF::::::::::::::::::::F     A:::A              
I::::::::IB::::::BBBBBB:::::B    CC:::::::::::::::CF::::::::::::::::::::F    A:::::A             
II::::::IIBB:::::B     B:::::B  C:::::CCCCCCCC::::CFF::::::FFFFFFFFF::::F   A:::::::A            
  I::::I    B::::B     B:::::B C:::::C       CCCCCC  F:::::F       FFFFFF  A:::::::::A           
  I::::I    B::::B     B:::::BC:::::C                F:::::F              A:::::A:::::A          
  I::::I    B::::BBBBBB:::::B C:::::C                F::::::FFFFFFFFFF   A:::::A A:::::A         
  I::::I    B:::::::::::::BB  C:::::C                F:::::::::::::::F  A:::::A   A:::::A        
  I::::I    B::::BBBBBB:::::B C:::::C                F:::::::::::::::F A:::::A     A:::::A       
  I::::I    B::::B     B:::::BC:::::C                F::::::FFFFFFFFFFA:::::AAAAAAAAA:::::A      
  I::::I    B::::B     B:::::BC:::::C                F:::::F         A:::::::::::::::::::::A     
  I::::I    B::::B     B:::::B C:::::C       CCCCCC  F:::::F        A:::::AAAAAAAAAAAAA:::::A    
II::::::IIBB:::::BBBBBB::::::B  C:::::CCCCCCCC::::CFF:::::::FF     A:::::A             A:::::A   
I::::::::IB:::::::::::::::::B    CC:::::::::::::::CF::::::::FF    A:::::A               A:::::A  
I::::::::IB::::::::::::::::B       CCC::::::::::::CF::::::::FF   A:::::A                 A:::::A 
IIIIIIIIIIBBBBBBBBBBBBBBBBB           CCCCCCCCCCCCCFFFFFFFFFFF  AAAAAAA                   AAAAAAA                                                                                                
*/
int itemBasedCosine (int user, int movie)
{
    int rating = 0;
    int minimumPairs = 1;
    float relevantMovies[1000][4]; 
    int i, j, count;
    float numerator, denominator, denominatorLeft, denominatorRight;
    
    //claring the data in the array that will be used with train data
    for (i=0; i<1000; i++)
    {
        relevantMovies[i][0] = 2000;    //this value is out of bounds
        relevantMovies[i][1] = 0;
        relevantMovies[i][2] = 0;
        relevantMovies[i][3] = 0;
    }
    
	//the first thing we need to do is find movies that are rated similarly
	//this is traditionally referred to as the step where we find co-rated pairs

	//so we look through the train data	
    for (i=0; i<1000; i++)
    {
    	//start at the beginning
        count = 0;
        //and iterate through all the users
        for (j=0; j<200; j++)
        {
        	//cheking bounds
            if ((ratings[j][i] != 0 && ratings[j][i] != 9) && (ratings[j][i] != 0 && ratings[j][i] != 9) && (ratings[j][movie] != 0 && ratings[j][movie] != 9) && (ratings[j][movie] != 0 && ratings[j][movie] != 9))
                count++;
        }
        if (count >= minimumPairs)
        {
            count = 0;
            for (j=0; j<200; j++)
            {
            	//again, cheecking bounds
                if ((ratings[j][i] != 0 && ratings[j][i] != 9) && (ratings[j][i] != 0 && ratings[j][i] != 9) && (ratings[j][movie] != 0 && ratings[j][movie] != 9) && (ratings[j][movie] != 0 && ratings[j][movie] != 9))
                {
                    relevantMovies[count][0] = i; //storing the placement for the array
                    relevantMovies[count][1] = j;
                    count++;
                }
            }
        }
    }
    
    //now we want to find the average for all the users that we have collected
    //this is really similar to the way we found the average in all the other algorithms
    for (i=0; i<1000; i++)
    {
        if (relevantMovies[i][0] != 2000)//if it has been changed from the original cleared data
        {
            count = 0;
            for (j=0; j<1000; j++)
            {
            	//checking bounds
                if ((ratings[(int)relevantMovies[i][1]][j] != 0) && (ratings[(int)relevantMovies[i][1]][j] != 9))
                {
                    relevantMovies[i][2] += ratings[(int)relevantMovies[i][1]][j]; //summation
                    count++; 
                }
            }
            relevantMovies[i][2] = relevantMovies[i][2] / count; //take the average
        }
    }
    
    //now what we want to do is calculate the average weights for all the movies that we have collected
    //first we want to clear all the values
    i = 0;
    numerator = 0;
    denominator = 0;
    denominatorLeft = 0;
    denominatorRight = 0;

    //then we want to sum everything up into the numerator and the two parts of the denominator
    //splitting it up into three parts like this actually causes some extra rounding errors but 
    //it makes the calculations feasible so that they dont take up a ton more lines
    while ((i < 1000) && (relevantMovies[i][0] != 2000))
    {
        numerator += (ratings[(int)relevantMovies[i][1]][movie] - relevantMovies[i][2]) * (ratings[(int)relevantMovies[i][1]][(int)relevantMovies[i][0]] - relevantMovies[i][2]);
        denominatorLeft += (ratings[(int)relevantMovies[i][1]][movie] - relevantMovies[i][2]) * (ratings[(int)relevantMovies[i][1]][movie] - relevantMovies[i][2]);
        denominatorRight += (ratings[(int)relevantMovies[i][1]][(int)relevantMovies[i][0]] - relevantMovies[i][2]) * (ratings[(int)relevantMovies[i][1]][(int)relevantMovies[i][0]] - relevantMovies[i][2]);
        j = i + 1;

        while ((j < 1000) && (relevantMovies[j][0] == relevantMovies[i][0]))
        {
            numerator += (ratings[(int)relevantMovies[j][1]][movie] - relevantMovies[j][2]) * (ratings[(int)relevantMovies[j][1]][(int)relevantMovies[j][0]] - relevantMovies[j][2]);
            denominatorLeft += (ratings[(int)relevantMovies[j][1]][movie] - relevantMovies[j][2]) * (ratings[(int)relevantMovies[j][1]][movie] - relevantMovies[j][2]);
            denominatorRight += (ratings[(int)relevantMovies[j][1]][(int)relevantMovies[j][0]] - relevantMovies[j][2]) * (ratings[(int)relevantMovies[j][1]][(int)relevantMovies[j][0]] - relevantMovies[j][2]);
            j++;
        }

        denominator = sqrt (denominatorLeft) * sqrt (denominatorRight);
        relevantMovies[i][3] = numerator / denominator;
        i = j;
    }
    
    // then we actually want to the actual calculation
    //so agai we need to clear the values
    numerator = 0;
    denominator = 0;
    i = 0;

    //then we take another summation
    while ((i < 1000) && (relevantMovies[i][0] != 2000))
    {
        numerator += relevantMovies[i][3] * ratings[user][(int)relevantMovies[i][0]];
        denominator += fabsf (relevantMovies[i][3]);
        j = i + 1;

        while ((j < 1000) && (relevantMovies[j][0] == relevantMovies[i][0]))
            j++;
        i = j;
    }

    //and do the actual calculation for the rating
    rating = fabsf (numerator / denominator) + 0.5;
    
    //generic logic catches for rounding errors
    if (rating > 5)
	{
		rating = 5;
	}
	if (rating < 1)
	{
		rating = 1;
	}
    
	return rating;
}
/*                                                                                                                 
        CCCCCCCCCCCCC                                            tttt                                                   
     CCC::::::::::::C                                         ttt:::t                                                   
   CC:::::::::::::::C                                         t:::::t                                                   
  C:::::CCCCCCCC::::C                                         t:::::t                                                   
 C:::::C       CCCCCCuuuuuu    uuuuuu      ssssssssss   ttttttt:::::ttttttt       ooooooooooo      mmmmmmm    mmmmmmm   
C:::::C              u::::u    u::::u    ss::::::::::s  t:::::::::::::::::t     oo:::::::::::oo  mm:::::::m  m:::::::mm 
C:::::C              u::::u    u::::u  ss:::::::::::::s t:::::::::::::::::t    o:::::::::::::::om::::::::::mm::::::::::m
C:::::C              u::::u    u::::u  s::::::ssss:::::stttttt:::::::tttttt    o:::::ooooo:::::om::::::::::::::::::::::m
C:::::C              u::::u    u::::u   s:::::s  ssssss       t:::::t          o::::o     o::::om:::::mmm::::::mmm:::::m
C:::::C              u::::u    u::::u     s::::::s            t:::::t          o::::o     o::::om::::m   m::::m   m::::m
C:::::C              u::::u    u::::u        s::::::s         t:::::t          o::::o     o::::om::::m   m::::m   m::::m
 C:::::C       CCCCCCu:::::uuuu:::::u  ssssss   s:::::s       t:::::t    tttttto::::o     o::::om::::m   m::::m   m::::m
  C:::::CCCCCCCC::::Cu:::::::::::::::uus:::::ssss::::::s      t::::::tttt:::::to:::::ooooo:::::om::::m   m::::m   m::::m
   CC:::::::::::::::C u:::::::::::::::us::::::::::::::s       tt::::::::::::::to:::::::::::::::om::::m   m::::m   m::::m
     CCC::::::::::::C  uu::::::::uu:::u s:::::::::::ss          tt:::::::::::tt oo:::::::::::oo m::::m   m::::m   m::::m
        CCCCCCCCCCCCC    uuuuuuuu  uuuu  sssssssssss              ttttttttttt     ooooooooooo   mmmmmm   mmmmmm   mmmmmm                                                                                                                        
*/
int custom (int user, int movie)
{
	int rating = 0;
    
    int pearsonRating = pearsonCorrelation (user, movie);
    int pearsonCaseRating = pearsonCase (user, movie);
    int pearsonInverseRating = pearsonIUF (user, movie);
    int vectorRating = cosineSimilarity (user, movie);
    int cosineRating = itemBasedCosine (user, movie);
    
    //just add them all up and then divide by the number of algorithms
    //the + .3 and /4.5 instead of /5 actually were just me tweaking the values to get a better rating
    rating = ((pearsonRating + pearsonCaseRating + pearsonInverseRating + vectorRating + cosineRating) + 0.3) / 4.5;

	return rating;
}

/*                                                                                                           
DDDDDDDDDDDDD                             iiii                                                               
D::::::::::::DDD                         i::::i                                                              
D:::::::::::::::DD                        iiii                                                               
DDD:::::DDDDD:::::D                                                                                          
  D:::::D    D:::::D rrrrr   rrrrrrrrr  iiiiiiivvvvvvv           vvvvvvv eeeeeeeeeeee    rrrrr   rrrrrrrrr   
  D:::::D     D:::::Dr::::rrr:::::::::r i:::::i v:::::v         v:::::vee::::::::::::ee  r::::rrr:::::::::r  
  D:::::D     D:::::Dr:::::::::::::::::r i::::i  v:::::v       v:::::ve::::::eeeee:::::eer:::::::::::::::::r 
  D:::::D     D:::::Drr::::::rrrrr::::::ri::::i   v:::::v     v:::::ve::::::e     e:::::err::::::rrrrr::::::r
  D:::::D     D:::::D r:::::r     r:::::ri::::i    v:::::v   v:::::v e:::::::eeeee::::::e r:::::r     r:::::r
  D:::::D     D:::::D r:::::r     rrrrrrri::::i     v:::::v v:::::v  e:::::::::::::::::e  r:::::r     rrrrrrr
  D:::::D     D:::::D r:::::r            i::::i      v:::::v:::::v   e::::::eeeeeeeeeee   r:::::r            
  D:::::D    D:::::D  r:::::r            i::::i       v:::::::::v    e:::::::e            r:::::r            
DDD:::::DDDDD:::::D   r:::::r           i::::::i       v:::::::v     e::::::::e           r:::::r            
D:::::::::::::::DD    r:::::r           i::::::i        v:::::v       e::::::::eeeeeeee   r:::::r            
D::::::::::::DDD      r:::::r           i::::::i         v:::v         ee:::::::::::::e   r:::::r            
DDDDDDDDDDDDD         rrrrrrr           iiiiiiii          vvv            eeeeeeeeeeeeee   rrrrrrr                                                                                                                        
                                                                                                             
this is the man driver function, It is imperative to every single Algorithm 					
and the program cannot run without this function being flawless.								
This function first asks the user what algorithm they would like to use 						
and then searches throgh the designated text file which has been selected earlier				
and calls the proper algorithm until the file is finished 										
*/

void driver (void)
{
	int rating;
	int i = 0;
	int j = 0;

	int algorithm;
	printf ("\nPlease select which algorithm you would like to use:\n");
	printf ("1: Cosine Similarity\n");
	printf ("2: Pearson Correlation\n");
	printf ("3: Pearson Correlation - Inverse User Frequency\n");
	printf ("4: Pearson Correlation - Case Modification\n");
	printf ("5: Item Based Similarity w/ Cosine\n");
	printf ("6: Custom Algorithm\n");
	scanf ("%d", &algorithm);

	for (i=offset; i<offset+100; i++)
	{
		for (j=0; j<1000; j++)
		{
			if (ratings[i][j] == 0)
			{
				// Switch statement on algos
				switch (algorithm)
				{
					case 1:
						rating = cosineSimilarity (i, j);
						break;
						
					case 2:
						rating = pearsonCorrelation (i, j);
						break;
						
					case 3:
						rating = pearsonIUF (i, j);
						break;
						
					case 4:
						rating = pearsonCase (i, j);
						break;
						
					case 5:
						rating = itemBasedCosine (i, j);
						break;
						
					case 6:
						rating = custom (i, j);
						break;
					
					default:
						rating = pearsonCorrelation (i, j);
						break;
				}
				ratingResults[i-offset][j] = rating;
			}
		}
	}
}
/*                                                                         
MMMMMMMM               MMMMMMMM                    iiii                   
M:::::::M             M:::::::M                   i::::i                  
M::::::::M           M::::::::M                    iiii                   
M:::::::::M         M:::::::::M                                           
M::::::::::M       M::::::::::M  aaaaaaaaaaaaa   iiiiiiinnnn  nnnnnnnn    
M:::::::::::M     M:::::::::::M  a::::::::::::a  i:::::in:::nn::::::::nn  
M:::::::M::::M   M::::M:::::::M  aaaaaaaaa:::::a  i::::in::::::::::::::nn 
M::::::M M::::M M::::M M::::::M           a::::a  i::::inn:::::::::::::::n
M::::::M  M::::M::::M  M::::::M    aaaaaaa:::::a  i::::i  n:::::nnnn:::::n
M::::::M   M:::::::M   M::::::M  aa::::::::::::a  i::::i  n::::n    n::::n
M::::::M    M:::::M    M::::::M a::::aaaa::::::a  i::::i  n::::n    n::::n
M::::::M     MMMMM     M::::::Ma::::a    a:::::a  i::::i  n::::n    n::::n
M::::::M               M::::::Ma::::a    a:::::a i::::::i n::::n    n::::n
M::::::M               M::::::Ma:::::aaaa::::::a i::::::i n::::n    n::::n
M::::::M               M::::::M a::::::::::aa:::ai::::::i n::::n    n::::n
MMMMMMMM               MMMMMMMM  aaaaaaaaaa  aaaaiiiiiiii nnnnnn    nnnnnn                                                                         
*/
int main (void)
{
	int i, j;
	FILE *infile;

	//clear all the data in any arrays
	for (i=0; i<500; i++)
	{
		for (j=0; j<1000; j++)
			ratings[i][j] = 9;
	}

	for (i=0; i<100; i++)
	{
		for (j=0; j<1000; j++)
			ratingResults[i][j] = 0;
	}

	//with notrpad in windows you have to open the file with rt otherwise it scans on values wrong
	//spent a lot of time trying to figure that one out >.<
	infile = fopen ("train.txt", "rt");
	//standard logic catch
	if (infile == NULL)
	{
		fprintf (stderr, "Can't open input file train.txt.\n");
		exit (1);
	}
	else //if it opened properly then we want to scan in the values
	{
		int ratingInput;
		i = 0;
		j = 0;
		while (fscanf (infile, "%d", &ratingInput) != EOF)
		{
			if (j < 1000)
			{
				ratings[i][j] = ratingInput;
				j++;
			}
			if (j >= 1000)
			{
				j = 0;
				i++;
			}
		}
	}
	fclose (infile); //make sure to close files!!

	int testFile;
	char inputFile[20];
	char outputFile[20];
	//user prompt
	printf ("\nWhich file would you like to import for testing?\n");
	//I found that by only doing a single file at once I ran into significantly less issues with crashing etc.
	printf ("1 - test5.txt\t");
	printf ("2 - test10.txt\t");
	printf ("3 - test20.txt\n");
	scanf ("%d", &testFile);

	// Calculate the offst based on file length differences and designate the input and output files
	if (testFile == 1)
	{
		offset = 200;
		numRated = 5;
		strcpy (inputFile, "test5.txt");
		strcpy (outputFile, "result5.txt");
	}
	if (testFile == 2)
	{
		offset = 300;
		numRated = 10;
		strcpy (inputFile, "test10.txt");
		strcpy (outputFile, "result10.txt");
	}
	if (testFile == 3)
	{
		offset = 400;
		numRated = 20;
		strcpy (inputFile, "test20.txt");
		strcpy (outputFile, "result20.txt");
	}

	// Input file to operate on
	infile = fopen (inputFile, "rt"); //again with the weird "rt" that windwos requires
	//generic logic catch
	if (infile == NULL)
	{
		fprintf (stderr, "Can't open input file for testing.\n");
		exit (1);
	}
	else
	{
		int ratingInput; //scan in the values if the file opened properly
		i = 0;
		j = 0;
		while (fscanf (infile, "%d%d%d", &i, &j, &ratingInput) != EOF)
		{
			ratings[i-1][j-1] = ratingInput;
		}
	}
	fclose (infile); //close files!

	// Call the driver function to handle algorithm selection
	driver ();

	// Output results
	int user, movie, rating;
	FILE *outfile;
	outfile = fopen (outputFile, "wt"); //again because of windows and notepad i have to use this weird "wt"
	if (outfile == NULL)
	{
		fprintf (stderr, "Can't open output file for writing.\n");
		exit (1);
	}
	else //fairly generic iterative looping printing
	{
		for (i=0; i<100; i++)
		{
			for (j=0; j<1000; j++)
			{
				if (ratingResults[i][j] != 0)
				{
					user = i + offset + 1;
					movie = j + 1;
					rating = ratingResults[i][j];
					fprintf (outfile, "%d %d %d\n", user, movie, rating);
				}
			}
		}
	}
	fclose (outfile);
	printf ("Results successfully outputted.\n");
}
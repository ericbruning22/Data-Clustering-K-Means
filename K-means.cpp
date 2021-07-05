#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <time.h>
#include <math.h>

using namespace std;

int main()
{
	//initialize variables
	string f = "ionosphere.txt";
	int i = 100;
	int r = 100;
	double t = .001;
	double sse = 0;
	double threshold = 0;
	double temp_sse = 0;
	double min_sse = pow(10,10);
	int best_run = 0;
	ofstream output;
	clock_t time;
	double tp = 0;
	double tn = 0;
	double fp = 0;
	double fn = 0;
	double n_total = 0;
	double rand_value = 0;
	double best_rand = 0;
	double best_jacc = 0;
	int best_run_jacc = 0;
	double best_fm = 0;
	int best_run_fm = 0;
	double jaccard = 0;
	double fm = 0;
	double prec = 0;
	double recall = 0;

	/*
	//get file name
	cout << "Enter file name: ";
	getline(cin, f);

	//get other values
	cout << "Enter number of clusters (greater than 1): ";
	cin >> k;

	cout << "Enter max number of iterations (positive integer): ";
	cin >> i;

	cout << "Enter convergence threshold (non-negative real number): ";
	cin >> t;

	cout << "Enter number of runs (positive integer): ";
	cin >> r;
	*/
	

	//put dataset into array
	ifstream file;
	file.open(f);
	cout << "Obtaining dataset from file...\n";

	int n = 0;
	int k = 0;
	int count;
	int d = 0;
	vector<vector<double> > numbers;
	vector<int> true_centers;
	int m = 0;
	int tc = 0;

	if (file.is_open())
	{
			//resize array for dataset
			file >> count;
			numbers.resize(count);
			file >> d;
			d = d - 1;
			file >> k;

			for (int a = 0; a < count; ++a)
				numbers[a].resize(d);

			for (int j = 0; j < count; j++)
			{
				for (int m = 0; m < d; m++)
				{
					file >> numbers[j][m];
				}

				file >> tc;
				true_centers.push_back(tc);
			}
		file.close();
		cout << "Data successfully read\n";
	}

	cout << "Algorithm running...\n";
	//open output file
	output.open("output_" + f + ".csv");
	output << "regular k-means test " << f << " " << k << " " << i << " " << t << " " << r << endl << endl;
	output << "External Validation" << endl;
	int runs = 1;
	time = clock();
	//run algorithm r times
	while (runs <= r)
	{
		//initialize variables for algorithm
		int random_no;
		double temp_distance = 0;
		vector<vector<int> > clusters;
		clusters.resize(k);
		int c = 0;
		double distance = 0;
		vector<vector<double> > cluster_means;
		cluster_means.resize(k);
		double min_dist = pow(10, 6);
		vector<int> part_centers;

		//resize clusters vector to fit data
		for (int i = 0; i < k; ++i)
			cluster_means[i].resize(d);

		//randomly select clusters and create cluster arrays
		for (int x = 0; x < k; x++)
		{
			random_no = rand() % count;
			clusters[x].push_back(random_no);
			for (int q = 0; q < d; q++)
			{
				cluster_means[x][q] = numbers[(random_no)][q];
			}
		}
		int iterations = 1;

		while (iterations <= i)
		{
			//go through each point
			for (int a = 0; a < count; a++)
			{
				//find which cluster the point is closest to
				for (int b = 0; b < k; b++)
				{
					distance = 0;
					//calculate the distance between each cluster and the point
					for (int s = 0; s < d; s++)
					{
						distance += (numbers[a][s] - cluster_means[b][s]) * (numbers[a][s] - cluster_means[b][s]);
					}

					//square root total distance to get euclidian distance value
					temp_distance = sqrt(distance);

					/* this statement will assign the
					first distance as the minimum distance
					but after that, the minimum distance will be assigned */
					if (temp_distance <= min_dist)
					{
						min_dist = temp_distance;
						c = b;
					}
				}

				//place point into cluster
				sse += min_dist;
				clusters[c].push_back(a);
				part_centers.push_back(c);
				min_dist = pow(10, 6);
			}

			//recompute centroids
			for (int x = 0; x < k; x++)
			{
				double clus_total = 0;
				int z = 0;
				for (int y = 0; y < d; y++)
				{
					clus_total = 0;
					for (z = 0; z < clusters[x].size(); z++)
					{
						clus_total += numbers[clusters[x][z]][y];
					}
					cluster_means[x][y] = clus_total / ((clusters[x].size()));
				}
			}

			threshold = abs(sse - temp_sse);
			temp_sse = sse;
			sse = 0;

			//check if threshold is met break out of algorithm
			if (threshold < t)
			{
				/*if (iterations <= min_it)
				{
					if (temp_sse <= min_sse)
					{
						min_sse = temp_sse;
						min_it = iterations;
						best_run = runs;
					}
				}*/

				break;
			}
			iterations = iterations + 1;
			threshold = 0;
			clusters.clear();
			part_centers.clear();
			clusters.resize(k);
		}

		//external validity
		//calculate TP,TN,FP,FN
		//pair each point to all other points
		int p = 1;
		for (int a = 0; a < count; a++)
		{
			for (int b = p; b < count - 1; b++)
			{
				//if pair is a true positive
				if ((true_centers[a] == true_centers[b]) && (part_centers[a] == part_centers[b]))
				{
					tp++;
					n_total++;
				}
				//if pair is a true negative
				else if ((true_centers[a] != true_centers[b]) && (part_centers[a] != part_centers[b]))
				{
					tn++;
					n_total++;
				}
				//if pair is a false positive
				else if ((true_centers[a] == true_centers[b]) && (part_centers[a] != part_centers[b]))
				{
					fp++;
					n_total++;
				}
				//if pair is a false negative
				else if ((true_centers[a] != true_centers[b]) && (part_centers[a] != part_centers[b]))
				{
					fn++;
					n_total++;
				}
			}
			//add 1 to p so there is no duplicate pairs
			p++;
		}

		//calculate rand
		rand_value = (tp + tn)/n_total;

		//calculate jaccard
		jaccard = tp /(tp + fn + fp);

		//calculate fowlkes-mallows
		prec = tp / (tp + fp);
		recall = tp / (tp + fn);
		fm = sqrt(prec * recall);

		//check to see if its best value and run
		if (rand_value > best_rand)
		{
			best_rand = rand_value;
			best_run = runs;
		}

		if (jaccard > best_jacc)
		{
			best_jacc = jaccard;
			best_run_jacc = runs;
		}

		if (fm > best_fm)
		{
			best_fm = fm;
			best_run_fm = runs;
		}

		runs = runs + 1;
		part_centers.clear();
		clusters.clear();
		tp = 0;
		tn = 0;
		fp = 0;
		fn = 0;
		n_total = 0;
		rand_value = 0;
		jaccard = 0;
		fm = 0;
	}
	time = clock() - time;
	output << "Best Run: " << best_run << endl;
	output << "Best Rand Index Value: " << best_rand << endl << endl;
	output << "Best Jaccard Index Value: " << best_jacc << endl;
	output << "Best Jaccard Run: " << best_run_jacc << endl << endl;
	output << "Best Fowlkes-Mallows Index Value: " << best_fm << endl;
	output << "Best Fowlkes-Mallows Run: " << best_run_fm << endl << endl;
	output << "CPU Time: " << time << " milliseconds\n";
	output.close();
	cout << "Algorithm successfully completed\n";
	cout << "Output file created successfully\n";
	cout << "Program complete\n";
}
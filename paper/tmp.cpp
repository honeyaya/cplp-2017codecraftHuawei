#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>
#include <vector>
#include <string>
#include <ctime> 
#include <cstring>
#include <algorithm>
#include <map>
#include <set> 
#include <stack>
using namespace std;

#define Del(a,b) memset(a,b,sizeof(a))
#define Max 1000
#define eps 0.01

const int inf = 0x3f3f3f3f;
const int N = 1500;

struct Node{
	int from,to,cap,flow,cost;
};

vector<Node> e;
vector<Node> ec;
vector<int> v[N];
vector<int> vc[N];

vector<int> directNode;
vector<int> demand;

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	
	int netNum, edgeNum,consumerNum,serverDeployCost;
	clock_t start = clock();		
		
		sscanf(topo[0],"%d %d %d",&netNum,&edgeNum,&consumerNum);
		
		sscanf(topo[2],"%d",&serverDeployCost);
		
		int vNum = netNum + consumerNum;
		
		for(int i = 0; i< N; i++){
			v[i].clear();
		}	
		e.clear();
		ec.clear();
		
		for(int i = 4; i < 4 + edgeNum; i++) {
			int from,to,cap,cost;
			sscanf(topo[i],"%d %d %d %d",&from,&to,&cap,&cost);
			e.push_back((Node){from,to,cap,0,cost});
			e.push_back((Node){to,from,0,0,-cost});
			
			int len = e.size() - 1;
			v[to].push_back(len);
			v[from].push_back(len-1);
			
			e.push_back((Node){to,from,cap,0,cost});
			e.push_back((Node){from,to,0,0,-cost});
			
			len = e.size() - 1;
			v[from].push_back(len);
			v[to].push_back(len-1);	
		}
		
		//add the topo about the consumer
		
		int totalneed = 0;
		for(int i = 5 + edgeNum; i < 5 + edgeNum + consumerNum; i++) {
			int from,to,cap;
			sscanf(topo[i],"%d %d %d",&from,&to,&cap);
			demand.push_back(cap);
			directNode.push_back(to);
			
			from = from + netNum;
			totalneed += cap;
			
			e.push_back((Node){to,from,cap,0,0});
			e.push_back((Node){from,to,0,0,0});
			
			int len = e.size() - 1;
			v[from].push_back(len);
			v[to].push_back(len-1);
			
		}
		
}





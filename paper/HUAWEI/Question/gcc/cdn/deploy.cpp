#include "deploy.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "vector"
#include "iostream"
#include "algorithm"
#include "string"
#include "sstream"
const int maxint=~0U>>1;
const int inf = 0x3f3f3f3f;
const int N = 500000;

#define Del(a,b) memset(a,b,sizeof(a))

//You need to complete the function
struct edge{
	int pFrom,pEnd;
	int edgeTotalFlow;
	int uniteCost;
};

//extern std::vector<edge> E;
//extern std::vector<int> G[N];

/*
void swap(int &a,int &b){
	int temp = a;
	a = b;
	b = temp;
}*/


/*
int vis[N],dis[N];
int p[N],a[N];

void Clear(int x){
	for(int i = 0; i <= x; i++){
		G[i].clear();
	}
	E.clear();
}
bool BellmanFord(int s,int t, int& flow, int& cost){
	Del(dis,inf);
	Del(vis,0);
	dis[s]=0;
	vis[s]=1;
	p[s]=0;
	a[s]=inf;
	queue<int> q;
	q.push(s);
	while(!q.empty()){
		int u = q.front();
		q.pop();
		vis[u]=0;
		for(int i = 0; i < G[u].size(); i++){
			edge& g = E[G[u][i]];
			if(dis[g.pEnd] > dis[u] + g.uniteCost) {
				dis[g.pEnd] = dis[u] + g.uniteCost;
				p[g.pEnd] = G[u][i];
				a[g.pEnd] = min(a[u],g.cap-g.flow);
				if(!vis[g.pEnd]){
					q.push(g.pEnd);
					vis[g.pEnd]=1;
				}
			}
		}
	}

	if(dis[t] == inf){
		return false;
	}		
	flow += a[t];
	cost += dis[t]*a[t];
	int u = t;
	while(u!=s){
		E[p[u]].flow += a[t];
		E[p[u]^1].flow -= a[t];
		u = E[p[u]].pFrom;
	}
	return true;
} 
*/

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{

	std::vector<edge> E;
	
	//read data in topo
	int netNum, edgeNum,consumerNum;
	const char * split=" ";
	char * p;
	p = strtok(topo[0],split);
	std::vector<int> tempread;
	while(p!=NULL) {
		if(p!=NULL) {
			tempread.push_back((int)atoi(p));
			printf("BUG=%d\n",(int)atoi(p));
		}
		p = strtok(NULL,split);
	}
	netNum = tempread[0];
	edgeNum = tempread[1];
	consumerNum = tempread[2];

	int serverDeployCost = atoi(topo[2]);
	
	//printf("netNum= %d  edgeNum = %d   consumerNum=%d serverDeployCost=%d \n",netNum, edgeNum,consumerNum,serverDeployCost);
	int vertexNum = netNum + consumerNum;
	
	std::vector<int> G[vertexNum];
	//std::vector<vector<int>>G;
	int edgecnt = 0;
	for(int i = 4; i < 4 + edgeNum; i++) {
		p = strtok(topo[i],split);
		tempread.clear();
		while(p!=NULL){
			if(p!=NULL) {
				tempread.push_back(atoi(p));
			}
			p = strtok(NULL,split);	
		}
		edge edgetemp;
		edgetemp.pFrom = tempread[0];
		edgetemp.pEnd = tempread[1];
		edgetemp.edgeTotalFlow = tempread[2];
		edgetemp.uniteCost = tempread[3];
		E.push_back(edgetemp);
		edgecnt++;
		std::swap(edgetemp.pFrom, edgetemp.pEnd);
		E.push_back(edgetemp);
		edgecnt++;
	}
	
	
	/*for(int i = 5 + edgeNum; i < 5 + edgeNum + consumerNum; i++) {
		p = strtok(topo[i],split);
		tempread.clear();
		while(p!=NULL) {
			if(p!=NULL) {
				tempread.push_back(atoi(p));
			}
			p = strtok(NULL,split);
		}
		edge edgetemp;
		edgetemp.pFrom = tempread[0] + netNum;
		edgetemp.pEnd = tempread[1];
		edgetemp.edgeTotalFlow = tempread[2];
		edgetemp.uniteCost = 0;
		E.push_back(edgetemp);
		edgecnt++;
		std::swap(edgetemp.pFrom, edgetemp.pEnd);
		E.push_back(edgetemp);
		edgecnt++;
	}*/
	
	//vector<bool> mark(edgecnt,0);
	
	
	/*for(int i = 0; i < E.size(); i++) {
		printf("%d %d %d %d\n",E[i].pFrom,E[i].pEnd,E[i].edgeTotalFlow, E[i].uniteCost);
	}*/
	
	//1.compute the mininum cost 
	/*
	int cost[vertexNum][vertexNum];
	int flow[vertexNum][vertexNum];

	for(int i = 0; i < netNum; i++){
		for(int j = 0;j < consumerNum; j++){
			int maxFlow = 0, minCost = 0;
			while(BellmanFord(i,j+netNum,maxFlow,minCost));
			cost[i][j+netNum] = minCost;
			flow[i][j+netNum] = maxFlow;
		}
	}
          */
 	
	//2.branch and bound algorithm
	
	//3.write the *topo_file;
  
	// Output demo
	std::string ans="";
	//ans = std::to_string(consumerNum);
	std::ostringstream os;
	os<<consumerNum;
	ans += os.str();
	ans += "\n";
	ans += "\n";
	for(int i = 5 + edgeNum; i < 5 + edgeNum + consumerNum; i++) {
		p = strtok(topo[i],split);
		std::vector<std::string> v;
		v.clear();
		while(p!=NULL){
			if(p!=NULL){
				v.push_back(p);
			}
			p = strtok(NULL,split);
		}
		std::string s;
		s =s + v[1] + " ";
		s =s + v[0] + " ";
		s =s + v[2] + "\n";
		ans += s;
         }
	 const char * topo_file = ans.c_str();

	//char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	write_result(topo_file, filename);

}

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

struct bigpath{
	int bigpathcost,bigpathflow,bigwhoflow; 
	vector<int> bigroad;
};

struct finalnode{
	int source;
	double ration;
	int ori;
};

struct bigedge{
	int cost,flow;
};

// the need datastructure
vector<Node> e;
vector<Node> ec;
vector<int> v[N];
vector<int> vc[N];

//vector<vector<int>> v; 
vector<int> vis(N,0);
vector<int> dis(N,0);
vector<int> p(N,-1);
vector<int> a(N,-1);
vector<int> path;
vector<int> pathflow;
vector<int> directNode;
vector<int> demand;

int minflow = 0,first = -1,mincost=inf;

int D[Max];// 存储到各个顶点的最短距离
int F[Max];// 存储到各个顶点的最短距离时候的流量 
int mark[Max]; //标记已确定最短路径的点
int gridpath[Max]; 
bigedge grid[Max][Max];
bigpath bigpre[Max][Max]; 

void init(){
	for(int i = 0; i < N; i++){
		p[i] = -1;
		a[i] = -1;
	}
}

	
bool BellmanFord(int s,int t,int& flow, int & cost){
	
	for(int i = 0; i < (int)dis.size(); i++)
    	dis[i] = inf;
    for(int i = 0; i < (int)vis.size(); i++)
    	vis[i] = 0;
    	
    dis[s] = 0;
    vis[s] = 1;
    p[s] = 0;
    a[s] = inf;
    queue<int> q;
    
    q.push(s);
    while(!q.empty())
    {
        int u = q.front();
        q.pop();
        vis[u] = 0;
        for(int i=0; i< (int)v[u].size(); i++)
        {
            Node& g = e[v[u][i]];
            if(g.cap>g.flow && dis[g.to] > dis[u]+g.cost)
            {
                dis[g.to] = dis[u] + g.cost;
                p[g.to] = v[u][i];  //保存前驱
                a[g.to] = min(a[u],g.cap-g.flow);
                if(!vis[g.to])
                {
                    q.push(g.to);
                    vis[g.to]=1;
                }

            }
        }
    }
    if(dis[t] == inf)
        return false;
    flow += a[t];
    cost += dis[t]*(a[t]);
    int u = t;
    while(u!=s)
    {
        e[p[u]].flow += a[t];
        e[p[u]^1].flow -= a[t];
        u = e[p[u]].from;
    }
    return true;
}

bool BellmanFord2(int s,int t,int& flow, int & cost){
	
	for(int i = 0; i < (int)dis.size(); i++)
    	dis[i] = inf;
    for(int i = 0; i < (int)vis.size(); i++)
    	vis[i] = 0;
	
    dis[s] = 0;
    vis[s] = 1;
    p[s] = 0;
    a[s] = inf;
    queue<int> q;
    
    q.push(s);
    while(!q.empty())
    {
        int u = q.front();
        q.pop();
        vis[u] = 0;
        for(int i=0; i< (int)v[u].size(); i++)
        {
            Node& g = e[v[u][i]];
            if(g.cap>g.flow && dis[g.to] > dis[u]+g.cost)
            {
                dis[g.to] = dis[u] + g.cost;
                p[g.to] = v[u][i];  //保存前驱
                a[g.to] = min(a[u],g.cap-g.flow);
                if(!vis[g.to])
                {
                    q.push(g.to);
                    vis[g.to]=1;
                }

            }
        }
    }
    if(dis[t] == inf)
        return false;
    flow += a[t];
    cost += dis[t]*(a[t]);
    int u = t;
    while(u!=s)
    {
    	path.push_back(u);
    	pathflow.push_back(flow);
        e[p[u]].flow += a[t];
        e[p[u]^1].flow -= a[t];
        u = e[p[u]].from;
    }
    return true;
}


bool cmp(const finalnode& a, const finalnode& b){
	return a.ration < b.ration;
}

void dijkstra(int s,int netNum){
	int pos = -1;
	memset(mark,0,sizeof(mark));
	
	for(int i = 0; i < netNum;i++){
		D[i] = grid[s][i].cost;
		if(grid[s][i].cost!=inf){
			gridpath[i] = s;
		} else {
			gridpath[i] = -1;
		}
	}
	mark[s] = 1;
	
	for(int i = 0; i < netNum; i++){
		int min = inf;
		for(int j = 0; j < netNum; j++){
			if(!mark[j] && D[j]<min){
				min = D[j];
				pos = j; 
			}
		}
		
		mark[pos] = 1;
		
		for(int j = 0; j < netNum; j++){
			if(!mark[j] && grid[pos][j].cost<inf){
				if(D[j]>(D[pos]+grid[pos][j].cost)){
					D[j] = D[pos]+grid[pos][j].cost;
					
					gridpath[j] = pos;
				}
			}
		}
	}
}

void showPath(int v, int v0,vector<int> &spath){
	stack<int> s;
	//int u = v;
	while(v!=v0){
		s.push(v);
		v = gridpath[v];
	}
	s.push(v);
	while(!s.empty()){
		spath.push_back(s.top());
		s.pop();
	}
}

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
	
		// add the topo about netdot
		//int edgecnt = 0;
		for(int i = 0; i < Max;i++){
			for(int j = 0; j < Max;j++){
				if(i == j) {
					grid[i][j].cost = 0;
					grid[i][j].flow = inf;
				}
				else {
					grid[i][j].cost = inf;
					grid[i][j].flow = inf;
				}
			}
		}
		
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
			
			grid[from][to].cost = cost;
			grid[from][to].flow = cap;
			
			grid[to][from].cost = cost;
			grid[to][from].flow = cap;
				
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
			
			//grid[to][to].flow = cap;
		}
		
		
		//check the topo 
		//printf("netNum=%d edgeNum=%d consumerNum=%d serverDeployCost=%d vNum=%d totalneed=%d\n",netNum, edgeNum,consumerNum,serverDeployCost,vNum,totalneed);
		//printf("e size is %d\n",(int)e.size());
		/*
		for(int i = 0; i < (int)e.size(); i++) {
			printf("%d %d %d %d\n", e[i].from, e[i].to ,e[i].cap, e[i].cost);
		}
		*/
		
		// get potential server set	
		
		// caculate the min_cost
		
		// multi-source and multi-des
		
		int delta = 0, maxdelta = -100000000;
		vector<int> changepos;
		vector<int> J0;
		vector<int> J1;
		
		for(int i = 0; i < netNum; i++){
				J1.push_back(i);
			}
			
		if(vNum < 600){
			for(int i = 0; i < netNum; i++){
				J1.push_back(i);
			}
		} 
		else{
			for(int i = 0; i < consumerNum;i++){
				J1.push_back(directNode[i]);
			}
		}
		/*
		else {
			// -----------------------------------------------------------------------big graph 
			
			vector<finalnode> re;
			string bigans;
			map<int,int> mp;
			map<int,int>:: iterator it;
			vector<int> sc;
			
			for(int i = 0; i < netNum; i++) {
				dijkstra(i,netNum);	
				for(int j = 0; j < netNum; j++){
					if(j == i){
						bigpre[i][j].bigpathcost = 0;
						bigpre[i][j].bigpathflow = grid[i][j].flow;
						bigpre[i][j].bigwhoflow = i;
						bigpre[i][j].bigroad.push_back(i);
					} 
					else{
						
						bigpre[i][j].bigpathcost = D[j];
						bigpre[i][j].bigwhoflow = i;
						vector<int> tmpbigroad;
						tmpbigroad.clear();
						showPath(j,i,tmpbigroad);
						int bigroadflow = inf;
						
						for(int k = 0; k < (int)tmpbigroad.size();k++){
							bigpre[i][j].bigroad.push_back(tmpbigroad[k]);
							
							if(k != ((int)tmpbigroad.size()-1)){
								int be = tmpbigroad[k],en = tmpbigroad[k+1];
								int xxflow = grid[be][en].flow;
								if(xxflow < bigroadflow) {
									bigroadflow = xxflow;
								} 
							}
						}
						bigpre[i][j].bigpathflow = bigroadflow;	
							
							
					}
					
				}			
			} 
			
			for(int i = 0; i < (int)directNode.size(); i++){
				re.clear();
				int bigfrom1 = directNode[i];
				int bigsource = -1;
				for(int j = 0; j < (int)directNode.size(); j++){
					int bigfrom2 = directNode[j];
					
					if(bigpre[bigfrom2][bigfrom1].bigpathcost != inf){ //two nodes connect edges
						
						if(bigfrom1 == bigfrom2){
							re.push_back((finalnode){bigfrom1,serverDeployCost*1.0/demand[i],i}); 
						} else{
								double bigroadmincost = inf;
								int bigpos = -1; 
								
								for(int k = 0; k < (int)bigpre[bigfrom2][bigfrom1].bigroad.size(); k++){
							
									bigsource = bigpre[bigfrom2][bigfrom1].bigroad[k];
									
									if((bigpre[bigsource][bigfrom1].bigpathcost == inf) || (bigpre[bigsource][bigfrom2].bigpathcost == inf))
										continue; // if the source not connect to both nodes
									
									
									int bf1 = bigpre[bigsource][bigfrom1].bigpathflow;
									if(bf1 > demand[i]) bf1 = demand[i];
									
									int bf2 = bigpre[bigsource][bigfrom2].bigpathflow;
									if(bf2 > demand[j])	bf2 = demand[j];
									
									  
									int bigtmpcost = bigpre[bigsource][bigfrom1].bigpathcost * bf1 + bigpre[bigsource][bigfrom2].bigpathcost * bf2;
									double ration = (bigtmpcost + serverDeployCost) * 1.0/(bf1 + bf2);
									
									if((bigroadmincost - ration) > eps){
											bigroadmincost = ration;
											bigpos = bigsource; 
									}
									
								}
								
								if(bigpos!=-1){
									re.push_back((finalnode){bigpos,bigroadmincost,j});
								}
						}
						
					//	cout << "bigroadmincost" << bigroadmincost<< endl;
					}
					
				}
				
				if(re.size() == 0) { // no road
					string direcres; 
					direcres += to_string(directNode[i]) + " " + to_string(i)+ " " + to_string(demand[i]);
					if(i != ((int)directNode.size() - 1)){
						direcres += "\n";
					} 
					bigans += direcres;
				} 
				
				
				else{
					for(int j = 0; j < (int)re.size();j++){
						bigpre[directNode[re[j].ori]][directNode[i]].bigroad.size()
					}
					
					if(i == 0){
						int cnt = 0;
						sort(re.begin(),re.end(),cmp);
						for(int j = 0; j < (int)re.size();j++){
							cout << "server is :" << re[j].source << endl;
							cout << "ratio is:" << re[j].ration << endl;
							//cout << "ori server is :" << directNode[re[j].ori] << endl;
							for(int k = 0; k < bigpre[directNode[re[j].ori]][directNode[i]].bigroad.size();k++){
								cout << bigpre[directNode[re[j].ori]][directNode[i]].bigroad[k] << " " ;
								cnt++;
							}
							cout << endl;
							
							cout << "left-flow is : " << bigpre[re[j].source][directNode[i]].bigpathflow << endl;
							cout << "left-totalunitcost is : " << bigpre[re[j].source][directNode[i]].bigpathcost << endl;
							
							//cout << "right-flow is : " << bigpre[re[j].source][directNode[re[j].ori]].bigpathflow << endl;
							//cout << "right-totalunitcost is : " << bigpre[re[j].source][directNode[re[j].ori]].bigpathcost << endl; 
							
							//cout << ;
						}	
					}
					cout << "cnt=" << cnt << endl;
				} 
				
				
				/*
				else {
					string undirecres;
					// server to be selected
					sort(re.begin(),re.end(),cmp);
					
					if(re[0].source == directNode[i]){    // 性价比最高的是服务器本身 
						string direcres; 
						direcres += to_string(directNode[i]) + " " + to_string(i)+ " " + to_string(demand[i]);
						if(i != ((int)directNode.size() - 1)){
							direcres += "\n";
						} 
						bigans += direcres;
						cout << "directres " << direcres << endl; 
					}
					else{ 
						
						int nodeflow = 0;
						
						for(int j = 0; j < (int)re.size();i++){
							if(bigpre[re[j].source][directNode[i]].bigroad.size() != 1)
								nodeflow += bigpre[re[j].source][directNode[i]].bigpathflow;
						}
						cout << "nodeflow= " << nodeflow << endl;
						if(nodeflow < demand[i]){    //找到的边总和不满足 则服务器本身 
							string direcres; 
							direcres += to_string(directNode[i]) + " " + to_string(i)+ " " + to_string(demand[i]);
							if(i != ((int)directNode.size() - 1)){
								direcres += "\n";
							} 
							bigans += direcres;
						} 
						else{                 //进行分配   
								int onereduce = 0;
								int anoreduce = 0;
								for(int j = 0; j < (int)re.size(); j++){
									if(demand[i] == 0) break; 
									if(demand[directNode[re[j].ori]]==0) continue;
									 
									int onesupply = bigpre[re[j].source][directNode[i]].bigpathflow;  // one node
									if(demand[i] > onesupply){
										onereduce = onesupply;
										demand[i] -= onesupply;
									} else {
										onereduce = demand[i];
										demand[i] = 0;
									}
									
									int ano = re[j].ori;                                            // another node
									int anosupply = bigpre[re[j].source][directNode[ano]].bigpathflow;
									if(demand[ano] > anosupply){
										anoreduce = anosupply; 
										demand[ano] -= anosupply;
									} else {
										anoreduce = demand[ano];
										demand[ano] = 0;
									}
									 
									//reduce and modify and judge server
									sc.push_back(re[j].source);
									
									undirecres = "";
									int len = (int)bigpre[re[j].source][directNode[i]].bigroad.size();
									for(int k = 0; k < len;k++){
										if(k!=(len-1)){
											int s =  bigpre[re[j].source][directNode[i]].bigroad[k], t = bigpre[re[j].source][directNode[i]].bigroad[k+1];
											grid[s][t].flow -= onereduce;
											//bigpre[s][t].bigpathflow -= onereduce;
										}
										undirecres += to_string(bigpre[re[j].source][directNode[i]].bigroad[k]) + " ";
									}
									undirecres += to_string(i) + " " + to_string(onereduce) + "\n";
									
									bigans += undirecres;
									
									undirecres = "";
									len = (int) bigpre[re[j].source][directNode[ano]].bigroad.size();
									for(int k = 0; k < len; k++){
										if(k!=(len-1)) {
											int s = bigpre[re[j].source][directNode[ano]].bigroad[k];
											int t = bigpre[re[j].source][directNode[ano]].bigroad[k+1];
											grid[s][t].flow -= anoreduce;
											//bigpre[s][t].bigpathflow -=  anoreduce;
										}
										undirecres+= to_string(bigpre[re[j].source][directNode[ano]].bigroad[k]) + " ";
									}
									undirecres += to_string(ano) + " " + to_string(anoreduce) + "\n";
									bigans += undirecres;
								
								}		
						} 	
					}
						
				}
				*/
			/*	
			}
			
			cout << "Server: "  << endl;
			for(int i = 0; i < (int)sc.size();i++){
				cout << sc[i] << " ";
			}
			cout << endl;
			
			char * topo_filebig = (char *)bigans.c_str();
	    	write_result(topo_filebig, filename); 
			*/
			
 			// -----------------------------------------------------------------------big graph
		
		/* 
			return ;
		}
		
		*/
		
		int  supert = vNum+1;
		for(int i = 0; i < consumerNum; i++){
			e.push_back((Node){i+netNum,supert,demand[i],0,0});
			e.push_back((Node){supert,i+netNum,0,0,0});
			//cout << "supert: " << supert << " to:" << i + netNum << endl;	
			int len = e.size() - 1;
			v[supert].push_back(len);
			v[i+netNum].push_back(len-1);
		}
		
		int supers = vNum;
		
		for(int i = 0; i < (int)e.size(); i++){
			ec.push_back(e[i]);
		}
		for(int i = 0; i < N; i++){
			for(int j = 0; j < (int)v[i].size();j++){
				vc[i].push_back(v[i][j]);
			}
		}
		
		bool deadline = false;
		while(1){
			
			if((clock()-start) > 75*CLOCKS_PER_SEC){
				deadline = true;
				break;
			}
			
			delta = 0, maxdelta = -100000000;
			changepos.clear();
			supers = vNum;
			for(int j = 0; j < (int)J1.size();j++){			
				e.push_back((Node){supers,J1[j],inf,0,0});
				e.push_back((Node){J1[j],supers,0,0,0});
							
				int len = e.size() - 1;
				v[J1[j]].push_back(len);
				v[supers].push_back(len-1);
			}	 
			init();
			int onetotalflow = 0, onetotalcost = 0;
			while(BellmanFord(supers,supert,onetotalflow,onetotalcost));
		//	cout << "onetotalflow=" << onetotalflow << " onetotalcos=" <<onetotalcost << endl;
			
			e.clear();
			for(int k = 0; k < (int)ec.size();k++){
				e.push_back(ec[k]);
			}
			
			for(int i = 0; i < N; i++){
				v[i].clear();
				for(int j = 0; j < (int)vc[i].size();j++){
					v[i].push_back(vc[i][j]);
				}
			}
			
			
			for(int i = 0; i < (int)J1.size(); i++){
				if((clock()-start) > 75*CLOCKS_PER_SEC){
					deadline = true;
					break;
				}
				supers = vNum;
				for(int j = 0; j < (int)J1.size();j++){
					if(J1[j]!=J1[i]){
						e.push_back((Node){supers,J1[j],inf,0,0});
						e.push_back((Node){J1[j],supers,0,0,0});
							
						int len = e.size() - 1;
						v[J1[j]].push_back(len);
						v[supers].push_back(len-1);
					}
				}
				
			
				init();
				int oneflow = 0, onecost = 0;
				while(BellmanFord(supers,supert,oneflow,onecost));
			//	cout << "oneflow=" << oneflow << " onecos=" <<onecost << endl;
				
				if(oneflow == totalneed){
					//delta = serverDeployCost * (J1.size()) + onetotalcost - (serverDeployCost * (J1.size()-1) + onecost);
					delta = serverDeployCost + onetotalcost - onecost;
					
					if(delta > maxdelta) {
						maxdelta = delta;
						changepos.clear();
						changepos.push_back(J1[i]);
					}
					
					if(delta == maxdelta) {
						changepos.push_back(J1[i]);
					}	
				} 
				
				
				e.clear();
				for(int k = 0; k < (int)ec.size(); k++){
					e.push_back(ec[k]);
				} 
				for(int i = 0; i < N; i++){
					v[i].clear();
					for(int j = 0; j < (int)vc[i].size();j++){
						v[i].push_back(vc[i][j]);
					}
				}	 
				
			}
			
			if(maxdelta > 0){
					for(int i = 0; i < (int)changepos.size(); i++){
						J0.push_back(changepos[i]);	
					}
					changepos.clear();
			}
			else {
				break;		
			}	
			J1.clear();
			for(int i = 0; i < netNum; i++){
				vector<int>::iterator result = find(J0.begin(),J0.end(),i);
				if(result == J0.end()){
					J1.push_back(i);
				}	
			}			
		}
		
		if(!deadline){
			
		supers = vNum;
		for(int j = 0; j < (int)J1.size();j++){			
			e.push_back((Node){supers,J1[j],inf,0,0});
			e.push_back((Node){J1[j],supers,0,0,0});
							
			int len = e.size() - 1;
			v[J1[j]].push_back(len);
			v[supers].push_back(len-1);
		}	 
		init();
		int onetotalflow = 0, onetotalcost = 0;
		while(BellmanFord(supers,supert,onetotalflow,onetotalcost));
		
		ec.clear();
		for(int i = 0; i < N;i++)
			vc[i].clear();
			
		for(int i = 0; i < (int)e.size();i++){
			if(e[i].flow > 0) {
				ec.push_back((Node){e[i].from,e[i].to,e[i].flow,0,e[i].cost});
				ec.push_back((Node){e[i].to,e[i].from,e[i].flow,0,e[i].cost});
				
				int len = ec.size() - 1;
				vc[e[i].to].push_back(len);
				vc[e[i].from].push_back(len-1);
			}
		}
		
		init();
		e.clear();
		for(int i = 0; i < (int)ec.size();i++){
			e.push_back(ec[i]);
		}
		
		for(int i = 0; i < N;i++){
			v[i].clear();
			for(int j = 0; j < (int)vc[i].size();j++)
				v[i].push_back(vc[i][j]);
		}
		//cout << "----------------" << endl;
		init();
		onetotalflow = 0, onetotalcost = 0;
		while(BellmanFord2(supers,supert,onetotalflow,onetotalcost));
		//cout << "onetotalflow=" << onetotalflow << " onetotalcost=" << onetotalcost +J1.size() * serverDeployCost << endl;
				
		/*
		cout << "a is :" << endl;
		for(int i = 0; i < N;i++){
			if(a[i]!=-1) {
				cout << a[i] << " ";
			}
		}
		cout << endl; 
		*/
		
		//cout << "onetotalflow=" << onetotalflow << " onetotalcos=" <<onetotalcost+J1.size() * serverDeployCost  << endl;
		
		/*
		cout << "J1:" << endl;
		for(int i = 0; i < J1.size();i++)
			cout << J1[i] << " ";
		cout << endl;
		*/
		
		/*cout << "consumer:" << endl;
		for(int i = 0; i < consumerNum; i++){
			cout << demand[i] << " " ;
		}
		cout << endl;
		*/
		
		vector<string> recordpath;
		string onepath;
		for(int i = (int)path.size()-1; i >= 0; i--){
			if(path[i] == supert){
				if(i>0)
					onepath += to_string(pathflow[i] - pathflow[i-1]);
				else 
					onepath += to_string(pathflow[0]);
				recordpath.push_back(onepath); 
				onepath = "";
			} else {
				if(path[i-1] == supert){
					path[i] -= netNum;
				}
				onepath += to_string(path[i]) + " ";
			}
		}
		
		string final = to_string(recordpath.size());
		final += "\n\n";
		
		for(int i = 0; i < (int)recordpath.size() - 1;i++){
			final += recordpath[i] + "\n";
		} 
		final += recordpath[recordpath.size()-1];
		
		char * topo_file = (char *)final.c_str();
	    write_result(topo_file, filename);
		return ;	
		
		}
	
		char * c;
	    int spaceCount = 0;
	    string res;
	    char a[20];
	    sprintf(a, "%d\n\n",consumerNum);
	    res = a;
	    int netnode, need;
	    for (int i = 1; i < consumerNum+1; i++)
	    {
	        c = topo[line_num-i];
	        netnode = need = spaceCount = 0;
	        while (*c != '\0' && *c != '\n' && *c != '\r') 
	        {
	            if (*c == ' ') 
	            {
	                c++;
	                spaceCount++;
	                continue;
	            }
	            if (spaceCount == 1)
	            {
	                netnode = *c - '0' + netnode * 10;
	            }
	            else if (spaceCount == 2) 
	            {
	                need = *c - '0' + need * 10;
	            }
	            c++;
	        }
	        sprintf(a, "%d %d %d",netnode,consumerNum-i,need);
	        res += a;
	        if (i != consumerNum) 
	        {
	            res += "\n";
	        }
	    }
	    
	    char * topo_file = (char *)res.c_str();
	    write_result(topo_file, filename); 
		return ;
}





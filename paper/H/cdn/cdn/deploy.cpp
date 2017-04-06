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
#include <limits.h>
using namespace std;

const int inf = 0x3f3f3f3f;
const int N = 1500;

struct edge{
	int from,to,cap,cost,flow;
};

vector<edge> e;
vector<edge> ec;
vector<int> v[N];
vector<int> vc[N];

vector<int> demand;
vector<int> directNode;

int mincost;
int supers,supert;
int serverDeployCost;
int netNum,edgeNum,consumerNum,vNum;

//   big structure
struct bigedge{
	int cost,flow;
};

struct bigpath{
	int bigpathcost,bigpathflow,bigfrom,bigto; 
	vector<int> bigroad;
};

int D[N];// 存储到各个顶点的最短距离
int F[N];// 存储到各个顶点的最短距离时候的流量 
int mark[N]; //标记已确定最短路径的点
int gridpath[N]; 
bigedge grid[N][N];
bigpath bigpre[N][N]; 

vector<int> J0;
vector<int> J2;

vector<int> path;
vector<int> pathflow;

vector<int> vis(N,0);
vector<int> dis(N,0);
vector<int> p(N,-1);
vector<int> a(N,-1);

struct mapp{
	int net,con,need;	
};

vector<mapp> nc;

bool cmp(const mapp& a, const mapp& b){
	return a.need < b.need;
}

bool cmp1(const bigpath& a, const bigpath& b){
	if(a.bigpathcost == b.bigpathcost){
		return a.bigpathflow > b.bigpathflow;
	}
	return a.bigpathcost < b.bigpathcost;
}

void addedge(int from,int to,int cap,int cost,int flow){
	e.push_back((edge){from,to,cap,cost,0});
	e.push_back((edge){to,from,0,-cost,0});
			
	int len = e.size() - 1;
	v[to].push_back(len);
	v[from].push_back(len-1);	
}	

void initMCMF(){
	for(int i = 0; i < N; i++){
		p[i] = -1;
		a[i] = -1;
	}
	for(int i = 0; i < (int)dis.size(); i++)
    	dis[i] = inf;
    for(int i = 0; i < (int)vis.size(); i++)
    	vis[i] = 0;
}

bool MCMF(int s,int t,int& flow, int & cost){
	
	initMCMF();
    	
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
            edge& g = e[v[u][i]];
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

bool MCMF2(int s,int t,int& flow, int & cost){
	
	initMCMF();
	
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
            edge& g = e[v[u][i]];
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

void repaint(vector<int> sc){
	
	for(int i = 0; i < (int)sc.size();i++){
		e.push_back((edge){supers,sc[i],inf,0,0});
		e.push_back((edge){sc[i],supers,0,0,0});
		int len = e.size() - 1;
		v[sc[i]].push_back(len);
		v[supers].push_back(len-1);
		
	}
}

void evinit(){
	e.clear();
	for(int i = 0; i < (int)ec.size();i++)
		e.push_back(ec[i]);
	for(int i = 0;i < N;i++){
		v[i].clear();
		for(int j = 0; j < (int)vc[i].size();j++){
			v[i].push_back(vc[i][j]);
		}
	}
	
}

void dijkstra(int s){
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
				
		vector<int> J1;
			
	    clock_t start = clock();		
		
		sscanf(topo[0],"%d %d %d",&netNum,&edgeNum,&consumerNum);
		
		sscanf(topo[2],"%d",&serverDeployCost);
		
		vNum = netNum + consumerNum;
	
		// construct the ori graph
		for(int i = 0; i < N;i++){
			for(int j = 0; j < N;j++){
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
			int from,to,ca,cos;
			sscanf(topo[i],"%d %d %d %d",&from,&to,&ca,&cos);
			addedge(from,to,ca,cos,0);
			addedge(to,from,ca,cos,0);
			
			grid[from][to].cost = cos;
			grid[from][to].flow = ca;
			
			grid[to][from].cost = cos;
			grid[to][from].flow = ca;
		}
		
		//add the topo about the consumer
		int totalneed = 0;
		for(int i = 5 + edgeNum; i < 5 + edgeNum + consumerNum; i++) {
			int from,to,ca;
			sscanf(topo[i],"%d %d %d",&from,&to,&ca);
			
			nc.push_back((mapp){to,from,ca});
			
			from = from + netNum;
			totalneed += ca;
			
			addedge(to,from,ca,0,0);
			//addedge(to,from,0,0,0);
			 
			demand.push_back(ca);
			directNode.push_back(to);
			J1.push_back(to);
			
		}
		
		//add the supert
		supert = vNum + 1;
		for(int i = 0; i < consumerNum; i++){
			e.push_back((edge){i+netNum,supert,demand[i],0,0}); 
			e.push_back((edge){supert,i+netNum,0,0,0}); 
			int len = e.size() - 1;
			v[supert].push_back(len);
			v[i+netNum].push_back(len-1);
			
		}
		
		
		//cout <<"e size : "<< e.size() << endl;
		
		// backup the graph 
		for(int i = 0; i < (int)e.size(); i++){
			ec.push_back(e[i]);
		}	
		for(int i = 0; i < N; i++){
			for(int j = 0; j < (int)v[i].size();j++){
				vc[i].push_back(v[i][j]);
			}
		}	
		
		if(netNum >= 500){
		
			for(int i = 0 ; i < consumerNum; i++){
				int source = directNode[i];
				dijkstra(source);
				
				for(int j = 0; j < consumerNum; j++){
					int to = directNode[j];
					if(source == to){
						bigpre[i][j].bigpathcost = 0;
						bigpre[i][j].bigpathflow = inf;
						bigpre[i][j].bigfrom = i;
						bigpre[i][j].bigto = j;
						bigpre[i][j].bigroad.push_back(i);
					}
					else{
						bigpre[i][j].bigpathcost = D[to];
						vector<int> tmpbigroad;
						tmpbigroad.clear();
						showPath(to,source,tmpbigroad);
						
						int bigroadflow = inf;
						for(int k = 0; k < (int)tmpbigroad.size();k++){
							bigpre[i][j].bigroad.push_back(tmpbigroad[k]);
							if(k!=((int)tmpbigroad.size()-1)){
								int be = tmpbigroad[k], en = tmpbigroad[k+1];
								int oneflow = grid[be][en].flow;
								if(oneflow < bigroadflow) {
									bigroadflow = oneflow;
								}
							}
						}
						
						bigpre[i][j].bigpathflow = bigroadflow;
						bigpre[i][j].bigfrom = i;
						bigpre[i][j].bigto = j;	
						
								
					}
				}
			}
			
			sort(nc.begin(),nc.end(),cmp); // demand[i] is small , the prior to satisfy
			
			
			bool isuse[consumerNum];    // isuse to record the server is use ? 
			for(int i = 0; i < consumerNum; i++){
				isuse[i] = true;
			}
			
			string bigoutput = "";   // output the res
			int bigroadnum = 0 ;
			int sum = 0;
			
			
			for(int i = 0; i < consumerNum; i++){
				int des = nc[i].con;
				
				//cout << "des: " << des << endl;
				vector<bigpath> re;
				vector<bigpath> reback;
				re.clear();
				int othertotalsupply = 0;
				for(int  j = 0; j < consumerNum; j++){
					if(isuse[j] && (j!=des) && (bigpre[j][des].bigpathflow>0))  {
						re.push_back(bigpre[j][des]);
						reback.push_back(bigpre[j][des]);
						othertotalsupply += bigpre[j][des].bigpathflow;	
					}
				}
				
			
				if(othertotalsupply < demand[des]) {  // all other together can't satisfy  || "=" is question ??
					// direct output
					sum += serverDeployCost;
					//cout << "direct sum= " << sum << endl;
					bigoutput += to_string(directNode[des]) + " " + to_string(des) + " " + to_string(demand[des]) + "\n"; 
					bigroadnum++; 
					demand[des] = 0;
					continue;	 	
				}
				
				sort(re.begin(),re.end(),cmp1);  // sort other server by their give node i cost
				sort(reback.begin(),reback.end(),cmp1);
				
				int owncost = serverDeployCost; 
				int othercost = 0;
				int ownneed = demand[des];
				
				for(int j = 0; j < (int)re.size(); j++){
					if(ownneed == 0) break;
					if(re[j].bigpathflow > ownneed){
						othercost += ownneed * re[j].bigpathcost;
						ownneed = 0;
					} else {
						othercost += re[j].bigpathflow * re[j].bigpathcost;
						ownneed -= re[j].bigpathflow;
					}
				}
				
				if(othercost >= owncost){
					// direct
					sum += serverDeployCost;
					
					bigroadnum++;
					bigoutput += to_string(directNode[des]) + " " + to_string(des) + " " + to_string(demand[des]) + "\n";
					demand[des] = 0;
					continue;
				} else {
					
					string otherroad = "";
					int ownneed = demand[des];
					int trynum = 0;
					
					for(int j = 0; j < (int)re.size(); j++){
							//cout << "from:" << re[j].bigfrom <<" to:"<< re[j].bigto << " bigpathcost=" << re[j].bigpathcost << endl; 
							
							
							// more judge --------------------------------------done
							if(ownneed == 0) {
								isuse[des] = false;
								bigoutput += otherroad;
								bigroadnum += trynum;
								break;
							}
							
							int reduce = 0;
							if(re[j].bigpathflow >= ownneed){
								reduce = ownneed;
								ownneed = 0;
							} else {
								reduce = re[j].bigpathflow;
								ownneed -= re[j].bigpathflow;
							}
							
							//cout << "reduce= " << reduce << " re[j].bigpathcost=" 
							
							sum += reduce * re[j].bigpathcost;
							
							//cout << "undirect sum= " << sum << endl;
							// change bigpre

							int len = (int)re[j].bigroad.size();
							
							for(int k = 0; k < len; k++){
								
								if(k!=(len - 1)){
									int be = re[j].bigroad[k] , en = re[j].bigroad[k+1];
									grid[be][en].flow -= reduce;
								}
								// output the road
								
								otherroad += to_string(re[j].bigroad[k]) + " ";
							}
							otherroad += to_string(des) + " " + to_string(reduce) + "\n";
						
							
							// change the road flow
							for(int p = 0; p < consumerNum; p++){
								//int source = directNode[p];
								for(int q = 0; q < consumerNum; q++){
									//int to = directNode[q];
									if(p != q){
										int bigroadflow = inf;
										int le = bigpre[p][q].bigroad.size();
										for(int r = 0; r < le; r++){
											if(r != (le - 1)){
												int be = bigpre[p][q].bigroad[r], en = bigpre[p][q].bigroad[r+1];
												int oneflow = grid[be][en].flow;
												if(oneflow < bigroadflow){
													bigroadflow = oneflow;
												}
											}
										}
										bigpre[p][q].bigpathflow = bigroadflow;
									}
								}
								
							}	
					
				
								int ressupply = 0;
								for(int index = j + 1; index < (int)re.size(); index++){
									int ff = re[j].bigfrom , ee = re[j].bigto ;
									re[j].bigpathflow = bigpre[ff][ee].bigpathflow;
									ressupply += bigpre[ff][ee].bigpathflow;
								}
								
								if(ressupply < ownneed){
									// direct
									bigroadnum++;
									bigoutput += to_string(directNode[des]) + " " + to_string(des) + " " + to_string(demand[des]) + "\n";
									//  roll back
									//cout << "ressupply" << ressupply << " ownneed: " << ownneed << endl;
									for(int index = j; index >= 0; index--){
										int len = (int)reback[index].bigroad.size();
							
										for(int kindex = 0; kindex < len; kindex++){
											
											if(kindex!=(len - 1)){
												int be = reback[index].bigroad[kindex] , en = reback[index].bigroad[kindex+1];
												grid[be][en].flow += reback[index].bigpathflow;
											}
											// output the road
											
										}
										ownneed += reback[index].bigpathflow;
									}
									
					//				cout << "roll back : "  << "  ownneed: " << ownneed << endl;
									for(int p = 0; p < consumerNum; p++){
										//int source = directNode[p];
										for(int q = 0; q < consumerNum; q++){
											//int to = directNode[q];
											if(p != q){
												int bigroadflow = inf;
												int le = bigpre[p][q].bigroad.size();
												for(int r = 0; r < le; r++){
													if(r != (le - 1)){
														int be = bigpre[p][q].bigroad[r], en = bigpre[p][q].bigroad[r+1];
														int oneflow = grid[be][en].flow;
														if(oneflow < bigroadflow){
															bigroadflow = oneflow;
														}
													}
												}
												bigpre[p][q].bigpathflow = bigroadflow;
											}
										}
										
									}
						
									break;
								}	  
								
					}
					
				}
				
			}
			
			int servernum = 0 ;
			for(int i = 0 ; i < consumerNum; i++){
				if(isuse[i])
					servernum++;
			}
			//cout << "sum= " << sum << " serverNum: " << servernum << endl;
			string finalout = "";
			//cout << "set num : " << server.size() << endl;
			finalout = to_string(bigroadnum) + "\n\n" + bigoutput; 
			
			char * ans_file = (char *)finalout.c_str();
		    write_result(ans_file, filename);
			return ;	
		
		}
		
		// the oricost
		mincost = 0; supers = vNum;
		
		//greedy algorithm to delete the ori set J1
				
		//cout << "ori: " << J1.size() << endl ;
		while(1) {
			if((clock()-start) > 85*CLOCKS_PER_SEC){   // time is out
					break;
			}
			
			if((int)J1.size() == consumerNum){
				mincost = 0;
			} else{
				repaint(J1);
				int flow = 0, cost = 0;
				while(MCMF(supers,supert,flow,cost));	
				evinit();
				mincost = cost;
			}
			
			int delta = 0;
			int maxdelta = -100000000;
			vector<int> deletepos;	
			for(int i = 0; i < (int)J1.size(); i++)	{	
				
				if((clock()-start) > 85*CLOCKS_PER_SEC){   // time is out
					break;
				}
				
				J2.clear();
				for(int j = 0; j < (int)J1.size();j++){
					if(J1[j] != J1[i]){
						J2.push_back(J1[j]);
					}
				}
				
				repaint(J2);	
				int flow = 0, cost = 0;
				while(MCMF(supers,supert,flow,cost));
				evinit();
				
				if(flow == totalneed){
					delta = serverDeployCost + mincost - cost;
					//cout << "delta=" <<delta << endl; 
					
					if(delta > maxdelta) {
						maxdelta = delta;
						deletepos.clear();
						deletepos.push_back(J1[i]); 
					}
					else if(delta == maxdelta){
						deletepos.push_back(J1[i]);
					}
				}	
			}
			
			
			if(maxdelta > 0){
				vector<int> tmp;
				for(int i = 0; i < (int)J1.size(); i++){
					vector<int>:: iterator iter = find(deletepos.begin(),deletepos.end(),J1[i]);
					if(iter == deletepos.end())	{
						tmp.push_back(J1[i]);
					} else{
						J0.push_back(J1[i]);
					}
				}
				J1.clear();
				for(int i = 0; i < (int)tmp.size();i++){
					J1.push_back(tmp[i]); 
				} 
				deletepos.clear();
			}
			else{
				break;
			}
			
		}
		
		//cout << " after J1.size() " << J1.size() << endl; 
		repaint(J1);
		int onetotalflow = 0, onetotalcost = 0;
		while(MCMF(supers,supert,onetotalflow,onetotalcost));
		//cout << "onetotalflow: " << onetotalflow << " onetotalcost: " << onetotalcost << endl;
		
		ec.clear();
		for(int i = 0; i < N;i++)
			vc[i].clear();
				
		for(int i = 0; i < (int)e.size();i++){
			if(e[i].flow > 0) {
				ec.push_back((edge){e[i].from,e[i].to,e[i].flow,e[i].cost,0});
				ec.push_back((edge){e[i].to,e[i].from,e[i].flow,e[i].cost,0});
				
				
				int len = ec.size() - 1;
				vc[e[i].to].push_back(len);
				vc[e[i].from].push_back(len-1);
			}
		}
		
		evinit();
		repaint(J1);
		onetotalflow = 0, onetotalcost = 0;
		while(MCMF2(supers,supert,onetotalflow,onetotalcost));
		//cout << "consumerNum : " << consumerNum << " totalneed" << totalneed <<endl;
		cout << "flow:" << onetotalflow << " server is: " <<  J1.size() << " cost is:" << onetotalcost + J1.size() *  serverDeployCost << endl;
		//cout << "onetotalflow: " << onetotalflow << " onetotalcost: " << onetotalcost << endl;
		//cout << "----------------------" << endl;
		
		
		// output to the file
		
		string output;
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
		
		output = to_string(recordpath.size());
		output += "\n\n";
		
		for(int i = 0; i < (int)recordpath.size() - 1;i++){
			output += recordpath[i] + "\n";
		} 
		output += recordpath[recordpath.size()-1];
		
		char * topo_file = (char *)output.c_str();
	    write_result(topo_file, filename);
	    return ;
		    
}




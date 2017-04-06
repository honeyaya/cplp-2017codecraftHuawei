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
const int inf = 0x3f3f3f3f;
const int N = 1500;

struct Node{
	int from,to,cap,flow,cost;
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
int minflow = 0,first = -1,mincost=inf;
		
//record pa[N][N];
		
//vector<int> p(N,0);
//vector<int> a(N,0);
vector<int> demand;

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
    	//cout << "iterator u=" << u << " flow= " << flow << " cost=" << cost << endl;
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
			from = from + netNum;
			
			totalneed += cap;
			
			e.push_back((Node){to,from,cap,0,0});
			e.push_back((Node){from,to,0,0,0});
			
			int len = e.size() - 1;
			v[from].push_back(len);
			v[to].push_back(len-1);
			
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
		
		int delta = 0, maxdelta = -100000000;
		vector<int> changepos;
		vector<int> J0;
		vector<int> J1;

		for(int i = 0; i < netNum; i++){
			J1.push_back(i);
		}
		
		bool deadline = false;
		
		if(vNum > 800){
			init();
			int onetotalflow = 0, onetotalcost = 0;
			while(BellmanFord(supers,supert,onetotalflow,onetotalcost));
			//cout << "onetotalflow=" << onetotalflow << " onetotalcos=" <<onetotalcost << endl;
			
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
		}
		
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
				//cout << "oneflow=" << oneflow << " onecos=" <<onecost << endl;
				
				if(oneflow == totalneed){
					//delta = serverDeployCost * (J1.size()) + onetotalcost - (serverDeployCost * (J1.size()-1) + onecost);
					delta = serverDeployCost + onetotalcost - onecost;
					
					if((delta > 0) && (delta > maxdelta)) {
						maxdelta = delta;
						changepos.clear();
						changepos.push_back(J1[i]);
					}
					
					if(delta == maxdelta) {
						changepos.push_back(J1[i]);
					}	
					
					if(delta < 0){
						J0.push_back(J1[i]);
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
		cout << "onetotalflow=" << onetotalflow << " onetotalcost=" << onetotalcost +J1.size() * serverDeployCost << endl;
				
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
		
		cout << "J1:" << endl;
		for(int i = 0; i < J1.size();i++)
			cout << J1[i] << " ";
		cout << endl;
		
		
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




//5 19 25 35 39 5 44 1061109545 3 19 17 27 5 1061109519 30 1061109518 32 5 19 10 6 6 1061109531 39 21 5 5 5 19 19 19 30 28 28 1 19 21 1061109521 1061109504 46 29 38 39 1061109528 31 14 19 19 19 7 15 15 15 15 15 15 15 15 8 1061109567 15

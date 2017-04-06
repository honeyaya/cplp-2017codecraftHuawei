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
using namespace std;

#define Del(a,b) memset(a,b,sizeof(a))
const int inf = 0x3f3f3f3f;
const int N = 500;

struct Node{
	int from,to,cap,flow,cost;
};


struct server{
	int degree;
	int num;
	int totalcap;
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


		
bool BellmanFord(int s,int t,int& flow, int & cost){
	//Del(dis,inf);
    //Del(vis,0);

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
	//Del(dis,inf);
    //Del(vis,0);

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


bool cmp(const server& a, const server& b){
	if(a.degree == b.degree) 
		return a.totalcap > b.totalcap; 
	return a.degree > b.degree;
}


void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	    int netNum, edgeNum,consumerNum,serverDeployCost;
				
		sscanf(topo[0],"%d %d %d",&netNum,&edgeNum,&consumerNum);
		
		sscanf(topo[2],"%d",&serverDeployCost);
		
		int vNum = netNum + consumerNum;
		
		
		for(int i = 0; i< vNum; i++){
			v[i].clear();
		}	
		e.clear();
		ec.clear();
		
		int s[vNum];
		int t[vNum];
		for(int i = 0; i < vNum; i++){
			s[i] = 0;
			t[i] = 0;
		}
		
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
			
			s[from]++;
			s[to]++;
			t[from]+=cap;
			t[to] += cap;
			
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
			
			s[to]++;
			s[from]++;
			t[to]+=cap;
			t[from]+=cap;
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
		server ps[vNum];
		int index = 0;
		for(int i = 0; i < vNum; i++) {
			//if(s[i] >= 3) {
				ps[index].num = i;
				ps[index].degree = s[i];
				ps[index].totalcap = t[i];
				index++;
			//}
		}
		//sort(ps,ps+index,cmp);
		
		/*
		for(int i = 0; i < index; i++){
			cout << ps[i].num << " " << ps[i].degree << " " << ps[i].totalcap<< endl;
		}
		*/
		
		
		// caculate the min_cost
		
		/*
		int cc[netNum][consumerNum];	
		map<int,int>mp;
		map<int,int> ::iterator it;
			
		for(int i = 0; i < index; i++){
			for(int j = 0; j < consumerNum; j++){
				int flow = 0,cost = 0;
				int source = ps[i].num , des = j + netNum;
				//cout << "source= " << source << " des= " << des << endl; 
				while(BellmanFord(source,des,flow,cost));
				cc[i][j] = cost;
				if(cost < serverDeployCost) {
					if(mp.find(source)!=mp.end()){
						mp[source]++;
					} else {
						mp[source]=1;
					}
				}
				//ff[i][j] = flow;		
				for(int k = 0; k < N; k++){
					p[k] = -1 ,a[k] = -1;
				}
				e.clear();
				for(int k = 0; k < (int)ec.size(); k++){
					e.push_back(ec[k]);
				}
			} 
		} 
		
		for(it=mp.begin();it!=mp.end();it++){
			if(it->second == 1){
				mp.erase(it);
			}
		}
	
		*/
		
		/*
			for(int i = 0; i < consumerNum; i++){
			for(int j = 0; j < index; j++){
				int flow = 0,cost = 0;
				int source = ps[j].num , des = i + netNum;
				cout << "source= " << source << " des= " << des << endl; 
				while(BellmanFord(source,des,flow,cost));
				cc[i][j] = cost;
				ff[i][j] = flow;
						
				for(int k = 0; k < N; k++){
					p[k] = -1 ,a[k] = -1;
				}
				e.clear();
				for(int k = 0; k < (int)ec.size(); k++){
					e.push_back(ec[k]);
				}
			} 
		} 
		
		for(int i = 0; i < consumerNum; i++){
			for(int j = 0; j < index; j++){
				cout << cc[i][j] <<  " ";
			}
			cout << endl;
		}
		
		cout << "the num:" << endl;
		for(int i = 0; i < index; i++) {
			cout << ps[i].num << " " ;
		}
		cout << endl;
		*/
		
		/*
		for(int i = 0; i < consumerNum; i++){
			for(int j = 0; j < 5; j++){
				cout << ff[i][j] <<  " ";
			}
			cout << endl;
		}
		 */
		
		
		// multi-source and multi-des
		
		/*
		for(int i = 0; i < (int)e.size(); i++){
			ec.push_back(e[i]);
		}
		int flow = 0 , cost = 0;
		while(BellmanFord(supers,supert,flow,cost));
		cout << "flow=" << flow << " cost=" << cost << endl;		
		*/
		int  supert = vNum+1;
		for(int i = 0; i < consumerNum; i++){
			e.push_back((Node){i+netNum,supert,demand[i],0,0});
			e.push_back((Node){supert,i+netNum,0,0,0});
				
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
		
		int delta = 0, maxdelta = -1000000, changepos = -1;
		vector<int> J0;
		vector<int> J1;
		
		for(int i = 0; i < netNum; i++){
			J1.push_back(i);
		}
		
		while(1){
			delta = 0, maxdelta = -100000,changepos = -1;
			supers = vNum;
			for(int j = 0; j < (int)J1.size();j++){			
				e.push_back((Node){supers,J1[j],inf,0,0});
				e.push_back((Node){J1[j],supers,0,0,0});
							
				int len = e.size() - 1;
				v[J1[j]].push_back(len);
				v[supers].push_back(len-1);
			}	 
			
			int onetotalflow = 0, onetotalcost = 0;
			while(BellmanFord(supers,supert,onetotalflow,onetotalcost));
			//cout << "onetotalflow=" << onetotalflow << " onetotalcos=" <<onetotalcost << endl;
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
				
				int oneflow = 0, onecost = 0;
				while(BellmanFord(supers,supert,oneflow,onecost));
				//cout << "oneflow=" << oneflow << " onecos=" <<onecost << endl;
				
				if(oneflow == totalneed){
					delta = serverDeployCost * (J1.size()) + onetotalcost - (serverDeployCost * (J1.size()-1)+onecost);
					//delta =  serverDeployCost + onetotalcost - onecost;
					//cout << "delta= " << delta << endl;
				}
				
				if(delta > maxdelta) {
					maxdelta = delta;
					changepos = J1[i];
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
					J0.push_back(changepos);
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
		/*
		for(int i = 0; i < (int)J1.size();i++){
			cout << J1[i] << " ";
		}
		cout << endl;
		*/
		
		supers = vNum;
		for(int j = 0; j < (int)J1.size();j++){			
			e.push_back((Node){supers,J1[j],inf,0,0});
			e.push_back((Node){J1[j],supers,0,0,0});
							
			int len = e.size() - 1;
			v[J1[j]].push_back(len);
			v[supers].push_back(len-1);
		}	 
			
		int onetotalflow = 0, onetotalcost = 0;
		while(BellmanFord2(supers,supert,onetotalflow,onetotalcost));
		//cout << "onetotalflow=" << onetotalflow << " onetotalcos=" <<onetotalcost << endl;
		
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
		/*
		for(int i = 0; i < (int)pathflow.size();i++){
			//if()
			//sprintf(a, "%d %d %d",netnode,consumerNum-i,need);
			cout << pathflow[i] << " ";
		}
		cout << endl;
		*/
		
	/*	
	clock_t start = clock();
	do{	
	}while((clock()-start) < 80*CLOCKS_PER_SEC);
	*/
	/*
		char * c;
	    int spaceCount = 0;
	    string res;
	    char a[20];
	    sprintf(a, "%d\n\n",consumerNum);
	    res = a;
	    int netnode, need;
	    printf("I am here!!\n");
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
	    
	    //char * topo_file = (char *)res.c_str();
	    write_result(topo_file, filename); 
    */
}




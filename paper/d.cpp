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

bool cmp(const mapp& a, const mapp& b){
	return a.need < b.need;
}

bool cmp1(const bigpath& a, const bigpath& b){
	if(a.bigpathcost == b.bigpathcost){
		return a.bigpathflow > b.bigpathflow;
	}
	return a.bigpathcost < b.bigpathcost;
}

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
				cost[i][j] = 0;
				cap[i][j] = 0;
			}
		}
		
		
		
		grid[from][to].cost = cos;
			grid[from][to].flow = ca;
			
			grid[to][from].cost = cos;
			grid[to][from].flow = ca;
			
			cost[from][to] = cos;
			cost[to][from] = cos;
			
			cap[from][to] = ca;
			cap[to][from] = ca;
			
			
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




/*
		if(vNum >= 500){
		
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
		
		*/
		
		
		
		
		
		//  another min-cost-max-flow
//#define CLR(a, x)  memset( a, x, sizeof( a ) )
const int Inf = 0x3f3f3f3f;
#define BUBL { \
    t = q[i]; q[i] = q[j]; q[j] = t; \
    t = inq[q[i]]; inq[q[i]] = inq[q[j]]; inq[q[j]] = t; }

bool DD( int n, int s, int t )
{
	
    for(int i = 0; i < N;i++){
    	d[i] = 0x3F;
		par[i] = -1;
		inq[i] = -1; 
	}
    //for( int i = 0; i < n; i++ ) d[i] = Inf, par[i] = -1;
    d[s] = qs = 0;
    inq[q[qs++] = s] = 0;
    par[s] = n;

    while( qs )
    {
        // get the minimum from q and bubble down
        int u = q[0]; inq[u] = -1;
        q[0] = q[--qs];
        if( qs ) inq[q[0]] = 0;
        for( int i = 0, j = 2*i + 1, t; j < qs; i = j, j = 2*i + 1 )
        {
            if( j + 1 < qs && d[q[j + 1]] < d[q[j]] ) j++;
            if( d[q[j]] >= d[q[i]] ) break;
            BUBL;
        }

        // relax edge (u,i) or (i,u) for all i;
        for( int k = 0, v = adj[u][k]; k < deg[u]; v = adj[u][++k] )
        {
            // try undoing edge v->u
            if( fnet[v][u] && d[v] > Pot(u,v) - cost[v][u] )
                d[v] = Pot(u,v) - cost[v][par[v] = u];

            // try using edge u->v
            if( fnet[u][v] < cap[u][v] && d[v] > Pot(u,v) + cost[u][v] )
                d[v] = Pot(u,v) + cost[par[v] = u][v];

            if( par[v] == u )
            {
                // bubble up or decrease key
                if( inq[v] < 0 ) { inq[q[qs] = v] = qs; qs++; }
                for( int i = inq[v], j = ( i - 1 )/2, t;
                     d[q[i]] < d[q[j]]; i = j, j = ( i - 1 )/2 )
                     BUBL;
            }
        }
    }

    for( int i = 0; i < n; i++ ) if( pi[i] < Inf ) pi[i] += d[i];

    return par[t] >= 0;
}

int mcmf4( int n, int s, int t, int &fcost )
{
    // build the adjacency list
    //CLR( deg, 0 );
    for(int i = 0; i < N;i++)
		deg[i] = 0;
    for(int i = 0; i < N;i++){
    	for(int j=0;j<N;j++){
    		
    		fnet[i][j] = 0;
		}
	}
    for( int i = 0; i < n; i++ )
    for( int j = 0; j < n; j++ )
        if( cap[i][j] || cap[j][i] ) adj[i][deg[i]++] = j;
	
	for(int i = 0; i < N;i++)
		pi[i] = 0;
	
    int flow = fcost = 0;

    // repeatedly, find a cheapest path from s to t
    while( DD( n, s, t ) )
    {
        // get the bottleneck capacity
        int bot = inf;
        for( int v = t, u = par[v]; v != s; u = par[v = u] ){
        	if(bot > fnet[v][u]) bot = fnet[v][u];
        	else 
        		bot = cap[u][v] - fnet[u][v];
		}
            //bot <?= fnet[v][u] ? fnet[v][u] : ( cap[u][v] - fnet[u][v] );

        // update the flow network
        for( int v = t, u = par[v]; v != s; u = par[v = u] )
            if( fnet[v][u] ) { fnet[v][u] -= bot; fcost -= bot * cost[v][u]; }
            else { fnet[u][v] += bot; fcost += bot * cost[u][v]; }

        flow += bot;
    }

    return flow;
}



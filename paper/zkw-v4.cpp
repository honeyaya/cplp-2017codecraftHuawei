bool use[N];

void BellmanFord(int n, int s, int t, int &flow, int &cost){
    for (int i = 0; i <= n; i++)
        dis[i] = inf;
    dis[s] = 0;
    vis[s] = 1;
    p[s] = 0;
    a[s] = inf;
    queue<int> q;

    q.push(s);
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        vis[u] = 0;
        for (int i = 0; i< (int)v[u].size(); i++)
        {
            edge& g = e[v[u][i]];

            if (g.cap>g.flow && dis[g.to] > dis[u] + g.cost)
            {
                dis[g.to] = dis[u] + g.cost;
                p[g.to] = v[u][i];  //保存前驱
                a[g.to] = min(a[u], g.cap - g.flow);
                //if (!vis[g.to])
                //{
                    q.push(g.to);
                //    vis[g.to] = 1;
               // }

            }
        }
    }
    for (int i = 0; i <= n; i++) dis[i] = dis[t] - dis[i];
    //if (dis[t] == inf)
    //    return false;
    /* flow += a[t];
    cost += dis[t]*(a[t]);
    int u = t;
    while(u!=s)
    {
    e[p[u]].flow += a[t];
    e[p[u]^1].flow -= a[t];
    u = e[p[u]].from;
    }*/
    //return true;
}

int add_flow(int st, int ed, int u, int &flow, int &cost,  int f){
    if (u == ed){
        flow += f;
        cost += dis[st] * f;
        return f;
    }
    use[u] = 1;
    int now = f;
    for (int i = 0; i< (int)v[u].size(); i++) {
        edge& g = e[v[u][i]];
        if (g.cap >g.flow  && !use[g.to] && dis[u] == dis[g.to] + g.cost){
            int temp = add_flow(st, ed, g.to, flow, cost, min(now, g.cap-g.flow));
            //cap[p] -= temp;
            //cap[p ^ 1] += temp;

            e[p[u]].flow += temp;
            e[p[u] ^ 1].flow -= temp;
            now -= temp;
            if (!now)
                break;
        }
    }
    return f - now;
}


bool modify_label(int n)
{
    int di = inf;
    for (int u = 0; u <= n; u++)
        if (use[u])
            for (int i = 0; i< (int)v[u].size(); i++) {
                edge& g = e[v[u][i]];
                if (g.cap>g.flow && !use[g.to]) di = min(di, dis[g.to] + g.cost - dis[u]);
            }
    if (di == inf)
        return false;
    for (int i = 0; i <=n; i++)
        if (use[i])
            dis[i] += di;
    return true;
}

void min_cost_flow(int ss, int tt, int nn, int &flow, int &cost)
{
    int st = ss;
    int ed = tt;
    int    n = nn;
    //int cost = 0;
    //int flow = 0;
    BellmanFord(n, st, ed, flow, cost);
    while (1){
        while (1){
            for (int i = 0; i <= n; i++) use[i] = 0;
            int tmp = inf;
            if (!add_flow(st, ed, st, flow, cost, tmp)) break;
        }
        if (!modify_label(n)) break;
    }
    //return cost;
}

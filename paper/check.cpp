#include<iostream>
#include<cstring>
#include<cstdio>
#include<vector>
#include<bitset>
using namespace std;
#define MAX_EDGE_NUM 60000
char *topoC[MAX_EDGE_NUM];
//你要完成的功能总入口
int map[2100][2100];
int w[2100][2100];
int check[3000];
int nodenum,linknum,costnum,servecost;
int need_width[2000];
int total_need=0;
//前linknum保存的是网络中的边数, u v width cost
//后costnum保存的是消费结点到网络结点 消费结点ID  网络结点ID 视频需求
int linkEdge[MAX_EDGE_NUM][4];
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
//    for(int i = 0;i < line_num; i++){
//        cout<<topo[i]<<endl;
//    }

    sscanf(topo[0],"%d%d%d",&nodenum,&linknum,&costnum);
    sscanf(topo[1],"%d",&servecost);
    memset(map,0,sizeof(map));
    for(int i = 0;i < linknum; i++){
        sscanf(topo[i+2],"%d%d%d%d",&linkEdge[i][0],&linkEdge[i][1],&linkEdge[i][2],&linkEdge[i][3]);
        int u = linkEdge[i][0];
        int v = linkEdge[i][1];
        map[u][v] = map[v][u] = linkEdge[i][2];
        w[u][v] = w[v][u] = linkEdge[i][3];
    }
    memset(check,0,sizeof(check));
    memset(need_width,0,sizeof(need_width));
    for(int i = 0;i < costnum; i++){
        sscanf(topo[i+2+linknum],"%d%d%d",&linkEdge[i+linknum][0],&linkEdge[i+linknum][1],&linkEdge[i+linknum][2]);
        //linkEdge[i+linknum][4] = 0;
        int u = linkEdge[i+linknum][1];
        check[u] = linkEdge[i+linknum][0];
        need_width[linkEdge[i+linknum][0]] = linkEdge[i+linknum][2];
        total_need += linkEdge[i+linknum][2];
    }
//    for(int i = 0;i < nodenum; i++){
//        for(int j = 0;j < nodenum; j++)
//            cout<<map[i][j]<<" ";
//        cout<<endl;
//    }
//    cout<<endl;
//    for(int i = 0;i < nodenum; i++){
//        for(int j = 0;j < nodenum; j++)
//            cout<<w[i][j]<<" ";
//        cout<<endl;
//    }
//    cout<<endl;
//    for(int i = 0;i < nodenum; i++){
//        cout<<i<<":"<<check[i]<<"  ";
//    }
//    cout<<endl;

}
int getnum(int &flag){
    int num = 0;
    int x;
    do{
        x = getchar();
    }while(!(x>='0' && x <= '9'));

    do{
        num = num * 10 + x - '0';
        if((x = getchar()) == EOF)            break;
    }while(x >= '0' && x <= '9');
    if(x == '\n')
        flag = 1;
    else
        flag = 0;    if(x == EOF)        flag = 1;
    return num;
}

int main(){
    //数据文件    char * data_file = "case-1.txt";    char * answer_file = "case-1answer.txt";    char * debug_file = "check_result.txt";    freopen(data_file,"r",stdin);
    //输出文件
    //freopen(debug_file,"w",stdout);
    int linenum = 0;
    topoC[linenum] = new char[30];
    while(gets(topoC[linenum])){
        if(strlen(topoC[linenum]) != 0){
            linenum++;
            topoC[linenum] = new char[30];
        }
    }
    deploy_server(topoC,linenum,"answer.txt");
    //答案文件
    //freopen("case0out.txt","r",stdin);
    freopen(answer_file,"r",stdin);
    char word[1000];
    //消耗三行无关输出
    //gets(word);
    //gets(word);
    //gets(word);
    int num;
    cin>>num;
    //cout<<num<<endl;
    int flag ;
    getchar();    gets(word);    int server_place[2100],s_num=0;
    memset(server_place,0,sizeof(server_place));
    int total_cost = 0,total_width=0;
    for(int nn = 0; nn < num; nn++){
        vector<int> ans;
        do{
            int x = getnum(flag);
            //cout<<x<<" ";
            ans.push_back(x);
        }while(!flag);
        //cout<<endl;
        int u = ans[0], v = ans[ans.size()-2], cost = ans[ans.size()-1];
        total_width += cost;        int local_cost = 0;
        if(server_place[u] == 0){
            server_place[u] = 1;
            s_num += 1;
            total_cost += servecost;
        }
        if(need_width[v] < cost){
            printf("消费结点%d流量超过了需要 剩余流量:%d 增加流量:%d\n",v,need_width[v],cost);
            need_width[v] -= cost;
        }
        for(int i = 1;i < ans.size()-2; i++){
            int p = ans[i];
            if(map[u][p] < cost){
                printf("第%d条路径 流量不足 %d -> %d left:%d need:%d\n",nn,u,p,map[u][p],cost);
            }
            else {
                map[u][p] -= cost;
                local_cost += cost * w[u][p];
            }
            u = p;
        }        total_cost += local_cost;
        if(check[u] != v){
            printf("最后的服务器结点%d与消费结点%d不匹配\n",u,v);
        }//        cout<<local_cost<<endl;
//        for(int i = 0;i < ans.size(); i++)
//        {
//            cout<<ans[i]<<" ";
//        }
        //cout<<endl;        //cout<<nn<<" "<<num<<endl;
    }
    printf("%s: Total cost:%d Server num: %d 需要总流量: %d  提供的流量: %d \n",data_file,total_cost,s_num,total_need,total_width);

}

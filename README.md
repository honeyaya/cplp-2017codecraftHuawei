在一个网络中存在消费节点和网络结点，从网络结点中选择一些结点作为服务器结点，给消费结点传输流量，服务器的流量是无限的，但是传输的过程中经过的线路是有容量和单元花费限制的，同时，服务器本身有安装成本，选哪些结点使得总花费最小？

经过分析，发现可以转化为经典的cplp问题

最直接的就是直连相连，这个本身就是一种方案

处理的时候，针对中小数据，使用的是启发式搜索方法，每次都从当前可选的结点中选择某个结点删除后减少花费最大的那个进行删除，最开始还蛮好的效果，不过随着大数据一直处理不好，小数据的优化也就没时间做了，一个是我每次都会遍历所有的找到减少最大的，这样时间有点长，另外，本身这个算法是基于贪心策略的，也就可能出现花费更小的方案

不过，最主要的是大样例的时候，这个时候搜索并没有优化，考虑的是跟消费结点直接相连的网络节点，查看这些结点之间的最短路径，从而，对于所有的这些直连结点，查看是否有别的直连结点来代替，如果可以代替的话，就会把对应的路径上的流量都减去当前链路上可以提供的流量，不然就回到之前的状态，在众多的路径选择中，我每次选择的是针对可选的的路径中，选择花费最小的路径来提供，由于本身选择的对象就是这部分直连的网络节点，时间非常快，这个bug就是在两个服务器之间其实存在多条路径的，不过每次我都选择的是最短的那条，这就把可选的方案减少了，比赛结束的前一天还想着试试bfs，不过也因为在优化最大流最小费用流的模型没能实施

这次的比赛，值得肯定的是，代码是自己一点点写出来的，效率不是很高，不过一个人从设计想法到实现想法，还是蛮充实的，不过也让我意识到找个队友的重要性

ps：拉格朗日松弛下的启发式算法貌似是解决MIP问题的比较成熟的办法，奈何数学太捉急，很多公式推导并不是那么顺利

pps：随着代码量的急速增长，我觉得代码重构是何其重要！！！

ppps：算法导论这部巨著绝逼不是搞竞赛的神器，原来里面有那么多的实际中的问题解决，之前看的时候太肤浅了


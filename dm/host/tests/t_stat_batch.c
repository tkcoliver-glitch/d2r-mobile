#include "../dm_stat_batch.h"
#include <assert.h>
#include <stdio.h>
static void spend(int n,int left,int want,int packets) {
 int modes[24],count=dm_stat_batch_modes(n,left,modes),total=0;
 assert(count==packets);
 for(int i=0;i<count;i++){assert(modes[i]>=1&&modes[i]<=3);total+=modes[i]==1?left:modes[i]==3?5:1;}
 assert(total==want);
}
int main(void){
 spend(10,505,10,2);spend(100,505,100,20);spend(100,97,97,21);
 spend(10,3,3,3);spend(100,0,0,0);spend(-1,505,505,1);spend(-1,0,0,0);
 spend(1,2,1,1);spend(5,3,3,3);spend(5,9,5,1);
 spend(0,500,0,0);spend(-2,500,0,0);spend(101,500,0,0);spend(100,-1,0,0);
 assert(dm_stat_batch_request(0,10)==10);assert(dm_stat_batch_request(3,100)==(3<<8|100));
 assert(dm_stat_batch_request(1,-1)==(1<<8));
 assert(dm_stat_batch_request(-1,10)==-1);assert(dm_stat_batch_request(4,100)==-1);assert(dm_stat_batch_request(0,99)==-1);
 puts("stat_batch: count clamping, native modes, all, invalid input passed");
}

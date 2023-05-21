/*
  Copyright (c) 2006 James R. Bruce
  RoboCup Small-Size League grouping program

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

// Compile with:
// g++ -Wall -O2 -o grouping grouping.cpp

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

static int NumGroups = 2;
static const int MaxAllowableCountryBias = 0;
static const char *TeamFile = "teams.txt";

static const bool Debug = false;

//====================================================================//

template <class data>
void swap(data &a,data &b)
{
  data t;
  t = a;
  a = b;
  b = t;
}

//====================================================================//
// RNG from Knuth's TAOCP(2), Seminumerical Algorithms, pp 108
//--------------------------------------------------------------------//

class KnuthRng{
  static const uint64_t A1 = 271828183ULL;
  static const uint64_t A2 = 314159269ULL;
  static const uint64_t M = 2147483647ULL; // 2^31-1
  uint32_t state0,state1;
public:
  KnuthRng()
    {state0=1; state1=1;}
  void seed(uint32_t s0,uint32_t s1);
  uint32_t get();
};

void KnuthRng::seed(uint32_t s0,uint32_t s1)
{
  if(s0 == 0) s0 = 1;
  if(s1 == 0) s1 = 1;

  state0 = (uint64_t)s0 % M;
  state1 = (uint64_t)s1 % M;
}

uint32_t KnuthRng::get()
{
  uint32_t state2 = (A1*(uint64_t)state1 + A2*(uint64_t)state0) % M;
  state0 = state1;
  state1 = state2;

  return(state2);
}

//====================================================================//

struct TeamEntry{
  int rank;
  char *country;
  char *name;
};

std::vector<TeamEntry> teams;
KnuthRng rng;

const char *GetInt(const char *str,uint32_t &val)
{
  char *end = NULL;
  val = 0;
  if(!str) return(NULL);
  errno = 0;
  val = strtoul(str,&end,0);
  return(errno? NULL : end);
}

int ReadTeamFile(const char *filename)
{
  static const int MaxBuf = 256;
  char buf[MaxBuf];
  char country[MaxBuf];
  char name[MaxBuf];

  int line = 1;
  int last_rank = 1;
  FILE *in = fopen(filename,"rt");
  if(!in) return(0);

  teams.clear();

  while(fgets(buf,MaxBuf,in)){
    char ch = buf[0];
    if(ch>='0' && ch<='9'){
      TeamEntry te;
      if(sscanf(buf,"%d %256s %256s\n",&te.rank,country,name) == 3){
        if(te.rank < last_rank){
          printf("Error: non-monotonic rank at line %d\n",line);
          return(0);
        }
        te.country = strdup(country);
        te.name = strdup(name);
        teams.push_back(te);
        last_rank = te.rank;
      }else{
        printf("Error at line %d\n",line);
        return(0);        
      }
    }else if(ch=='#' || ch=='\n'){
      // skip comments and empty lines
    }else{
      printf("Junk at line %d\n",line);
      return(0);
    }
    line++;
  }

  fclose(in);

  return((int)teams.size());
}

bool RandomizeGroups()
{
  int n = teams.size();
  int i=0,j=0;

  while(i < n){
    // find subset of list at next rank
    while(j<n && teams[i].rank==teams[j].rank) j++;
    int r = teams[i].rank;
    int nr = j - i;

    if(Debug) printf("rank %d: start=%d num=%d\n",r,i,nr);
    if(nr % NumGroups != 0){
      printf("Error: teams at rank %d not divisible by number of groups\n",r);
      printf("       (add dummy entries if needed)\n");
      return(false);
    }

    // permute entries at this rank
    for(int k=0; k<nr-1; k++){
      uint32_t l = rng.get() % (nr-k);
      swap(teams[i+k],teams[i+k+l]);
      if(Debug) printf("  %d:[0..%d]\n",l,nr-k-1);
    }
    if(Debug) printf("\n");

    // advance indicies
    i = j;
  }
  return(true);
}

int CalcCountryBias()
{
  int bias = 0;
  int n = teams.size();
  for(int g=0; g<NumGroups; g++){
    for(int i=g; i<n; i+=NumGroups){
      for(int j=i+NumGroups; j<n; j+=NumGroups){
        if(strcmp(teams[i].country,teams[j].country) == 0) bias++;
      }
    }
  }
  if(Debug){
    printf("country bias = %d\n\n",bias);
  }else{
    printf(" %d",bias);
    fflush(stdout);
  }
  return(bias);
}

void PrintTeams()
{
  for(unsigned i=0; i<teams.size(); i++){
    const TeamEntry &te = teams[i];
    printf("%d %s (%s)\n",te.rank,te.name,te.country);
  }
}

void PrintGroups()
{
  int n = teams.size();
  for(int g=0; g<NumGroups; g++){
    printf("Group %c:\n",'A'+g);
    for(int i=g; i<n; i+=NumGroups){
      const TeamEntry &te = teams[i];
      printf("\t%s (%s)\n",te.name,te.country);
    }
  }
}

int main(int argc,char **argv)
{
  if(argc != 4){
    printf("usage:\n  grouping <nrOfGroups> <seed 1> <seed 2>\n");
    return(1);
  }

  uint32_t localGroups;
  if(!GetInt(argv[1], localGroups)){
    printf("invalid #groups\n");
    return(3);
  }

  NumGroups = localGroups;

  uint32_t seed0 = 0;
  uint32_t seed1 = 0;
  if(!GetInt(argv[2],seed0) || !GetInt(argv[3],seed1)){
    printf("invalid seed\n");
    return(2);
  }
  printf("seed: 0x%08X 0x%08X\n",seed0,seed1);

  int n = ReadTeamFile(TeamFile);
  if(n <= 0){
    printf("error reading teams\n");
    return(3);
  }

  printf("\n===== Teams (%d) ====\n",n);
  PrintTeams();

  printf("\n==== Randomizing ====\n");
  rng.seed(seed0,seed1);
  do{
    RandomizeGroups();
  }while(CalcCountryBias() > MaxAllowableCountryBias);
  if(!Debug) printf("\n");

  printf("\n==== Grouping ====\n");
  PrintGroups();

  return(0);
}

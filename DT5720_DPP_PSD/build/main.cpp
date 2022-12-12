#include "Build.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdlib.h>

#include "TString.h"

int main(int argc, char const *argv[])
{
  if(argc != 2){
    std::cout << "need parameter" << std::endl;
    std::cout << "like: build 33 " << std::endl;
    std::cout << "means analysis file R33 " << std::endl;
    return -1;
  }

  int run = atoi(argv[1]);
  TString file_in = TString::Format("../sort/run%04d_sort.root", run);
  std::cout << "build " << file_in << std::endl;

  TString file_out = TString::Format("./run%04d_build.root", run);
  std::cout << "build " << file_out << std::endl;

  Build *me = new Build(file_in.Data(), file_out.Data());
  me->Process();

  delete me;

  return 0;
}

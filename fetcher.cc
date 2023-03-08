#include <sstream>
#include <fstream>

#include <cstdlib>
#include <cstring>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#define DEBUG_LEVEL -1

int main(int argc, char *argv[])
{
  if (argc !=2 ) {
    std::cout<<"Please run as "<<argv[0] <<" <api key>"<<std::endl;
    exit(0);
  }
  char url[256], outfilename[64];
  for (int i = 0; i<4; i++) {
    snprintf(url, sizeof url, "https://api.dimu.org/api/solr/select?q=Kosta&wt=json&fq=(artifact.exhibitionUids:\"336485B3-1E4E-443C-8800-F93BDD12520A\")&start=%d&rows=100&api.key=%s", i*100, argv[1]);
    snprintf(outfilename, sizeof outfilename, "data_%d.json", i);
   
   /*if (DEBUG_LEVEL > 0) */std::cout<<"url to fetch is "<<std::endl<<url<<std::endl;
   std::cout<<"writing output to "<<outfilename<<std::endl;
  
  try {
    curlpp::Cleanup cleaner;
    curlpp::Easy request1;

    // Setting the URL to retrive.
    request1.setOpt(new curlpp::options::Url(url));

    if (DEBUG_LEVEL > 0) std::cout << request1 << std::endl;

    std::ofstream outfile;
    outfile.open (outfilename);
    outfile << curlpp::options::Url(url) << std::endl;
    outfile.close();
  }
  catch ( curlpp::LogicError & e ) {
    if (DEBUG_LEVEL > 0) std::cout << e.what() << std::endl;
  }
  catch ( curlpp::RuntimeError & e ) {
    if (DEBUG_LEVEL > 0) std::cout << e.what() << std::endl;
  }

 
}
   return 0;
}

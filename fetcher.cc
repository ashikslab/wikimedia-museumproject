#include <fstream>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>

#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

#define DEBUG_LEVEL -1

int main(int argc, char *argv[]) {
  std::string exbid, apikey;
  if (argc != 7) {
    std::cout << "Usage: " << argv[0]
              << " -e <exhibition id> -k <api key> -e <number of records>"
              << std::endl;
    exit(0);
  }
  int c;
  int numrec = 0;
  while ((c = getopt(argc, argv, "e:k:n:")) != -1)
    switch (c) {
    case 'e':
      exbid = optarg;
      break;
    case 'k':
      apikey = optarg;
      break;
    case 'n':
      numrec = atoi(optarg);
      break;
    default:
      break;
    }
  std::cout << numrec << " records to fetch\n";
  int numfetches = (numrec / 100) + 1;
  char url[256], outfilename[64];
  for (int i = 0; i < numfetches; i++) {
    snprintf(url, sizeof url,
             "https://api.dimu.org/api/solr/"
             "select?q=Kosta&wt=json&fq=(artifact.exhibitionUids:\"%s\")&start="
             "%d&rows=100&api.key=%s",
             exbid.c_str(), i * 100, apikey.c_str());

    std::cout << url << "\n";

    snprintf(outfilename, sizeof outfilename, "data_%d.json", i);

    if (DEBUG_LEVEL > 0) {
      std::cout << "url to fetch is " << std::endl << url << std::endl;
      std::cout << "writing output to " << outfilename << std::endl;
    }

    try {
      curlpp::Cleanup cleaner;
      curlpp::Easy request1;

      // Setting the URL to retrive.
      request1.setOpt(new curlpp::options::Url(url));

      if (DEBUG_LEVEL > 0)
        std::cout << request1 << std::endl;

      std::ofstream outfile;
      outfile.open(outfilename);
      outfile << curlpp::options::Url(url) << std::endl;
      outfile.close();
    } catch (curlpp::LogicError &e) {
      if (DEBUG_LEVEL > 0)
        std::cout << e.what() << std::endl;
    } catch (curlpp::RuntimeError &e) {
      if (DEBUG_LEVEL > 0)
        std::cout << e.what() << std::endl;
    }
  }
  return 0;
}

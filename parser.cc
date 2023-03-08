#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>

std::string yearstr(int year) {
  return year==-1? "Unknown": std::to_string(year);
}

int main()
{
  char filename[64];
  snprintf(filename, sizeof filename, "out.csv");
  std::ofstream out_file1;
  out_file1.open(filename);
  out_file1<<"id, Caption/title, production start year, end year, Description, Source, image_filename, collection name, museum name, linceses\n";
  for (auto i = 0; i<4; i++) {
    char in_file_i[64];
    snprintf(in_file_i, sizeof in_file_i, "data_%d.json", i);
    // Open the file
    FILE* fp = fopen(in_file_i, "rb");
    // Check if the file was opened successfully
    if (!fp) {
      std::cerr << "Error: unable to open file"
                << std::endl;
      return -1;
    }

    // Read the file 
    char readBuffer[192000];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    // Parse the JSON document
    rapidjson::Document doc;
    doc.ParseStream(is);

    // Check if the document is valid
    if (doc.HasParseError()) {
      std::cerr << "Error: failed to parse JSON document"
                << std::endl;
      fclose(fp);
      return 1;
    }

    // Close the file
    fclose(fp);

    if (doc.HasMember("response")
        && doc["response"].IsObject()) {
      const rapidjson::Value& response = doc["response"];
      if (response.HasMember("docs")
          && response["docs"].IsArray()) {
        std::cout<<"yes, response is an array"<<std::endl;
        for (rapidjson::SizeType i = 0; i < response["docs"].Size(); i++) {
          //std::cout<<i<<std::endl;
          int yearb = -1, yeare = -1;
          std::string title = "";
          const rapidjson::Value& article = response["docs"][i];

          std::string article_id = "";
          if (article.HasMember("identifier.id")) {
            article_id = article["identifier.id"].GetString();
          }

          if (article.HasMember("artifact.ingress.title")) {
            title  = article["artifact.ingress.title"].GetString();
          }

          if (article.HasMember("artifact.ingress.production.fromYear")) {
            const rapidjson::Value& yearbval = article["artifact.ingress.production.fromYear"];
            yearb = yearbval.GetInt();
          }

          if (article.HasMember("artifact.ingress.production.toYear")) {
            const rapidjson::Value& yeareval = article["artifact.ingress.production.toYear"];
            yeare = yeareval.GetInt();
          }

        std::string mediaid = "";
        int picid = -1;
        std::string picdim = "";
        
        if (article.HasMember("artifact.hasPictures") && article["artifact.hasPictures"].GetBool() == true) {

          if (article.HasMember("artifact.defaultMediaIdentifier")) {
            const rapidjson::Value& mediaidv = article["artifact.defaultMediaIdentifier"];
            if (mediaidv.IsString()) {
              mediaid = mediaidv.GetString();
            }
          }

          if (article.HasMember("artifact.defaultPictureIndex") && article["artifact.defaultPictureIndex"].IsInt()) {
            picid = article["artifact.defaultPictureIndex"].GetInt();
          }

          if (article.HasMember("artifact.defaultPictureDimension") && article["artifact.defaultPictureDimension"].IsString()) {
            picdim = article["artifact.defaultPictureDimension"].GetString();
          }
        }
        
        //  image file is at https://mm.dimu.org/image/<identifier>?dimension=<dim>
        // as per documentation available at http://api.dimu.org/doc/public_api.html
        char imglink[128];
        snprintf(imglink, sizeof imglink, "https://mm.dimu.org/image/%s", mediaid.c_str());

        
        
        char descfilename[64];
        snprintf(descfilename, sizeof descfilename, "%s.txt", article_id.c_str());

        std::string description = "";
        if (article.HasMember("artifact.ingress.description")) {
          const rapidjson::Value& descv = article["artifact.ingress.description"];
          if (descv.IsString()) {
            description = descv.GetString();
            std::replace( description.begin(), description.end(), ',', ':'); 
            std::replace( description.begin(), description.end(), '\n', ' ');
            std::ofstream desc_file;
            desc_file.open(descfilename);
            desc_file<<description;
            desc_file.close();
          }
        }

        std::string license = "";
        if (article.HasMember("artifact.ingress.license")) {
          if (article[ "artifact.ingress.license"].IsString())
            license = article[ "artifact.ingress.license"].GetString();
          else if (article[ "artifact.ingress.license"].IsArray()) {
            for (auto i = 0; i<article[ "artifact.ingress.license"].Size(); i++) {
              license += article[ "artifact.ingress.license"][i].GetString();
            }
          }
        }

        bool is_cc_license = false;
        if (license.find("CC by")!= std::string::npos) {
          is_cc_license = true;
          char imgfetch[256];
          snprintf(imgfetch, sizeof imgfetch, "wget %s -O \"%s-%s.jpeg\"", imglink, article_id.c_str(), mediaid.c_str());
          std::cout<<imgfetch<<std::endl;
          //          system (imgfetch);
          std::cout<<"Found CC by license, fetched the image for "<< mediaid<<"\n";
        }
        if (is_cc_license) {
        out_file1<<
          /*  article["artifact.defaultMediaIdentifier"].GetString()<<
              ", "<< article["artifact.defaultPictureIndex"].GetInt()<<
              ", "<< article["artifact.defaultPictureDimension"].GetString()<<
              ", "<<*/
          article_id <<
          ", "<< title <<
          ", "<< yearstr(yearb) <<
          ", "<< yearstr(yeare) <<
          ", "<< descfilename <<
          ", "<< imglink <<
          ", "<< article_id+"-"+ mediaid +".jpeg" <<
          ", "<<"Länge leve Kosta! exhibition" <<
          ", "<<"Kulturparken Småland / Smålands museum" <<
          ", "<< license <<
          std::endl;
        }
        }
      }
    }
  }
  out_file1.close();   
  return 0;
}


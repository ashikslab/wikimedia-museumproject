#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>

#define debug_level 0
std::string yearstr(int year) {
  return year==-1? "Unknown": std::to_string(year);
}

int main(int argc, char **argv)
{
  bool download_mode =false;
  if (argc == 2) {
    std::string arg1 = argv[1];
    download_mode = arg1=="--download";
  }
  char filename[64];
  snprintf(filename, sizeof filename, "out.csv");
  std::ofstream out_file1;
  out_file1.open(filename);
  out_file1<<"id, Caption/title, production start year, end year, Description, Source, image_filename, subjects, date published, collection name, museum name, exif_model, exif_iso, exif_focallength, exif_exposuretime, exif_aperture,  exif_datetimeoriginal, liceses\n";
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

        std::string subjects;
        if (article.HasMember("artifact.ingress.subjects")) {
          const rapidjson::Value& subjv = article["artifact.ingress.subjects"];
          if (subjv.IsArray()) {
            for (auto i = 0; i<subjv.GetArray().Size(); i++) {
              subjects += subjv[i].GetString();
            }
            std::replace( description.begin(), description.end(), ',', ':'); 
            std::replace( description.begin(), description.end(), '\n', ' ');
          }
        }

        std::string publishdate;
        if (article.HasMember("artifact.publishedDate")) {
          const rapidjson::Value& pubdatev = article["artifact.publishedDate"];
          if (pubdatev.IsString()) {
            publishdate = pubdatev.GetString();
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
          if (debug_level > 0) std::cout<<imgfetch<<std::endl;
          if (download_mode) {
            std::cout<<"running in download mode\n";
            system (imgfetch);
            if (debug_level >0) std::cout<<"Found CC by license, fetched the image for "<< mediaid<<"\n";
          }
        }

        if (is_cc_license) {
          char exif_file[128];
          snprintf(exif_file, sizeof exif_file, "%s-%s.jpeg.exif.json", article_id.c_str(), mediaid.c_str());
          // Open the file
          FILE* exiffp = fopen(exif_file, "rb");
          if (!exiffp) {
            std::cerr << "Error: unable to open file" << std::string(exif_file)
                      << std::endl;
            return -1;
          }
        

          char exifreadBuffer[4096];
          rapidjson::FileReadStream exifis(exiffp, exifreadBuffer, sizeof(exifreadBuffer));
          rapidjson::Document exifdoc;
          exifdoc.ParseStream(exifis);

          // Check if the document is valid
          if (exifdoc.HasParseError()) {
            std::cerr << "Error: failed to parse JSON document exif data"
                      << std::endl;
          }
          fclose(exiffp);
        
          std::string exif_model, exif_iso, exif_focallength, exif_exposuretime, exif_aperture, exif_fnumber, exif_datetimeoriginal;
          if (exifdoc.IsArray()) {
            if (exifdoc[0].IsObject()) {
              if (exifdoc[0].GetObject().HasMember("Model")) {
                exif_model = exifdoc[0].GetObject()["Model"].GetString();
              }
              if (exifdoc[0].GetObject().HasMember("ISO")) {
                exif_iso = std::to_string(exifdoc[0].GetObject()["ISO"].GetInt());
              }
              if (exifdoc[0].GetObject().HasMember("FocalLength")) {
                exif_focallength = exifdoc[0].GetObject()["FocalLength"].GetString();
              }
              if (exifdoc[0].GetObject().HasMember("ExposureTime")) {
                if (exifdoc[0].GetObject()["ExposureTime"].IsString())
                  exif_exposuretime = exifdoc[0].GetObject()["ExposureTime"].GetString();
              }
              if (exifdoc[0].GetObject().HasMember("ApertureValue")) {
                exif_aperture = std::to_string(exifdoc[0].GetObject()["ApertureValue"].GetDouble());
              }
              if (exifdoc[0].GetObject().HasMember("DateTimeOriginal")) {
                exif_datetimeoriginal = exifdoc[0].GetObject()["DateTimeOriginal"].GetString();
              }
            }
          }


        out_file1<<
          /*  article["artifact.defaultMediaIdentifier"].GetString()<<
              ", "<< article["artifact.defaultPictureIndex"].GetInt()<<
              ", "<< article["artifact.defaultPictureDimension"].GetString()<<
              ", "<<*/
          article_id <<
          ", "<< title <<
          ", "<< yearstr(yearb) <<
          ", "<< yearstr(yeare) <<
          ", "<< description <<
          ", "<< imglink <<
          ", "<< article_id+"-"+ mediaid +".jpeg" <<
          ", "<< subjects <<
          ", "<< publishdate <<
          ", "<<"Länge leve Kosta! exhibition" <<
          ", "<<"Kulturparken Småland / Smålands museum" <<
          ", "<< exif_model <<
          ", "<< exif_iso <<
          ", "<< exif_focallength <<
          ", "<< exif_exposuretime <<
          ", "<< exif_aperture <<
          ", "<< exif_datetimeoriginal<<
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


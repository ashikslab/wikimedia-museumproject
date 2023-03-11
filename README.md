Dependencies: curlpp, rapidjson

To build please do:

mkdir build
cd build
cmake ..
make

How to use?
-----------
1. First run the fetch_all binary with your api key as argument.
  It will fetch the json data into your local folder.

2. Run parser with argument "--download" (this will
  download all the images).

3. Run the "genexifall" script- Supply the folder name
  containing the dwnloaded images as parameter.

4. Run parser again without any arguments.
   You should get the out.csv file generated at this step
  (This is the file that you can use to import the data on OpenRefine).


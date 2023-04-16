import sys
import csv
import urllib.response
import urllib.request

# check if the user supplied the correct number of command line arguments
if len(sys.argv) != 6:
    print("Usage: python titlegen.py inputfilename.csv  startlineno endlineno outputfilename.csv API_key")
    sys.exit(1)
    # set the api key from the command line argument

startlineno = int(sys.argv[2])
endlineno = int(sys.argv[3])
lineno = 0
api_key = sys.argv[5]
with open(sys.argv[1], "r") as file:
        outfile = open(sys.argv[4], "w")
        writer = csv.writer(outfile)
        reader = csv.reader(file)
        for row in reader:
            lineno += 1
            if (lineno < startlineno or lineno > endlineno):
                continue
            if lineno == 1:
                newrow = row
                row.append("Materials")
                row.append("Dimensions")
                writer.writerow(newrow)
                continue
                
            # get the 7th column of the row to a string variable
            itemurl = row[5]
            print(itemurl)
            # Extract the string after the last slash in the itemurl
            itemid = itemurl.rsplit('/', 1)[-1]
            print(itemid)
            
            # Fetch the xml data from "https://api.dimu.org/api/artifact?unique_id=<itemid>&mapping=ESE&api.key=<api_key"
            url = "https://api.dimu.org/api/artifact?unique_id=" + itemid + "&mapping=ESE&api.key=" + api_key
            print(url)
            response = urllib.request.urlopen(url)
            data = response.read()
            # Convert the xml data to a string
            data = data.decode("utf-8")
            # Extract the fields in the xml data that has are inside <dcterms:medium> tag as string variables
            
            # Use an xml parser and extract the fields in the xml data that has are inside <dcterms:medium> tag as string variables
            from xml.dom.minidom import parse, parseString  
            
            dom = parseString(data)
            # Extract the fields in the xml data that has are inside <dcterms:medium> tag as string variables
            allmediums = ""
            medium_list = dom.getElementsByTagName('dcterms:medium')
            # iterate through the list of medium_list and print the value of the first child node of each element
            
            for medium in medium_list:
                mediumname = medium.firstChild.nodeValue
                allmediums += mediumname + " "
            print(allmediums)
            
            allextents = ""
            extent_list = dom.getElementsByTagName('dcterms:extent')
            # iterate through the list of extent_list and print the value of the first child node of each element
            for extent in extent_list:
                extentitem = extent.firstChild.nodeValue
                allextents += extentitem + " "
            print(allextents)
            
            # Generate a new row with the previosu row appended with the new fields medium and extent
            newrow = row
            newrow.append(allmediums)
            newrow.append(allextents)
            writer.writerow(newrow)
            
        outfile.close()
        file.close()
